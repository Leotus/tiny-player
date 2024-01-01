/********************************************************************************
** Form generated from reading UI file 'displaywind.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAYWIND_H
#define UI_DISPLAYWIND_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DisplayWind
{
public:
    QGridLayout *gridLayout;
    QLabel *label;

    void setupUi(QWidget *DisplayWind)
    {
        if (DisplayWind->objectName().isEmpty())
            DisplayWind->setObjectName(QString::fromUtf8("DisplayWind"));
        DisplayWind->resize(798, 509);
        gridLayout = new QGridLayout(DisplayWind);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(DisplayWind);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);


        retranslateUi(DisplayWind);

        QMetaObject::connectSlotsByName(DisplayWind);
    } // setupUi

    void retranslateUi(QWidget *DisplayWind)
    {
        DisplayWind->setWindowTitle(QCoreApplication::translate("DisplayWind", "Form", nullptr));
        label->setText(QCoreApplication::translate("DisplayWind", "\346\230\276\347\244\272\350\247\206\351\242\221\347\224\273\351\235\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DisplayWind: public Ui_DisplayWind {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAYWIND_H
