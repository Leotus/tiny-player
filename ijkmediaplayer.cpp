#include "ijkmediaplayer.h"

IjkMediaPlayer::IjkMediaPlayer()
{
    std::cout << "IjkMediaPlayer construct!"<<std::endl;
}

IjkMediaPlayer::~IjkMediaPlayer()
{
    std::cout << "~IjkMediaPlayer() "<<std::endl;
}

int IjkMediaPlayer::ijkmp_create(std::function<int (void *)> msg_loop)
{
    int ret = 0;
    ffplayer_ = new FFPlayer();
    if(!ffplayer_)
    {
        std::cout << " new FFPlayer() failed" << std::endl;
        return -1;
    }
    msg_loop_ = msg_loop;
    // c++ mutex已经初始化了，无需再初始化
    ret = ffplayer_->ffp_create();
    if(ret < 0)
        return -1;
    return 0;
}

int IjkMediaPlayer::ijkmp_destroy()
{
    ffplayer_->ffp_destroy();
    return 0;
}

int IjkMediaPlayer::ijkmp_set_data_source(const char *url)
{
    if(!url)
        return -1;
    data_source_ = strdup(url); // strdup即分配内存也拷贝字符串
    return 0;
}

int IjkMediaPlayer::ijkmp_prepare_async()
{
    // 判断mp状态
    // 正在准备中
    mp_state_ = MP_STATE_ASYNC_PREPARING;
    // 启用消息队列
    msg_queue_start(&ffplayer_->msg_queue_);
    // 创建循环线程
    msg_thread_ = new std::thread(&IjkMediaPlayer::ijkmp_msg_loop, this, this);
    // 调用ffplayer
    int ret = ffplayer_->ffp_prepare_async_l(data_source_);
    if(ret < 0){
        mp_state_ = MP_STATE_ERROR;
        return -1;
    }
    return 0;
}

int IjkMediaPlayer::ijkmp_get_msg(AVMessage *msg, int block)
{
    while(1)
    {
        int continue_wait_next_msg = 0;
        int retval = msg_queue_get(&ffplayer_->msg_queue_, msg, block);
        if(retval<=0)
            return retval; // -1 abort, 0 在非阻塞时没有消息
        switch(msg->what)
        {
        case FFP_MSG_FLUSH:
            std::cout << __FUNCTION__ <<" FFP_MSG_FLUSH"<<std::endl;
            break;
        case FFP_MSG_PREPARED:
            std::cout << __FUNCTION__ <<" FFP_MSG_PREPARED"<<std::endl;
            break;
        case FFP_REQ_START:
            std::cout << __FUNCTION__ <<" FFP_REQ_START"<<std::endl;
            continue_wait_next_msg = 1;
            break;
        default:
            std::cout << __FUNCTION__ <<" default"<<std::endl;
            break;
        }
        if(continue_wait_next_msg){
            msg_free_res(msg);
            continue; // 当不希望MainWind处理该消息时，这里直接continue，处理下一个消息
        }
        return retval;
    }
    return -1;
}

int IjkMediaPlayer::ijkmp_stop()
{
    int retval = ffplayer_->ffp_stop_l();
    return retval;
}

int IjkMediaPlayer::ijkmp_msg_loop(void *arg)
{
    msg_loop_(arg);
    return 0;
}

void IjkMediaPlayer::add_video_refresh_callback(std::function<int (const MyFrame *)> callback)
{
    ffplayer_->add_video_refresh_callback(callback);
}

int IjkMediaPlayer::ijkmp_start()
{
    ffp_notify_msg1(ffplayer_, FFP_REQ_START);
    return 0;
}
