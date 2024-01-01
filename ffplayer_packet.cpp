#include "ffplayer_basic.h"

#include <iostream>

static AVPacket flush_pkt;

static int packet_queue_put_private(PacketQueue *q,AVPacket *pkt)
{
    MyPacket* pkt1;

    if(q->abort_request) // 已经终止
        return -1;

    pkt1 = (MyPacket*)av_malloc(sizeof(MyPacket)); // 分配节点内存
    if(!pkt1) // 内存不足
        return -1;
    pkt1->pkt = *pkt; // 浅拷贝
    pkt1->next = NULL;
    /* 如果放入了flush pkt，说明需要增加播放序列号，以区分不连续的两段数据 */
    if(pkt == &flush_pkt)
    {
        q->serial++;
        std::cout << "q->serial = " << q->serial <<std::endl;
    }
    pkt1->serial = q->serial;
    /* 队列插入操作 */
    if(!q->last_pkt)
        q->first_pkt = pkt1;
    else
        q->last_pkt->next = pkt1;
    q->last_pkt = pkt1;

    // 队列属性更新
    q->nb_packets++;
    q->size += pkt1->pkt.size + sizeof(*pkt1);
    q->duration += pkt1->pkt.duration;

    SDL_CondSignal(q->cond); // 有数据发送信号
    return 0;
}

int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    int ret;
    SDL_LockMutex(q->mutex);
    ret = packet_queue_put_private(q, pkt);
    SDL_UnlockMutex(q->mutex);
    if(pkt != &flush_pkt && ret < 0) // 放入失败，释放AVPacket
        av_packet_unref(pkt);
    return ret;
}

int packet_queue_put_nullpacket(PacketQueue *q, int stream_index)
{
    AVPacket pkt1, *pkt = &pkt1;
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;
    pkt->stream_index = stream_index;
    return packet_queue_put(q, pkt);
}

int packet_queue_init(PacketQueue *q)
{
    memset(q, 0, sizeof(PacketQueue));
    q->mutex = SDL_CreateMutex();
    if(!q->mutex){
        av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
        return AVERROR(ENOMEM);
    }
    q->cond = SDL_CreateCond();
    if(!q->cond){
        av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
        return AVERROR(ENOMEM);
    }
    q->abort_request = 1;
    return 0;
}

void packet_queue_flush(PacketQueue *q)
{
    MyPacket *pkt, * pkt1;

    SDL_LockMutex(q->mutex);
    for(pkt = q->first_pkt ; pkt ; pkt = pkt1)
    {
        pkt1 = pkt->next;
        av_packet_unref(&pkt->pkt);
        av_freep(&pkt);
    }
    q->last_pkt = NULL;
    q->first_pkt = NULL;
    q->nb_packets = 0;
    q->size = 0;
    q->duration = 0;
    SDL_UnlockMutex(q->mutex);
}

void packet_queue_destroy(PacketQueue *q)
{
    packet_queue_flush(q);
    SDL_DestroyMutex(q->mutex);
    SDL_DestroyCond(q->cond);
}

void packet_queue_abort(PacketQueue *q)
{
    SDL_LockMutex(q->mutex);
    q->abort_request = 1;
    SDL_CondSignal(q->cond); // 之间释放一个信号
    SDL_UnlockMutex(q->mutex);
}

void packet_queue_start(PacketQueue *q)
{
    SDL_LockMutex(q->mutex);
    q->abort_request = 0;
    packet_queue_put_private(q, &flush_pkt);
    SDL_UnlockMutex(q->mutex);
}

int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block, int *serial)
{
    MyPacket *pkt1;
    int ret;

    SDL_LockMutex(q->mutex);
    while(1)
    {
        if(q->abort_request){
            ret = -1;
            break;
        }
        pkt1 = q->first_pkt;
        if(pkt1)
        {
            q->first_pkt = pkt1->next;
            if(!q->first_pkt)
                q->last_pkt = NULL;
            q->nb_packets--;
            q->size -= pkt1->pkt.size + sizeof(*pkt1);
            q->duration -= pkt1->pkt.duration;
            *pkt = pkt1->pkt;
            if(serial)
                *serial = pkt1->serial;
            av_free(pkt1); // ?
            ret = 1;
            break;
        }
        else if(!block) // 非阻塞调用，而且没有数据
        {
            ret = 0;
            break;
        }
        else // 阻塞调用，没有数据
        {
            SDL_CondWait(q->cond, q->mutex);
        }
    }
    SDL_UnlockMutex(q->mutex);
    return ret;
}
