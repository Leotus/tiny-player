#ifndef DISPLAYWIND_H
#define DISPLAYWIND_H
#include <QMutex>
#include <QWidget>
#include "ffplayer_basic.h"
#include "imagescaler.h"

namespace Ui {
class DisplayWind;
}

class DisplayWind : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayWind(QWidget *parent = nullptr);
    ~DisplayWind();
    int Draw(const MyFrame* frame);
protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::DisplayWind *ui;

    int m_last_frame_width_;
    int m_last_frame_height_;
    bool is_display_size_change_ = false;

    int x_ = 0;
    int y_ = 0;
    int video_width_ = 0;
    int video_height_ = 0;
    int img_width_ = 0;
    int img_height_ = 0;
    QImage img_;
    VideoFrame dst_video_frame_;
    QMutex m_mutex_;
    ImageScaler *img_scaler_ = NULL;
};

#endif // DISPLAYWIND_H
