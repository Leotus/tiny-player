#ifndef FFPLAYER_H
#define FFPLAYER_H
#include <functional>
#include <iostream>
#include <thread>

#include "ffmsg_queue.h"
#include "ffmsg.h"
#include "ffplayer_basic.h"
#include "decoder.h"
#include "clock.h"

#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30
#define REFRESH_RATE 0.04 // 每视频帧休眠10ms

class FFPlayer
{
public:
    FFPlayer();
    int ffp_create();
    void ffp_destroy();
    int ffp_prepare_async_l(const char *file_name);


    // 播放控制
    int ffp_start_l();
    int ffp_stop_l();
    int stream_open(const char* file_name);
    void stream_close();
    // 打开指定stream对应解码器、创建解码器线程、以及初始化对应的输出
    int stream_component_open(int stream_index);
    // 关闭指定stream的解码线程，释放解码器资源
    void stream_component_close(int stream_index);

    // 打开和关闭音频设备
    int audio_open(int64_t wanted_channel_layout, int wanted_nb_channels, int wanted_sample_rate, struct AudioParams *audio_hw_params);
    void audio_close();


    int read_thread();

    // 视频输出
    int video_refresh_thread();
    void video_refresh(double *remaining_time);
    void add_video_refresh_callback(std::function<int(const MyFrame*)> callback);

    // 时钟相关
    int get_master_sync_type();
    double get_master_clock();


public:
    MessageQueue msg_queue_;
    char *input_filename_;


    std::thread *read_thread_;


    // 帧队列
    FrameQueue pictq; // 视频帧队列
    FrameQueue sampq; // 采用帧队列

    // 包队列
    PacketQueue audioq; // 音频包队列
    PacketQueue videoq; // 视频包队列
    int abort_request = 0;

    AVStream *audio_st = NULL;
    AVStream *video_st = NULL;
    int audio_stream = -1;
    int video_stream = -1;

    Decoder auddec;
    Decoder viddec;

    int eof = 0;
    AVFormatContext *ic = NULL;

    // 音频输出相关
    struct AudioParams audio_src; // 音频解码后的frame参数
    struct AudioParams audio_tgt; // SDL支持的音频参数
    struct SwrContext *swr_ctx = NULL; // 音频重采样上下文
    int audio_hw_buf_size = 0; // SDL音频缓冲区的大小
    uint8_t *audio_buf = NULL; // 指向需要重采样的数据
    uint8_t *audio_buf1 = NULL; // 指向重采样后的数据
    unsigned int audio_buf_size = 0; // 待播放的一帧音频数据（audio_buf指向）
    unsigned int audio_buf1_size = 0; // 申请到的音频缓冲区audio_buf1指向
    int audio_buf_index = 0; // 更新拷贝位置，当前音频帧中已拷贝入SDL音频缓冲区

    // 视频输出相关
    std::thread *video_refresh_thread_ = NULL;
    std::function<int(const MyFrame*)>video_refresh_callback_ = NULL;

    // 时钟相关
    int av_sync_type = AV_SYNC_AUDIO_MASTER;
    Clock audclk; // 音频时钟
    double audio_clock = 0;
};

inline static void ffp_notify_msg1(FFPlayer *ffp, int what)
{
    msg_queue_put_sample3(&ffp->msg_queue_, what, 0 ,0);
}

inline static void ffp_notify_msg2(FFPlayer *ffp, int what, int arg1)
{
    msg_queue_put_sample3(&ffp->msg_queue_, what, arg1 ,0);
}

inline static void ffp_notify_msg3(FFPlayer *ffp, int what, int arg1, int arg2)
{
    msg_queue_put_sample3(&ffp->msg_queue_, what, arg1 , arg2);
}

inline static void ffp_notify_msg4(FFPlayer *ffp, int what, int arg1, int arg2, void* obj, int obj_len)
{
    msg_queue_put_sample4(&ffp->msg_queue_, what, arg1 , arg2, obj, obj_len);
}

inline static void ffp_remove_msg(FFPlayer *ffp, int what)
{
    msg_queue_remove(&ffp->msg_queue_, what);
}

#endif // FFPLAYER_H
