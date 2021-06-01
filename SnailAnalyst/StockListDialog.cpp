#include <qpushbutton.h>
#include <qgridlayout.h>
#include "widget/StockListWidgetWithFilter.h"
#include "StockListView.h"

StockListDialog::StockListDialog(StockListViewEnum flag,const QMap<QString,Stock>& stocks,QWidget* parent):
QDialog(parent)
{
	setWindowTitle("股票清单");
	setFixedWidth(512);
	auto view = new StockListWidgetWithFilter(this);

	auto itr = stocks.begin();
	while(itr != stocks.end())
	{
		if(!itr->code.isEmpty())
			view->addStock(*itr);
		itr ++;
	}
	
	auto grid = new QGridLayout(this);
	grid->addWidget(view,0,0,1,2);

    auto horizontalSpacer = new QSpacerItem(334,20,QSizePolicy::Expanding,QSizePolicy::Minimum);
	grid->addItem(horizontalSpacer,1,0,1,1);

    QPushButton* button = new QPushButton("确定",this);
    grid->addWidget(button,1,1,1,1);
	connect(button,SIGNAL(clicked()),this,SLOT(close()));
}

StockListDialog::~StockListDialog()
{
}
