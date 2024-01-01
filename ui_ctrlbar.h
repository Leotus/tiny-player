/********************************************************************************
** Form generated from reading UI file 'ctrlbar.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CTRLBAR_H
#define UI_CTRLBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CtrlBar
{
public:
    QGridLayout *gridLayout_2;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSlider *playSlider;
    QWidget *widget_2;
    QPushButton *volumeBtn;
    QSlider *volumeSlider;
    QGridLayout *gridLayout;
    QPushButton *stopBtn;
    QTimeEdit *totalTimeEdit;
    QTimeEdit *playtimeEdit;
    QPushButton *playListBtn;
    QPushButton *backwardBtn;
    QLabel *label;
    QPushButton *forwardBtn;
    QPushButton *speedBtn;
    QPushButton *settingBtn;
    QPushButton *playOrPauseBtn;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *CtrlBar)
    {
        if (CtrlBar->objectName().isEmpty())
            CtrlBar->setObjectName(QString::fromUtf8("CtrlBar"));
        CtrlBar->resize(600, 60);
        CtrlBar->setMaximumSize(QSize(16777215, 60));
        gridLayout_2 = new QGridLayout(CtrlBar);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(CtrlBar);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMaximumSize(QSize(16777215, 25));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        playSlider = new QSlider(widget);
        playSlider->setObjectName(QString::fromUtf8("playSlider"));
        playSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(playSlider);

        widget_2 = new QWidget(widget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setMinimumSize(QSize(108, 25));
        volumeBtn = new QPushButton(widget_2);
        volumeBtn->setObjectName(QString::fromUtf8("volumeBtn"));
        volumeBtn->setGeometry(QRect(2, 2, 20, 20));
        volumeBtn->setMaximumSize(QSize(20, 16777215));
        volumeSlider = new QSlider(widget_2);
        volumeSlider->setObjectName(QString::fromUtf8("volumeSlider"));
        volumeSlider->setGeometry(QRect(24, 0, 80, 25));
        volumeSlider->setMinimumSize(QSize(80, 25));
        volumeSlider->setMaximumSize(QSize(80, 25));
        volumeSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(widget_2);


        gridLayout_2->addWidget(widget, 0, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        stopBtn = new QPushButton(CtrlBar);
        stopBtn->setObjectName(QString::fromUtf8("stopBtn"));
        stopBtn->setMinimumSize(QSize(30, 30));
        stopBtn->setMaximumSize(QSize(30, 30));

        gridLayout->addWidget(stopBtn, 0, 1, 1, 1);

        totalTimeEdit = new QTimeEdit(CtrlBar);
        totalTimeEdit->setObjectName(QString::fromUtf8("totalTimeEdit"));
        totalTimeEdit->setMinimumSize(QSize(70, 0));
        totalTimeEdit->setMaximumSize(QSize(70, 16777215));
        totalTimeEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(totalTimeEdit, 0, 7, 1, 1);

        playtimeEdit = new QTimeEdit(CtrlBar);
        playtimeEdit->setObjectName(QString::fromUtf8("playtimeEdit"));
        playtimeEdit->setMinimumSize(QSize(70, 0));
        playtimeEdit->setMaximumSize(QSize(70, 16777215));
        playtimeEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);

        gridLayout->addWidget(playtimeEdit, 0, 5, 1, 1);

        playListBtn = new QPushButton(CtrlBar);
        playListBtn->setObjectName(QString::fromUtf8("playListBtn"));
        playListBtn->setMinimumSize(QSize(30, 30));
        playListBtn->setMaximumSize(QSize(30, 30));

        gridLayout->addWidget(playListBtn, 0, 9, 1, 1);

        backwardBtn = new QPushButton(CtrlBar);
        backwardBtn->setObjectName(QString::fromUtf8("backwardBtn"));
        backwardBtn->setMinimumSize(QSize(30, 30));
        backwardBtn->setMaximumSize(QSize(30, 30));

        gridLayout->addWidget(backwardBtn, 0, 2, 1, 1);

        label = new QLabel(CtrlBar);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMaximumSize(QSize(20, 16777215));

        gridLayout->addWidget(label, 0, 6, 1, 1);

        forwardBtn = new QPushButton(CtrlBar);
        forwardBtn->setObjectName(QString::fromUtf8("forwardBtn"));
        forwardBtn->setMinimumSize(QSize(30, 30));
        forwardBtn->setMaximumSize(QSize(30, 30));

        gridLayout->addWidget(forwardBtn, 0, 3, 1, 1);

        speedBtn = new QPushButton(CtrlBar);
        speedBtn->setObjectName(QString::fromUtf8("speedBtn"));
        speedBtn->setMinimumSize(QSize(100, 30));
        speedBtn->setMaximumSize(QSize(100, 30));

        gridLayout->addWidget(speedBtn, 0, 4, 1, 1);

        settingBtn = new QPushButton(CtrlBar);
        settingBtn->setObjectName(QString::fromUtf8("settingBtn"));
        settingBtn->setMinimumSize(QSize(30, 30));
        settingBtn->setMaximumSize(QSize(30, 30));

        gridLayout->addWidget(settingBtn, 0, 10, 1, 1);

        playOrPauseBtn = new QPushButton(CtrlBar);
        playOrPauseBtn->setObjectName(QString::fromUtf8("playOrPauseBtn"));
        playOrPauseBtn->setMinimumSize(QSize(30, 30));
        playOrPauseBtn->setMaximumSize(QSize(30, 30));

        gridLayout->addWidget(playOrPauseBtn, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 8, 1, 1);


        gridLayout_2->addLayout(gridLayout, 1, 0, 1, 1);


        retranslateUi(CtrlBar);

        QMetaObject::connectSlotsByName(CtrlBar);
    } // setupUi

    void retranslateUi(QWidget *CtrlBar)
    {
        CtrlBar->setWindowTitle(QCoreApplication::translate("CtrlBar", "Form", nullptr));
        volumeBtn->setText(QString());
        stopBtn->setText(QString());
        totalTimeEdit->setDisplayFormat(QCoreApplication::translate("CtrlBar", "HH:mm:ss", nullptr));
        playtimeEdit->setDisplayFormat(QCoreApplication::translate("CtrlBar", "HH:mm:ss", nullptr));
        playListBtn->setText(QString());
        backwardBtn->setText(QString());
        label->setText(QCoreApplication::translate("CtrlBar", "/", nullptr));
        forwardBtn->setText(QString());
        speedBtn->setText(QCoreApplication::translate("CtrlBar", "\345\200\215\351\200\2371.0", nullptr));
        settingBtn->setText(QString());
        playOrPauseBtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CtrlBar: public Ui_CtrlBar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CTRLBAR_H
