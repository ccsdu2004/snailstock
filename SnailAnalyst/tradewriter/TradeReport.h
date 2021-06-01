#ifndef TRADE_REPORT_H
#define TRADE_REPORT_H
#include <QString>
#include <QVector>
#include "config.h"

#define QUANT_TRADE_COUNT        0
#define QUANT_TRADE_AVG_HOLDDAY  1
#define QUANT_TRADE_SUCCESS      2
#define QUANT_TRADE_PROFIT       3
#define QUANT_TRADE_PROFIT_NET   4
#define QUANT_TRADE_AVG_INCREASE 5
#define QUANT_TRADE_MAXDOWN      6
#define QUANT_TRADE_BUYDAY       7

struct StockProfitVar
{
	QString label;
	QPen color;
	QMap<QString,double> list;
	double maxdown;
	double profit;

	StockProfitVar():maxdown(0.0),profit(0.0){}
};

struct StockTradeItem
{
	QString stock;
	QString buyDate;
	double  buy;
	QString saleDate;
	double  sale;
	QString saleType;
	int     holdDays;
	double  vol;
	double  increase;
	int     index;
	bool    hasBS;

	double  p1;

#ifdef SUPPORT_OUTPUT_ARGUMENT
	QVector<float> arguments;
#endif

	enum State
	{
		State_waitForBuy,
		State_waitForSale,
		State_done
	};

	State state;

	QMap<QString,DataWrapper2<double,double>> profits;

	StockTradeItem()
	{
		clear();
	}

	void clear()
	{
		stock.clear();
		buy = 100.0;
		buyDate.clear();
		sale = 100.0;
		saleDate.clear();
		saleType.clear();
		holdDays = 0;
		vol = 0;
		increase = 0;
		index = 0;
		hasBS = false;
		profits.clear();
		state = State_waitForBuy;
#ifdef SUPPORT_OUTPUT_ARGUMENT
		arguments.clear();
#endif
	}
};

struct TradeReport
{
	QString scriptName;
	QString desc;
	QVector<StockTradeItem> item;
	QVector<StockProfitVar> profitvar;
	QVector<double> profit;
	QString scriptContent;
};


#endif