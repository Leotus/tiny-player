#include "mainwind.h"
#include "ui_mainwind.h"

MainWind::MainWind(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWind)
{
    ui->setupUi(this);
    // 初始化信号槽相关
    InitSignalsAndSlots();
}

MainWind::~MainWind()
{
    delete ui;
}

int MainWind::InitSignalsAndSlots()
{
    connect(ui->ctrlBarWind, &CtrlBar::SigPlayOrPause, this, &MainWind::OnPlayOrPause);
    connect(ui->ctrlBarWind, &CtrlBar::SigStop, this, &MainWind::OnStop);
    return 0;
}

int MainWind::message_loop(void *arg)
{
    IjkMediaPlayer *mp = (IjkMediaPlayer*)arg;
    qDebug() << "MainWind::message_loop into";
    while(1)
    {
        AVMessage msg;
        // 阻塞式等待消息
        int retval = mp->ijkmp_get_msg(&msg, 1);
        if(retval < 0)
            break;
        // 得到消息后，ijkmp_get_msg内部选择将msg交给ui，ui仍然处理一次该消息
        switch(msg.what)
        {
        case FFP_MSG_FLUSH:
            qDebug() << __FUNCTION__ << " FFP_MSG_FLUSH";
            break;
        case FFP_MSG_PREPARED:
            qDebug() << __FUNCTION__ << " FFP_MSG_PREPARED";
            mp->ijkmp_start();
            break;
        case FFP_REQ_START:
            qDebug() << __FUNCTION__ << " FFP_REQ_START";
            break;
        default:
            qDebug() << __FUNCTION__ << " default";
            break;
        }
        msg_free_res(&msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    qDebug() << "MainWind::message_loop quit!";
    return 0;
}

void MainWind::OnPlayOrPause()
{
    qDebug() << "MainWind OnPlayOrPause called";
    int ret = 0;
    // 1. 先检查mp是否已经创建
    if(!mp_){
        mp_ = new IjkMediaPlayer();
        // 1.1
        ret = mp_->ijkmp_create(std::bind(&MainWind::message_loop, this, std::placeholders::_1));
        if(ret < 0)
        {
            qDebug() << "IjkMediaPlayer ijkmp_create failed!";
            delete mp_;
            mp_ = NULL;
            return;
        }
        mp_->add_video_refresh_callback(std::bind(&MainWind::OutputVideo, this, std::placeholders::_1));

        // 1.2
        mp_->ijkmp_set_data_source("N:\\Gamepad摄像\\Captures\\with judy.mp4");
        // 1.3
        ret = mp_->ijkmp_prepare_async();
        if(ret < 0)
        {
            qDebug() << "IjkMediaPlayer ijkmp_prepare_async failed!";
            delete mp_;
            mp_ = NULL;
            return;
        }
    }else{
        // 已经创建过了，则暂停或恢复播放
    }
}

void MainWind::OnStop()
{
    qDebug() << "MainWind OnStop called";
    if(mp_)
    {
        mp_->ijkmp_stop();
        mp_->ijkmp_destroy();
        delete mp_;
        mp_ = NULL;
    }
}

int MainWind::OutputVideo(const MyFrame *frame)
{
    return ui->showWind->Draw(frame);
}

