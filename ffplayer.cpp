#include "ffplayer.h"

void print_error(const char* filename, int err)
{
    char errbuf[128];
    const char *errbuf_ptr = errbuf;

    if(av_strerror(err, errbuf, sizeof(errbuf)) < 0)
        errbuf_ptr = strerror(AVUNERROR(err));
    av_log(NULL, AV_LOG_ERROR, "%s:%s\n", filename, errbuf_ptr);
}


FFPlayer::FFPlayer()
{

}

int FFPlayer::ffp_create()
{
    std::cout << "ffp_create" << std::endl;
    msg_queue_init(&msg_queue_);
    return 0;
}

void FFPlayer::ffp_destroy()
{
    stream_close();
    msg_queue_destroy(&msg_queue_);
}

int FFPlayer::ffp_prepare_async_l(const char *file_name)
{
    input_filename_ = strdup(file_name);
    int reval = stream_open(input_filename_);
    return reval;
}

int FFPlayer::ffp_start_l()
{
    // 触发播放
    std::cout << __FUNCTION__ << std::endl;
    return 0;
}

int FFPlayer::ffp_stop_l()
{
    abort_request = 1; // 请求退出
    msg_queue_abort(&msg_queue_);
    return 0;
}

int FFPlayer::stream_open(const char *file_name)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO | SDL_INIT_TIMER))
    {
        av_log(NULL, AV_LOG_FATAL, "Could not initialize SDL - %s\n", SDL_GetError());
        av_log(NULL, AV_LOG_FATAL, "Did you set the Display variable?\n");
        return -1;
    }
    // 初始化Frame帧队列
    if(frame_queue_init(&pictq, &videoq, VIDEO_PICTURE_QUEUE_SIZE_DEFAULT) < 0)
        goto failed;
    if(frame_queue_init(&sampq, &audioq, SAMPLE_QUEUE_SIZE) < 0)
        goto failed;
    // 初始化Packet包队列
    if(packet_queue_init(&videoq) < 0 || packet_queue_init(&audioq) < 0 )
        goto failed;
    // 初始化时钟
    init_clock(&audclk);
    // 创建解复用器读取线程read_thread
    read_thread_ = new std::thread(&FFPlayer::read_thread, this);

    // 创建视频创新线程
    video_refresh_thread_ = new std::thread(&FFPlayer::video_refresh_thread, this);
    return 0;
failed:
    stream_close();
    return -1;
}

void FFPlayer::stream_close()
{
    abort_request = 1; // 请求退出
    if(read_thread_ && read_thread_->joinable()){
        read_thread_->join(); // 等待线程退出
    }

    if(audio_stream >= 0)
        stream_component_close(audio_stream);
    if(video_stream >= 0)
        stream_component_close(video_stream);



    // 关闭解复用器 avformat_close_input(&is->ic);
    // 释放packet队列
    packet_queue_destroy(&videoq);
    packet_queue_destroy(&audioq);
    // 释放frame队列
    frame_queue_destroy(&pictq);
    frame_queue_destroy(&sampq);

    if(input_filename_)
    {
        free(input_filename_);
        input_filename_ = NULL;
    }
}

int FFPlayer::stream_component_open(int stream_index)
{
    AVCodecContext *avctx;
    AVCodec *codec;
    int sample_rate;
    int nb_channels;
    int64_t channel_layout;
    int ret = 0;

    // 判断stream_index是否合法
    if(stream_index < 0 || stream_index >= ic->nb_streams)
        return -1;
    avctx = avcodec_alloc_context3(NULL);
    if(!avctx)
        return AVERROR(ENOMEM);

    ret = avcodec_parameters_to_context(avctx, ic->streams[stream_index]->codecpar);
    if(ret < 0)
        goto fail;
    avctx -> pkt_timebase = ic->streams[stream_index]->time_base;

    codec = avcodec_find_decoder(avctx->codec_id);
    if(!codec){
        av_log(NULL, AV_LOG_WARNING, "No decoder could be found for codec %s\n", avcodec_get_name(avctx->codec_id));
        ret = AVERROR(EINVAL);
        goto fail;
    }
    if((ret = avcodec_open2(avctx, codec, NULL)) < 0 )
        goto fail;
    switch(avctx->codec_type)
    {
    case AVMEDIA_TYPE_AUDIO:
        sample_rate = avctx->sample_rate;
        nb_channels = avctx->channels;
        channel_layout = avctx->channel_layout;
        // 准备音频输出
        if((ret = audio_open(channel_layout, nb_channels, sample_rate, &audio_tgt)) < 0)
            goto fail;
        audio_hw_buf_size = ret;
        audio_src = audio_tgt; // 暂时相等赋值
        audio_buf_size = 0;
        audio_buf_index = 0;

        audio_stream = stream_index;
        audio_st = ic->streams[stream_index];
        // 初始化ffplay封装的音频解码器，并将解码器上下文avctx与decoder绑定
        auddec.decoder_init(avctx, &audioq);
        // 启动音频解码线程
        auddec.decoder_start(AVMEDIA_TYPE_AUDIO, "audio_thread", this);
        // 允许音频输出
        SDL_PauseAudio(0);
        break;
    case AVMEDIA_TYPE_VIDEO:
        video_stream = stream_index;
        video_st = ic->streams[stream_index];
        // 初始化ffplay封装的视频解码器，并将解码器上下文avctx与decoder绑定
        viddec.decoder_init(avctx, &videoq);
        // 启动视频解码线程
        if((ret = viddec.decoder_start(AVMEDIA_TYPE_VIDEO, "video_thread", this)) < 0)
            goto out;
        // 允许视频输出
        break;
    default:
        break;
    }
    goto out;
fail:
    avcodec_free_context(&avctx);
out:
    return ret;
}

void FFPlayer::stream_component_close(int stream_index)
{
    AVCodecParameters *codecpar;
    if(stream_index < 0 || stream_index >= ic->nb_streams)
        return;

    codecpar = ic->streams[stream_index]->codecpar;

    switch(codecpar->codec_type)
    {
    case AVMEDIA_TYPE_AUDIO:
        std::cout << __FUNCTION__ << " AVMEDIA_TYPE_AUDIO\n";
        // 请求终止解码器线程
        auddec.decoder_abort(&sampq);
        // 关闭音频设备
        audio_close();
        // 销毁解码器
        auddec.decoder_destroy();
        // 释放重采样器
        swr_free(&swr_ctx);
        // 释放audio buf
        av_freep(&audio_buf1);
        audio_buf1_size = 0;
        audio_buf = NULL;
        break;
    case AVMEDIA_TYPE_VIDEO:
        std::cout << __FUNCTION__ << " AVMEDIA_TYPE_VIDEO\n";
        // 请求终止解码器线程
        viddec.decoder_abort(&pictq);
        // 关闭视频渲染
        if(video_refresh_thread_ && video_refresh_thread_->joinable())
            video_refresh_thread_->join();
        // 销毁解码器
        viddec.decoder_destroy();
        break;
    default:
        break;
    }

    // ic->streams[stream_index]->discard = AVDISCARD_ALL; // ????
    switch(codecpar->codec_type)
    {
    case AVMEDIA_TYPE_AUDIO:
        audio_st = NULL;
        audio_stream = -1;
        break;
    case AVMEDIA_TYPE_VIDEO:
        video_st = NULL;
        video_stream = -1;
        break;
    default:
        break;
    }
}

static int audio_resample_frame(FFPlayer *is)
{
    int data_size, resampled_data_size;
    int64_t dec_channel_layout;
    int wanted_nb_samples;
    MyFrame *af;
    int ret = 0;

    if(!(af = frame_queue_peek_readable(&is->sampq)))
        return -1;

    data_size = av_samples_get_buffer_size(NULL, af->frame->channels, af->frame->nb_samples, (enum AVSampleFormat)af->frame->format, 1);
    dec_channel_layout = (af->frame->channel_layout &&
                          af->frame->channels == av_get_channel_layout_nb_channels(af->frame->channel_layout))?
                        af->frame->channel_layout : av_get_default_channel_layout(af->frame->channels);
    wanted_nb_samples = af->frame->nb_samples;

    // 判断一下是否需要
    if(af->frame->format != is->audio_src.fmt ||
        dec_channel_layout != is->audio_src.channel_layout ||
        af->frame->sample_rate != is->audio_src.freq){
        swr_free(&is->swr_ctx);
        is->swr_ctx = swr_alloc_set_opts(NULL,
                                         is->audio_tgt.channel_layout,
                                         is->audio_tgt.fmt,
                                         is->audio_tgt.freq,
                                         dec_channel_layout,
                                         (enum AVSampleFormat)af->frame->format,
                                         af->frame->sample_rate,
                                         0, NULL);
        if (!is->swr_ctx || swr_init(is->swr_ctx) < 0) {
            av_log(NULL, AV_LOG_ERROR,
                   "Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
                   af->frame->sample_rate, av_get_sample_fmt_name((enum AVSampleFormat)af->frame->format), af->frame->channels,
                   is->audio_tgt.freq, av_get_sample_fmt_name(is->audio_tgt.fmt), is->audio_tgt.channels);
            swr_free(&is->swr_ctx);
            ret = -1;
            goto fail;
        }
        is->audio_src.channel_layout = dec_channel_layout;
        is->audio_src.channels = af->frame->channels;
        is->audio_src.freq = af->frame->sample_rate;
        is->audio_src.fmt = (enum AVSampleFormat)af->frame->format;
    }
    if(is->swr_ctx)
    {
        // 重采样输入参数1 af->frame->nb_samples
        // 重采样输入参数2 输入音频缓冲区
        const uint8_t **in = (const uint8_t **) af->frame->extended_data; // data[0] data[1]

        // 重采样输出参数1 输出音频缓冲区
        uint8_t **out = &is->audio_buf1;

        // 重采样输出参数2 输出音频缓冲区尺寸，+256目的是重采样内部有一定的缓存
        int out_count = (int64_t)wanted_nb_samples * is->audio_tgt.freq / af->frame->sample_rate + 256;
        int out_size = av_samples_get_buffer_size(NULL, is->audio_tgt.channels, out_count, is->audio_tgt.fmt, 0);

        int len2;
        if(out_size < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "av_sample_get_buffer_size() failed\n");
            ret = -1;
            goto fail;
        }
        av_fast_malloc(&is->audio_buf1, &is->audio_buf1_size, out_size);
        if(!is->audio_buf1){
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        len2 = swr_convert(is->swr_ctx, out, out_count, in, af->frame->nb_samples);
        if(len2 < 0){
            av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
            ret = -1;
            goto fail;
        }
        if(len2 == out_count){
            av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
            if (swr_init(is->swr_ctx) < 0)
                swr_free(&is->swr_ctx);
        }
        is->audio_buf = is->audio_buf1;
        resampled_data_size = len2 * is->audio_tgt.channels * av_get_bytes_per_sample(is->audio_tgt.fmt);
    }
    else
    {
        // 不需要重采样
        is->audio_buf = af->frame->data[0];
        resampled_data_size = data_size;
    }

    if(!isnan(af->pts))
        is->audio_clock = af->pts;
    else
        is->audio_clock = NAN;

    frame_queue_next(&is->sampq); // 释放一帧
    ret = resampled_data_size;
fail:
    return ret;
}


static void sdl_audio_callback(void* opaque, Uint8* stream, int len)
{
    FFPlayer *is = (FFPlayer *)opaque;
    int audio_size, len1;
    while(len > 0) // 循环读取，直到读到足够的数据
    {
        // (1)如果is->audio_buf_index < is->audio_buf_size则说明上次拷贝还剩余数据
        // 先拷贝到stream再调用重采样
        // (2)如果audio_buf消耗完了，则调用重采样重新填充audio_buf
        if(is->audio_buf_index >= is->audio_buf_size){
            audio_size = audio_resample_frame(is);
            if(audio_size < 0){
                // 出错的时候执行静音逻辑
                is->audio_buf = NULL;
                is->audio_buf_size = SDL_AUDIO_MIN_BUFFER_SIZE / is->audio_tgt.frame_size * is->audio_tgt.frame_size;
            }else{
                is -> audio_buf_size = audio_size;
            }
            is->audio_buf_index = 0;
        }
        len1 = is->audio_buf_size - is->audio_buf_index;
        if(len1 > len)
            len1 = len;
        if(is->audio_buf)
            memcpy(stream, (uint8_t*)is->audio_buf + is->audio_buf_index, len1);
        len -= len1;
        stream += len1;
        is->audio_buf_index += len1;
    }
    if(!isnan(is->audio_clock))
        set_clock(&is->audclk, is->audio_clock);
}

int FFPlayer::audio_open(int64_t wanted_channel_layout, int wanted_nb_channels, int wanted_sample_rate, AudioParams *audio_hw_params)
{
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = wanted_sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = wanted_nb_channels;
    wanted_spec.silence = 0;
    wanted_spec.samples = 2048;
    wanted_spec.callback = sdl_audio_callback; // 回调函数
    wanted_spec.userdata = this;

    // 打开音频设备
    if(SDL_OpenAudio(&wanted_spec, NULL) != 0)
    {
        std::cout << "Failed to open audio device, " << SDL_GetError() << std::endl;
        return -1;
    }
    // audio_hw_params的值将会被设置到audio_tgt里
    audio_hw_params->fmt = AV_SAMPLE_FMT_S16;
    audio_hw_params->freq = wanted_spec.freq;
    audio_hw_params->channel_layout = wanted_channel_layout;
    audio_hw_params->channels = wanted_spec.channels;
    audio_hw_params->frame_size = av_samples_get_buffer_size(NULL, audio_hw_params->channels,
                                                             1,audio_hw_params->fmt,1);
    audio_hw_params->bytes_per_sec = av_samples_get_buffer_size(NULL, audio_hw_params->channels,
                                                                audio_hw_params->freq,audio_hw_params->fmt, 1);
    if(audio_hw_params->bytes_per_sec <= 0 || audio_hw_params->frame_size <= 0)
    {
        av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
        return -1;
    }
    return wanted_spec.size;
}

void FFPlayer::audio_close()
{
    SDL_CloseAudio();
}

int FFPlayer::read_thread()
{
    int err, i, ret;
    int st_index[AVMEDIA_TYPE_NB]; // AVMEDIA_TYPE_VIDEO or AVMEDIA_TYPE_AUDIO等，用来保持stream_index
    AVPacket pkt1;
    AVPacket* pkt = &pkt1;

    memset(st_index, -1, sizeof(st_index));
    video_stream = -1;
    audio_stream = -1;
    eof = 0;

    // 1.创建上下文结构体，输入上下文
    ic = avformat_alloc_context();
    if(!ic)
    {
        av_log(NULL, AV_LOG_FATAL, "Could not allocate context.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    // 3. 打开文件，探测协议类型，如果是网络文件则创建网络链接
    err = avformat_open_input(&ic, input_filename_, NULL, NULL);
    if(err < 0)
    {
        print_error(input_filename_, err);
        ret = -1;
        goto fail;
    }
    ffp_notify_msg1(this, FFP_MSG_OPEN_INPUT);
    std::cout << "read thread FFP_MSG_OPEN_INPUT" << this << std::endl;

    // 4. 探测媒体类型，可得到当前文件的封装格式，音视频编码参数等信息
    err = avformat_find_stream_info(ic, NULL);
    if(err < 0){
        av_log(NULL, AV_LOG_WARNING, "%s: could not find codec parameters\n", input_filename_);
        ret = -1;
        goto fail;
    }
    ffp_notify_msg1(this, FFP_MSG_FIND_STREAM_INFO);
    std::cout << "read thread FFP_MSG_FIND_STREAM_INFO " << this << std::endl;

    // 利用av_find_best_stream选择流
    st_index[AVMEDIA_TYPE_VIDEO] = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, st_index[AVMEDIA_TYPE_VIDEO], -1, NULL, 0);
    st_index[AVMEDIA_TYPE_AUDIO] = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, st_index[AVMEDIA_TYPE_AUDIO], st_index[AVMEDIA_TYPE_VIDEO], NULL, 0);

    // 打开视频、音频解码器，在此会打开相应解码器，并创建相应的解码线程
    if(st_index[AVMEDIA_TYPE_AUDIO] >= 0)
        stream_component_open(st_index[AVMEDIA_TYPE_AUDIO]);
    ret = -1;
    if(st_index[AVMEDIA_TYPE_VIDEO] >= 0)
        ret = stream_component_open(st_index[AVMEDIA_TYPE_VIDEO]);
    ffp_notify_msg1(this, FFP_MSG_COMPONENT_OPEN);
    std::cout << "read thread FFP_MSG_COMPONENT_OPEN " << this << std::endl;

    if(video_stream < 0 && audio_stream < 0)
    {
        av_log(NULL, AV_LOG_FATAL, "Failed to open file '%s' or configure filtergraph\n", input_filename_);
        ret = -1;
        goto fail;
    }

    ffp_notify_msg1(this, FFP_MSG_PREPARED);
    std::cout << "read thread FFP_MSG_PREPARED" << std::endl;
    while(1)
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if(abort_request)
        {
            break;
        }
        // 读取媒体数据，得到音视频分离后，解码前的数据
        ret = av_read_frame(ic, pkt);
        if(ret < 0)
        {
            if((ret == AVERROR_EOF || avio_feof(ic->pb)) && !eof)
                eof = 1; // 读取完了
            if(ic->pb && ic->pb->error)
                break; // io出错
            std::this_thread::sleep_for(std::chrono::milliseconds(10));// 读取完数据了，这里可以使用timeout的方式休眠等待下一步的检测
            continue;
        }
        else
        {
            eof = 0;
        }
        // 插入队列，先处理音频包
        if(pkt->stream_index == audio_stream)
        {
            printf("audio ===== pkt pts:%ld, dts:%lld\n", pkt->pts/48, pkt->dts);
            packet_queue_put(&audioq, pkt);
        }
        else if(pkt->stream_index == video_stream)
        {
            printf("video ===== pkt pts:%ld, dts:%ld\n", pkt->pts/48, pkt->dts);
            packet_queue_put(&videoq, pkt);
        }
        else
        {
            av_packet_unref(pkt);
        }
    }
    std::cout << __FUNCTION__ << " leave" << std::endl;
    return 0;
fail:
    return -1;
}

int FFPlayer::video_refresh_thread()
{
    double remaining_time = 0.0;
    while ( ! abort_request ){
        if(remaining_time > 0.0)
            av_usleep((int)(int64_t)(remaining_time * 1000000.0));
        remaining_time = REFRESH_RATE;
        video_refresh(&remaining_time);
    }
    std::cout << __FUNCTION__ << " leave " << std::endl;
    return 0;
}

void FFPlayer::video_refresh(double *remaining_time)
{
    MyFrame *vp = NULL;
    if(video_st){
        if(frame_queue_nb_remaining(&pictq) == 0)
            return;
        vp = frame_queue_peek(&pictq);

        // 对比时钟
        double diff = vp->pts - get_clock(&audclk);
        std::cout << __FUNCTION__ << "vp->pts:" << vp->pts << " - af->pts:" << get_clock(&audclk) << ", diff:" << diff << std::endl;
        if(diff > 0){
            *remaining_time = FFMIN(*remaining_time, diff);
            return;
        }

        // 刷新
        if(video_refresh_callback_)
            video_refresh_callback_(vp);
        else
            std::cout << __FUNCTION__ << " video_refresh_callback_ NULL" << std::endl;
        frame_queue_next(&pictq);
    }
}

void FFPlayer::add_video_refresh_callback(std::function<int (const MyFrame *)> callback)
{
    video_refresh_callback_ = callback;
}

int FFPlayer::get_master_sync_type()
{
    if (av_sync_type == AV_SYNC_VIDEO_MASTER) {
        if (video_st)
            return AV_SYNC_VIDEO_MASTER;
        else
            return AV_SYNC_AUDIO_MASTER;	 /* 如果没有视频成分则使用 audio master */
    } else if (av_sync_type == AV_SYNC_AUDIO_MASTER) {
        if (audio_st)
            return AV_SYNC_AUDIO_MASTER;
        else if(video_st)
            return AV_SYNC_VIDEO_MASTER;        // 只有音频的存在
        else
            return AV_SYNC_UNKNOW_MASTER;
    }
}

double FFPlayer::get_master_clock()
{
    double val;

    switch (get_master_sync_type()) {
    case AV_SYNC_VIDEO_MASTER:
        //        val = get_clock(&vidclk);
        break;
    case AV_SYNC_AUDIO_MASTER:
        val = get_clock(&audclk);
        break;
    default:
        val = get_clock(&audclk);
        break;
    }
    return val;
}
