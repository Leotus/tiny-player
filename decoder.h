#ifndef DECODER_H
#define DECODER_H

#include <thread>

#include "ffplayer_basic.h"

class Decoder
{
public:
    AVPacket pkt_;
    PacketQueue *queue_;
    AVCodecContext *avctx_;
    int pkt_serial_;
    int finished_;
    std::thread *decoder_thread_ = NULL;

public:
    Decoder();
    ~Decoder();

    void decoder_init(AVCodecContext* avctx, PacketQueue *queue);
    int decoder_start(enum AVMediaType codec_type, const char *thread_name, void *arg);
    void decoder_abort(FrameQueue *fq);
    void decoder_destroy();
    int decoder_decode_frame(AVFrame* frame);
    int get_video_frame(AVFrame* frame);
    int queue_picture(FrameQueue* fq, AVFrame* src_frame, double pts, double duration, int64_t pos, int serial);

    int audio_thread(void *arg);
    int video_thread(void *arg);
};

#endif // DECODER_H
