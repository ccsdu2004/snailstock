#ifndef STOCKLISTWIDGETWITHFILTER_H
#define STOCKLISTWIDGETWITHFILTER_H
#include "../SnailAddonHost.h"
#include "ExtendedTableWidget.h"

#pragma execution_character_set("utf-8")

class StockListWidgetWithFilter : public ExtendedTableWidget
{
    Q_OBJECT
public:
	StockListWidgetWithFilter(QWidget* parent = 0);
	~StockListWidgetWithFilter();

	void enableMenu(bool enable);
	void clearTable();
public slots:
	void addStock(const Stock& stock);
	void addStocks(const QVector<Stock>& stocks);
	void onClick(int r,int c);
signals:
	void senderData(const QString& code,const QString& date);
protected:
	virtual void onClickImpl(int row,int col){}
	virtual void addStockImpl(const Stock& stock){}

	QMap<QString,Stock> stocks;
	int curRow;
	int curCol;
	QMenu* contextMenu;
	bool hasMenu;
};

#endif