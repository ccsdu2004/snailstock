#include <qgridlayout.h>
#include "CandlestickWidget.h"

CandlestickWidget::CandlestickWidget(QWidget* parent):
QDialog(parent)
{
	setWindowTitle("股票日K浏览");
	setWindowIcon(QIcon(":logo/snail.png"));

	QGridLayout* grid = new QGridLayout();
	setLayout(grid);
	
	grid->addWidget(stockList = new QListWidget(),0,0,2,1);
	stockList->setFixedWidth(68);

    grid->addWidget(candlestickView = new CandlestickView(this),0,1,1,2);
	grid->addWidget(searchEdit = new QLineEdit(),1,1,1,1);
    grid->addWidget(button = new QPushButton("查看"),1,2,1,1);
}

CandlestickWidget::~CandlestickWidget()
{
}
 