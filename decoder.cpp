#include "decoder.h"
#include "ffplayer.h"
#include <iostream>

Decoder::Decoder()
{
    av_init_packet(&pkt_);
}

Decoder::~Decoder()
{

}

void Decoder::decoder_init(AVCodecContext *avctx, PacketQueue *queue)
{
    avctx_ = avctx;
    queue_ = queue;
}

int Decoder::decoder_start(AVMediaType codec_type, const char *thread_name, void *arg)
{
    int ret = 0;
    packet_queue_start(queue_);
    switch(codec_type)
    {
    case AVMEDIA_TYPE_VIDEO:
        decoder_thread_ = new std::thread(&Decoder::video_thread, this, arg);
        break;
    case AVMEDIA_TYPE_AUDIO:
        decoder_thread_ = new std::thread(&Decoder::audio_thread, this, arg);
        break;
    default:
        ret = -1;
        break;
    }
    return 0;
}

void Decoder::decoder_abort(FrameQueue *fq)
{
    packet_queue_abort(queue_);
    frame_queue_signal(fq);
    if(decoder_thread_ && decoder_thread_ -> joinable())
    {
        decoder_thread_->join();
        delete decoder_thread_;
        decoder_thread_ = NULL;
    }
    packet_queue_flush(queue_);
}

void Decoder::decoder_destroy()
{
    av_packet_unref(&pkt_);
    avcodec_free_context(&avctx_);
}

int Decoder::decoder_decode_frame(AVFrame *frame)
{
    int ret = AVERROR(EAGAIN);
    while(1)
    {
        AVPacket pkt;
        do{ // 将codec里的frame全部读出来
            if(queue_->abort_request)
                return -1;
            switch(avctx_->codec_type)
            {
            case AVMEDIA_TYPE_VIDEO:
                ret = avcodec_receive_frame(avctx_, frame);
                if(ret >= 0)
                {

                }
                else
                {
                    char errStr[256] = {0};
                    av_strerror(ret, errStr,sizeof(errStr));
                    std::cout << "video dec: " << errStr << std::endl;
                }
                break;
            case AVMEDIA_TYPE_AUDIO:
                ret = avcodec_receive_frame(avctx_, frame);
                if(ret >= 0)
                {
                    AVRational tb{1, frame->sample_rate};
                    if(frame->pts != AV_NOPTS_VALUE)
                    {
                        // 如果frame->pts正常则先将其从pkt_timebase转成{1, frame->sample_rate}
                        // pkt_timebase实质就是stream->time_base
                        frame->pts = av_rescale_q(frame->pts, avctx_->pkt_timebase, tb);
                    }
                }
                else
                {
                    char errStr[256] = {0};
                    av_strerror(ret, errStr,sizeof(errStr));
                    std::cout << "audio dec: " << errStr << std::endl;
                }
                break;
            }
            if(ret == AVERROR_EOF){ // 已经结束，解码结束返回0
                std::cout << "avcodec_flush_buffers " << __FUNCTION__ << "(" << __LINE__ << ")" << std::endl;
                avcodec_flush_buffers(avctx_);
                return 0;
            }
            if(ret >= 0)
                return 1; // 正常解码结束得到一帧，返回1
        }while(ret != AVERROR(EAGAIN)); // 没帧可读ret返回EAGIN，需要继续送packet

        // 执行到这里说明没有或得到frame，需要送packet
        // 阻塞式读取packet
        if(packet_queue_get(queue_, &pkt, 1, &pkt_serial_) < 0)
        {
            return -1;
        }
        if(avcodec_send_packet(avctx_, &pkt) == AVERROR(EAGAIN))
        {
            av_log(avctx_, AV_LOG_ERROR, "Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
        }
        av_packet_unref(&pkt);
    }
}

int Decoder::get_video_frame(AVFrame *frame)
{
    int got_picture;
    // 获取解码后的视频帧
    if((got_picture = decoder_decode_frame(frame)) < 0){
        return -1;
    }
    if(got_picture){
        // 分析或得到的帧是否要drop掉，该机制的目的是在放入帧队列前先drop掉过时的帧
    }
    return got_picture;
}

int Decoder::queue_picture(FrameQueue *fq, AVFrame *src_frame, double pts, double duration, int64_t pos, int serial)
{
    MyFrame *vp;

    if(!(vp = frame_queue_peek_writable(fq)))
        return -1;

    vp->width = src_frame->width;
    vp->height = src_frame->height;
    vp->format = src_frame->format;

    vp->pts = pts;
    vp->duration = duration;

    av_frame_move_ref(vp->frame, src_frame); // 将src中所有数据转移到vp->frame中，复位src
    frame_queue_push(fq);
    return 0;
}

int Decoder::audio_thread(void *arg)
{
    std::cout << __FUNCTION__ << " into " << std::endl;
    FFPlayer *is = (FFPlayer*) arg;
    AVFrame *frame = av_frame_alloc();
    MyFrame *af;
    int got_frame = 0;
    AVRational tb;
    int ret = 0;

    if(!frame)
        return AVERROR(ENOMEM);
    do{
        // 读取解码帧
        if((got_frame = decoder_decode_frame(frame)) < 0)
            goto the_end;
        if(got_frame){
            tb = {1, frame->sample_rate};
            // 获取可写frame
            if(!(af = frame_queue_peek_writable(&is->sampq)))
                goto the_end;
            // 设置MyFrame并放入FrameQueue
            af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
            af->duration = av_q2d({frame->nb_samples, frame->sample_rate});

            av_frame_move_ref(af->frame, frame); // 将src中所有数据转移到ap->frame中，复位src
            frame_queue_push(&is->sampq);
        }
    }while(ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);
the_end:
    std::cout << __FUNCTION__ << " leave " << std::endl;
    av_frame_free(&frame);
    return ret;
}

int Decoder::video_thread(void *arg)
{
    std::cout << __FUNCTION__ << " into " << std::endl;
    FFPlayer *is = (FFPlayer*) arg;
    AVFrame *frame = av_frame_alloc();
    double pts;
    double duration;
    int ret;
    // 获取timebase
    AVRational tb = is->video_st->time_base;
    // 获取帧率，以便计算每帧的duration
    AVRational frame_rate = av_guess_frame_rate(is->ic, is->video_st, NULL);

    if(!frame)
        return AVERROR(ENOMEM);

    while(1)
    {
        ret = get_video_frame(frame);
        if(ret < 0)
            goto the_end;
        if(!ret)
            continue;

        duration = (frame_rate.num && frame_rate.den ? av_q2d({frame_rate.den, frame_rate.num}): 0);
        pts = (frame->pts == AV_NOPTS_VALUE ? NAN : frame->pts * av_q2d(tb));
        ret = queue_picture(&is->pictq, frame, pts, duration, frame->pkt_pos, is->viddec.pkt_serial_);
        av_frame_unref(frame);
        if(ret < 0)
            goto the_end;
    }

the_end:
    std::cout << __FUNCTION__ << " leave " << std::endl;
    av_frame_free(&frame);
    return 0;
}


