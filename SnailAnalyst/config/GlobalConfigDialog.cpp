#include <QGridLayout>
#include <QColorDialog>
#include <qtextedit.h>
#include <qsettings.h>
#include <qfile.h>
#include <qtextedit.h>
#include <qcompleter.h>
#include <qstringlistmodel.h>
#include <QtUiTools/quiloader.h>
#include "ConfigurationParser.h"
#include "style/WidgetStyle.h"
#include "GlobalConfigDialog.h"
#include "common/FileUtils.h"
#include "common/HighLighter.h"
#include "StockScriptEngine.h"
#include "widget/AutoTextEdit.h"
#include "SnailContext.h"

#define TEXT_CONFIG_FILE "config/edit.ini"
#define COLOR_TO_INT(color) \
	(int)(((unsigned int)color.blue()<< 16) | (unsigned short)(((unsigned short)color.green()<< 8) | color.red()))  

TextEditConfigWidget::TextEditConfigWidget(QWidget* parent):
QWidget(parent)
{
	QSettings config(TEXT_CONFIG_FILE,QSettings::IniFormat);
	int textcolor = config.value("edit/textcolor",0).toInt();
	int bgcolor = config.value("edit/bgcolor",0).toInt();
	int fontsize = config.value("edit/fontsize",8).toInt();
	int fontIndex = config.value("edit/font",0).toInt();
	QString font = config.value("edit/fontname",QString()).toString();

	setWindowTitle("文本编辑配置");
    QGridLayout* grid = new QGridLayout(this); 

    QLabel* label = new QLabel(this);
	label->setText("字体大小");
	grid->addWidget(label,0,2,1,1);

    fontSize = new QSpinBox(this);
	fontSize->setRange(6,18);
	fontSize->setValue(fontsize);

    grid->addWidget(fontSize,0,3,1,1);

    textColorButton = new QPushButton(this);
	textColorButton->setText("文本颜色");
    grid->addWidget(textColorButton,1,0,1,2);
	connect(textColorButton,SIGNAL(clicked()),this,SLOT(onTextColor()));

    textColor = new QLabel(this);
    //textColor->setStyleSheet("background-color: rgb(255, 255, 255);");

    grid->addWidget(textColor,1,2,1,2);

    textBackgroundColorButton = new QPushButton(this);
	textBackgroundColorButton->setText("背景颜色");
	connect(textBackgroundColorButton,SIGNAL(clicked()),this,SLOT(onTextBackgroundColor()));
    grid->addWidget(textBackgroundColorButton,2,0,1,2);

    textBackgroundColor = new QLabel(this);
    //textBackgroundColor->setStyleSheet("background-color: rgb(0,0,0);");

    grid->addWidget(textBackgroundColor,2,2,1,2);

    ok = new QPushButton(this);
	ok->setText("确定");
    grid->addWidget(ok,3,0,1,2);
	connect(ok,SIGNAL(clicked()),this,SLOT(onOK()));

    cancel = new QPushButton(this);
	cancel->setText("取消");
    grid->addWidget(cancel,3,2,1,2);
	connect(cancel,SIGNAL(clicked()),this,SLOT(close()));

    fontBox = new QFontComboBox(this);
	fontBox->setCurrentIndex(fontIndex);
    grid->addWidget(fontBox,0,0,1,2);

    textColor->setAutoFillBackground(true);
    textBackgroundColor->setAutoFillBackground(true);
	
	textColor->setPalette(IntToQColor(textcolor));
	textBackgroundColor->setPalette(IntToQColor(bgcolor));
}

void TextEditConfigWidget::onTextColor()
{
	auto color = QColorDialog::getColor(Qt::black,this,"选择文本颜色",QColorDialog::DontUseNativeDialog);

    if(color.isValid()) 
	{
        textColor->setPalette(QPalette(color));
    }
}

void TextEditConfigWidget::onTextBackgroundColor()
{
	auto color = QColorDialog::getColor(Qt::white,this,"选择背景颜色",QColorDialog::DontUseNativeDialog);

    if(color.isValid()) 
	{
        textBackgroundColor->setPalette(QPalette(color));
    }
}

void TextEditConfigWidget::onOK()
{
	QSettings config(TEXT_CONFIG_FILE,QSettings::IniFormat);
	config.setValue("edit/textcolor",COLOR_TO_INT(textColor->palette().background().color()));
	config.setValue("edit/bgcolor",COLOR_TO_INT(textBackgroundColor->palette().background().color()));
	config.setValue("edit/fontsize",fontSize->value());
	config.setValue("edit/font",fontBox->currentIndex());
	config.setValue("edit/fontname",fontBox->currentText());

    close();
}

GlobalConfigDialog::GlobalConfigDialog(QWidget* parent):
QDialog(parent)
{
	setWindowTitle("全局配置");
	setMinimumHeight(180);

	auto box = new QVBoxLayout(this);
	QToolBox* content = new QToolBox();
	box->addWidget(content);
	content->addItem(new TextEditConfigWidget(),"编辑器配置");

	QWidget* editor = new QWidget();
	QGridLayout* grid = new QGridLayout(editor);
	
	AutoTextEdit* textEdit = new AutoTextEdit();
	grid->addWidget(textEdit,0,0,1,3);
	grid->addItem(new QSpacerItem(180,36,QSizePolicy::Expanding,QSizePolicy::Minimum),1,0,1,1);
	QPushButton* applyButton = new QPushButton("应用",this);
	QPushButton* resetButton = new QPushButton("重置",this);
	grid->addWidget(applyButton,1,1,1,1);
	grid->addWidget(resetButton,1,2,1,1);

	auto completer = new QCompleter(this);
	completer->setModel(new QStringListModel(SnailContext::getIntelligentPrompts(),this));
	completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(false);
	textEdit->setCompleter(completer);

	QStringList keywords = StockScriptList::GetInstance().getScriptEngine()->keywords();
	new Highlighter(keywords,textEdit->document());

	auto buffer = readAllBufferFromFile("config/config.conf");
	if (!buffer.isEmpty())
		textEdit->setText(buffer);

	connect(applyButton,QOverload<bool>::of(&QPushButton::clicked),[=]()
	{
		auto buffer = textEdit->toPlainText();
		saveToFile("config/config.conf",buffer.toLatin1());
		parseConfiguration(buffer,globalConfig);
	});

	connect(resetButton,QOverload<bool>::of(&QPushButton::clicked),[=]()
	{
		auto buffer = readAllBufferFromFile(":qrc/config.conf");
		textEdit->setText(buffer);
		parseConfiguration(buffer,globalConfig);
	});

	setTextEditStyle(textEdit);
	content->addItem(editor,"全局配置项");
}
