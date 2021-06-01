#include <QHeaderView>
#include "LineDelegate.h"
#include "common/common.h"
#include "StockListWidgetWithFilter.h"

#define CELL(i,value) (i = new QTableWidgetItem(QString("%1").arg(value)))
#define CELL_ORDER(i,value) (i = new OrderTableWidgetItem(QString("%1").arg(value)))

StockListWidgetWithFilter::StockListWidgetWithFilter(QWidget* parent):
ExtendedTableWidget(parent)
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

	connect(this,SIGNAL(cellClicked(int,int)),this,SLOT(onClick(int,int)));
	curRow = curCol = -1;
	hasMenu = true;
}

StockListWidgetWithFilter::~StockListWidgetWithFilter()
{
}

void StockListWidgetWithFilter::enableMenu(bool enable)
{
	hasMenu = enable;
}

void StockListWidgetWithFilter::clearTable()
{
	clearContents();
	setRowCount(0);
	stocks.clear();
}

void StockListWidgetWithFilter::addStock(const Stock& stock)
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

	stocks.insert(stock.code,stock);
}

void StockListWidgetWithFilter::addStocks(const QVector<Stock>& stocks)
{
	foreach(Stock stock,stocks)
		addStock(stock);
}

void StockListWidgetWithFilter::onClick(int r,int c)
{
	curRow = r;
	curCol = c;
	onClickImpl(r,c);
}


#undef CELL
#undef CELL_ORDER