/********************************************************************************
** Form generated from reading UI file 'lv2GULaas.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef LV2GULAAS_H
#define LV2GULAAS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <widget/maptablewidget.h>

QT_BEGIN_NAMESPACE

class Ui_lv2
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *input;
    QPushButton *execute;
    QPushButton *save;
    QPushButton *config;
    MapTableWidget *tableWidget;
    MapTableWidget *tableWidget2;
    QProgressBar *progressBar;

    void setupUi(QWidget *lv2)
    {
        if (lv2->objectName().isEmpty())
            lv2->setObjectName(QStringLiteral("lv2"));
        lv2->resize(637, 426);
        gridLayout = new QGridLayout(lv2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(lv2);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        input = new QLineEdit(lv2);
        input->setObjectName(QStringLiteral("input"));

        gridLayout->addWidget(input, 0, 1, 1, 1);

        execute = new QPushButton(lv2);
        execute->setObjectName(QStringLiteral("execute"));

        gridLayout->addWidget(execute, 0, 2, 1, 1);

        save = new QPushButton(lv2);
        save->setObjectName(QStringLiteral("save"));

        gridLayout->addWidget(save, 0, 3, 1, 1);

        config = new QPushButton(lv2);
        config->setObjectName(QStringLiteral("config"));

        gridLayout->addWidget(config, 0, 4, 1, 1);

        tableWidget = new MapTableWidget(lv2);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));

        gridLayout->addWidget(tableWidget, 1, 0, 1, 5);

        tableWidget2 = new MapTableWidget(lv2);
        tableWidget2->setObjectName(QStringLiteral("tableWidget2"));

        gridLayout->addWidget(tableWidget2, 2, 0, 1, 5);

        progressBar = new QProgressBar(lv2);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(24);

        gridLayout->addWidget(progressBar, 3, 0, 1, 5);


        retranslateUi(lv2);

        QMetaObject::connectSlotsByName(lv2);
    } // setupUi

    void retranslateUi(QWidget *lv2)
    {
        lv2->setWindowTitle(QApplication::translate("lv2", "Form", nullptr));
        label->setText(QApplication::translate("lv2", "\350\202\241\347\245\250\346\210\226\346\227\245\346\234\237", nullptr));
        execute->setText(QApplication::translate("lv2", "\346\211\247\350\241\214", nullptr));
        save->setText(QApplication::translate("lv2", "\345\257\274\345\207\272", nullptr));
        config->setText(QApplication::translate("lv2", "\350\256\276\347\275\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class lv2: public Ui_lv2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // LV2GULAAS_H
