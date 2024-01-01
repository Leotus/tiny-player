#ifndef FFPLAYER_BASIC_H
#define FFPLAYER_BASIC_H

#include <inttypes.h>

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libavutil/pixdesc.h"
#include "libavutil/time.h"
}

#include <SDL.h>
#include <SDL_mutex.h>

/* Basic */
enum RET_CODE
{
    RET_ERR_UNKNOWN = -2,                   // 未知错误
    RET_FAIL = -1,							// 失败
    RET_OK	= 0,							// 正常
    RET_ERR_OPEN_FILE,						// 打开文件失败
    RET_ERR_NOT_SUPPORT,					// 不支持
    RET_ERR_OUTOFMEMORY,					// 没有内存
    RET_ERR_STACKOVERFLOW,					// 溢出
    RET_ERR_NULLREFERENCE,					// 空参考
    RET_ERR_ARGUMENTOUTOFRANGE,				//
    RET_ERR_PARAMISMATCH,					//
    RET_ERR_MISMATCH_CODE,                  // 没有匹配的编解码器
    RET_ERR_EAGAIN,
    RET_ERR_EOF
};

#define VIDEO_PICTURE_QUEUE_SIZE	3       // 图像帧缓存数量
#define VIDEO_PICTURE_QUEUE_SIZE_MIN        (3)
#define VIDEO_PICTURE_QUEUE_SIZE_MAX        (16)
#define VIDEO_PICTURE_QUEUE_SIZE_DEFAULT    (VIDEO_PICTURE_QUEUE_SIZE_MIN)
#define SUBPICTURE_QUEUE_SIZE		16      // 字幕帧缓存数量
#define SAMPLE_QUEUE_SIZE           9       // 采样帧缓存数量
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))


typedef struct AudioParams {
    int			freq;                   // 采样率
    int			channels;               // 通道数
    int64_t		channel_layout;         // 通道布局，比如2.1声道，5.1声道等
    enum AVSampleFormat	fmt;            // 音频采样格式，比如AV_SAMPLE_FMT_S16表示为有符号16bit深度，交错排列模式。
    int			frame_size;             // 一个采样单元占用的字节数（比如2通道时，则左右通道各采样一次合成一个采样单元）
    int			bytes_per_sec;          // 一秒时间的字节数，比如采样率48Khz，2 channel，16bit，则一秒48000*2*16/8=192000
} AudioParams;

/* Packets */
typedef struct MyPacket{
    AVPacket pkt; // 解封装后的包
    struct MyPacket* next; // 下一个节点
    int serial; // 播放序列
} MyPacket;

typedef struct PacketQueue{
    MyPacket *first_pkt, *last_pkt; // 首尾指针
    int nb_packets; // 包数量，也就是队列元素数量
    int size; // 队列所有元素的数据大小总和
    int64_t duration; // 队列所有元素的数据播放持续时间
    int abort_request; // 用户退出请求队列
    int serial; // 播放序列号，和MyAVPacketList的serial作用相同，但改变的时序稍微有点不同
    SDL_mutex *mutex; // 用于维持PacketQueue的多线程安全
    SDL_cond *cond; // 用于读、写线程相互通知
}PacketQueue;

/* Frames */
typedef struct MyFrame
{
    AVFrame *frame; // 解码后的数据帧
    double pts; // 时间戳，单位为s
    double duration; // 该帧持续时间，单位为s
    int width; // 图像宽度
    int height; // 图像高度
    int format; // 对应图像的AVPixelFormat
} MyFrame;

typedef struct FrameQueue{
    MyFrame queue[FRAME_QUEUE_SIZE]; // FRAME_QUEUE_SIZE 最大size，数字太大会占用大量资源，需要注意该值
    int rindex; // 读索引，待播放时读取此帧进行播放，播放后此帧成为上一帧
    int windex; // 写索引
    int size; // 当前总帧数
    int max_size; // 可存储最大帧数
    SDL_mutex *mutex; // 互斥量
    SDL_cond *cond;
    PacketQueue *pktq; // 数据包队列指针
}FrameQueue;

/* Functions */
// 包队列操作
int packet_queue_put(PacketQueue *q, AVPacket* pkt);
int packet_queue_put_nullpacket(PacketQueue *q, int stream_index);
int packet_queue_init(PacketQueue *q);
void packet_queue_flush(PacketQueue *q);
void packet_queue_destroy(PacketQueue *q);
void packet_queue_abort(PacketQueue *q);
void packet_queue_start(PacketQueue *q);
int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block, int *serial);

// 帧队列操作
int frame_queue_init(FrameQueue *f, PacketQueue *pktq, int max_size);
void frame_queue_destroy(FrameQueue *f);
void frame_queue_signal(FrameQueue *f);

MyFrame* frame_queue_peek(FrameQueue *f); // 获取队列当前frame，在调用该函数前先调用frame_queue_nb_remaining确保有frame可读
MyFrame* frame_queue_peek_next(FrameQueue *f); // 获取当前frame的下一个frame，此时要确保queue里面要至少有2个frame
MyFrame* frame_queue_peek_last(FrameQueue *f); // 获取last frame
MyFrame *frame_queue_peek_writable(FrameQueue *f); // 获取可写指针
MyFrame *frame_queue_peek_readable(FrameQueue *f); // 获取可读
void frame_queue_push(FrameQueue *f); // 更新写指针
void frame_queue_next(FrameQueue *f); // 释放当前frame，并更新读索引rindex
int frame_queue_nb_remaining(FrameQueue *f);
int64_t frame_queue_last_pos(FrameQueue *f);

#endif // FFPLAYER_BASIC_H
