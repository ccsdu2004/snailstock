#include <QMap>
#include <functional>
#include <qdebug>
#include "MetaEvalutor.h"
#include "StockScriptEngine.h"
#include "k/StockKAnalyzer.h"
#include "StockTickScanner.h"
#include "Indicator.h"
#include "VarMeta.h"
#include "Stock.h"
#include "config.h"
#include "fma/Fma_indicator.h"
#ifdef SUPPORT_SCRIPT
#include "script/ScriptManager.h"
#endif
#include "eval/cppeval.h"
#ifdef SUPPORT_LEVEL2
#include "lv2/StockLV2Reader2019.h"
#endif
#include "config/ConfigItem.h"
#include "math/UnitAlgo.h"
#include "SnailContext.h"
#include "ObjectFactory.h"
#include "DataProvider.h"
#ifdef SUPPORT_GPDX
#include "gpdx/GPDX.h"
#endif
#include "addon/Addon_Boll_C.h"

#define REBUILD_SPAN_DATA \
if (span == day5)\
{\
	stocks = getStockSpanK(stocks.mid(0,current + 1),5);\
	current = stocks.size() + offset - 1;\
	data = getIndicatorValue(stocks,&indicator);\
}\
else if (span == day10)\
{\
	stocks = getStockSpanK(stocks.mid(0,current + 1),10);\
	current = stocks.size() + offset - 1;\
	data = getIndicatorValue(stocks,&indicator);\
}\
else if (span == day20)\
{\
	stocks = getStockSpanK(stocks.mid(0,current + 1),20);\
	current = stocks.size() + offset - 1;\
	data = getIndicatorValue(stocks,&indicator);\
}\
else if(span == minute5)\
{\
    stocks = getTickData(dataProvider.code,current,5);\
	current = stocks.size() + offset - 1;\
	data = getIndicatorValue(stocks,&indicator);\
}\
else if(span == minute10)\
{\
    stocks = getTickData(dataProvider.code,current,10);\
	current = stocks.size() + offset - 1;\
	data = getIndicatorValue(stocks,&indicator);\
}\
else if(span == minute15)\
{\
    stocks = getTickData(dataProvider.code,current,15);\
	current = stocks.size() + offset - 1;\
	data = getIndicatorValue(stocks,&indicator);\
}\
else if(span == minute30)\
{\
    stocks = getTickData(dataProvider.code,current,30);\
	current = stocks.size() + offset - 1;\
	data = getIndicatorValue(stocks,&indicator);\
}\
else if (span == minute60)\
{\
    stocks = getTickData(dataProvider.code,current,60);\
    current = stocks.size() + offset - 1;\
    data = getIndicatorValue(stocks,&indicator);\
}\
else\
{\
}

QStringList MetaEvalutor::splitStatement(const QString& statement,const QString& regex)
{
	QStringList output;
	QRegExp rx(regex);
	int index = rx.indexIn(statement);
	if (index >= 0)
	{
		int count = rx.captureCount();
		for (int i = 1;i < count + 1;i++)
			output.append(rx.cap(i));
	}
	else
		output.append(statement);
	return output;
}

DataWrapper3<MetaEvalutor::Span,QString,QVector<double>> MetaEvalutor::splitArgument(const QString& command)
{
	DataWrapper3<MetaEvalutor::Span,QString,QVector<double>> data;
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

	CHECK_AND_SET("周线.",MetaEvalutor::day5)
	CHECK_AND_SET("半月线.",MetaEvalutor::day10)
	CHECK_AND_SET("月线.",MetaEvalutor::day20)
	CHECK_AND_SET("分时5分钟.",MetaEvalutor::minute5)
	CHECK_AND_SET("分时10分钟.",MetaEvalutor::minute10)
	CHECK_AND_SET("分时15分钟.",MetaEvalutor::minute15)
	CHECK_AND_SET("分时30分钟.",MetaEvalutor::minute30)
	CHECK_AND_SET("分时60分钟.",MetaEvalutor::minute60)

	if (!init)
		data._2 = items[0];

	for (int i = 1;i < items.size();i++)
	{
		double val = evalute_express(items[i]);
		data._3.push_back(val);
	}
	return data;
}

DataWrapper5s MetaEvalutor::getIndicatorValue(const QVector<Stock>& stocks,StockIndicator* indicator)
{
	DataWrapper5s data;
	foreach(auto item,stocks)
		data.push_back(indicator->update(item));
	return data;
}

QVector<Stock> MetaEvalutor::getTickData(const QString& code,int current,int k)
{
	QVector<Stock> stocks;

#ifdef SUPPORT_TICKET
	QString date = dataProvider.stocks[current].date;
	bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
	if (!ok)
		return stocks;

	int count = 5;
	for (int i = current - 1;i >= current - 4;i--)
	{
		QString date = dataProvider.stocks[i].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
		{
			count = current - i;
			break;
	    }
    }  

	for(int i = current - count+1;i <= current;i++)
	{
		QString date = dataProvider.stocks[i].date;
		stocks += convertToTimeLine(dataProvider.tickData[date],date,k);
	}
#endif
	return stocks;
}

MetaEvalutor::~MetaEvalutor()
{
}

QStringList MetaEvalutor::keywords()
{
	QStringList items;
	items.append("ohlc.open");
	items.append("ohlc.high");
	items.append("ohlc.low");
	items.append("ohlc.close");
	items.append("ohlc.increase");
	items.append("ohlc.swing");
	items.append("ohlc.vol");

	items.append("ohlc.总市值");
	items.append("ohlc.换手率");
	items.append("ohlc.成交金额");

	items.append("boll.upper");
	items.append("boll.mid");
	items.append("boll.lower");
	items.append("boll.width");

	items.append("ma");
	items.append("volma");

	items.append("macd.diff");
	items.append("macd.bar");
	items.append("macd.dea");

#ifdef SUPPORT_BASIC
	items.append("wad");
	items.append("obv");
	items.append("ad");
	items.append("bop");
	items.append("pvi");
	items.append("ema");
	items.append("kdj.k");
	items.append("kdj.d");
	items.append("kdj.j");

	items.append("dmi.pdi");
	items.append("dmi.mdi");
	items.append("dmi.adx");
	items.append("dmi.adxr");

#ifdef SUPPORT_hyperbola_algo
	items.append("fma");
#endif 

	items.append("bias");
	items.append("wr");
	items.append("psy");
	items.append("rsi");
	items.append("tr");
	items.append("pvi");
	items.append("obv");
	items.append("wad");
	items.append("ad");
	items.append("bop");

	items.append("value");
	items.append("pass");
#ifdef SUPPORT_TICKET
	items += StockTickAnalyzer::keywords();
#endif 

#ifdef SUPPORT_LEVEL2
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
	items.append("lv2.总卖单数");
	items.append("lv2.总买单数");

	items.append("lv2.净买入天数");
	items.append("lv2.净卖出天数");
#endif

#ifdef SUPPORT_BID
	items << "bid.买1" << "bid.买2" << "bid.买3" << "bid.买4" << "bid.买5";
	items << "bid.卖1" << "bid.卖2" << "bid.卖3" << "bid.卖4" << "bid.卖5";
	items << "bid.成交量" << "bid.成交额" << "bid.五档卖单和" << "bid.五档买单和" << "bid.价格" << "bid.涨幅";
#endif

#ifdef SUPPORT_PER
	items.append("per.换手率");
	items.append("per.昨收");
	items.append("per.low");
	items.append("per.high");
	items.append("per.open");
	items.append("per.close");
	items.append("per.成交额");
	items.append("per.成交量");
	items.append("per.振幅");
	items.append("per.涨幅");
	items.append("per.总市值");
	items.append("per.流通市值");
	items.append("per.pb");
	items.append("per.per");

	items.append("per.换手率前50");
	items.append("per.换手率前100");
	items.append("per.涨幅前50");
	items.append("per.涨幅前100");
	items.append("per.振幅前50");
	items.append("per.涨幅前100");
	items.append("per.成交量前50");
	items.append("per.成交量前100");
	items.append("per.成交额前50");
	items.append("per.成交额前100");
	items.append("per.高价前50");
	items.append("per.高价前100");
	items.append("per.低价前50");
	items.append("per.低价前100");
	items.append("per.小市值前50");
	items.append("per.小市值前100");
	items.append("per.大市值前50");
	items.append("per.大市值前100");
	items.append("per.涨停率");
#endif
#endif
#ifdef SUPPORT_PAIRS_TRADE
	items.append("pairtrade");
#endif

#ifdef SUPPORT_GPDX
	items.append("unit.高抛低吸.调整买入");
	items.append("unit.高抛低吸.升破买入");
#endif
	return items;
}

#ifdef SUPPORT_LEVEL2
struct MetaEvalutor_LV2 : public MetaEvalutor
{
    int offset;
	QString flag;
	QVector<Stock> stocks;

	int length = 0;

	MetaEvalutor_LV2()
	{
		offset = 0;
	}

	MetaEvalutor_LV2(Span spn):MetaEvalutor(spn)
	{
		offset = 0;
	}

	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		QStringList items;
		items.append("lv2.成交金额");
		items.append("lv2.成交量");
		items.append("lv2.总买数");
		items.append("lv2.总卖数");
		items.append("lv2.散买单金额");
		items.append("lv2.散卖单金额");
		items.append("lv2.散买单数");
		items.append("lv2.散卖单数");
		items.append("lv2.小买单金额");
		items.append("lv2.小卖单金额");
		items.append("lv2.小买单数");
		items.append("lv2.小卖单数");
		items.append("lv2.中买单金额");
		items.append("lv2.中卖单金额");
		items.append("lv2.中买单数");
		items.append("lv2.中卖单数");
		items.append("lv2.大买单金额");
		items.append("lv2.大卖单金额");
		items.append("lv2.大买单数");
		items.append("lv2.大卖单数");
		items.append("lv2.超大买单金额");
		items.append("lv2.超大卖单金额");
		items.append("lv2.超大买单数");
		items.append("lv2.超大卖单数");
		items.append("lv2.净卖出天数");
		items.append("lv2.净买入天数");
		return items;
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if(label=="lv2.净卖出天数" || label=="lv2.净买入天数")
		{
			if(arguments.size()==2)
			{
				length = arguments[0];
				if(length < 1)
					return false;

				offset = arguments[1];
				if(offset>0)
					return false;

				if(span!=normal)
					return false;

				flag = label;
				return true;
			}
			return false;
		}

		if (arguments.size() != 1)
			return false;

		if (!tag().contains(label))
			return false;

		offset = arguments[0];
		if (offset > 0)
			return false;

		if (span != normal)
			return false;

		flag = label;
		return true;
	}
	
	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0 || current >= stocks.size())
			return false;

		QString stock = dataProvider.code;
		if(stock.size() == 8)
			stock = stock.mid(2);
		
		if(flag=="lv2.净买入天数"||flag=="lv2.净卖出天数")
		{
			int cur = current;

			QStringList dates;
			for(int i = 0;i<length;i++)
			{
				cur --;
				if(cur<0)
					break;

				QString date = stocks[cur].date;
				dates << date;
			}

			int count = 0;

			foreach(auto date,dates)
			{
				auto lv2 = StockLV2Reader2019::getInstance().data(date);

				if(!lv2)
				{
					QString zip = config.stockLevel2Path+"//"+date+".zip";
					StockLV2Reader2019::getInstance().readFileFromZip(zip,stock);
				}

				lv2 = StockLV2Reader2019::getInstance().data(date);

				if(!lv2)
					continue;
				else
				{
					if(!lv2->valid)
						continue;

					if(flag=="lv2.净买入天数" && lv2->netIn())
						count ++;
					else if(flag=="lv2.净卖出天数" && lv2->netOut())
						count ++;
				}
			}				

			output.double_ = count;
			return true;
		}

		QString date = stocks[current].date;
		QString zip = config.stockLevel2Path + "//" + date + ".zip";
		bool ok = StockLV2Reader2019::getInstance().readFileFromZip(zip,stock);
		if(!ok)
			return false;

		auto lv2 = StockLV2Reader2019::getInstance().data(date);
		if(lv2 == nullptr)
			return false;

		if(!lv2->valid)
			return false;

		output.type = ResultType::TYPE_DOUBLE;
		if(flag == "lv2.成交金额")
			output.double_ = lv2->captical;
		else if(flag == "lv2.成交量")
			output.double_ = lv2->volume;
		else if(flag == "lv2.总买数")
			output.double_ = lv2->buyCount();
		else if(flag == "lv2.总卖数")
			output.double_ = lv2->saleCount();

		else if(flag == "lv2.散买单金额")
			output.double_ = lv2->tinyBuy;
		else if(flag == "lv2.散卖单金额")
			output.double_ = lv2->tinySale;
		else if(flag == "lv2.散买单数")
			output.double_ = lv2->tinyBuyCount;
		else if(flag == "lv2.散卖单数")
			output.double_ = lv2->tinySaleCount;

		else if(flag=="lv2.小买单金额")
			output.double_ = lv2->smallBuy;
		else if(flag=="lv2.小卖单金额")
			output.double_ = lv2->smallSale;
		else if(flag=="lv2.小买单数")
			output.double_ = lv2->smallBuyCount;
		else if(flag=="lv2.小卖单数")
			output.double_ = lv2->smallSaleCount;
		 
		else if(flag=="lv2.中买单金额")
			output.double_ = lv2->normalBuy;
		else if(flag=="lv2.中卖单金额")
			output.double_ = lv2->normalSale;
		else if(flag=="lv2.中买单数")
			output.double_ = lv2->normalBuyCount;
		else if(flag=="lv2.中卖单数")
			output.double_ = lv2->normalSaleCount;

		else if(flag=="lv2.大买单金额")
			output.double_ = lv2->bigBuy;
		else if(flag=="lv2.大卖单金额")
			output.double_ = lv2->bigSale;
		else if(flag=="lv2.大买单数")
			output.double_ = lv2->bigBuyCount;
		else if(flag=="lv2.大卖单数")
			output.double_ = lv2->bigSaleCount;

		else if(flag=="lv2.超大买单金额")
			output.double_ = lv2->overBuy;
		else if(flag=="lv2.超大卖单金额")
			output.double_ = lv2->overSale;
		else if(flag=="lv2.超大买单数")
			output.double_ = lv2->overBuyCount;
		else if(flag=="lv2.超大卖单数")
			output.double_ = lv2->overSaleCount;
		else
			return false;

		return true;
	}

	bool loadData()
	{
		stocks = dataProvider.stocks;
		return true;
	}

	void clear()
	{
	    stocks.clear();
	}
};

#endif

struct MetaEvalator_OHLC : public MetaEvalutor
{
    int offset;
	QVector<Stock> stocks;
	QString flag;
	int aux;

	MetaEvalator_OHLC()
	{
		offset = 0;
		aux = 0;
	}

	MetaEvalator_OHLC(Span spn):MetaEvalutor(spn)
	{
		offset = 0;
		aux = 0;
	}

	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QString("ohlc.open|ohlc.high|ohlc.low|ohlc.close|ohlc.increase|ohlc.swing|ohlc.vol|ohlc.总市值|ohlc.换手率|ohlc.成交金额").split("|");
	}
	bool build(const QString& label,const QVector<double>& arguments)
	{
		flag = label;

#ifdef SUPPORT_BASIC
		if(arguments.size() < 1 || arguments.size() > 2)
			return false;

		if(arguments.size() == 2 && flag == "ohlc.increase")
		{
			aux = arguments[0];
			offset = arguments[1];
			if(offset > 0)
				return false;
			if(aux < 1 || aux > 4)
				return false;
			return tag().contains(flag);
		}
        else if(arguments.size() == 2 && flag == "ohlc.swing")
		{
			aux = arguments[0];
			offset = arguments[1];

			if(offset > 0)
				return false;
			if(aux > 5 || aux < 1)
				return false;

			return tag().contains(flag);
		}
#endif 
		if(arguments.size() != 1)
			return false;

		offset = arguments[0];
		if(offset > 0)
			return false;
		return tag().contains(flag);
	}
	
	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (span == normal)
		{
			if (current < 0 || current >= stocks.size())
				return false;
		}

		auto data = stocks;

		if (span == day5)
		{
			data = getStockSpanK(data.mid(0,current + 1),5);
			current = data.size() + offset - 1;
		}
		else if (span == day10)
		{
			data = getStockSpanK(data.mid(0,current + 1),10);
			current = data.size() + offset - 1;
		}
		else if (span == day20)
		{
			data = getStockSpanK(data.mid(0,current + 1),20);
			current = data.size() + offset - 1;
		}
#ifdef  SUPPORT_TICKET
		else if (span == minute5)
		{
			data = getTickData(dataProvider.code,current,5);
			current = data.size() + offset - 1;
		}
		else if (span == minute10)
		{
			data = getTickData(dataProvider.code,current,10);
			current = data.size() + offset - 1;
		}
		else if (span == minute15)
		{
			data = getTickData(dataProvider.code,current,15);
			current = data.size() + offset - 1;
		}
		else if (span == minute30)
		{
			data = getTickData(dataProvider.code,current,30);
			current = data.size() + offset - 1;
		}
		else if (span == minute60)
		{
			data = getTickData(dataProvider.code,current,60);
			current = data.size() + offset - 1;
		}
#endif 
		if (current < 0)
			return false;

		output.type = ResultType::TYPE_DOUBLE;
		if (flag == "ohlc.open")
			output.double_ = data[current].open;
		else if (flag == "ohlc.high")
			output.double_ = data[current].top;
		else if (flag == "ohlc.low")
			output.double_ = data[current].low;
		else if (flag == "ohlc.close")
			output.double_ = data[current].current;
		else if (flag == "ohlc.vol")
			output.double_ = data[current].tradeVolume;
		else if (flag == "ohlc.increase")
		{
			if (aux == 0 || aux == 4)
				output.double_ = GET_INCREASE(data[current].current,data[current - 1].current);
			if (aux == 1)
				output.double_ = GET_INCREASE(data[current].open,data[current - 1].current);
			if (aux == 2)
				output.double_ = GET_INCREASE(data[current].top,data[current - 1].current);
			if (aux == 3)
				output.double_ = GET_INCREASE(data[current].low,data[current - 1].current);
		}
#ifdef SUPPORT_BASIC
		else if (flag == "ohlc.swing")
		{
			if (aux == 0)
				output.double_ = 100.0*(data[current].top - data[current].low) / data[current].low;
			else
			{
				double mid = data[current].current + data[current].open;
				mid *= 0.5;
				if (aux == 1)
					output.double_ = GET_INCREASE(data[current].top,mid);
				else if (aux == 2)
					output.double_ = GET_INCREASE(mid,data[current].low);
				else if (aux == 3)
					output.double_ = GET_INCREASE(data[current].current,data[current].open);
				else if (aux == 4)
					output.double_ = GET_INCREASE(data[current].open,data[current].low);
				else if (aux == 5)
					output.double_ = GET_INCREASE(data[current].top,data[current].current);
			}
		}
#endif
		else if (flag == "ohlc.总市值")
		{
			if (config.stockList.find(dataProvider.code) == config.stockList.end())
				return false;

			if (span != normal)
				return false;

			double total = config.stockList[dataProvider.code].data[Stock::TAG_ZONGSHIZHI].toDouble();
			output.double_ = data[current].current / stocks[data.size() - 1].current*total;
		}
		else if (flag == "ohlc.换手率")
			output.double_ = data[current].turnover;
		else if (flag == "ohlc.成交金额")
			output.double_ = data[current].tradeCapital;
		else
			return false;

		return true;
	}

	bool loadData()
	{
		stocks = dataProvider.stocks;
		return true;
	}

	void clear()
	{
	    stocks.clear();
	}
};

class MetaEvalutor_Double : public MetaEvalutor
{
	double value;
public:
	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QStringList("value");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if(arguments.size() != 1)
			return false;

		if(label != "value")
			return false;

		value = arguments[0];
		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = value;
		return true;
	}

	bool loadData()
	{	
		return true;
	}

	bool isConstValue()const { return true; }

	void clear()
	{
	}
};

class MetaEvalutor_Pass : public MetaEvalutor
{
public:
	MetaEvalutor_Pass(Span spn):
		MetaEvalutor(spn)
	{
	}
	bool returnBool()const{ return true; }
	const QStringList tag()const
	{
		return QStringList("pass");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if (label != "pass")
			return false;

		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		output.type = ResultType::TYPE_BOOL;
		output.bool_ = true;
		return true;
	}

	bool loadData()
	{
		return true;
	}

	bool isConstValue()const { return true; }

	void clear()
	{
	}
};

class MetaEvalator_BOLL : public MetaEvalutor
{
private:
    int offset;
	int n;
	double m;
	QVector<DataWrapper5<bool,float,float,float,float>> data;
	QString flag;
public:
	MetaEvalator_BOLL():MetaEvalator_BOLL(normal){}

	MetaEvalator_BOLL(Span spn):MetaEvalutor(spn)
	{
		n = 20;
		m = 2.0;
	}
	
	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QString("boll.upper|boll.mid|boll.lower|boll.width").split("|");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		flag = label;
#ifdef SUPPORT_BASIC
		if(arguments.size() != 3 && arguments.size() != 1)
			return false;

		if(arguments.size() == 3)
		{
			offset = arguments[2];
			n = arguments[0];
			m = arguments[1];
			if (offset > 0 || n < 10 || m < 1.5)
				return false;
			return true;
		}
#endif
		offset = arguments[0];
		if (offset > 0)
			return false;
		n = 20;
		m = 2;
		return tag().contains(flag);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (span == normal && (current < 0 || current >= data.size()))
			return false;

		auto stocks = dataProvider.stocks;
		BollIndicator indicator(n,m);

		REBUILD_SPAN_DATA

		if (current < 0)
			return false;

		auto item = data[current];
		if (!item._1)
			return false;

		double upper = item._2 + item._3;
		double midder = item._2;
		double lower = item._2 - item._3;

		output.type = ResultType::TYPE_DOUBLE;
		if (flag == "boll.upper")
			output.double_ = upper;
		else if (flag == "boll.mid")
			output.double_ = midder;
		else if (flag == "boll.lower")
			output.double_ = lower;
		else if (flag == "boll.width")
			output.double_ = item._3 / midder * 100.0;
		else
			return false;
		return true;
	}

	bool loadData()
	{
		if (span != normal)
			return true;

		data.clear();
		BollIndicator boll(n,m);
		foreach(auto stock,dataProvider.stocks)
		{
			auto d = boll.update(stock);
			data.push_back(d);
		}
		return true;
	}

	void clear()
	{
		data.clear();
	}
};

#define STR(s) #s  
#define TAG_EVALATOR(CName,INDICATOR,str) \
class CName : public MetaEvalutor\
{\
public:\
    CName(Span spn):MetaEvalutor(spn){}\
    int offset;\
	int n;\
	QVector<DataWrapper5<bool,float,float,float,float>> data;\
	bool returnBool()const{return false;}\
	const QStringList tag()const{return QString(str).split("|");}\
	bool build(const QString& tag,const QVector<double>& arguments)\
	{\
		if(arguments.size() != 2)\
            return false;\
        n = arguments[0];\
        offset = arguments[1];\
        if(n < 2)\
            return false;\
        if(offset > 0)\
            return false;\
		return true;\
	}\
	bool evalute(int pos,ResultType& output)\
	{\
        int current = pos+offset;\
        if (span == normal && (current < 0 || current >= data.size()))\
            return false;\
        auto stocks = dataProvider.stocks;\
        INDICATOR indicator(n);\
        if (span == day5)\
        {\
            stocks = getStockSpanK(stocks.mid(0,current + 1),5);\
	        current = stocks.size() + offset - 1;\
	        data = getIndicatorValue(stocks,&indicator);\
        }\
		else if (span == day10)\
		{\
			stocks = getStockSpanK(stocks.mid(0,current + 1),10);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
		else if (span == day20)\
		{\
			stocks = getStockSpanK(stocks.mid(0,current + 1),20);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
        else if(span == minute5)\
        {\
            stocks = getTickData(dataProvider.code,current,5);\
	        current = stocks.size() + offset - 1;\
	        data = getIndicatorValue(stocks,&indicator);\
		}\
		else if(span == minute10)\
		{\
			stocks = getTickData(dataProvider.code,current,10);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
		else if(span == minute15)\
		{\
			stocks = getTickData(dataProvider.code,current,15);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
		else if(span == minute30)\
		{\
			stocks = getTickData(dataProvider.code,current,30);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
		else if (span == minute60)\
		{\
			stocks = getTickData(dataProvider.code,current,60);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
		else\
		{\
		}\
		if (current < 0)\
	        return false;\
		auto item = data[current];\
		if(!item._1)\
			return false;\
		output.type = ResultType::TYPE_DOUBLE;\
		output.double_ = data[current]._2;\
		return true;\
	}\
	bool loadData()\
	{\
		if (span != normal)\
            return true;\
		data.clear();\
		INDICATOR indicator(n);\
		foreach(auto stock,dataProvider.stocks)\
			data.push_back(indicator.update(stock));\
		return true;\
	}\
	void clear()\
	{\
		data.clear();\
	}\
};

TAG_EVALATOR(Indicator_ma,MaNIndicator,"ma")
TAG_EVALATOR(Indicator_ema,EMAIndicator,"ema")
TAG_EVALATOR(Indicator_VolMa,VolMaNIndicator,"volma")
TAG_EVALATOR(Indicator_Bias,BiasIndicator,"bias")
TAG_EVALATOR(Indicator_WR,WRIndicator,"wr")
TAG_EVALATOR(Indicator_RSI,RSIIndicator,"rsi")
TAG_EVALATOR(Indicator_PSY,PSYIndicator,"psy")

#define TAG_EVALATOR_NO_ARGUMENT(CName,INDICATOR,str) \
class CName : public MetaEvalutor\
{\
public:\
    CName(Span spn):MetaEvalutor(spn){}\
    int offset;\
	QVector<DataWrapper5<bool,float,float,float,float>> data;\
	bool returnBool()const{return false;}\
	const QStringList tag()const{return QString(str).split("|");}\
	bool build(const QString& tag,const QVector<double>& arguments)\
	{\
		if(arguments.size() != 1)\
            return false;\
        offset = arguments[0];\
        if(offset > 0)\
            return false;\
		return true;\
	}\
	bool evalute(int pos,ResultType& output)\
	{\
        int current = pos+offset;\
        if (span == normal && (current < 0 || current >= data.size()))\
            return false;\
        auto stocks = dataProvider.stocks;\
        INDICATOR indicator;\
        if (span == day5)\
        {\
            stocks = getStockSpanK(stocks.mid(0,current + 1),5);\
	        current = stocks.size() + offset - 1;\
	        data = getIndicatorValue(stocks,&indicator);\
        }\
		else if (span == day10)\
		{\
			stocks = getStockSpanK(stocks.mid(0,current + 1),10);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
		else if (span == day20)\
		{\
			stocks = getStockSpanK(stocks.mid(0,current + 1),20);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
        else if(span == minute5)\
        {\
            stocks = getTickData(dataProvider.code,current,5);\
	        current = stocks.size() + offset - 1;\
	        data = getIndicatorValue(stocks,&indicator);\
		}\
		else if(span == minute10)\
		{\
			stocks = getTickData(dataProvider.code,current,10);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
		else if(span == minute15)\
		{\
			stocks = getTickData(dataProvider.code,current,15);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
		else if(span == minute30)\
		{\
			stocks = getTickData(dataProvider.code,current,30);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
		else if (span == minute60)\
		{\
			stocks = getTickData(dataProvider.code,current,60);\
			current = stocks.size() + offset - 1;\
			data = getIndicatorValue(stocks,&indicator);\
		}\
		else\
		{\
		}\
		if (current < 0)\
	        return false;\
		auto item = data[current];\
		if(!item._1)\
			return false;\
		output.type = ResultType::TYPE_DOUBLE;\
		output.double_ = data[current]._2;\
		return true;\
	}\
	bool loadData()\
	{\
		if (span != normal)\
            return true;\
		data.clear();\
		INDICATOR indicator;\
		foreach(auto stock,dataProvider.stocks)\
			data.push_back(indicator.update(stock));\
		return true;\
	}\
	void clear()\
	{\
		data.clear();\
	}\
};

TAG_EVALATOR_NO_ARGUMENT(Indicator_PVI,PVIStockIndicator,"pvi")
TAG_EVALATOR_NO_ARGUMENT(Indicator_OBV,OBVStockIndicator,"obv")
TAG_EVALATOR_NO_ARGUMENT(Indicator_WAD,StockWadIndicator,"wad")
TAG_EVALATOR_NO_ARGUMENT(Indicator_AD,ADStockIndicator,"ad")
TAG_EVALATOR_NO_ARGUMENT(Indicator_BOP,BOPStockIndicator,"bop")
TAG_EVALATOR_NO_ARGUMENT(Indicator_TR,TRIndicator,"tr")

class MetaEvalator_MACD : public MetaEvalutor
{
private:
	QVector<DataWrapper5<bool,float,float,float,float>> data;
	int offset;
	QString flag;
	int a;
	int b;
	int c;
public:
	MetaEvalator_MACD(Span spn = normal):MetaEvalutor(spn)
	{
	}
	
	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QString("macd.bar|macd.diff|macd.dea").split("|");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		a = 12;
		b = 26;
		c = 9;
		flag = label;

		if(arguments.size() == 1)
		{
			offset = arguments[0];
			if(offset > 0)
				return false;
		}
		else if(arguments.size() == 4)
		{
			a = arguments[0];
			b = arguments[1];
			c = arguments[2];
			if(a >= b || a < 5 || c < 4 || c > 15)
				return false;
			offset = arguments[3];
			if (offset > 0)
				return false;
		}

		return tag().contains(flag);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (span == normal)
		{
			if (current <= 0 || current >= data.size())
				return false;
		}
		auto stocks = dataProvider.stocks;
		MacdIndicator indicator(a,b,c);

		REBUILD_SPAN_DATA
		
		auto item = data[current];
		if (!item._1)
			return false;

		double bar = item._2;
		double diff = item._3;
		double dea = item._4;

		output.type = ResultType::TYPE_DOUBLE;
		if (flag == "macd.bar")
			output.double_ = bar;
		else if (flag == "macd.diff")
			output.double_ = diff;
		else if (flag == "macd.dea")
			output.double_ = dea;
		else
			return false;
		return true;
	}

	bool loadData()
	{
		if (span != normal)
			return true;
		data.clear();
		MacdIndicator indicator(a,b,c);
		foreach(auto stock,dataProvider.stocks)
			data.push_back(indicator.update(stock));
		
		return true;
	}

	void clear()
	{
		data.clear();
	}
};

class MetaEvalator_KDJ : public MetaEvalutor
{
private:
	QVector<DataWrapper5<bool,float,float,float,float>> data;
	int offset;
	QString flag;
public:
	MetaEvalator_KDJ(Span spn = normal):MetaEvalutor(spn)
	{
	}

	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QString("kdj.k|kdj.d|kdj.j").split("|");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if(arguments.size() != 1)
			return false;

		flag = label;
		offset = arguments[0];
		if (offset > 0)
			return false;
		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (span == normal)
		{
			if (current <= 0 || current >= data.size())
				return false;
		}

		auto stocks = dataProvider.stocks;
		KDJIndicator indicator;
		REBUILD_SPAN_DATA
		
		auto item = data[current];
		if (!item._1)
			return false;

		double k = item._2;
		double d = item._3;
		double j = item._4;

		output.type = ResultType::TYPE_DOUBLE;
		if (flag == "kdj.k")
			output.double_ = k;
		else if (flag == "kdj.d")
			output.double_ = d;
		else if (flag == "kdj.j")
			output.double_ = j;
		else
			return false;
		return true;
	}

	bool loadData()
	{
		if (span != normal)
			return true;
		data.clear();
		KDJIndicator indicator;
		foreach(auto stock,dataProvider.stocks)
			data.push_back(indicator.update(stock));
		
		return true;
	}

	void clear()
	{
		data.clear();
	}
};

class MetaEvalator_DMI : public MetaEvalutor
{
private:
	QVector<DataWrapper5<bool,float,float,float,float>> data;
	int n;
	int m;
	int offset;
	QString flag;
public:
	MetaEvalator_DMI(Span spn = normal):MetaEvalutor(spn)
	{
	}
	
	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QString("dmi.pdi|dmi.mdi|dmi.adx|dmi.adxr").split("|");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if(arguments.size() != 3)
			return false;

		flag = label;
		if(!tag().contains(flag))
			return false;

		n = arguments[0];
		m = arguments[1];
		if(n < m || n < 6 || m < 2)
			return false;

		offset = arguments[2];
		if(offset > 0)
			return false;
        return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (span == normal)
		{
			if (current <= 0 || current >= data.size())
				return false;
		}

		auto stocks = dataProvider.stocks;
		DMIStockIndicator indicator(n,m);
		REBUILD_SPAN_DATA
		
		auto item = data[current];
		if (!item._1)
			return false;

		double pdi = item._2;
		double mdi = item._3;
		double adx = item._4;
		double adxr = item._5;

		output.type = ResultType::TYPE_DOUBLE;
		if (flag == "dmi.pdi")
			output.double_ = pdi;
		else if (flag == "dmi.mdi")
			output.double_ = mdi;
		else if (flag == "dmi.adx")
			output.double_ = adx;
		else if (flag == "dmi.adxr")
			output.double_ = adxr;
		else
			return false;
		return true;
	}

	bool loadData()
	{
		if (span != normal)
			return true;
		data.clear();
		DMIStockIndicator indicator(n,m);
		foreach(auto stock,dataProvider.stocks)
			data.push_back(indicator.update(stock));
		
		return true;
	}

	void clear()
	{
		data.clear();
	}
};

class MetaEvalutor_K : public MetaEvalutor
{
	int offset;
	QString flag;
public:
	MetaEvalutor_K(){}
	MetaEvalutor_K(Span spn):MetaEvalutor(spn),offset(0)
	{
	}

	bool returnBool()const{return true;}
	const QStringList tag()const
	{
		return StockKAnalyzer::instance()->keywords();
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if (arguments.size() != 1)
			return false;

		offset = arguments[0];
		if(offset > 0)
			return false;

		flag = label;
		return tag().contains(flag);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 3)
			return false;

		auto data = dataProvider.stocks.mid(0,current + 1);

		if (span == day5)
		{
			data = getStockSpanK(data.mid(0,current + 1),5);
			current = data.size() + offset - 1;
		}
		else if (span == day10)
		{
			data = getStockSpanK(data.mid(0,current + 1),10);
			current = data.size() + offset - 1;
		}
		else if (span == day20)
		{
			data = getStockSpanK(data.mid(0,current + 1),20);
			current = data.size() + offset - 1;
		}
		else if (span == minute5)
		{
			data = getTickData(dataProvider.code,current,5);
			current = data.size() + offset - 1;
		}
		else if (span == minute10)
		{
			data = getTickData(dataProvider.code,current,10);
			current = data.size() + offset - 1;
		}
		else if (span == minute15)
		{
			data = getTickData(dataProvider.code,current,15);
			current = data.size() + offset - 1;
		}
		else if (span == minute30)
		{
			data = getTickData(dataProvider.code,current,30);
			current = data.size() + offset - 1;
		}
		else if (span == minute60)
		{
			data = getTickData(dataProvider.code,current,60);
			current = data.size() + offset - 1;
		}

		if (current < 0)
			return false;
		
		auto stocks = data.mid(0,current+1);
		output.type = ResultType::TYPE_BOOL;
		return output.bool_ = StockKAnalyzer::instance()->isThisK(flag,stocks);
	}

	bool loadData()
	{	
		return true;
	}

	void clear()
	{
	}
};

#ifdef SUPPORT_BASIC
class MetaEvalutor_Var : public MetaEvalutor
{
	QString var;
public:
	MetaEvalutor_Var(Span spn):MetaEvalutor(spn)
	{
	}

	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QStringList("@");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if(!label.startsWith("@"))
			return false;
		auto ptr = StockScriptList::GetInstance().getScriptEngine()->getVarMetaByVar(label);
		if(ptr == nullptr)
			return false;
		var = label;
		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		auto ptr = StockScriptList::GetInstance().getScriptEngine()->getVarMetaByVar(var);
		return ptr->fetch(pos,output);
	}

	bool loadData()
	{	
		return true;
	}

	void clear()
	{
	}
};

#ifdef SUPPORT_IMPLICIT_VAR

class MetaEvalutor_ImplicitVar : public MetaEvalutor
{
	QString var;
public:
	MetaEvalutor_ImplicitVar(Span spn):MetaEvalutor(spn)
	{
	}

	bool returnBool()const{ return false; }
	const QStringList tag()const
	{
		return QStringList("@");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if (!label.startsWith("@"))
			return false;
		var = label;
		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		QPair<double,QString> node;
		bool ok = StockScriptList::GetInstance().getVarMetaManager()->getRegisterVar(var,node);
		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = node.first;
		if (!ok)
			qDebug() << "隐含变量:" << var << "不存在";
		return ok;
	}

	bool loadData()
	{
		return true;
	}

	void clear()
	{
	}
private:

};

#endif

/*
class MetaEvalutor_Not : public MetaEvalutor
{
	MetaEvalutor* evalutor;
public:
	MetaEvalutor_Not()
	{
		evalutor = nullptr;
	}

	MetaEvalutor_Not(Span spn):MetaEvalutor(spn)
	{
		evalutor = nullptr;
	}
	
	~MetaEvalutor_Not()
	{
		if(evalutor)
			delete evalutor;
	}

	bool returnBool()const{return true;}
	const QStringList tag()const
	{
		return QStringList("not.");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if(!label.startsWith("not."))
			return false;

		evalutor = MetaEvalutor::create(label.mid(4),span);
		if(!evalutor)
			return false;

		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		if (!evalutor)
			return false;

		return evalutor->evalute(pos,output);
	}

	bool loadData()
	{	
		return evalutor->loadData();
	}

	void clear()
	{
		evalutor->clear();
	}
};*/

class MetaEvalutor_store : public MetaEvalutor
{
	QString var;
public:
	MetaEvalutor_store() = default;
	MetaEvalutor_store(Span spn):MetaEvalutor(spn)
	{
	}

	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QStringList("@@");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if(!label.startsWith("@@"))
			return false;
		auto ptr = StockScriptList::GetInstance().getScriptEngine()->getStoreMetaByVar(label);
		if(ptr == nullptr)
			return false;
		var = label;
		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		auto ptr = StockScriptList::GetInstance().getScriptEngine()->getStoreMetaByVar(var);
		if (!ptr->hasData)
			return false;
		output.double_ = ptr->value;
		output.bool_ = ResultType::TYPE_BOOL;
		return true;
	}

	bool loadData()
	{	
		return true;
	}

	void clear()
	{
	}
};
#endif 

#ifdef SUPPORT_hyperbola_algo

class MetaEvalator_fma : public MetaEvalutor
{
private:
	QVector<Stock>  stocks;
	QVector<DataWrapper5<bool,float,float,float,float>> data;
	int offset;
	int m;
public:
	const QStringList tag()const
	{
		return QString("fma").split("|");
	}

	bool returnBool()const{return true;}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if(label != "fma")
			return false;
		
		m = arguments[0];
		offset = arguments[1];
		if(m < 12 || m > 120)
			return false;
		if(offset > 0)
			return false;
		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset+1;
		if(current <= 0 || current >/*=*/ data.size())
			return false;

		/*QVector<Stock> stks;
		if(pos == data.size()-1 && offset == 0)
			stks = stocks;
		else
		    stks = stocks.mid(0,current); */

		auto list = stocks.mid(0,current);
		auto d = data.mid(0,current);
		bool ok = isFMA(list,d);
		output.type = ResultType::TYPE_BOOL;
		if(ok)
		{
			output.bool_ = true;
			return true;
		}
		output.bool_ = false;
		return false;
	}

	bool loadData()
	{
		data.clear();
		MaNIndicator indicator(m);
		foreach(auto stock,dataProvider.stocks)
			data.push_back(indicator.update(stock));
		stocks = dataProvider.stocks;
		return true;
	}

	void clear()
	{
		data.clear();
		stocks.clear();
	}
};

#endif

#ifdef SUPPORT_GPDX

class MetaEvalator_gpdx_tzmr : public MetaEvalutor
{
private:
	QVector<Stock>  stocks;
	int offset;
public:
	const QStringList tag()const
	{
		return QString("unit.高抛低吸.调整买入").split("|");
	}

	bool returnBool()const{ return true; }

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if (label != "unit.高抛低吸.调整买入")
			return false;

		if (arguments.size() == 0)
			return false;

		offset = arguments[0];
		return offset <= 0;
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos + offset + 1;
		if (current <= 29 || current > stocks.size())
			return false;

		bool ok = isGPDX_FTMR(stocks,current);
		if (ok)
		{
			output.type = ResultType::TYPE_BOOL;
			output.bool_ = true;
			return true;
		}

		return false;
	}

	bool loadData()
	{
		stocks = dataProvider.stocks;
		return true;
	}

	void clear()
	{
		stocks.clear();
	}
};

class MetaEvalator_gpdx_spmr : public MetaEvalutor
{
private:
	QVector<Stock>  stocks;
	int offset;
public:
	const QStringList tag()const
	{
		return QString("unit.高抛低吸.升破买入").split("|");
	}

	bool returnBool()const{ return true; }

	bool build(const QString& label,const QVector<double>& arguments)
	{
		if (label != "unit.高抛低吸.升破买入")
			return false;

		if (arguments.size() == 0)
			return false;

		offset = arguments[0];
		return offset <= 0;
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos + offset + 1;
		if (current <= 29 || current > stocks.size())
			return false;

		bool ok = isGPDX_SPMR(stocks,current);
		if (ok)
		{
			output.type = ResultType::TYPE_BOOL;
			output.bool_ = true;
			return true;
		}

		return false;
	}

	bool loadData()
	{
		stocks = dataProvider.stocks;
		return true;
	}

	void clear()
	{
		stocks.clear();
	}
};


#endif

#ifdef SUPPORT_hyperbola_algo

class MetaEvalator_Unit_MACD : public MetaEvalutor
{
private:
	int a,b,c;
	QString order;
	std::function<bool(int,const QVector<Stock>&,const DataWrapper5s&)> fn;
	QVector<Stock>  stocks;
	DataWrapper5s data;
	int offset;
public:
	MetaEvalator_Unit_MACD(const QString& name,std::function<bool(int,const QVector<Stock>&,const DataWrapper5s&)> fptr)
	{
		a = 12;
		b = 26;
		c = 9;
        order = name;
        fn = fptr;

		QVariant var;
		bool ok = SnailContext::open()->getConfigFactory()->getVar(QString("config.unit.macd.佛手向上.macd参数"),var);
		if(ok)
		{
			QStringList items = var.toString().split(",");
			if(items.size() == 3)
			{
				a = items[0].toInt();
				b = items[1].toInt();
				c = items[2].toInt();
				if(a > b - 4 || c < 5 || a < 5)
				{
				    a = 12;
					b = 26;
					c = 9;
				}
			}
		}
	}

	const QStringList tag()const
	{
		return order.split("|");
	}

	bool returnBool()const{return true;}

	bool build(const QString& label)
	{
		QStringList items = label.split(",");
		if(items.size() != 2)
			return false;

		if(items[0] != order)
			return false;
	
		offset = items[1].toInt();
		if(offset > 0)
			return false;
		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset+1;
		if(current <= 0 || current > data.size())
			return false;

		bool ok = fn(current,stocks,data);
		output.type = ResultType::TYPE_BOOL;
		if(ok)
		{
			output.bool_ = true;
			return true;
		}
		output.bool_ = false;
		return false;
	}

	bool loadData()
	{
		data.clear();
		MacdIndicator indicator(a,b,c);
		foreach(auto stock,dataProvider.stocks)
			data.push_back(indicator.update(stock));
		stocks = dataProvider.stocks;
		return true;
	}

	void clear()
	{
		data.clear();
		stocks.clear();
	}
};
#endif

#ifdef SUPPORT_SCRIPT

class MetaEvalator_script : public MetaEvalutor
{
private:
	QVector<Stock>  stocks;
	QString flag;
	int offset;
public:
	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		auto list = ScriptManager::instance().getScriptInfos();
		QStringList tags;
		foreach(auto item,list)
			tags.append(item->name);
		return tags;
	}

	bool build(const QString& label)
	{
		QStringList items = label.split(",");
		if(items.size() != 2)
			return false;

		flag = items[0];
		
		offset = evalute_express(items[1]);
		if(offset > 0)
			return false;
		return true;
	}

	int evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if(current < 3)
			return -1;
		
		auto stocks = dataProvider.stocks.mid(0,current+1);
		QString ret = ScriptManager::instance().evalute(flag,dataProvider.code,stocks);
		if(ret == "true")
		{
			output.bool_ = true;
		    output.type = ResultType::TYPE_BOOL;
			return 1;
		}
		else if(ret == "false")
		{
			output.bool_ = false;
		    output.type = ResultType::TYPE_BOOL;
			return 1;
		}
		else if(ret == "null")
		{
			output.bool_ = false;
		    output.type = ResultType::TYPE_BOOL;
			return -1;
		}
		else
		{
			output.double_ = ret.toDouble();
			output.type = ResultType::TYPE_DOUBLE;
			return 1;
		}
		return -1;
	}

	bool loadData()
	{
		stocks = dataProvider.stocks;
		return true;
	}

	void clear()
	{
		stocks.clear();
	}
};

#endif

class MetaEvalator_bollC : public MetaEvalutor
{
private:
	QVector<DataWrapper5<bool, float, float, float, float>> data;
	int offset;
	double decrease = 0.0;
	QString flag;
public:
	MetaEvalator_bollC(Span spn = normal) :MetaEvalutor(spn)
	{
	}

	bool returnBool()const { return true; }
	const QStringList tag()const
	{
		return QStringList("bollC");
	}

	bool build(const QString& label, const QVector<double>& arguments)
	{
		if (arguments.size() != 2)
			return false;

		flag = label;

		decrease = arguments[0];

		offset = arguments[1];
		if (offset > 0)
			return false;
		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos + offset;
		if (span == normal)
		{
			if (current <= 0 || current >= data.size())
				return false;
		}

		if (current < 3)
			return false;

		auto stocks = dataProvider.stocks.mid(0,current + 1);
		BollIndicator indicator;
		REBUILD_SPAN_DATA

		auto item = data[current];
		if(!item._1)
			return false;

		output.type = ResultType::TYPE_BOOL;
		return output.bool_ = isFit(stocks,data.mid(0,current+1),decrease);
	}

	bool loadData()
	{
		if (span != normal)
			return true;
		data.clear();
		BollIndicator indicator;
		foreach(auto stock, dataProvider.stocks)
			data.push_back(indicator.update(stock));

		return true;
	}

	void clear()
	{
		data.clear();
	}
};

MetaEvalutor* MetaEvalutor::create(const QString& tag,Span spn)
{
	QString label(tag);
	if (label.startsWith("ohlc."))
		return new MetaEvalator_OHLC(spn);
	label = "boll.upper|boll.mid|boll.lower|boll.width";
	if (label.split("|").contains(tag))
		return new MetaEvalator_BOLL(spn);
	if (tag == "ma")
		return new Indicator_ma(spn);
	if (tag == "volma")
		return new Indicator_VolMa(spn);
	if (tag == "value")
		return new MetaEvalutor_Double();
	label = "macd.bar|macd.diff|macd.dea";
	if (label.split("|").contains(tag))
		return new MetaEvalator_MACD(spn);

	if (tag.startsWith("k."))
		return new MetaEvalutor_K(spn);
	if (tag == "bollC")
		return new MetaEvalator_bollC(spn);

#ifdef SUPPORT_BASIC
	if (tag == "ema")
		return new Indicator_ema(spn);
	if (tag == "bias")
		return new Indicator_Bias(spn);
	if (tag == "wr")
		return new Indicator_WR(spn);
	if (tag == "rsi")
		return new Indicator_RSI(spn);
	if(tag == "psy")
		return new Indicator_PSY(spn);
	if (tag == "obv")
		return new Indicator_OBV(spn);
	if (tag == "wad")
		return new Indicator_WAD(spn);
	if (tag == "bop")
		return new Indicator_BOP(spn);
	if (tag == "ad")
		return new Indicator_AD(spn);
	if (tag == "pvi")
		return new Indicator_PVI(spn);
	if (tag == "tr")
		return new Indicator_TR(spn);
	if (tag == "pass")
		return new MetaEvalutor_Pass(spn);
	label = "kdj.k|kdj.d|kdj.j";
	if (label.split("|").contains(tag))
		return new MetaEvalator_KDJ(spn);
	label = "dmi.pdi|dmi.mdi|dmi.adx|dmi.adxr";
	if (label.split("|").contains(tag))
		return new MetaEvalator_DMI(spn);

#ifdef SUPPORT_LEVEL2
	if (tag.startsWith("lv2.") && spn == normal)
		return new MetaEvalutor_LV2(spn);
#endif 

	if (tag.startsWith("@@"))
		return new MetaEvalutor_store(spn);

#ifdef SUPPORT_IMPLICIT_VAR
	if (tag.startsWith("@IMPLICIT_VAR"))
	{
		return new MetaEvalutor_ImplicitVar(spn);
	}
#endif

	if (tag.startsWith("@"))
	{
		return new MetaEvalutor_Var(spn);
	}
#ifdef SUPPORT_TICKET
	if (tag.startsWith("tick.") && spn == normal)
		return StockTickAnalyzer::create(tag);
#endif
	//if (tag.startsWith("not."))
	//	return new MetaEvalutor_Not(spn);

#ifdef SUPPORT_hyperbola_algo
	/*if (tag.startsWith("unit.macd.佛手向上"))
	{
		std::function<bool(int,const QVector<Stock>&,const DataWrapper5s&)> fp;
		fp = &isFitness_MACD_FSXS;
		return new MetaEvalator_Unit_MACD(QString("unit.macd.佛手向上"),fp);
	}*/

	if (tag.startsWith("fma"))
		return new MetaEvalator_fma();
#endif

#ifdef SUPPORT_GPDX
	if (tag.startsWith("unit.高抛低吸.调整买入"))
		return new MetaEvalator_gpdx_tzmr();
	if (tag.startsWith("unit.高抛低吸.升破买入"))
		return new MetaEvalator_gpdx_spmr();
#endif

	QString prefix = tag.mid(0,tag.indexOf("."));
	return ObjectFactory<MetaEvalutor,QString>::create(prefix);
#endif
	return nullptr;
}