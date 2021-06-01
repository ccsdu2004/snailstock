#include <QSpacerItem>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDir>
#include "StockHotFibonacciWidget.h"
#include "interface.h"
#include <iostream>

bool StockHotFibonacciThreadChecker(const QString& filename,const QString& addon,int flag)
{
	StockHotStockFibonacciScanner* scanner = StockHotStockFibonacciScanner::createScanner(flag);
	if(!scanner)
		return false;

	QString code = filename.mid(filename.size()-6);
	QString buffer = readAllBufferFromFile(filename);

	StockQQHistoryInterface face;
	QVector<Stock> stocks = face.parseContent(buffer,code);

	QStringList desc;
	if(scanner->check(code,stocks,desc) == 0)
	{
		scanner->deleteLater();
	    return true;
	}
	scanner->deleteLater();
	return false;
}

StockHotFibonacciWidget::StockHotFibonacciWidget(QWidget* parent):
QWidget(parent)
{
	QGridLayout* grid = new QGridLayout(this);

    listWidget = new QListWidget();
	connect(listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onClick(QListWidgetItem*)));
    listWidget->setFixedWidth(72);
    grid->addWidget(listWidget,0,0,3,1);

	candlestick = new CandlestickWidget(true);
	candlestick->setLogCS(true);
    grid->addWidget(candlestick,0,1,1,3);
    
	QGroupBox* box = createGroupBox();
	grid->addWidget(box,1,1,1,3);

    QPushButton* configButton = new QPushButton("设置");
	connect(configButton,SIGNAL(clicked()),this,SLOT(onConfig()));
	configButton->setFixedWidth(72);
	grid->addWidget(configButton,2,2,1,1);

    button = new QPushButton("扫描");
	button->setFixedWidth(72);
    grid->addWidget(button,2,3,1,1);
	connect(button,SIGNAL(clicked()),this,SLOT(onScan()));

    progressBar = new QProgressBar();
	progressBar->setValue(0);
	progressBar->setVisible(false);
    grid->addWidget(progressBar,2,1,1,1);

	threadTarget = new StockThreadTarget(StockHotFibonacciThreadChecker,this);
	connect(threadTarget,SIGNAL(updateStock(int,const QString&,int)),this,SLOT(onUpdateResult(int,const QString&,int)));
	connect(threadTarget,SIGNAL(finished()),this,SLOT(onScannFinished()));

	threadTarget->setFlag(0);
}
	
StockHotFibonacciWidget::~StockHotFibonacciWidget()
{
}

void StockHotFibonacciWidget::onConfig()
{
    StockHotFibonacciConfig config;
	config.exec();
}

QGroupBox* StockHotFibonacciWidget::createGroupBox()
{
	QGroupBox* box = new QGroupBox("操作类型");
	box->setFixedHeight(64);
	QHBoxLayout* hbox = new QHBoxLayout(box);
	lineMode[0] = new QRadioButton("调整买入");
	connect(lineMode[0],SIGNAL(clicked()),this,SLOT(onClickRadioButton1()));
	lineMode[1] = new QRadioButton("升破买入");
	connect(lineMode[1],SIGNAL(clicked()),this,SLOT(onClickRadioButton2()));
    lineMode[2] = new QRadioButton("反弹卖出");
	connect(lineMode[2],SIGNAL(clicked()),this,SLOT(onClickRadioButton3()));
    lineMode[3] = new QRadioButton("跌破卖出");
	connect(lineMode[3],SIGNAL(clicked()),this,SLOT(onClickRadioButton4()));
	for(int i=0;i<4;i++)
	{
		hbox->addWidget(lineMode[i]);
		if(i > 1)
			lineMode[i]->setCheckable(false);
	}

	lineMode[0]->setChecked(true);
	hbox->addItem(new QSpacerItem(40,20,QSizePolicy::Expanding,QSizePolicy::Minimum));
    return box;
}

void StockHotFibonacciWidget::onScan()
{
    QDir dir("data/");
    if(!dir.exists())
        return;
 
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

	QMap<QString,QString> input;
    
	QFileInfoList list = dir.entryInfoList();
	int count = 0;
    foreach(QFileInfo info,list)
    {
		QString file = info.absoluteFilePath();
		input.insert(file,file);
    }

	if(progressBar->isVisible())
		return;

    listWidget->clear();
	progressBar->setVisible(true);

	threadTarget->setData(input);
	threadTarget->start();
}

void StockHotFibonacciWidget::onUpdateResult(int percent,const QString& code,int id)
{
	progressBar->setValue(percent);
	if(!code.isEmpty())
		listWidget->addItem(code.mid(code.size()-6));    
}

void StockHotFibonacciWidget::onScannFinished()
{
	progressBar->setVisible(false);

	#ifdef SNAIL_VERSION_FREE
        POP_MSGBOX("提示","免费版股票池数量有限,且不支持参数设定,建议购买收费版本",this)
	#endif 
}

void StockHotFibonacciWidget::onClick(QListWidgetItem* item)
{
	QString code = item->text();
	QString file = "data/" + code;
	QString buffer = readAllBufferFromFile(file);

	StockQQHistoryInterface face;
	QVector<Stock> stocks = face.parseContent(buffer,code);
	candlestick->setLogCS(false);

	int flag = StockHotStockFibonacciScanner::TAG_HOTSTOCK_FIBONACCI_TIAOZHENGMAIRU;
	if(lineMode[1]->isChecked())
		flag = StockHotStockFibonacciScanner::TAG_HOTSTOCK_FIBONACCI_SHENGPOMAIRU;
	else if(lineMode[2]->isChecked())
		flag = StockHotStockFibonacciScanner::TAG_HOTSTOCK_FIBONACCI_FANTANMAICHU;
	else if(lineMode[3]->isChecked())
		flag = StockHotStockFibonacciScanner::TAG_HOTSTOCK_FIBONACCI_DIEPOMAICHU;

	StockHotStockFibonacciScanner* scanner = StockHotStockFibonacciScanner::createScanner(flag);
	if(!scanner)
		return;

	QStringList desc;
	flag = scanner->check(code,stocks,desc);
	if(flag != 0)
		return;

	QString text = desc.join("\n");
	candlestick->setText(text);
	candlestick->updateChart(240,stocks,scanner->clstTop,scanner->clstLow,scanner->clstCur);
	scanner->deleteLater();
}

void StockHotFibonacciWidget::onClickRadioButton1()
{
	threadTarget->setFlag(0);
}

void StockHotFibonacciWidget::onClickRadioButton2()
{
	threadTarget->setFlag(1);
}

void StockHotFibonacciWidget::onClickRadioButton3()
{
	threadTarget->setFlag(2);
}

void StockHotFibonacciWidget::onClickRadioButton4()
{
	threadTarget->setFlag(3);
}