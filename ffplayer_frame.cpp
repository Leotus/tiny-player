#include "ffplayer_basic.h"

static void frame_queue_unref_item(MyFrame *vp)
{
    av_frame_unref(vp->frame); // 释放对vp->frame中数据缓冲区的引用，注意，不是释放frame对象本身
}

int frame_queue_init(FrameQueue *f, PacketQueue *pktq, int max_size)
{
    memset(f, 0, sizeof(FrameQueue));
    f->mutex = SDL_CreateMutex();
    if(!f->mutex){
        av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
        return AVERROR(ENOMEM);
    }
    f->cond = SDL_CreateCond();
    if(!f->cond){
        av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
        return AVERROR(ENOMEM);
    }
    f->pktq = pktq;
    f->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
    for(int i = 0;i < f->max_size;i++)
        if(!(f->queue[i].frame = av_frame_alloc()))
            return AVERROR(ENOMEM);
    return 0;
}

void frame_queue_destroy(FrameQueue *f)
{
    for(int i = 0 ; i < f->max_size ; i++)
    {
        MyFrame *vp = &f->queue[i];
        frame_queue_unref_item(vp);
        av_frame_free(&vp->frame);
    }
    SDL_DestroyMutex(f->mutex);
    SDL_DestroyCond(f->cond);
}

void frame_queue_signal(FrameQueue *f)
{
    SDL_LockMutex(f->mutex);
    SDL_CondSignal(f->cond);
    SDL_UnlockMutex(f->mutex);
}

MyFrame* frame_queue_peek(FrameQueue *f)
{
    return &f->queue[(f->rindex) % f->max_size];
}

MyFrame* frame_queue_peek_next(FrameQueue *f)
{
    return &f->queue[(f->rindex + 1) % f->max_size];
}

MyFrame* frame_queue_peek_last(FrameQueue *f)
{
    return &f->queue[(f->rindex)];
}

MyFrame *frame_queue_peek_writable(FrameQueue *f)
{
    SDL_LockMutex(f->mutex);
    while(f->size >= f->max_size && !f->pktq->abort_request)
        SDL_CondWait(f->cond, f->mutex);
    SDL_UnlockMutex(f->mutex);

    if(f->pktq->abort_request)
        return NULL;

    return &f->queue[f->windex];
}

MyFrame *frame_queue_peek_readable(FrameQueue *f)
{
    SDL_LockMutex(f->mutex);
    while(f->size <= 0 && !f->pktq->abort_request)
        SDL_CondWait(f->cond, f->mutex);
    SDL_UnlockMutex(f->mutex);

    if(f->pktq->abort_request)
        return NULL;

    return &f->queue[(f->rindex) % f->max_size];
}

void frame_queue_push(FrameQueue *f)
{
    if(++f->windex == f->max_size)
        f->windex = 0;
    SDL_LockMutex(f->mutex);
    f->size++;
    SDL_CondSignal(f->cond);
    SDL_UnlockMutex(f->mutex);
}

void frame_queue_next(FrameQueue *f)
{
    frame_queue_unref_item(&f->queue[f->rindex]);
    if(++f->rindex == f->max_size)
        f->rindex = 0;
    SDL_LockMutex(f->mutex);
    f->size--;
    SDL_CondSignal(f->cond);
    SDL_UnlockMutex(f->mutex);
}

int frame_queue_nb_remaining(FrameQueue *f)
{
    return f->size;
}

int64_t frame_queue_last_pos(FrameQueue *f)
{
    return -1;
}
