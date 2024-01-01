/********************************************************************************
** Form generated from reading UI file 'playlistwind.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYLISTWIND_H
#define UI_PLAYLISTWIND_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlayListWind
{
public:
    QListWidget *listWidget;

    void setupUi(QWidget *PlayListWind)
    {
        if (PlayListWind->objectName().isEmpty())
            PlayListWind->setObjectName(QString::fromUtf8("PlayListWind"));
        PlayListWind->resize(254, 581);
        listWidget = new QListWidget(PlayListWind);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(0, 0, 251, 571));

        retranslateUi(PlayListWind);

        QMetaObject::connectSlotsByName(PlayListWind);
    } // setupUi

    void retranslateUi(QWidget *PlayListWind)
    {
        PlayListWind->setWindowTitle(QCoreApplication::translate("PlayListWind", "Form", nullptr));

        const bool __sortingEnabled = listWidget->isSortingEnabled();
        listWidget->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listWidget->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("PlayListWind", "\351\201\207\350\247\201-\345\255\231\347\207\225\345\247\277", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = listWidget->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("PlayListWind", "\345\211\221\351\255\202-\346\261\252\350\213\217\346\263\267", nullptr));
        listWidget->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class PlayListWind: public Ui_PlayListWind {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYLISTWIND_H
