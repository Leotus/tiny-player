/********************************************************************************
** Form generated from reading UI file 'titlebar.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TITLEBAR_H
#define UI_TITLEBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TitleBar
{
public:
    QHBoxLayout *horizontalLayout;
    QPushButton *menuBtn;
    QLabel *movieNameLabel;
    QSpacerItem *horizontalSpacer;
    QPushButton *minBtn;
    QPushButton *maxBtn;
    QPushButton *fullScreenBtn;
    QPushButton *closeBtn;

    void setupUi(QWidget *TitleBar)
    {
        if (TitleBar->objectName().isEmpty())
            TitleBar->setObjectName(QString::fromUtf8("TitleBar"));
        TitleBar->resize(826, 50);
        TitleBar->setMinimumSize(QSize(0, 50));
        TitleBar->setMaximumSize(QSize(16777215, 50));
        horizontalLayout = new QHBoxLayout(TitleBar);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        menuBtn = new QPushButton(TitleBar);
        menuBtn->setObjectName(QString::fromUtf8("menuBtn"));
        menuBtn->setMinimumSize(QSize(160, 50));
        menuBtn->setMaximumSize(QSize(160, 16777215));

        horizontalLayout->addWidget(menuBtn);

        movieNameLabel = new QLabel(TitleBar);
        movieNameLabel->setObjectName(QString::fromUtf8("movieNameLabel"));
        movieNameLabel->setMinimumSize(QSize(426, 60));

        horizontalLayout->addWidget(movieNameLabel);

        horizontalSpacer = new QSpacerItem(37, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        minBtn = new QPushButton(TitleBar);
        minBtn->setObjectName(QString::fromUtf8("minBtn"));
        minBtn->setMinimumSize(QSize(50, 50));
        minBtn->setMaximumSize(QSize(50, 50));

        horizontalLayout->addWidget(minBtn);

        maxBtn = new QPushButton(TitleBar);
        maxBtn->setObjectName(QString::fromUtf8("maxBtn"));
        maxBtn->setMinimumSize(QSize(50, 50));
        maxBtn->setMaximumSize(QSize(50, 50));

        horizontalLayout->addWidget(maxBtn);

        fullScreenBtn = new QPushButton(TitleBar);
        fullScreenBtn->setObjectName(QString::fromUtf8("fullScreenBtn"));
        fullScreenBtn->setMinimumSize(QSize(50, 50));
        fullScreenBtn->setMaximumSize(QSize(50, 50));

        horizontalLayout->addWidget(fullScreenBtn);

        closeBtn = new QPushButton(TitleBar);
        closeBtn->setObjectName(QString::fromUtf8("closeBtn"));
        closeBtn->setMinimumSize(QSize(50, 50));
        closeBtn->setMaximumSize(QSize(50, 50));

        horizontalLayout->addWidget(closeBtn);


        retranslateUi(TitleBar);

        QMetaObject::connectSlotsByName(TitleBar);
    } // setupUi

    void retranslateUi(QWidget *TitleBar)
    {
        TitleBar->setWindowTitle(QCoreApplication::translate("TitleBar", "Form", nullptr));
        menuBtn->setText(QCoreApplication::translate("TitleBar", "PushButton", nullptr));
        movieNameLabel->setText(QCoreApplication::translate("TitleBar", "TextLabel", nullptr));
        minBtn->setText(QCoreApplication::translate("TitleBar", "PushButton", nullptr));
        maxBtn->setText(QCoreApplication::translate("TitleBar", "PushButton", nullptr));
        fullScreenBtn->setText(QCoreApplication::translate("TitleBar", "PushButton", nullptr));
        closeBtn->setText(QCoreApplication::translate("TitleBar", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TitleBar: public Ui_TitleBar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TITLEBAR_H
