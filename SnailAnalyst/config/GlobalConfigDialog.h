#ifndef GLOBAL_CONFIG_WIDGET_H
#define GLOBAL_CONFIG_WIDGET_H
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <qtoolbox.h>
#include <QFontComboBox>

#pragma execution_character_set("utf-8")

class TextEditConfigWidget: public QWidget
{
    Q_OBJECT
public:
	TextEditConfigWidget(QWidget* parent = nullptr);
private slots:
    void onTextColor();
	void onTextBackgroundColor();
	void onOK();
private:
	QFontComboBox* fontBox;
	QSpinBox* fontSize;
	QPushButton* textColorButton;
	QLabel* textColor;
	QPushButton* textBackgroundColorButton;
	QLabel* textBackgroundColor;
	QPushButton* ok;
	QPushButton* cancel;
};

class GlobalConfigDialog : public QDialog
{
    Q_OBJECT
public:
	GlobalConfigDialog(QWidget* parent = nullptr);
private:
	QWidget* editor;
};

#endif