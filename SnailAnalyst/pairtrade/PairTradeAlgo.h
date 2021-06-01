#ifndef PAIR_TRADE_ALGO_H
#define PAIR_TRADE_ALGO_H
#include <qvector.h>
#include <qpair.h>
#include "SnailAddonHost.h"
#include "ConfigurationParser.h"

#pragma execution_character_set("utf-8")

class AbstractPairTrader
{
public:
	AbstractPairTrader();
	virtual ~AbstractPairTrader(){}

	void setConfiguration(Configuration* conf)
	{
		config = conf;
	}

	void clear()
	{
		stockA.clear();
		stockB.clear();
		data.clear();
		stockbuy1.clear();
		stocksale1.clear();
		stockbuy2.clear();
		stocksale2.clear();
	}

	virtual void run() = 0;

	QString getStock1()const{ return stock1; }
	QString getStock2()const{ return stock2; }
	void getStockTradeInfo(QSet<QString>& stcokbuy1,QSet<QString>& stocksale1,QSet<QString>& stockbuy2,QSet<QString>& stocksale2);
protected:
	Configuration* config;

	double minCor,maxCor;
	QString stock1,stock2;
	double p;
	int minCount;
	bool longTermHoldStock;

	double axisOffset;
	double bandWidth;
	double k;
	
	QVector<Stock> stockA,stockB;
	QVector<float> data;

	QSet<QString> stockbuy1,stockbuy2,stocksale1,stocksale2;
};

class PairTraderAlgo : public AbstractPairTrader
{
public:
	void run();
};


#endif