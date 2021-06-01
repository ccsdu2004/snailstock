/********************************************************************************
** Form generated from reading UI file 'lv2configlRPXSQ.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef LV2CONFIGLRPXSQ_H
#define LV2CONFIGLRPXSQ_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *spinBox1;
    QLabel *label_2;
    QSpinBox *spinBox2;
    QLabel *label_3;
    QSpinBox *spinBox3;
    QLabel *label_4;
    QSpinBox *spinBox4;
    QLabel *label_6;
    QLineEdit *filePath;
    QCheckBox *checkBox;
    QCheckBox *selectStock;
    QLabel *label_5;
    QPushButton *save;
    QSpinBox *spinBox;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(239, 260);
        gridLayout_2 = new QGridLayout(Form);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        verticalSpacer = new QSpacerItem(20, 134, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 4, 2, 1, 1);

        groupBox = new QGroupBox(Form);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        spinBox1 = new QSpinBox(groupBox);
        spinBox1->setObjectName(QStringLiteral("spinBox1"));
        spinBox1->setMinimum(4);
        spinBox1->setMaximum(10);
        spinBox1->setValue(8);

        gridLayout->addWidget(spinBox1, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        spinBox2 = new QSpinBox(groupBox);
        spinBox2->setObjectName(QStringLiteral("spinBox2"));
        spinBox2->setMinimum(20);
        spinBox2->setMaximum(50);
        spinBox2->setValue(40);

        gridLayout->addWidget(spinBox2, 1, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        spinBox3 = new QSpinBox(groupBox);
        spinBox3->setObjectName(QStringLiteral("spinBox3"));
        spinBox3->setMinimum(80);
        spinBox3->setMaximum(120);

        gridLayout->addWidget(spinBox3, 2, 1, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        spinBox4 = new QSpinBox(groupBox);
        spinBox4->setObjectName(QStringLiteral("spinBox4"));
        spinBox4->setMinimum(200);
        spinBox4->setMaximum(500);
        spinBox4->setValue(360);

        gridLayout->addWidget(spinBox4, 3, 1, 1, 1);


        gridLayout_2->addWidget(groupBox, 1, 0, 5, 2);

        label_6 = new QLabel(Form);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_2->addWidget(label_6, 0, 0, 1, 1);

        filePath = new QLineEdit(Form);
        filePath->setObjectName(QStringLiteral("filePath"));

        gridLayout_2->addWidget(filePath, 0, 1, 1, 2);

        checkBox = new QCheckBox(Form);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        gridLayout_2->addWidget(checkBox, 2, 2, 1, 1);

        selectStock = new QCheckBox(Form);
        selectStock->setObjectName(QStringLiteral("selectStock"));

        gridLayout_2->addWidget(selectStock, 3, 2, 1, 1);

        label_5 = new QLabel(Form);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 6, 0, 1, 1);

        save = new QPushButton(Form);
        save->setObjectName(QStringLiteral("save"));

        gridLayout_2->addWidget(save, 6, 2, 1, 1);

        spinBox = new QSpinBox(Form);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setMinimum(3);
        spinBox->setMaximum(4);

        gridLayout_2->addWidget(spinBox, 6, 1, 1, 1);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", nullptr));
        groupBox->setTitle(QApplication::translate("Form", "\350\265\204\351\207\221\345\217\202\346\225\260", nullptr));
        label->setText(QApplication::translate("Form", "\346\225\243\345\215\225\344\270\212\351\231\220", nullptr));
        label_2->setText(QApplication::translate("Form", "\345\260\217\345\215\225\344\270\212\351\231\220", nullptr));
        label_3->setText(QApplication::translate("Form", "\344\270\255\345\215\225\344\270\212\351\231\220", nullptr));
        label_4->setText(QApplication::translate("Form", "\345\244\247\345\215\225\344\270\212\351\231\220", nullptr));
        label_6->setText(QApplication::translate("Form", "\346\225\260\346\215\256\345\214\205\350\267\257\345\276\204", nullptr));
        checkBox->setText(QApplication::translate("Form", "\344\275\277\347\224\250\350\204\232\346\234\254\351\205\215\350\211\262", nullptr));
        selectStock->setText(QApplication::translate("Form", "\351\200\211\350\202\241\346\250\241\345\274\217", nullptr));
        label_5->setText(QApplication::translate("Form", "\344\270\252\346\225\260", nullptr));
        save->setText(QApplication::translate("Form", "\344\277\235\345\255\230", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // LV2CONFIGLRPXSQ_H
