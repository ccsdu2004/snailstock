#include <QHeaderView>
#include <QTableWidget>
#include <QStringList>
#include <QStyledItemDelegate>
#include <QContextMenuEvent>
#include "StockListView.h"
#include "interface/StockRealTimeInfo.h"
#include "SnailApplication.h"
#include "widget/LineDelegate.h"
#include "widget/FilterTableHeader.h"
#include "widget/ExtendedTableWidget.h"
#include "Indicator.h"
#include "interface/StockParser.h"
#include "common/common.h"

#define CELL(i,value) (i = new QTableWidgetItem(QString("%1").arg(value)))
#define CELL_ORDER(i,value) (i = new OrderTableWidgetItem(QString("%1").arg(value)))

class BasicStockListView : public StockListView
{
public:
	BasicStockListView(QWidget* parent = 0);
	~BasicStockListView();
protected:
	void addStockImpl(const Stock& stock);
};

BasicStockListView::BasicStockListView(QWidget* parent):
StockListView(parent)
{
	QStringList header;
    setColumnCount(7);     
    header<<"代码"<<"名称"<<"行业"<<"地域"<<"股本"<<"流通股本"<<"上市日期";
	setHorizontalHeaderLabels(header);
}

BasicStockListView::~BasicStockListView()
{
}

void BasicStockListView::addStockImpl(const Stock& stock)
{
	int row = rowCount();
    insertRow(row);

	QTableWidgetItem* i = 0;
	setItem(row,0,CELL_ORDER(i,stock.code));
	i->setTextAlignment(Qt::AlignCenter);
	setItem(row,1,CELL(i,stock.name));
	i->setTextAlignment(Qt::AlignCenter);
	setItem(row,2,CELL(i,stock.industry));
	i->setTextAlignment(Qt::AlignCenter);
	setItem(row,3,CELL(i,stock.area));
	i->setTextAlignment(Qt::AlignCenter);
	setItem(row,4,CELL_ORDER(i,stock.data[Stock::TAG_ZONGGUBEN]));
	i->setTextAlignment(Qt::AlignCenter);
	setItem(row,5,CELL_ORDER(i,stock.data[Stock::TAG_LIUTONGGUBEN]));
	i->setTextAlignment(Qt::AlignCenter);
	QString date(stock.timeToMarket);
	date.insert(6,"-");
	date.insert(4,"-");
	setItem(row,6,CELL(i,date));
	i->setTextAlignment(Qt::AlignCenter);
}

class OCHLBasicStockListView : public StockListView
{
public:
	OCHLBasicStockListView(QWidget* parent = 0);
	~OCHLBasicStockListView();
protected:
	void addStockImpl(const Stock& stock);
};

OCHLBasicStockListView::OCHLBasicStockListView(QWidget* parent):
StockListView(parent)
{
    setColumnCount(13);    
	QStringList header;
    header<<"代码"<<"名称"<<"涨幅"<<"现价"<<"昨收"<<"今开"<<"最高"<<"最低"<<"成交量"<<"成交额"<<"换手率"<<"形态评分"<<"资金评分";
	setHorizontalHeaderLabels(header);
}

OCHLBasicStockListView::~OCHLBasicStockListView()
{
}

void OCHLBasicStockListView::addStockImpl(const Stock& stock)
{
	int row = rowCount();
    insertRow(row);

	QTableWidgetItem* i = 0;
	setItem(row,0,CELL_ORDER(i,stock.code));
	i->setTextAlignment(Qt::AlignCenter);

	setItem(row,1,CELL(i,stock.name));
	i->setTextAlignment(Qt::AlignCenter);
	
	setItem(row,2,CELL_ORDER(i,stock.increase));
	i->setTextAlignment(Qt::AlignCenter);

	setItem(row,3,CELL_ORDER(i,stock.current));
	i->setTextAlignment(Qt::AlignCenter);

	setItem(row,4,CELL_ORDER(i,stock.privClose));
	i->setTextAlignment(Qt::AlignCenter);

	setItem(row,5,CELL_ORDER(i,stock.open));
	i->setTextAlignment(Qt::AlignCenter);

	setItem(row,6,CELL_ORDER(i,stock.top));
	i->setTextAlignment(Qt::AlignCenter);

	setItem(row,7,CELL_ORDER(i,stock.low));
	i->setTextAlignment(Qt::AlignCenter);

	//成交量,成交额,换手率,形态分,资金分
	//if(!stock.realTimeInfo)
	{
	    setItem(row,8,CELL_ORDER(i,stock.tradeVolume));
	    i->setTextAlignment(Qt::AlignCenter);

		setItem(row,9,CELL(i,stock.tradeCapital));
	    i->setTextAlignment(Qt::AlignCenter);

		setItem(row,10,CELL(i,stock.turnover));
	    i->setTextAlignment(Qt::AlignCenter);

		setItem(row,11,CELL(i,stock.data[Stock::TAG_P1].toFloat()));
	    i->setTextAlignment(Qt::AlignCenter);

		setItem(row,12,CELL(i,stock.data[Stock::TAG_P3].toFloat()));
	    i->setTextAlignment(Qt::AlignCenter);
	}
}

class BidStockListView : public StockListView
{
public:
	BidStockListView(QWidget* parent = 0)
	{
	    setColumnCount(20);  
		QStringList header;
        header<<"代码"<<"名称"<<"得分"<<"价格"<<"买卖比"<<"涨幅"
			  <<"买1"
			  <<"买2"<<"买2"<<"买3"<<"买4"<<"买5"
			  <<"卖1"<<"卖2"<<"卖3"<<"卖4"<<"卖5"
			  <<"行业"<<"地域"<<"市值";	
		setHorizontalHeaderLabels(header);
	}
	~BidStockListView(){}
protected:
	void addStockImpl(const Stock& stock);
};

void BidStockListView::addStockImpl(const Stock& stock)
{
	int row = rowCount();
	insertRow(row);

	QTableWidgetItem* i = 0;
	setItem(row,0,CELL_ORDER(i,stock.code));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);
	setItem(row,1,CELL_ORDER(i,stock.name));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,2,CELL_ORDER(i,stock.score));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,3,CELL_ORDER(i,stock.current));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,4,CELL_ORDER(i,stock.p1));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,5,CELL_ORDER(i,STR_CEIL(stock.increase)));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,6,CELL_ORDER(i,STR_CEIL(stock.buy[0])));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,7,CELL_ORDER(i,STR_CEIL(stock.buy[1])));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,8,CELL_ORDER(i,STR_CEIL(stock.buy[2])));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,9,CELL_ORDER(i,STR_CEIL(stock.buy[3])));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,10,CELL_ORDER(i,STR_CEIL(stock.buy[4])));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,11,CELL_ORDER(i,STR_CEIL(stock.sale[0])));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);
	i->setTextAlignment(Qt::AlignCenter);

	setItem(row,12,CELL_ORDER(i,STR_CEIL(stock.sale[1])));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);
	i->setTextAlignment(Qt::AlignCenter);

	setItem(row,13,CELL_ORDER(i,STR_CEIL(stock.sale[2])));
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,14,CELL_ORDER(i,STR_CEIL(stock.sale[3])));
	i->setTextAlignment(Qt::AlignCenter);
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);
	i->setTextAlignment(Qt::AlignCenter);

	setItem(row,15,CELL_ORDER(i,STR_CEIL(stock.sale[4])));
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,16,CELL_ORDER(i,stock.industry));
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);
				
	setItem(row,17,CELL_ORDER(i,stock.area));
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);

	setItem(row,18,CELL_ORDER(i,STR_CEIL(stock.current*stock.data[Stock::TAG_ZONGGUBEN].toDouble()/10000.0f)));
	i->setForeground(stock.increase >= 0 ? Qt::red:Qt::green);
}
/*
class StockScannerListView : public StockListView
{
public:
	StockScannerListView(QWidget* parent = 0)
	{
	    QStringList header;
	    setColumnCount(10);     
        header<<"代码"<<"名称"<<"涨幅"<<"数目"<<"趋势指标"<<"成交量指标"<<"价格位置"<<"缺口"<<"K线"<<"形态";
		setHorizontalHeaderLabels(header);
	}
	~StockScannerListView(){}
protected:
	void addStockImpl(const Stock& stock)
	{
		if(stock.desc.size() < 3)
			return;
		int row = rowCount();
		insertRow(row);

		QTableWidgetItem* i = 0;
		setItem(row,0,CELL_ORDER(i,stock.code));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,1,CELL(i,stock.name));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,2,CELL_ORDER(i,STR_CEIL(stock.increase)));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,3,CELL_ORDER(i,stock.desc.size()));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,4,CELL(i,stock.desc[StockScanner::SCANNER_PRICE].join(",")));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,5,CELL(i,stock.desc[StockScanner::SCANNER_VOL].join(",")));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,6,CELL(i,stock.desc[StockScanner::SCANNER_PRICEPOS].join(",")));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,7,CELL(i,stock.desc[StockScanner::SCANNER_BREACH].join(",")));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,8,CELL(i,stock.desc[StockScanner::SCANNER_K].join(",")));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,9,CELL(i,stock.desc[StockScanner::SCANNER_SHAPE].join(",")));
	}
};*/

class LHBStockListView : public StockListView
{
public:
	LHBStockListView(QWidget* parent = 0)
	{
	    QStringList header;
	    setColumnCount(7);     
        header<<"代码"<<"名称"<<"价格"<<"成交量(万手)"<<"成交额"<<"类型"<<"日期"; 
		setColumnWidth(0,64);
	    setColumnWidth(1,72);
	    setColumnWidth(2,64);
	    setColumnWidth(3,100);
	    setColumnWidth(4,100);
	    setColumnWidth(5,270);
	    setColumnWidth(6,72);
		setHorizontalHeaderLabels(header);
	}
	~LHBStockListView(){}
protected:
	void addStockImpl(const Stock& stock)
	{
		int row = rowCount();
		insertRow(row);

		QTableWidgetItem* i = 0;
		setItem(row,0,CELL_ORDER(i,stock.code));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,1,CELL(i,stock.name));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,2,CELL(i,stock.current));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,3,CELL(i,stock.tradeVolume));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,4,CELL(i,stock.tradeCapital));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,5,CELL(i,stock.type));

		setItem(row,6,CELL(i,stock.date));
		i->setTextAlignment(Qt::AlignCenter);
	}
};

class LHBPKStockListView : public StockListView
{
public:
	LHBPKStockListView(QWidget* parent = 0)
	{
	    QStringList header;  
	    setColumnCount(6);     
        header<<"代码"<<"名称"<<"涨幅"<<"类型"<<"点评"<<"日期"; 
	    
		setColumnWidth(0,64);
	    setColumnWidth(1,64);
	    setColumnWidth(2,64);
	    setColumnWidth(3,240);
	    setColumnWidth(4,240);
	    setColumnWidth(5,100);
		setHorizontalHeaderLabels(header);
	}
	~LHBPKStockListView(){}
protected:
	void addStockImpl(const Stock& stock)
	{
		int row = rowCount();
		insertRow(row);

		QTableWidgetItem* i = 0;
		setItem(row,0,CELL_ORDER(i,stock.code));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,1,CELL(i,stock.name));
		i->setTextAlignment(Qt::AlignCenter);
		setItem(row,2,CELL(i,stock.increase));
		//i->setTextAlignment(Qt::AlignLeft);
		setItem(row,3,CELL(i,stock.type));
		//i->setTextAlignment(Qt::AlignLeft);
		setItem(row,4,CELL(i,stock.dp));
		//i->setTextAlignment(Qt::AlignLeft);
		setItem(row,5,CELL(i,stock.date));
		i->setTextAlignment(Qt::AlignCenter);
	}
};

class StockListViewStrategy : public StockListView
{
public:
	StockListViewStrategy(QWidget* parent = 0)
	{
	    QStringList header;  
	    setColumnCount(9);     
        header<<"代码"<<"名称"<<"买入日期"<<"买入价位"<<"卖出日期"<<"卖出价位"<<"持股天数"<<"涨幅"<<"卖出类型";
	    
		setColumnWidth(0,48);
		setColumnWidth(1,72);
	    setColumnWidth(2,72);
	    setColumnWidth(3,72);
		setColumnWidth(4,72);
	    setColumnWidth(5,72);
	    setColumnWidth(6,72);
		setColumnWidth(7,72);
		setColumnWidth(8,86);
		setHorizontalHeaderLabels(header);
	}
	~StockListViewStrategy()
	{
	}
protected:
	void addStockImpl(const Stock& stock)
	{
		int row = rowCount();
		insertRow(row);

		QTableWidgetItem* i = 0;
		setItem(row,0,CELL(i,stock.code));
		setItem(row,1,CELL(i,stock.name));
		double buy = stock.data[Stock::TAG_BUY_PRICE].toDouble();
		setItem(row,2,CELL(i,stock.data[Stock::TAG_BUY_DATE]));
		setItem(row,3,CELL_ORDER(i,buy));

		if(!stock.data[Stock::TAG_SALE_DATE].isEmpty())
		{
			setItem(row,4,CELL(i,stock.data[Stock::TAG_SALE_DATE]));
		    double sale = stock.data[Stock::TAG_SALE_PRICE].toDouble();
            setItem(row,5,CELL_ORDER(i,sale));
		    setItem(row,6,CELL_ORDER(i,stock.data[Stock::TAG_BS_HOLDDAY_COUNT]));

			double increase = 0;
			if(buy > 0)
				increase = GET_INCREASE(sale,buy);
			setItem(row,7,CELL_ORDER(i,STR_CEIL(increase)));
		}
		setItem(row,8,CELL(i,stock.data[Stock::TAG_SALE_TYPE]));
	}

	void onClickImpl(int row,int col){}
};

class StockListViewSnailStockList : public StockListView
{
public:
	StockListViewSnailStockList(QWidget* parent = 0)
	{
	    QStringList header;  
	    setColumnCount(7);     
        header<<"代码"<<"股票"<<"市盈率"<<"市净率"<<"总市值"<<"流通市值"<<"数据文件";
	    
		setColumnWidth(0,48);
		setColumnWidth(1,72);
	    setColumnWidth(2,72);
	    setColumnWidth(3,72);
		setColumnWidth(4,72);
	    setColumnWidth(5,72);
	    setColumnWidth(6,72);
		setHorizontalHeaderLabels(header);
	}
	~StockListViewSnailStockList()
	{
	}
protected:
	void addStockImpl(const Stock& stock)
	{
		int row = rowCount();
		insertRow(row);

		QTableWidgetItem* i = 0;
		setItem(row,0,CELL(i,stock.code));
		setItem(row,1,CELL(i,stock.name));
		setItem(row,2,CELL_ORDER(i,stock.data[Stock::TAG_PE_D].toDouble()));
		setItem(row,3,CELL_ORDER(i,stock.data[Stock::TAG_PB].toDouble()));
		setItem(row,4,CELL_ORDER(i,stock.data[Stock::TAG_ZONGSHIZHI].toDouble()*0.0001));
		setItem(row,5,CELL_ORDER(i,stock.data[Stock::TAG_LIUTONGSHIZHI].toDouble()*0.0001));
		setItem(row,6,CELL(i,stock.data[Stock::TAG_DATA_FILE]));
	}

	void onClickImpl(int row,int col)
	{
	}
};

StockListView::StockListView(QWidget* parent):
    QTableWidget(parent)
{	
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionBehavior(QAbstractItemView::SelectRows);
    
	verticalHeader()->setVisible(false);
	horizontalHeader()->setVisible(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
	setEditTriggers(QTableView::NoEditTriggers);
	setAlternatingRowColors(true);

    setShowGrid(false);
	setItemDelegate(new LineDelegate(this));
	setSortingEnabled(true);

	/*setContextMenuPolicy(Qt::CustomContextMenu);
	contextMenu = new QMenu(this);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showMenu(QPoint)));

	{
		QAction* desc = new QAction("简介",this);
	    connect(desc,SIGNAL(triggered()),this,SLOT(showDesc()));
	    contextMenu->addAction(desc);
	}
	{
		QAction* k = new QAction("K线",this);
	    connect(k,SIGNAL(triggered()),this,SLOT(showKChart()));
	    contextMenu->addAction(k);
	}
	{
	    QAction* holder = new QAction("股东",this);
	    connect(holder,SIGNAL(triggered()),this,SLOT(showStockHolderView()));
	    contextMenu->addAction(holder);
	}*/

	connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(onClick(int,int)));
	curRow = curCol = -1;
	hasMenu = true;
}

StockListView::~StockListView()
{
}

void StockListView::enableMenu(bool enable)
{
	hasMenu = enable;
}

void StockListView::clearTable()
{
	clearContents();
	setRowCount(0);
	stocks.clear();
}

void StockListView::addStock(const Stock& stock)
{
    addStockImpl(stock); 
	stocks.insert(stock.code,stock);
}

void StockListView::addStocks(const QVector<Stock>& stocks)
{
	foreach(Stock stock,stocks)
		addStock(stock);
}

void StockListView::onClick(int r,int c)
{
	curRow = r;
	curCol = c;
	onClickImpl(r,c);
}

void StockListView::contextMenuEvent(QContextMenuEvent* event)
{
    QPoint point = event->pos();
	QTableWidgetItem* item = this->itemAt(point);
    if(!item)
	{
	    contextMenu->exec(QCursor::pos());
		event->accept();
	}
}

StockListView* StockListView::createStockListView(StockListViewEnum flag,QWidget* parent)
{
    StockListView* view = 0;
	if(flag == StockListViewEnum_Bid)
		return new BidStockListView(parent);
	else if(flag == StockListViewEnum_LHB)
		return new LHBStockListView(parent);
	else if(flag == StockListViewEnum_LHB_PK)
		return new LHBPKStockListView(parent);
	else if(flag == StockListViewEnum_Basic)
		return new BasicStockListView(parent);
	//else if(flag == StockListViewEnum_Scanner)
	//	return new StockScannerListView(parent);
	else if(flag == StockListViewEnum_OCHL)
		return new OCHLBasicStockListView(parent);
	else if(flag == StockListViewEnum_Strategy)
		return new StockListViewStrategy(parent);
	else if(flag == StockListViewEnum_SnailStockList)
		return new StockListViewSnailStockList(parent);
	return nullptr;
}

#undef CELL
#undef CELL_ORDER