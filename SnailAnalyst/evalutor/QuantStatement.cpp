#include <tuple>
#include "config.h"
#include "ConfigurationParser.h"
#include "evalutor/QuantStatement.h"
#include "eval/cppeval.h"
#include "DataProvider.h"
#include "Indicator.h"
#include <iostream>

DataWrapper3<QuantStatement::Span,QString,QVector<double>> QuantStatement::splitArgument(const QString& command)
{
	DataWrapper3<QuantStatement::Span,QString,QVector<double>> data;
	data._1 = normal;

	auto items = command.split(",");

	if (items.isEmpty())
		return data;

	bool init = false;

#define CHECK_AND_SET(str,flag)\
    if(!init)\
    {\
        QString tag(str);\
        if(items[0].startsWith(tag))\
		{\
	        data._2 = items[0].mid(QString(str).size());\
            data._1 =  flag;\
            init = true;\
		}\
	}

	CHECK_AND_SET("周线.",QuantStatement::day5)
	CHECK_AND_SET("半月线.",QuantStatement::day10)
	CHECK_AND_SET("月线.",QuantStatement::day20)
	CHECK_AND_SET("分时5分钟.",QuantStatement::minute5)
	CHECK_AND_SET("分时10分钟.",QuantStatement::minute10)
	CHECK_AND_SET("分时15分钟.",QuantStatement::minute15)
	CHECK_AND_SET("分时30分钟.",QuantStatement::minute30)
	CHECK_AND_SET("分时60分钟.",QuantStatement::minute60)
	
	if (!init)
		data._2 = items[0];

	for(int i = 1;i < items.size();i++)
	{
        double val = evalute_express(items[1]);
	    data._3.push_back(val);
    }
	return data;
}

QVector<Stock> QuantStatement::fetchStockData(const QString& stock,int pos,Span span)
{
	QVector<Stock> output;
	if(span == normal)
		output = dataProvider.stocks;
	else if (span > normal && span <= day20)
	{
		int count = 5;
		if (span == day10)
			count = 10;
		else if (span == day20)
			count = 20;
		output = getStockSpanK(dataProvider.stocks.mid(0,pos+1),count);
	}
	else if (span >= minute60)
	{
		if (pos >= config.maxTickFileLoaded)
		{
			QStringList dates;
			for (int i = 0;i < config.maxTickFileLoaded;i++)
				dates.append(dataProvider.stocks[pos - config.maxTickFileLoaded + i + 1].date);

			foreach(QString item,dates)
			{
				if (dataProvider.tickData.find(item) == dataProvider.tickData.end())
				{
				    bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,item);
					if (!ok)
						break;
			    }
			}

			int count = 30;
			if (span == minute5)
				count = 5;
			else if (span == minute10)
				count = 10;
			else if (span == minute15)
				count = minute15;
			else if (span == minute60)
				count = minute60;

			foreach(QString item,dates)
			{
				output += convertToTimeLine(dataProvider.tickData[item],count);
			}
		}
	}

	return output;
}

DataWrapper5s QuantStatement::clacData(const QVector<Stock>& stocks,StockIndicator* indicator)
{
	DataWrapper5s data;
	foreach(auto item,stocks)
	{
		//std::cout << item.date.toStdString() << " " << item.current << std::endl;
		auto value = indicator->update(item);
		//std::cout << value._2 << " " << value._3 << " " << value._4 << std::endl;
		data.push_back(value);
	}
	return data;
}