 #ifndef MAINWIND_H
#define MAINWIND_H

#include <QMainWindow>
#include <QDebug>
#include "ijkmediaplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWind; }
QT_END_NAMESPACE

class MainWind : public QMainWindow
{
    Q_OBJECT

public:
    MainWind(QWidget *parent = nullptr);
    ~MainWind();
    // 信号槽绑定
    int InitSignalsAndSlots();
    // 消息循环主体
    int message_loop(void* arg);
    // 信号槽接收
    void OnPlayOrPause();
    void OnStop();
    int OutputVideo(const MyFrame* frame);
private:
    Ui::MainWind *ui;
    IjkMediaPlayer *mp_ = NULL;
};
#endif // MAINWIND_H
