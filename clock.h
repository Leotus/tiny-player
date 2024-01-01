#ifndef CLOCK_H
#define CLOCK_H
#include "ffplayer_basic.h"

typedef struct Clock{
    double pts; // 时钟基础, 当前帧(待播放)显示时间戳，播放后，当前帧变成上一帧
    double pts_drift; // 当前pts与当前系统时钟的差值, audio、video对于该值是独立的
    double last_update; // 当前时钟(如视频时钟)最后一次更新时间，也可称当前时钟时间
}Clock;

// 音视频同步方式，缺省以音频为准
enum {
    AV_SYNC_UNKNOW_MASTER = -1,
    AV_SYNC_AUDIO_MASTER,                   // 以音频为基准
    AV_SYNC_VIDEO_MASTER,                   // 以视频为基准
    //    AV_SYNC_EXTERNAL_CLOCK,                 // 以外部时钟为基准，synchronize to an external clock */
};

double get_clock(Clock *c);
void set_clock_at(Clock *c, double pts, double time);
void set_clock(Clock* c, double pts);
void init_clock(Clock* c);

#endif // CLOCK_H
