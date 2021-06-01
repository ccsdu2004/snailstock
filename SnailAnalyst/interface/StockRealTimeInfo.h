#ifndef STOCK_REALTIME_INFO_H
#define STOCK_REALTIME_INFO_H
#include <QString>
#include <QVector>
#include <QPair>
#include <QTime>
#include "common/common.h"
#include "common/DataWrapper.h"
#include "Stock.h"

struct StockTimeLine
{
    QTime  time;
	double price;
	double capital;
	double ma;
	double volume;
};

struct StockRealTimeInfo
{
	StockRealTimeInfo();
	bool flag;
    QString code,name,date;
	QVector<StockTimeLine> list;
	QPair<float,QTime> low,high;

	Stock stock;

	QPair<float,int> buy[5];
	QPair<float,int> sale[5];

	//时间轴买卖挡
	QVector<DataWrapper4<float,int,int,float> > priceBars;
	QVector<DataWrapper6<QString,float,int,QString,int,int> > details;
	DataWrapper4<QString,QString,float,int> bigTrades;

	//主力资金流向(流入,流出,净流入)
	DataWrapper3<float,float,float> capitcalFlow;
};

class StockTimeDataInterface
{
public:
	StockTimeDataInterface();
	QString buildLink(const QString& code);
	StockRealTimeInfo parseContent(const QString& content);
};

class StockTuShareTimeDataInterface
{
public:
	QString buildLink(const QString& code);
	StockRealTimeInfo parseContent(const QString& content);
};

QVector<Stock> convertToTimeLine(const StockRealTimeInfo& info,const QString& date,int minute = 5);

#endif 