#include <QFileInfo>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qset.h>
#include <QtCharts/qcandlestickset.h>
#include "dialog/SnailStockMsgBox.h"
#include "Stock.h"
#include "ConfigurationParser.h"
#include "common/common.h"

/*
QMap<QString,Stock> loadStocksFromPerBuffer(const QString& json)
{
	QMap<QString,Stock> output;
	QJsonDocument doc;
	QJsonParseError error;
	doc = QJsonDocument::fromJson(json.toLatin1(),&error);
	if(doc.isArray() && error.error == QJsonParseError::NoError)
	{
		Stock stock;
		auto list = doc.array();
		for (int i = 0;i < list.count();i++)
		{
			const auto ref = list[i];
			auto object = ref.toObject();
			
			if(object.contains("code"))
				stock.code = object["code"].toString();
			if(object.contains("amount"))
				stock.tradeVolume = object["amount"].toDouble();
			if(object.contains("changepercent"))
				stock.increase = object["changepercent"].toDouble();
			if(object.contains("high"))
				stock.top = object["high"].toDouble();
			if(object.contains("low"))
				stock.low = object["low"].toDouble();
			if(object.contains("open"))
				stock.open = object["open"].toDouble();
			if(object.contains("volume"))
				stock.tradeVolume = object["volume"].toDouble();
			if(object.contains("turnover"))
				stock.turnover = object["turnover"].toDouble();
			if(object.contains("trade"))
				stock.current = object["trade"].toDouble();
			if(object.contains("settlement"))
				stock.privClose = object["settlement"].toDouble();
			if(object.contains("mktcap"))
				stock.data[Stock::TAG_ZONGSHIZHI] = object["mktcap"].toDouble();
			if(object.contains("nmc"))
				stock.data[Stock::TAG_LIUTONGSHIZHI] = object["nmc"].toDouble();
			if(object.contains("pb"))
				stock.data[Stock::TAG_PB] = object["pb"].toDouble();
			if(object.contains("per"))
				stock.data[Stock::TAG_PE_D] = object["per"].toDouble();

			stock.code = buildStockCodeByDigitStockCode(stock.code);
			output[stock.code] = stock;
		}
	}

	return output;
}

/*
QMap<QString,QMap<QString,QString>> loadPerDataFromJsonContent(const QString& json)
{
	QMap<QString,QMap<QString,QString>> output;
	
	QJsonDocument doc;
	QJsonParseError error;
	doc = QJsonDocument::fromJson(json.toLatin1(),&error);
	if(doc.isArray() && error.error == QJsonParseError::NoError)
	{
		auto list = doc.array();
		for(int i = 0; i < list.count(); i++)
		{
			const auto ref = list[i];
			auto object = ref.toObject();

			QString code;
			QMap<QString,QString> unit;

			if(object.contains("code"))
			{
				code = object["code"].toString();
				unit.insert("code",code);
			}

			auto keys = object.keys();
			foreach(auto key,keys)
			{
				if(object[key].type() == QJsonValue::Type::String)
					unit.insert(key, object[key].toString());
				else
				{
					QString str = STR_CEIL(object[key].toDouble());
					unit.insert(key,str);
				}
			}

			if(config.stockList.contains(code))
				unit.insert("name",config.stockList[code].name);

			//stock.code = buildStockCodeByDigitStockCode(stock.code);
			output.insert(code,unit);
		}
	}

	return output;
}*/

QVector<Stock> stock_toLogStock(const QVector<Stock>& stocks)
{
	QVector<Stock> output;
    if(stocks.size() == 1)
		output.append(stocks[0]);
	else if(!stocks.isEmpty())
	{
		Stock one = stocks[0];

		float current = 0.0f;
		for(int i=1;i<stocks.size();i++)
		{
		    Stock stk = stocks[i];
			Stock temp = stocks[i-1];
			stk.current = 10.0f*(stk.current - temp.current)/temp.current;
			stk.open = 10.0f*(stk.open - temp.current)/temp.current;
			stk.low = 10.0f*(stk.low - temp.current)/temp.current;
			stk.top = 10.0f*(stk.top - temp.current)/temp.current;
			stk.current += current;
			stk.open += current;
			stk.low += current;
			stk.top += current;
			current = stk.current;
			output.push_back(stk);
		}
	}

	return output;
}

int stock_getLowPosition(const QVector<Stock>& stocks,int from,int to,float& price,bool all)
{
	if(from > stocks.size()-1 || to > stocks.size()-1 || from > to)
		return -1;

    DataWrapper2<int,float> d(-1.0,9999.99);

	for(int i=from;i<=to;i++)
	{
		if(stocks[i].current < d._2)
		{
			d._1 = i;
			d._2 = stocks[i].current;
		}

		if(all)
		{
			if(stocks[i].low < d._2)
			{
				d._1 = i;
				d._2 = stocks[i].low;
			}		
		}
	}

	price = d._2;
	return d._1;
}

int stock_getHighPosition(const QVector<Stock>& stocks,int from,int to,float& price,bool all)
{
	if(from > stocks.size()-1 || to > stocks.size()-1 || from > to)
		return -1;

    DataWrapper2<int,float> d(-1.0,9999.99);
	for(int i=from;i<=to;i++)
	{
		if(stocks[i].current > d._2)
		{
			d._1 = i;
			d._2 = stocks[i].current;
		}

		if(all)
		{
			if(stocks[i].top > d._2)
			{
				d._1 = i;
				d._2 = stocks[i].top;
			}
		}
	}

	price = d._2;
	return d._1;
}

bool stock_getMinMax(const QVector<Stock>& stocks, int from, int to, double& minv, double& maxv,bool all,bool* seq)
{
	if (from > stocks.size() - 1 || to > stocks.size() - 1 || from > to)
		return false;

	minv = std::numeric_limits<double>::max();
	maxv = std::numeric_limits<double>::min();

	int minIndex = from;
	int maxIndex = to;

	for(int i = from; i <= to; i++)
	{
		double minv_, maxv_;
		if(!all)
		{
			minv_ = std::min(minv,stocks[i].current);
			maxv_ = std::max(maxv,stocks[i].current);
		}
		else
		{
			minv_ = std::min(minv, stocks[i].low);
			maxv_ = std::max(maxv,stocks[i].top);
		}

		if(minv_ < minv)
			minIndex = i;

		if(maxv_ > maxv)
			maxIndex = i;

		minv = minv_;
		maxv = maxv_;
	}

	if (seq)
		*seq = minIndex < maxIndex;
	return true;
}

double stock_getAvg(const QVector<Stock>& stocks,int from,int to,bool vol)
{
	double sum = 0.0;
	for(int i=from;i<=to;i++)
	{
		if(vol == true)
			sum += stocks[i].tradeVolume;
		else
			sum += stocks[i].current;
	}

	sum /= (double)(to-from+1);
	return sum;
}

void stock_findStockPolar(const QVector<Stock>& list,int& toppos,int& lowpos,float& top,float& low)
{
    top = 0.0f;
	low = 99999.9f;
	for(int i=0;i<list.size();i++)
	{
	    if(list[i].top>top)
		{
		    top = list[i].top;
			toppos = i;
		}
		if(list[i].low < low)
		{
		    low = list[i].low;
			lowpos = i;
		}
	}
}

QVector<Stock> getStockSpanK(const QVector<Stock>& stocks,int count)
{
	if(count < 2)
		return stocks;

    QVector<Stock> output;
	auto input(stocks);
	int index = input.size();

	while(true)
	{
		index -= count;
		if(index < count)
			break;
	    QVector<Stock> group = input.mid(index,count);
	    Stock unit;
		unit.code = group[count-1].code;
		unit.date = group[count-1].date;
		int toppos,lowpos;
		float top,low;
		stock_findStockPolar(group,toppos,lowpos,top,low);
		unit.low = low;
		unit.top = top;
		unit.open = group[0].open;
		unit.current = group[count-1].current;
		foreach(Stock stock,group)
		{
			unit.tradeVolume += stock.tradeVolume;
			unit.tradeCapital += stock.tradeCapital;
			unit.turnover += stock.turnover;
		}

		unit.increase = GET_INCREASE(unit.current,input[index-1].current);
		output.push_back(unit);
	}

	std::reverse(output.begin(),output.end());
	return output;
}

void unionStocks(QVector<Stock>& stock1,QVector<Stock>& stock2)
{
	QSet<QString> s1,s2;
	foreach(auto item,stock1)
		s1 += item.date;
	foreach(auto item,stock2)
		s2 += item.date;
	QSet<QString> s = s1.intersect(s2);

	QVector<Stock> st1,st2;
	foreach(auto item,stock1)
		if (s.contains(item.date))
			st1.append(item);

	foreach(auto item,stock2)
		if (s.contains(item.date))
			st2.append(item);

	stock1.swap(st1);
	stock2.swap(st2);
}

void bindStockDataToCandlestickSeries(QCandlestickSeries* series,const QVector<Stock>& stocks)
{
	series->clear();
	foreach(auto item,stocks)
	{
		QDateTime date = QDateTime::fromString(item.date,"yyyy-MM-dd");
		QCandlestickSet* unit = new QCandlestickSet(date.toTime_t(),series);	
		unit->setOpen(item.open);
		unit->setHigh(item.top);
		unit->setLow(item.low);
		unit->setClose(item.current);
		series->append(unit);
	}
}

QMap<QString,Stock> vecToMapStock(const QVector<Stock>& stocks)
{
	QMap<QString,Stock> output;
	foreach(auto item,stocks)
	{
	    output.insert(item.date,item);
	}

	return output;
}