#include "evalutor/QuantStatement.h"
#include "lv2/StockLV2Reader.h"
#include "StockKAnalyzer.h"
#include "Indicator.h"
#include "SnailContext.h"
#include "DataProvider.h"
#include "ConfigurationParser.h"
#include <iostream>

class QuantStatementAdaptor_Value : public QuantStatement
{
public:
	QuantStatementAdaptor_Value()
	{
		value = 0.0;
	}

	QuantStatementAdaptor_Value(Span span)
	{
		value = 0.0;
	}

	QStringList getKeywords()const
	{
		QStringList output;
		output.append("value.");
		return output;
	}

	QString getMeta()const
	{
		return flag;
	}

	Span getSpan()const{ return normal; }
	bool returnBool()const{ return false; }
	bool prepare(const QString& meta,const QVector<double>& argument)
	{
		flag = meta;
		if (argument.size() == 1)
		{
			value = argument[0];
		}
		else
			return false;

		return true;
	}

	bool evalute(int pos,QVariant& result,const QString& tag = QString())
	{
		result.setValue<bool>(value);
		return true;
	}

	void clear()
	{
	}
private:
	QString flag;
	double value;
};

class QuantStatementAdaptor_MACD : public QuantStatement
{
public:
	QuantStatementAdaptor_MACD():QuantStatementAdaptor_MACD(normal)
	{
	}

	QuantStatementAdaptor_MACD(Span spn)
	{
		span = spn;
		a = 12;
		b = 26;
		c = 9;
		offset = 0;
	}
	
	QStringList getKeywords()const
	{
		QStringList output;
		output.append("macd.bar");
		output.append("macd.diff");
		output.append("macd.dea");
		return output;
	}
	
	QString getMeta()const
	{
		return meta;
	}
	
	Span getSpan()const{ return span; }
	bool returnBool()const{ return false; }
	bool prepare(const QString& tag,const QVector<double>& argument)
	{
		if (!getKeywords().contains(tag))
			return false;
		meta = tag;

		if (argument.size() == 1)
		{
			offset = argument[0];
		}
		else if (argument.size() == 4)
		{
			a = argument[0];
			b = argument[1];
			c = argument[2];
			offset = argument[3];
		}
		else
			return false;

		if (c < 5 || a < 5 || a + 5 > b || offset > 0)
			return false;

		return true;
	}
	
	bool evalute(int pos,QVariant& result,const QString& tag = QString())
	{
		if (span == normal && stockData.isEmpty())
		{
			auto stocks = fetchStockData(dataProvider.code,pos,span);

			MacdIndicator indicator(a,b,c);
			stockData = QuantStatement::clacData(stocks,&indicator);
			if (stockData.isEmpty())
				return false;
		}
		else
		{
		}

		int current = offset;
		if (span == normal)
			current += pos;
		else
		{

		}

		if (current < 0 || current >= stockData.size())
			return false;

		auto item = stockData[current];
		if (!item._1)
			return false;

		/*QString desc = tag;
		if (desc.isEmpty())
			desc = meta;*/

		double bar = item._2;
		double diff = item._3;
		double dea = item._4;

		if (meta == "macd.diff")
		{
			result.setValue<double>(diff);
		}
		else if (meta == "macd.dea")
		{
			result.setValue<double>(dea);
	    }   
		else
		{
			result.setValue<double>(bar);
		}
		/*if (current >= stockData.size() - 3)
		{
			std::cout << stockData.size() << " " << current << std::endl;
			std::cout << meta.toStdString() << " " << dataProvider.code.toStdString() << ","
				<< dataProvider.stocks[current].date.toStdString() << "," << result.toDouble() << std::endl;
			std::cout << item._2 << " " << item._3 << " " << item._4 << std::endl;
		}*/
		return true;
	}
	
	void clear()
	{
		stockData.clear();
	}
private:
	Span span;
	int a,b,c;
	int offset;
	QString meta;
	DataWrapper5s stockData;
};

class QuantStatementAdaptor_BOLL : public QuantStatement
{
public:
	QuantStatementAdaptor_BOLL()
	{
		span = normal;
		a = 20;
		b = 2;
		offset = 0;
		nodata = false;
	}

	QuantStatementAdaptor_BOLL(Span spn)
	{
		span = spn;
		a = 20;
		b = 2;
		offset = 0;
		nodata = false;
	}

	QStringList getKeywords()const
	{
		QStringList output;
		output.append("boll.mid");
		output.append("boll.upper");
		output.append("boll.lower");
		output.append("boll.width");
		return output;
	}

	QString getMeta()const
	{
		return flag;
	}

	Span getSpan()const{ return span; }
	bool returnBool()const{ return false; }
	bool prepare(const QString& meta,const QVector<double>& argument)
	{
		flag = meta;
		if (argument.size() == 1)
		{
			offset = argument[0];
		}
		else if (argument.size() == 3)
		{
			a = argument[0];
			b = argument[1];
			offset = argument[2];
		}
		else
			return false;

		if (b < 12 || b < 1 || offset > 0)
			return false;

		return true;
	}

	bool evalute(int pos,QVariant& result,const QString& tag = QString())
	{
		if (data.isEmpty() && !nodata)
		{
			auto stocks = fetchStockData(dataProvider.code,pos,span);

			BollIndicator indicator(a,b);
			data = QuantStatement::clacData(stocks,&indicator);
			if (data.isEmpty())
			{
				nodata = true;
			}
			return false;
		}

		int current = data.size() + offset;
		if (current < 0 || current >= data.size())
			return false;

		auto item = data[current];
		if (!item._1)
			return false;

		QString desc = tag;
		if (desc.isEmpty())
			desc = flag;

		if (desc == "boll.lower")
			result.setValue<double>(item._2 - b * item._3);
		else if (desc == "boll.upper")
			result.setValue<double>(item._2 + b * item._3);
		else if (desc == "boll.width")
			result.setValue<double>(item._3/item._2*100.0);
		else
			result.setValue<double>(item._2);
		return true;
	}

	void clear()
	{
		data.clear();
	}
private:
	Span span;
	int a,b;
	int offset;
	QString flag;
	bool nodata;
	DataWrapper5s data;
};

class QuantStatementAdaptor_K : public QuantStatement
{
public:
	QuantStatementAdaptor_K():QuantStatementAdaptor_K(normal)
	{
	}

	QuantStatementAdaptor_K(Span spn)
	{
		span = spn;
		offset = 0;
	}

	QStringList getKeywords()const
	{
		return StockKAnalyzer::instance()->keywords();
	}

	QString getMeta()const
	{
		return meta;
	}

	Span getSpan()const{ return span; }
	bool returnBool()const{ return true; }
	bool prepare(const QString& tag,const QVector<double>& argument)
	{
		meta = tag;
		if(argument.size() == 1)
		{
			offset = argument[0];
		}
		else
			return false;

		if (offset > 0)
			return false;
		return true;
	}

	bool evalute(int pos,QVariant& result,const QString& tag = QString())
	{
		auto stocks = fetchStockData(dataProvider.code,pos,span);
		int current = stocks.size() + offset;
		if (current < 0 || current >= stocks.size())
			return false;

		if (span == normal)
			stocks = stocks.mid(0,current+1);

		bool ok = StockKAnalyzer::instance()->isThisK(meta,stocks);
		result.setValue<bool>(ok);
		return true;
	}

	void clear()
	{
	}
private:
	Span span;
	int offset;
	QString meta;
};

/*
class QuantStatementAdaptor_OHLC : public QuantStatement
{
public:
	QuantStatementAdaptor_OHLC()
	{
		span = normal;
		a = 20;
		b = 2;
		offset = 0;
		nodata = false;
	}

	QuantStatementAdaptor_OHLC(Span spn)
	{
		span = spn;
		a = 20;
		b = 2;
		offset = 0;
		nodata = false;
	}

	QStringList getKeywords()const
	{
		QStringList output;
		output.append("boll.mid");
		output.append("boll.upper");
		output.append("boll.lower");
		output.append("boll.width");
		return output;
	}

	QString getMeta()const
	{
		return flag;
	}

	Span getSpan()const{ return span; }
	bool returnBool()const{ return false; }
	bool prepare(const QString& meta,const QVector<int>& argument)
	{
		flag = meta;
		if (argument.size() == 1)
		{
			offset = argument[0];
		}
		else if (argument.size() == 3)
		{
			a = argument[0];
			b = argument[1];
			offset = argument[2];
		}
		else
			return false;

		if (b < 12 || b < 1 || offset > 0)
			return false;

		return true;
	}

	bool evalate(int pos,QVariant& result,const QString& tag = QString())
	{
		if (data.isEmpty() && !nodata)
		{
			auto stocks = fetchStockData(dataProvider.code,pos,span);

			BollIndicator indicator(a,b);
			data = QuantStatement::clacData(stocks,&indicator);
			if (data.isEmpty())
			{
				nodata = true;
			}
			return false;
		}

		int current = data.size() + offset;
		if (current < 0 || current >= data.size())
			return false;

		auto item = data[current];
		if (!item._1)
			return false;

		QString desc = tag;
		if (desc.isEmpty())
			desc = flag;

		if (desc == "boll.lower")
			result.setValue<double>(item._2 - b * item._3);
		else if (desc == "boll.upper")
			result.setValue<double>(item._2 + b * item._3);
		else if (desc == "boll.width")
			result.setValue<double>(item._3 / item._2*100.0);
		else
			result.setValue<double>(item._2);
		return true;
	}

	void clear()
	{
		data.clear();
	}
private:
	Span span;
	int offset;
	QString flag;
};*/

#ifdef SUPPORT_LEVEL2
struct QuantStatement_LV2 : public QuantStatement
{
	int offset;
	QString flag;

	QuantStatement_LV2()
	{
		offset = 0;
	}

	bool returnBool()const{ return false; }
	QStringList getKeywords()const
	{
		QStringList items;
		items.append("lv2.散户买单金额");
		items.append("lv2.散户卖单金额");
		items.append("lv2.中买单金额");
		items.append("lv2.中买单数");
		items.append("lv2.中卖单金额");
		items.append("lv2.中卖单数");
		items.append("lv2.大买单金额");
		items.append("lv2.大买单数");
		items.append("lv2.大卖单金额");
		items.append("lv2.大卖单数");
		items.append("lv2.超大买单金额");
		items.append("lv2.超大卖单金额");
		items.append("lv2.超大买单数");
		items.append("lv2.超大卖单数");
		items.append("lv2.成交金额");
		return items;
	}

	bool prepare(const QString& meta,const QVector<double>& argument)
	{
		flag = meta;
		if (argument.size() == 1)
			offset = argument[0];
		else
			return false;

		if(offset > 0)
			return false;

		return true;
	}

	QString getMeta()const{return flag;}
	Span getSpan()const{ return normal; }

	bool evalute(int pos,QVariant& output,const QString& meta = QString())
	{
		int current = pos + offset;
		auto stocks = fetchStockData(dataProvider.code,pos,normal);
		if (current <= 0 || current >= stocks.size())
			return false;

		QString stock = dataProvider.code;
		QString date = stocks[current].date;

		QString zip = config.stockLevel2Path + "//" + date + ".zip";
		QString file = date + "/" + stock + ".csv";

		QMap<QString,StockLV2> lv2Data = StockLV2Reader::getInstance().getLV2Data();
		if (lv2Data.find(date) == lv2Data.end())
			StockLV2Reader::getInstance().readFileFromZip(zip,file,date);

		lv2Data = StockLV2Reader::getInstance().getLV2Data();
		if (lv2Data.find(date) == lv2Data.end())
			return false;

		StockLV2 lv2 = lv2Data[date];

		if (flag == "lv2.散户买单金额")
			output.setValue<double>(lv2.tinyBuy);
		else if (flag == "lv2.散户卖单金额")
			output.setValue<double>(lv2.tinySale);
		else if (flag == "lv2.中买单金额")
			output.setValue<double>(lv2.normalBuy);
		else if (flag == "lv2.中卖单金额")
			output.setValue<double>(lv2.normalSale);
		else if (flag == "lv2.大买单金额")
			output.setValue<double>(lv2.bigBuy);
		else if (flag == "lv2.大卖单金额")
			output.setValue<double>(lv2.bigSale);
		else if (flag == "lv2.超大买单金额")
			output.setValue<double>(lv2.overBuy);
		else if (flag == "lv2.超大卖单金额")
			output.setValue<double>(lv2.overSale);
		else if (flag == "lv2.中买单数")
			output.setValue<double>(lv2.normalBuyCount);
		else if (flag == "lv2.中卖单数")
			output.setValue<double>(lv2.normalSaleCount);
		else if (flag == "lv2.大买单数")
			output.setValue<double>(lv2.bigBuyCount);
		else if (flag == "lv2.大卖单数")
			output.setValue<double>(lv2.bigSaleCount);
		else if (flag == "lv2.超大买单数")
			output.setValue<double>(lv2.overBuyCount);
		else if (flag == "lv2.超大卖单数")
			output.setValue<double>(lv2.overSaleCount);
		else if (flag == "lv2.成交金额")
			output.setValue<double>(lv2.captical / 10000);
		else
			return false;
		return true;
	}

	void clear()
	{
	}
};

#endif

QStringList QuantStatementFactory::keywords()const
{
	QStringList output;
	output << "value";
	output << "周线." << "半月线." << "月线.";
	output << "分时5分钟" << "分时10分钟" << "分时15分钟" << "分时30分钟" << "分时60分钟";
#ifdef SUPPORT_LV2
	output.append("lv2.散户买单金额");
	output.append("lv2.散户卖单金额");
	output.append("lv2.中买单金额");
	output.append("lv2.中买单数");
	output.append("lv2.中卖单金额");
	output.append("lv2.中卖单数");
	output.append("lv2.大买单金额");
	output.append("lv2.大买单数");
	output.append("lv2.大卖单金额");
	output.append("lv2.大卖单数");
	output.append("lv2.超大买单金额");
	output.append("lv2.超大卖单金额");
	output.append("lv2.超大买单数");
	output.append("lv2.超大卖单数");
	output.append("lv2.成交金额");
#endif 

	output << StockKAnalyzer::instance()->keywords();
	output << "macd.bar" << "macd.diff" << "macd.dea";
	output << "boll.mid" << "boll.upper" << "boll.lower";
	output << "kdj.k" << "kdj.d" << "kdj.j";
	output << "ohlc.open" << "ohlc.close" << "ohlc.low" << "ohlc.swing" << "ohlc.increase" << "ohlc.vol";

	return output;
}

QuantStatement* QuantStatementFactory::create(const QString& tag)
{
	auto node = QuantStatement::splitArgument(tag);
	QuantStatement* ptr = nullptr;

	//std::cout << "span:"<<node._1 << "," << node._2.toStdString() << std::endl;

	if (node._2.startsWith("lv2."))
	{
		if (node._1 != QuantStatement::normal)
			return ptr;
		ptr = new QuantStatement_LV2();
	}

#define CHECK_AND_SET(str,flag)\
	if (node._2.startsWith(str))\
	{\
		ptr = new flag(node._1);\
		bool ok = ptr->prepare(node._2,node._3);\
		if (!ok)\
		{\
			delete ptr;\
			ptr = nullptr;\
		}\
        else\
            return ptr;\
	}

	CHECK_AND_SET("macd.",QuantStatementAdaptor_MACD)
	CHECK_AND_SET("boll.",QuantStatementAdaptor_BOLL)
	CHECK_AND_SET("k.",QuantStatementAdaptor_K)
	CHECK_AND_SET("value",QuantStatementAdaptor_Value)
	std::cout << "ptr:" << ptr << std::endl;
	return ptr;
}