QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clock.cpp \
    ctrlbar.cpp \
    decoder.cpp \
    displaywind.cpp \
    ffmsg_queue.cpp \
    ffplayer.cpp \
    ffplayer_frame.cpp \
    ffplayer_packet.cpp \
    ijkmediaplayer.cpp \
    main.cpp \
    mainwind.cpp \
    playlistwind.cpp \
    titlebar.cpp

HEADERS += \
    clock.h \
    ctrlbar.h \
    decoder.h \
    displaywind.h \
    ffmsg.h \
    ffmsg_queue.h \
    ffplayer.h \
    ijkmediaplayer.h \
    imagescaler.h \
    mainwind.h \
    playlistwind.h \
    titlebar.h \
    ffplayer_basic.h

FORMS += \
    ctrlbar.ui \
    displaywind.ui \
    mainwind.ui \
    playlistwind.ui \
    titlebar.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc

win32{
    INCLUDEPATH += $$PWD/ffmpeg-4.2.1-win32-dev/include
    INCLUDEPATH += $$PWD/SDL2/include

    LIBS += $$PWD/ffmpeg-4.2.1-win32-dev/lib/avformat.lib \
           $$PWD/ffmpeg-4.2.1-win32-dev/lib/avcodec.lib \
           $$PWD/ffmpeg-4.2.1-win32-dev/lib/avdevice.lib \
           $$PWD/ffmpeg-4.2.1-win32-dev/lib/avfilter.lib \
           $$PWD/ffmpeg-4.2.1-win32-dev/lib/avutil.lib \
           $$PWD/ffmpeg-4.2.1-win32-dev/lib/postproc.lib \
           $$PWD/ffmpeg-4.2.1-win32-dev/lib/swresample.lib \
           $$PWD/ffmpeg-4.2.1-win32-dev/lib/swscale.lib \
           $$PWD/SDL2/lib/x86/SDL2.lib \
}
