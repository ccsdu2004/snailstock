#include <QVBoxLayout>
#include <QTimerEvent>
#include <QDateTime>
#include "common.h"
#include "DataLoader.h"
#include "TailStockWidget.h"
#include "SnailApplication.h"

#define DESC "尾盘自动推荐,无需手动操作"

TailStockWidget::TailStockWidget(QWidget* parent):
QWidget(parent),
stockScanner(new J3StockScanner())
{
    QVBoxLayout* vbox = new QVBoxLayout(this);
	stockList = new StockListView(StockListViewEnum_Scanner);
	connect(stockList,SIGNAL(cellClicked(int,int)),this,SLOT(onClick(int,int)));
	textEdit = new QTextEdit();
	vbox->addWidget(stockList,3);
	vbox->addWidget(textEdit,1);
	textEdit->setText(DESC);

	textEdit->setTextColor(QColor(230,64,64));
	QFont font;
	font.setPointSize(12);
	textEdit->setFont(font);

	histFace.setParam(StockInterface::FLAG_TARGET_DIR,"data/");
	histFace.setParam(StockInterface::FLAG_TARGET_DAY,"day");
	histFace.setParam(StockInterface::FLAG_TARGET_FUQUAN,"qfq");

	step = TailStockWidget::SCANNER_STEP_NONE;
	startTimer(600);
}

TailStockWidget::~TailStockWidget()
{
}

void TailStockWidget::onClick(int r,int c)
{
    QTableWidgetItem* cell = stockList->item(r,0);
	if(!cell)
		return;

	QString code = cell->text();
	if(output.find(code) != output.end())
	{
		QString text = outputDesc[code].join(",");
		textEdit->setText(text);
	}
}

void TailStockWidget::doStockHistory()
{
	if(stocks.isEmpty())
	{
	    step = SCANNER_STEP_3;
		QStringList codes = output.uniqueKeys();
		links = sinaFace.buildLinks(codes);
	    return;
	}

	Stock stock = stocks.at(0);
	stocks.remove(0);

	QStringList links(stock.code);
	links = histFace.buildLinks(links);
	//std::cout<<qPrintable(links[0])<<std::endl;
	QString buffer = DataLoader::getText(links[0]);
	QVector<Stock> datas = histFace.parseContent(buffer);
	QStringList desc;
	int flag = stockScanner->check(stock.code,datas,desc);
	if(flag == 0)
	{
		output[stock.code] = stock;
		outputDesc[stock.code] = desc;
	}
}

void TailStockWidget::timerEvent(QTimerEvent* event)
{
	int id = event->timerId();
	killTimer(id);

	QTime time = QDateTime::currentDateTime().time();
#ifndef SNAIL_STOCK_COUNT_ALL
#ifdef SNAIL_STOCK_COUNT_FREE
    int d = 50;
#endif 
#ifdef SNAIL_STOCK_COUNT_360
    int d = 49;
#endif
#ifdef SNAIL_STOCK_COUNT_960
    int d = 47;
#endif 
#endif
#ifdef SNAIL_STOCK_COUNT_ALL
    int d = 45;
#endif 
	//int d = 50;

	if(time.hour() == 14 && time.minute() == d && step == SCANNER_STEP_NONE)
	//if(time.minute() == d && step == SCANNER_STEP_NONE)
	{
	    bool flag = isHoliday(QDateTime::currentDateTime().date());
	    if(flag)
		    return;

		for(int i=1;i<41;i++)
		    links.append(QString("%1").arg(i));
	    links = face.buildLinks(links);
		step = SCANNER_STEP_1;
	}

	if(step == SCANNER_STEP_1)
	{
		if(links.isEmpty())
		{
			step = SCANNER_STEP_2;
		}
		else
		{
			QString link = links.at(0);
			links.removeAt(0);

			QMap<QString,Stock> pl = snail->getStocks();

			QString buffer = DataLoader::getText(link);
			QVector<Stock> list = face.parseContent(buffer);
			foreach(Stock stock,list)
			{
#ifndef SNAIL_STOCK_COUNT_ALL
				if(pl.find(stock.code) != pl.end())
#endif 
				if(stock.increase <= 5.5 && stock.increase >= 0.6 && stock.current >= stock.start)
					stocks.append(stock);
			}	
		}
	}
	else if(step == SCANNER_STEP_2)
	    doStockHistory();
	else if(step == SCANNER_STEP_3)
	{
		if(links.isEmpty())
		{
		    step = SCANNER_STEP_4;
		}
		else
		{
			QString buffer = DataLoader::getText(links.at(0));
			links.removeAt(0);
			QVector<Stock> list = sinaFace.parseContent(buffer);
			foreach(Stock stock,list)
			{
				if(stock.increase > 8.5f || stock.increase < 0.8f)
				{
					output.remove(stock.code);
					outputDesc.remove(stock.code);
				}
			}
		}
	}
	else if(step == SCANNER_STEP_4)
	{
		if(output.isEmpty())
		{
			textEdit->setText("本日无数据或请使用收费版本");
		}
		else
		{
			senderMailMsg();
			QMap<QString,Stock>::iterator itr = output.begin();
			while(itr != output.end())
			{
				Stock stock = itr.value();
				stock.kDesc = outputDesc[stock.code].join(",");
				stockList->addStock(itr.value());
			    itr ++;
			}
		}
	    return;
	}
	
    startTimer(150);
}

void TailStockWidget::senderMailMsg()
{
    QStringList strs;
	foreach(Stock stock,output)
	{
		float p = (stock.current-stock.privClose)/stock.privClose*100;
		int ip = 100*p;
		p = ip/100.0f;
		QString buffer("代码%1,股票%2,现价:%3,所属板块:%4,涨幅:%5%");
		buffer = buffer.arg(stock.code).arg(stock.name).arg(stock.current).arg(stock.industry)
			.arg(p);
		strs.append(buffer);
	}

	QString buffer = strs.join("\n");
	buffer = "尾盘关注:\n" + buffer;

	int ret = senderMail("尾盘选股",buffer);
	if(ret == 0)
		textEdit->setText("尾盘选股已发送到您的邮箱");

}