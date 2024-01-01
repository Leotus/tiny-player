#include "displaywind.h"
#include "ui_displaywind.h"
#include <QPainter>
#include <fstream>

DisplayWind::DisplayWind(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayWind)
{
    ui->setupUi(this);
}

DisplayWind::~DisplayWind()
{
    delete ui;
    if(dst_video_frame_.data[0])
        free(dst_video_frame_.data[0]);
    if(img_scaler_)
    {
        delete img_scaler_;
        img_scaler_ = NULL;
    }
}

int DisplayWind::Draw(const MyFrame *frame)
{
    QMutexLocker locker(&m_mutex_);

    if(!img_scaler_){
        int win_width = width();
        int win_height = height();
        video_width_ = frame->width;
        video_height_ = frame->height;
        img_scaler_ = new ImageScaler();
        double video_aspect_ratio = frame->width *1.0 / frame->height;
        double win_aspect_ratio = win_width*1.0 / win_height;
        if(win_aspect_ratio > video_aspect_ratio){
            img_height_ = win_height;
            if(img_height_ % 2 != 0)
                img_height_ -= 1;
            img_width_ = img_height_ * video_aspect_ratio;
            if(img_width_ % 2 != 0)
                img_width_ -= 1;
            y_ = 0;
            x_ = (win_width - img_width_) / 2;
        }
        else{
            img_width_ = win_width;
            if(img_width_ %2 != 0) {
                img_width_ -= 1;
            }
            img_height_ = img_width_ / video_aspect_ratio;
            if(img_height_ %2 != 0) {
                img_height_ -= 1;
            }
            x_ = 0;
            y_ = (win_height - img_height_) / 2;
        }
        img_scaler_->Init(video_width_, video_height_, frame->format, img_width_, img_height_, AV_PIX_FMT_RGB24);
        memset(&dst_video_frame_, 0 , sizeof(VideoFrame));
        dst_video_frame_.width = img_width_;
        dst_video_frame_.height = img_height_;
        dst_video_frame_.format = AV_PIX_FMT_RGB24;
        if (dst_video_frame_.data[0])
        {
            free(dst_video_frame_.data[0]);
        }
        dst_video_frame_.data[0] = (uint8_t*)malloc(img_width_ * img_height_ * 3);
        dst_video_frame_.linesize[0] = img_width_ * 3;
    }
    RET_CODE code =  img_scaler_->Scale3(frame, &dst_video_frame_);

    QImage imageTmp = QImage((uint8_t*)dst_video_frame_.data[0],
        img_width_, img_height_, img_width_ * 3, QImage::Format_RGB888); // 这里需要手动加bytes_per_line，不然会因为内存补齐错位

    img_ = imageTmp.copy();

    update();
    return 0;
}

void DisplayWind::paintEvent(QPaintEvent *)
{
    QMutexLocker locker(&m_mutex_);
    if(img_.isNull())
        return;
    QPainter painter(this);

    QRect rect(x_,y_,img_.width(),img_.height());

    painter.drawImage(rect, img_);
}

void DisplayWind::resizeEvent(QResizeEvent *event)
{

}
