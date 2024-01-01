/********************************************************************************
** Form generated from reading UI file 'mainwind.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWIND_H
#define UI_MAINWIND_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <ctrlbar.h>
#include <displaywind.h>
#include <playlistwind.h>
#include <titlebar.h>

QT_BEGIN_NAMESPACE

class Ui_MainWind
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QWidget *showCtrlBarWidget;
    QGridLayout *gridLayout_2;
    DisplayWind *showWind;
    CtrlBar *ctrlBarWind;
    QStatusBar *statusbar;
    QDockWidget *playListDocWidget;
    PlayListWind *playListContent;
    QDockWidget *titleDockWidget;
    TitleBar *titleContents;

    void setupUi(QMainWindow *MainWind)
    {
        if (MainWind->objectName().isEmpty())
            MainWind->setObjectName(QString::fromUtf8("MainWind"));
        MainWind->resize(1280, 800);
        centralwidget = new QWidget(MainWind);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        showCtrlBarWidget = new QWidget(centralwidget);
        showCtrlBarWidget->setObjectName(QString::fromUtf8("showCtrlBarWidget"));
        gridLayout_2 = new QGridLayout(showCtrlBarWidget);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        showWind = new DisplayWind(showCtrlBarWidget);
        showWind->setObjectName(QString::fromUtf8("showWind"));

        gridLayout_2->addWidget(showWind, 0, 0, 1, 1);

        ctrlBarWind = new CtrlBar(showCtrlBarWidget);
        ctrlBarWind->setObjectName(QString::fromUtf8("ctrlBarWind"));
        ctrlBarWind->setMinimumSize(QSize(300, 60));

        gridLayout_2->addWidget(ctrlBarWind, 1, 0, 1, 1);


        gridLayout->addWidget(showCtrlBarWidget, 0, 0, 1, 1);

        MainWind->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWind);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWind->setStatusBar(statusbar);
        playListDocWidget = new QDockWidget(MainWind);
        playListDocWidget->setObjectName(QString::fromUtf8("playListDocWidget"));
        playListDocWidget->setMinimumSize(QSize(100, 121));
        playListContent = new PlayListWind();
        playListContent->setObjectName(QString::fromUtf8("playListContent"));
        playListContent->setMinimumSize(QSize(100, 100));
        playListDocWidget->setWidget(playListContent);
        MainWind->addDockWidget(Qt::RightDockWidgetArea, playListDocWidget);
        titleDockWidget = new QDockWidget(MainWind);
        titleDockWidget->setObjectName(QString::fromUtf8("titleDockWidget"));
        titleDockWidget->setMinimumSize(QSize(200, 41));
        titleContents = new TitleBar();
        titleContents->setObjectName(QString::fromUtf8("titleContents"));
        titleDockWidget->setWidget(titleContents);
        MainWind->addDockWidget(Qt::TopDockWidgetArea, titleDockWidget);

        retranslateUi(MainWind);

        QMetaObject::connectSlotsByName(MainWind);
    } // setupUi

    void retranslateUi(QMainWindow *MainWind)
    {
        MainWind->setWindowTitle(QCoreApplication::translate("MainWind", "MainWind", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWind: public Ui_MainWind {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWIND_H
