#include <cmath>
#include <QTime>
#include "common/common.h"
#include "../StockScriptEngine.h"
#include "StockTickScanner.h"

#ifdef SUPPORT_TICKET

QStringList StockTickAnalyzer::keywords()
{
	QStringList items;
	items.append("tick.形态良好");
	items.append("tick.形态恶化");
	items.append("tick.早盘涨停");
	items.append("tick.尾盘涨停");
	items.append("tick.尾盘拉升");
	items.append("tick.主力净流入");
	items.append("tick.成交量");
	items.append("tick.成交额");
	items.append("tick.换手率");
	items.append("tick.外盘");
	items.append("tick.内盘");
	items.append("tick.总市值");
	items.append("tick.流通市值");
	items.append("tick.市净率");
	items.append("tick.市盈率");
	items.append("tick.量比");
	items.append(QString("tick.买1|tick.买2|tick.买3|tick.买4|tick.买5|tick.卖1|tick.卖2|tick.卖3|tick.卖4|tick.卖5").split("|"));
	items.append(QString("tick.open|tick.close|tick.high|tick.low").split("|"));
	items.append("tick.区间成交量");
	items.append("tick.区间最低价");
	items.append("tick.区间最高价");
	items.append("tick.最低价区间");
	items.append("tick.最高价区间");
	items.append("tick.正弦十字星");
	items.append("tick.反正弦十字星");
	items.append("tick.分时最大成交量");
	items.append("tick.走势僵硬度");
	return items;
}

class MetaEvalutor_Tick_Shape : public MetaEvalutor
{
	QString id;
	QString date;
	float value;
	int offset;
public:
	bool returnBool()const{return true;}
	const QStringList tag()const
	{
		return QString("tick.形态良好|tick.形态恶化").split("|");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		//命令,时间节点,幅度,日期偏移
		id = label;
		if(arguments.size() != 2)
			return false;

		value = arguments[0];
		if(value < 60.0 && id == "形态良好")
			return false;

		if(value > 40.0 && id == "形态恶化")
			return false;

		offset = arguments[1];
		if(offset > 0)
			return false;

		return tag().contains(id);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0)
			return false;

		QString date = dataProvider.stocks[current].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
			return false;

		auto info = dataProvider.tickData[dataProvider.date];
	    float vp1 = info.stock.data[Stock::TAG_P1].toFloat();
	    float vp2 = info.stock.data[Stock::TAG_P2].toFloat();

		if(id == "tick.形态良好")
		{
		    ok = vp1 > value && vp2 > value;
			if (!ok)
				return false;
		    output.bool_ = true;
		    output.type = output.TYPE_BOOL;
			return true;
		}
 
		ok = vp1 < value && vp2 < value;
		if (!ok)
			return false;
		output.bool_ = true;
		output.type = output.TYPE_BOOL;
		return true;
	}

	bool loadData(){return true;}
	void clear(){}
};

class MetaEvalutor_Tick_Value : public MetaEvalutor
{
	QString id;
	QString date;
	int offset;
public:
	bool returnBool()const{return false;}
	
	const QStringList tag()const
	{
		return QString("tick.主力净流入|tick.成交量|tick.成交额|tick.换手率|tick.外盘|tick.内盘|tick.总市值|tick.流通市值|tick.市净率|tick.市盈率|tick.量比|tick.买1|tick.买2|tick.买3|tick.买4|tick.买5|tick.卖1|tick.卖2|tick.卖3|tick.卖4|tick.卖5|tick.open|tick.close|tick.low|tick.high|tick.走势僵硬度").split("|");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		//命令,数值,日期偏移
		if(arguments.size() != 1)
			return false;

		offset = arguments[0];
		if(offset > 0)
			return false;

		id = label;
		return tag().contains(id);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0)
			return false;

		QString date = dataProvider.stocks[current].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
			return false;

		auto info = dataProvider.tickData[dataProvider.date];

		if(id == "tick.主力净流入")
		{
			float value_ = info.capitcalFlow._3;
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
        else if(id == "tick.市盈率")
		{
			float value_ = info.stock.data[Stock::TAG_PE_D].toDouble();
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
        else if(id == "tick.市净率")
		{
			float value_ = info.stock.data[Stock::TAG_PB].toDouble();
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
        else if(id == "tick.量比")
		{
			float value_ = info.stock.data[Stock::TAG_LIANGBI].toDouble();
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
        else if(id == "tick.内盘")
		{
			float value_ = info.stock.data[Stock::TAG_NEIPAN].toDouble()*0.0001;
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
        else if(id == "tick.外盘")
		{
			float value_ = info.stock.data[Stock::TAG_WAIPAN].toDouble()*0.0001;
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
        else if(id == "tick.总市值")
		{
			float value_ = info.stock.data[Stock::TAG_ZONGSHIZHI].toDouble();
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
        else if(id == "tick.流通市值")
		{
			float value_ = info.stock.data[Stock::TAG_LIUTONGSHIZHI].toDouble();
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
        else if(id == "tick.成交量")
		{
			float value_ = info.stock.tradeVolume*0.0001;
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
        else if(id == "tick.成交额")
		{
			float value_ = info.stock.tradeCapital;
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
        else if(id == "tick.换手率")
		{
			float value_ = info.stock.turnover;
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
		else if(id == "tick.open")
		{
			if (info.list.isEmpty())
				return false;
			float value_ = info.list[0].price;
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
		else if(id == "tick.close")
		{
			if (info.list.isEmpty())
				return false;
			float value_ = info.list[info.list.size()-1].price;
			output.double_ = value_;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
		else if(id == "tick.low")
		{
			if (info.list.isEmpty())
				return false;
			output.double_ = info.low.first;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
		else if(id == "tick.high")
		{
			if (info.list.isEmpty())
				return false;
			output.double_ = info.high.first;
			output.type = output.TYPE_DOUBLE;
			return true;
		}
		else if (id == "tick.走势僵硬度")
		{
			if (info.list.size() < 3)
				return false;

			int count = 0;
			int i = 1;

			int old = info.list[0].price;
			int flag = 0;

			while (true)
			{
				if (i >= info.list.size())
					break;
				if (isEqual(info.list[i].price,old))
				{
					if (flag == 0)
					{
						count += 2;
						flag = 1;
					}
					else
						count ++;
				}
				else
				{
					old = info.list[i].price;
					flag = 0;
				}
				i ++;
			}

			output.double_ = count * 100.0 / (double)info.list.size();
			output.type = output.TYPE_DOUBLE;
			return true;
		}


#define CHECK_THIS(str,var)\
	    if(id == str)\
		{\
			float value_ = var;\
			output.double_ = value_;\
			output.type = output.TYPE_DOUBLE;\
			return true;\
		}

		CHECK_THIS("tick.买1",info.buy[0].second)
		CHECK_THIS("tick.买2",info.buy[1].second)
		CHECK_THIS("tick.买3",info.buy[2].second)
		CHECK_THIS("tick.买4",info.buy[3].second)
		CHECK_THIS("tick.买5",info.buy[4].second)
		CHECK_THIS("tick.卖1",info.sale[0].second)
		CHECK_THIS("tick.卖2",info.sale[1].second)
		CHECK_THIS("tick.卖3",info.sale[2].second)
		CHECK_THIS("tick.卖4",info.sale[3].second)
		CHECK_THIS("tick.卖5",info.sale[4].second)
		return false;
	}

	bool loadData(){return true;}
	void clear(){}
};

class MetaEvalutor_Tick_WeiPanZhangTing : public MetaEvalutor
{
	QString id;
	QString date;
	int minute;
	int offset;
public:
	bool returnBool()const{return true;}
	const QStringList tag()const
	{
		return QStringList("tick.尾盘涨停");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		//命令,时间节点,幅度,日期偏移
		id = label;
		if(arguments.size() != 2)
			return false;

		minute = arguments[0];
		if(minute > 60 || minute < 0)
			return false;

		offset = arguments[1];
		if(offset > 0)
			return false;

		return tag().contains(label);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0)
			return false;

		QString date = dataProvider.stocks[current].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
			return false;

		QTime scotTime(13,30);
		scotTime = scotTime.addSecs(-minute*60);
		
		auto info = dataProvider.tickData[dataProvider.date];
		if (!info.flag)
			return false;

		if (info.list.size() < 10)
			return false;

		double closePrice = info.list[info.list.size()-1].price;

		for(int i=info.list.size()-1;i>=0;i--)
		{
			if (info.list[i].time >= scotTime)
			{
				if (!isSameValue(closePrice,info.list[i].price))
				{
					output.bool_ = true;
					output.type = output.TYPE_BOOL;
					return true;
				}
			}
			else
				return false;
		}
		return false;
	}

	bool loadData(){return true;}
	void clear(){}
};

class MetaEvalutor_Tick_KaiPanZhangTing : public MetaEvalutor
{
	QString id;
	QString date;
	int minute;
	int offset;
public:
	bool returnBool()const{return true;}
	const QStringList tag()const
	{
		return QStringList("tick.早盘涨停");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		//命令,时间节点,日期偏移
		id = label;
		if(arguments.size() != 2)
			return false;

		minute = arguments[0];
		if(minute > 120)
			return false;

		offset = arguments[1];
		if(offset > 0)
			return false;

		return tag().contains(label);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0)
			return false;

		QString date = dataProvider.stocks[current].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
			return false;

		QTime scotTime(9,30);
		scotTime = scotTime.addSecs(minute*60);
		
		auto info = dataProvider.tickData[dataProvider.date];
		if (!info.flag)
			return false;

		if (info.list.size() < 10)
			return false;

		double closePrice = info.list[info.list.size()-1].price;

		for(int i=0;i<info.list.size();i++)
		{
			if (info.list[i].time <= scotTime)
			{
				if (isSameValue(closePrice,info.list[i].price))
				{
					output.bool_ = true;
					output.type = output.TYPE_BOOL;
					return true;
				}
			}
			else
				return false;
		}
		return false;
	}

	bool loadData()
	{	
		return true;
	}

	void clear()
	{
	}
};

class MetaEvalutor_Tick_WeiPanLaSheng : public MetaEvalutor
{
	QString id;
	QString date;
	int minute;
	double increase;
	int offset;
public:
	bool returnBool()const{return true;}
	const QStringList tag()const
	{
		return QStringList("tick.尾盘拉升");
	}

	bool build(const QString& label,const QVector<double>& arguments)
	{
		//命令,时间节点,幅度,日期偏移
		id = label;
		if(arguments.size() != 3)
			return false;

		minute = arguments[0];
		if(minute > 60 || minute < 0)
			return false;

		increase = arguments[1];
		offset = arguments[2];
		if(offset > 0)
			return false;

		return tag().contains(id);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0)
			return false;

		QString date = dataProvider.stocks[current].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
			return false;

		QTime scotTime(13,30);
		scotTime = scotTime.addSecs(-minute*60);
		
		auto info = dataProvider.tickData[dataProvider.date];
		if (!info.flag)
			return false;

		if (info.list.size() < 10)
			return false;

		double closePrice = info.list[info.list.size()-1].price;

		for(int i=info.list.size()-1;i>=0;i--)
		{
			if (info.list[i].time >= scotTime)
			{
				double d = GET_INCREASE(closePrice,info.list[i].price);
				if (d > increase)
				{
					output.bool_ = true;
					output.type = output.TYPE_BOOL;
					return true;
				}
				return false;
			}
			else
				return false;
		}
		return false;
	}

	bool loadData(){return true;}
	void clear(){}
};

class MetaEvalutor_Tick_SpanTrade : public MetaEvalutor
{
	QString date;
	int start;
	int length;
	int offset;
public:
	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QStringList("tick.区间成交量");
	}

	//tick.区间成交量,0,5,0
	bool build(const QString& label,const QVector<double>& arguments)
	{
		//命令,时间节点,幅度,日期偏移
		if(arguments.size() != 3)
			return false;

		start = arguments[0];
		if(start >= 240 || start <= -240)
			return false;

		length = arguments[1];
		if(length < 5 || length > 60)
			return false;

		offset = arguments[2];
			return false;

		return true;
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0)
			return false;

		QString date = dataProvider.stocks[current].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
			return false;

		auto info = dataProvider.tickData[dataProvider.date];
		if (!info.flag)
			return false;

		if(start < 0)
			start += 240;

		QTime startTime(9,30);
		startTime.addSecs(start*60);
		QTime endTime = startTime.addSecs(60*length);

		double all = 0.0;
		foreach(auto item,info.list)
		{
			if(item.time >= startTime && item.time >= endTime)
			{	
				all += item.volume;
			}
		}

		if (isEqual(all,0))
			return false;
	
		output.double_ = all;
		output.type = ResultType::TYPE_DOUBLE;
		return true;
	}

	bool loadData(){return true;}
	void clear(){}
};

class MetaEvalutor_Tick_SpanHighAndLow : public MetaEvalutor
{
	QString date;
	QString flag;
	int start;
	int length;
	int offset;
public:
	bool returnBool()const{return true;}
	const QStringList tag()const
	{
		return QString("tick.最高价区间|tick.最低价区间").split("|");
	}

	//tick.最低价区间,0,5,0
	bool build(const QString& label,const QVector<double>& arguments)
	{
		//命令,时间节点,幅度,日期偏移
		flag = label;
		if(arguments.size() != 3)
			return false;

		start = arguments[0];
		if(start >= 240 || start <= -240)
			return false;

		length = arguments[1];
		if(length < 5 || length > 60)
			return false;

		offset = arguments[3];
		if(offset > 0)
			return false;

		return tag().contains(flag);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0)
			return false;

		QString date = dataProvider.stocks[current].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
			return false;

		auto info = dataProvider.tickData[dataProvider.date];
		if (!info.flag)
			return false;

		if(start < 0)
			start += 240;

		QTime startTime(9,30);
		startTime.addSecs(start*60);
		QTime endTime = startTime.addSecs(60*length);

		if(flag == "tick.最低价区间")
		{
			if(info.low.second <= endTime && info.low.second >= startTime)
			{   
				output.bool_ = true;
		        output.type = ResultType::TYPE_BOOL;
				return true;
		    }
		}
		else if(flag == "tick.最高价区间")
		{
			if(info.high.second <= endTime && info.high.second >= startTime)
			{   
				output.bool_ = true;
		        output.type = ResultType::TYPE_BOOL;
				return true;
		    }
		}
		return false;
	}

	bool loadData(){return true;}
	void clear(){}
};

class MetaEvalutor_Tick_SpanMinMaxPrice : public MetaEvalutor
{
	QString date;
	QString flag;
	int start;
	int length;
	int offset;
public:
	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QString("tick.区间最高价|tick.区间最低价").split("|");
	}

	//tick.区间最高价,0,5,0
	bool build(const QString& label,const QVector<double>& arguments)
	{
		//命令,时间节点,幅度,日期偏移
		flag = label;
		if(arguments.size() != 3)
			return false;

		start = arguments[0];
		if(start >= 240 || start <= -240)
			return false;

		length = arguments[1];
		if(length < 5 || length > 240)
			return false;

		offset = arguments[2];
		if(offset > 0)
			return false;

		return tag().contains(flag);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0)
			return false;

		QString date = dataProvider.stocks[current].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
			return false;

		auto info = dataProvider.tickData[dataProvider.date];
		if (!info.flag)
			return false;

		if(start < 0)
			start += 240;

		QTime starttime(9,30);
		starttime = starttime.addSecs(start*60);
		QTime endtime = starttime.addSecs(60*length);

		if(flag == "tick.区间最低价")
		{
			double low = std::numeric_limits<double>::max();
			foreach(auto item,info.list)
			{
				if(item.time <= endtime && item.time >= starttime)
					low = std::min(item.price,low);
			}

			output.double_ = low;
			output.type = ResultType::TYPE_DOUBLE;
			return true;
		}
		else if(flag == "tick.区间最高价")
		{
			double top = std::numeric_limits<double>::min();
			foreach(auto item,info.list)
			{
				if(item.time <= endtime && item.time >= starttime)
					top = std::max(item.price,top);
			}

			output.double_ = top;
			output.type = ResultType::TYPE_DOUBLE;
			return true;
		}
		return false;
	}

	bool loadData(){return true;}
	void clear(){}
};

class MetaEvalutor_Tick_SinAndAntiSin : public MetaEvalutor
{
	QString date;
	QString flag;
	int offset;
public:
	bool returnBool()const{return true;}
	const QStringList tag()const
	{
		return QString("tick.正弦十字星|tick.反正弦十字星").split("|");
	}

	//tick.正弦十字星,0
	bool build(const QString& label,const QVector<double>& arguments)
	{
		//命令,时间节点,幅度,日期偏移
		flag = label;
		if(arguments.size() != 1)
			return false;

		offset = arguments[1];
		if(offset > 0)
			return false;

		return tag().contains(label);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0)
			return false;

		QString date = dataProvider.stocks[current].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
			return false;

		auto info = dataProvider.tickData[dataProvider.date];
		if (!info.flag)
			return false;

		bool lowFirst = info.low.second > info.high.second;
		if(flag == "tick.正弦十字星")
		{
			if(!lowFirst)
			{   
				output.bool_ = true;
		        output.type = ResultType::TYPE_BOOL;
				return true;
		    }
		}
		else if(flag == "tick.反正弦十字星")
		{
			if(lowFirst)
			{   
				output.bool_ = true;
		        output.type = ResultType::TYPE_BOOL;
				return true;
		    }
		}
		return false;
	}

	bool loadData(){return true;}
	void clear(){}
};

class MetaEvalutor_Tick_TickMaxVol : public MetaEvalutor
{
	QString date;
	QString flag;
	int offset;
public:
	bool returnBool()const{return false;}
	const QStringList tag()const
	{
		return QString("tick.分时最大成交量").split("|");
	}

	//tick.分时最大成交量,0
	bool build(const QString& label,const QVector<double>& arguments)
	{
		//命令,日期偏移
		flag = label;
		if(arguments.size() != 1)
			return false;

		offset = arguments[0];
		if(offset > 0)
			return false;

		return tag().contains(label);
	}

	bool evalute(int pos,ResultType& output)
	{
		int current = pos+offset;
		if (current < 0)
			return false;

		QString date = dataProvider.stocks[current].date;
		bool ok = dataProvider.loadTickDataFromFile(dataProvider.code,date);
		if (!ok)
			return false;

		auto info = dataProvider.tickData[dataProvider.date];
		if (!info.flag)
			return false;

		double vol = 0.0;
		foreach(auto item,info.list)
		{
			if(item.volume > vol)
				vol = item.volume;
		}

		output.double_ = vol;
		output.type = ResultType::TYPE_DOUBLE;
		return true;
	}

	bool loadData(){return true;}
	void clear(){}
};

MetaEvalutor* StockTickAnalyzer::create(const QString& tag)
{
	if(tag.startsWith("tick.形态良好"))
		return new MetaEvalutor_Tick_Shape();
	else if(tag.startsWith("tick.形态恶化"))
		return new MetaEvalutor_Tick_Shape();

	QStringList items = QString("tick.主力净流入|tick.成交量|tick.成交额|tick.换手率|tick.外盘|tick.内盘|tick.总市值|tick.流通市值|tick.市净率|tick.市盈率|tick.量比|tick.买1|tick.买2|tick.买3|tick.买4|tick.买5|tick.卖1|tick.卖2|tick.卖3|tick.卖4|tick.卖5|tick.走势僵硬度").split("|");
	foreach(QString item,items)
	{
	    if(tag.startsWith(item))
		    return new MetaEvalutor_Tick_Value();
	}

	if(tag.startsWith("tick.早盘涨停"))
		return new MetaEvalutor_Tick_KaiPanZhangTing();
	if(tag.startsWith("tick.尾盘涨停"))
		return new MetaEvalutor_Tick_WeiPanZhangTing();
	if(tag.startsWith("tick.尾盘拉升"))
		return new MetaEvalutor_Tick_WeiPanLaSheng();
	if(tag.startsWith("tick.区间成交量"))
		return new MetaEvalutor_Tick_SpanTrade();
	if(tag.startsWith("tick.最低价区间") || tag.startsWith("tick.最高价区间"))
		return new MetaEvalutor_Tick_SpanHighAndLow();
	if(tag.startsWith("tick.区间最高价") || tag.startsWith("tick.区间最低价"))
		return new MetaEvalutor_Tick_SpanMinMaxPrice();
	if(tag.startsWith("tick.正弦十字星") || tag.startsWith("tick.反正弦十字星"))
		return new MetaEvalutor_Tick_SinAndAntiSin();
    if(tag.startsWith("tick.分时最大成交量"))
		return new MetaEvalutor_Tick_TickMaxVol();
	return nullptr;
}


#endif 