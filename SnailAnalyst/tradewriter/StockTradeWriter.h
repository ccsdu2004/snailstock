#ifndef STOCK_TRADE_WRITER_H
#define STOCK_TRADE_WRITER_H
#include <QString>
#include <QMap>
#include <QPen>
#include "common/common.h"
#include "TradeReport.h"

#pragma execution_character_set("utf-8")

class StockTradeWriter
{
public:
	static StockTradeWriter* getInstance();

	void clear()
	{
		historyTradeInfo.clear();
		currentTradeInfo.clear();
		profitList.clear();
		profitByDate.clear();
	}
	
	int add(const QString& code,int pos,bool buy,bool sale,const QVector<float>& arguments);
	void reduce();

	TradeReport getTradeItem();

	bool toCSV(const QString& filename,const QVector<TradeReport>& info,const QStringList& arguments);

	void clearCurrentTradeInfo()
	{
	    currentTradeInfo.clear();
	}
private:
	StockTradeWriter();

	bool isSameStock(const QString& stock,int pos);
	int addStock(const QString& code,int pos,bool buy,bool sale,const QVector<float>& arguments);
	int saleStockImpl(int pos,const QString& desc);

	void iterateTradeItme();
	void iterateTradeNode(const StockTradeItem& item);
	void dumpProfit();

	int buyStock(const QString& code,int pos,const QVector<float>& arguments);
	int saleStock(const QString& code,int pos);
	int saleStockByRule(const QString& code,int pos);
	void calcIncreaseList(int current);
    QVector<StockTradeItem> trimStockTradeItemByHS300(const QVector<StockTradeItem>& list);
	QVector<StockTradeItem> outputTradeItems(QMap<int,double>& output);

	//date,stock list
	QMap<QString,QVector<StockTradeItem>> historyTradeInfo;
	StockTradeItem currentTradeInfo;
	QMap<QString,QMap<QString,DataWrapper2<double,double>>> profitList;
	QVector<StockProfitVar> profitByDate;
};

#endif