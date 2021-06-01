#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H
#include <QString>
#include <QMap>
#include "config.h"
#include "Stock.h"
#include "zip/quazip.h"
#include "interface/StockRealTimeInfo.h"
#include "bid/StockBidData.h"

struct DataProvider
{
	QString code;
	QString file;
    QVector<Stock> stocks;

	QString date;
	QMap<QString,StockRealTimeInfo> tickData;

	QMap<QString,QMap<QString,StockBidData>> bidData;

	QMap<QString,QMap<QString,Stock>> perStockData;
	QMap<QString,double> perStockLimitR;

	bool loadedRef300;
	
	bool loadStockFromFile(const QString& code,const QString& file,bool ref = false);
	int currentStockCursor;

#ifdef SUPPORT_TICKET
	bool loadTickDataFromFile(const QString& stock,const QString& date);
	bool loadTickDataFromZip(const QString& stock,const QString& date);
#endif

	bool loadStockBidData(const QString& date);
	bool hasStockBidData(const QString& date,const QString& code);

	bool loadStockPerData(const QString& date);
	bool hasStockPerData(const QString& date,const QString& code);

	QString getStockCodeByFilePath(const QString& stockFile);

	void clear();
	void reset();

	QMap<QString,double> hs300;
	QuaZip* ticketZip;

	DataProvider()
	{
	    ticketZip = new QuaZip();
	}
	~DataProvider()
	{
	    delete ticketZip;
	}
private:
#ifdef SUPPORT_TICKET
	bool loadTickDataFromFileIMPL(const QString& stock,const QString& date);
#endif
};

extern DataProvider dataProvider;

#endif 