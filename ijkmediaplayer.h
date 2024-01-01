#ifndef IJKMEDIAPLAYER_H
#define IJKMEDIAPLAYER_H

#include <iostream>
#include <functional>
#include <mutex>
#include <thread>

#include "ffmsg_queue.h"
#include "ffplayer.h"

#define MP_STATE_IDLE               0
#define MP_STATE_INITIALIZED        1
#define MP_STATE_ASYNC_PREPARING    2
#define MP_STATE_PREPARED           3
#define MP_STATE_STARTED            4
#define MP_STATE_PAUSED             5
#define MP_STATE_COMPLETED          6
#define MP_STATE_STOPPED            7
#define MP_STATE_ERROR              8
#define MP_STATE_END                9


class IjkMediaPlayer
{
public:
    IjkMediaPlayer();
    ~IjkMediaPlayer();
    int ijkmp_create(std::function<int(void*)> msg_loop);
    int ijkmp_destroy();
    // 设置要播放的url
    int ijkmp_set_data_source(const char* url);
    // 准备播放
    int ijkmp_prepare_async();
    // 触发播放
    int ijkmp_start();
    // 停止
    int ijkmp_stop();
    // 暂停
    int ijkmp_pause();
    // seek到指定位置
    int ijkmp_seek_to(long msec);
    // 获取播放状态
    int ijkmp_get_state();
    // 是不是播放中
    bool ijkmp_is_playing();
    // 当前播放位置
    long ijkmp_get_current_position();
    // 总长度
    long ijkmp_get_duration();
    // 已经播放的长度
    long ijkmp_get_playable_duration();
    // 设置循环播放
    void ijkmp_get_loop();
    // 读取消息
    int ijkmp_get_msg(AVMessage* msg, int block);
    // 设置音量
    void ijkmp_set_playback_volume(float volume);

    // 循环线程
    int ijkmp_msg_loop(void *arg);

    void add_video_refresh_callback(std::function<int(const MyFrame*)> callback);
private:
    // 互斥量
    std::mutex mutex_;
    // 播放器核心
    FFPlayer *ffplayer_ = NULL;
    // 函数指针，指向创建message_loop即消息循环函数
    std::function<int(void *)> msg_loop_ = NULL; // ui处理消息函数
    // 消息机制线程
    std::thread *msg_thread_; // 执行msg_loop
    // 字符串，播放url
    char *data_source_;
    // 播放器状态，例如prepared, resumed, error, completed等
    int mp_state_; // 播放状态
};

#endif // IJKMEDIAPLAYER_H
