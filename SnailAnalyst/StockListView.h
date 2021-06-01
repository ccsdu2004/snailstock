#ifndef STOCKLISTVIEW_H
#define STOCKLISTVIEW_H
#include <QTableWidget>
#include <QMap>
#include <qdialog.h>
#include <QVector>
#include <QMenu>
#include "Stock.h"

enum StockListViewEnum
{
	StockListViewEnum_Bid,
	StockListViewEnum_LHB,
	StockListViewEnum_LHB_PK,
	StockListViewEnum_OCHL,
	StockListViewEnum_Basic,
	StockListViewEnum_Scanner,
	StockListViewEnum_Strategy,
	StockListViewEnum_SnailStockList
};

class StockListView : public QTableWidget
{
    Q_OBJECT
public:
    static StockListView* createStockListView(StockListViewEnum flag,QWidget* parent = 0);
public:
	StockListView(QWidget* parent = 0);
	virtual ~StockListView();

	void enableMenu(bool enable);
	void clearTable();
public slots:
	void addStock(const Stock& stock);
	void addStocks(const QVector<Stock>& stocks);
	void onClick(int r,int c);
signals:
	void senderData(const QString& code,const QString& date);
private:
	void contextMenuEvent(QContextMenuEvent* event);
protected:
	virtual void onClickImpl(int row,int col){}
	virtual void addStockImpl(const Stock& stock){}

	QMap<QString,Stock> stocks;
	int curRow;
	int curCol;
	QMenu* contextMenu;
	bool hasMenu;
};

class StockListDialog : public QDialog
{
	Q_OBJECT
public:
	StockListDialog(StockListViewEnum flag,const QMap<QString,Stock>& stocks,QWidget* parent = nullptr);
	~StockListDialog();
private:
};

#endif 