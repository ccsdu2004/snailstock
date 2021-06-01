#include <QGridLayout>
#include "StockRealTimeView.h"
#include "StockScriptEngine.h"

#ifdef SUPPORT_TICKET

StockHistoryCandlestickView::StockHistoryCandlestickView(QWidget* parent):
QDialog(parent)
{
	setWindowTitle("显示股票历史日分笔");
    auto grid = new QGridLayout(this);
	setFixedSize(360,240);
	view = new StockTimeDataWidget(this);
    grid->addWidget(view,0,0,1,2);

    input = new QLineEdit(this);
	input->setPlaceholderText("输入格式: sh600001,2017-12-30");
    grid->addWidget(input,1,0,1,1);

    button = new QPushButton(this);
	button->setText("查看");
    grid->addWidget(button,1,1,1,1);
	connect(button,SIGNAL(clicked()),this,SLOT(onShow()));
}

StockHistoryCandlestickView::~StockHistoryCandlestickView()
{
}

void StockHistoryCandlestickView::onShow()
{
	QString text = input->text();
	QStringList items = text.split(",");
	if(items.size() != 2)
		return;

	QString stock = items[0];
	QString date = items[1];

	bool ok = dataProvider.loadTickDataFromFile(stock,date);
	if(ok)
	{
		auto info = dataProvider.tickData[date];
		QString p1 = info.stock.data[Stock::TAG_P1];
		QString p2 = info.stock.data[Stock::TAG_P2];
		//auto stocks = convertTo1MinuteTick(info);
		view->loadData(stock,p1,p2,info.list);
	}
}

#endif