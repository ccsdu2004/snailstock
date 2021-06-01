#ifndef STOCK_BID_DATA_H
#define STOCK_BID_DATA_H
#include <QMap>
#include <QStringList>
#include "common/common.h"

#pragma execution_character_set("utf-8")

struct StockBidData
{
	QString code;
	double  increase;
	double price;
	double privClose;
	int vol;
	int capital;
	double turnover;
	int buy[5];
	int sale[5];
	bool ok;

	StockBidData()
	{
		ok = false;
	}
	
	static StockBidData parse(const QString& buffer);
};


#endif