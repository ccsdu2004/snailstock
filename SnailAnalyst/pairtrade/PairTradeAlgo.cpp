#include <qdebug.h>
#include "PairTradeAlgo.h"
#include "ConfigurationParser.h"
#include "SnailContext.h"
#include "config/ConfigItem.h"
#include "common/FileUtils.h"
#include "ObjectFactory.h"
#include "math/math.h"
#include "interface/StockParser.h"

AbstractPairTrader::AbstractPairTrader()
{
	QVariant var;

	auto list = SnailContext::open()->getConfigFactory()->usage();

#define GET_VAR(str)\
    SnailContext::open()->getConfigFactory()->getVar(QString(str),var);

	GET_VAR("config.unit.pairtrade.mincor")
	minCor = var.toDouble();

	GET_VAR("config.unit.pairtrade.maxcor")
	maxCor = var.toDouble();

	minCor = std::clamp(minCor,-100.0,100.0);
	maxCor = std::clamp(maxCor,-100.0,100.0);

	if (minCor > maxCor)
	{
		double v = minCor;
		minCor = maxCor;
		maxCor = v;
	}

	GET_VAR("config.unit.pairtrade.stock1")
	stock1 = var.toString();

	GET_VAR("config.unit.pairtrade.stock2")
	stock2 = var.toString();

	GET_VAR("config.unit.pairtrade.长久持股");
	longTermHoldStock = var.toString() == "yes";

	GET_VAR("config.unit.pairtrade.最少股票数据量")
	minCount = var.toInt();
	minCount = std::clamp<int>(minCount,60,10000);

	GET_VAR("config.unit.pairtrade.中轴偏差")
	axisOffset = var.toDouble();
	axisOffset = std::clamp<double>(axisOffset,-20.0,20.0);

	GET_VAR("config.unit.pairtrade.带宽")
	bandWidth = var.toDouble();
	bandWidth = std::clamp<double>(bandWidth,0,50.0);

	//var.clear();
	GET_VAR("config.unit.pairtrade.k")
	k = var.toDouble();
}

void AbstractPairTrader::getStockTradeInfo(QSet<QString>& istockbuy1,QSet<QString>& istocksale1,QSet<QString>& istockbuy2,QSet<QString>& istocksale2)
{
	istockbuy1 = stockbuy1;
	istockbuy2 = stockbuy2;
	istocksale1 = stocksale1;
	istocksale2 = stocksale2;
}

void PairTraderAlgo::run()
{
	qDebug() << "config stock path:" << config->stockPath;
	qDebug() << "config stock format:" << config->stockFormat;
	qDebug() << "pair trade stock1:" << stock1 << ",stock2:" << stock2;
	qDebug() << "pair trade minCor:" << minCor << ",maxCor:" << maxCor;
	qDebug() << "pair trade minCount:" << minCount;
	qDebug() << "pair trade longTermHoldStock:" << longTermHoldStock;
	qDebug() << "pair trade axis offset:" << axisOffset;
	qDebug() << "pair trade bandwidth:" << bandWidth;

	/*
#ifndef SUPPORT_HK_MARKET
	if (stock1.startsWith("hk") || stock1.size() == 7)
		return;
	if (stock2.startsWith("hk") || stock2.size() == 7)
		return;
#endif*/

	{
		auto file = getStockFileByPathAndFormatAndCode(config->stockMarket,config->stockPath,config->stockFormat,stock1);
		if (file.isFile() && file.exists())
		{
			stockA = config->readStocksFromFile(file.absoluteFilePath());
		}
	}
	{
		auto file = getStockFileByPathAndFormatAndCode(config->stockMarket,config->stockPath,config->stockFormat,stock2);
		if (file.isFile() && file.exists())
		{
			stockB = config->readStocksFromFile(file.absoluteFilePath());
		}
	}

	qDebug() << "pair trade stock1 count:" << stockA.size();
	qDebug() << "pair trade stock2 count:" << stockB.size();

	QVector<Stock> a(stockA);
	QVector<Stock> b(stockB);

	unionStocks(a,b);
	if (a.size() < minCount)
		return;

	p = 100.0*pearsonStock(a,b);
	if (p < minCor || p > maxCor)
		return;

	qDebug() << "pair trade pearson p:" << p;

	double avg1 = stock_mean(a,Stock::TAG_CLOSE);
	double avg2 = stock_mean(b,Stock::TAG_CLOSE);
	double d = avg1 / avg2;

	if (!isSameValue(k,0.0))
		d = k;

	qDebug() << "pair trade ratio:" << d;

	for (int i = 0;i < a.size();i++)
		data += b[i].current/a[i].current*d;

	float maxValue = *std::max_element(data.begin(),data.end());
	qDebug() << "pair trade max element:" << maxValue;

	float minValue = *std::min_element(data.begin(),data.end());
	qDebug() << "pair trade min element:" << minValue;

	float baseAxis = axisOffset + 1.0;

	qDebug() << "baseValue:" << bandWidth
		<< ",min:" << minValue << ",maxValue:" << maxValue
		<< ",baseAxis:" << baseAxis;

	double top = baseAxis + bandWidth;
	double low = baseAxis - bandWidth;
	qDebug() << "top:" << top << ",low:" << low;

	for (int i = 1;i < data.size();i++)
	{
		if (data[i] > top && data[i-1] < top)
		{
			stockbuy1.insert(a[i].date);
			stocksale2.insert(b[i].date);
			qDebug() << stock1 << ",buy1:" << qPrintable(a[i].date) << "," << qPrintable(b[i].date);
		}
		else if (data[i] < low && data[i-1] > low)
		{
			stockbuy2.insert(a[i].date);
			stocksale1.insert(b[i].date);
			qDebug() << stock2 << ",buy2:" << qPrintable(a[i].date) << "," << qPrintable(b[i].date);
		}
		else if(longTermHoldStock)
		{
			stockbuy1.insert(a[i].date);
			stockbuy2.insert(b[i].date);
			qDebug() << stock1<<","<<stock2 << ",buy1,2:" << qPrintable(a[i].date) << "," << qPrintable(b[i].date);
		}
	}
}

