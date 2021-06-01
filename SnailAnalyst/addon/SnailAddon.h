#ifndef SNAIL_ADDON_H
#define SNAIL_ADDON_H
#include <QObject>
#include <QVector>
#include <QMap>
#include <QStringList>
#include <qmath.h>

#define GET_INCREASE(current,prev) (100.0*(current-prev)/prev)

struct SnailAddonInfo
{
	QString name;
	QString version;
	QString description;
	QString author;
};

//只保证code,start,low,top,current和tradeVolume有数据
struct Stock
{
	//市值
	static const int TAG_LIUTONGSHIZHI = 0;
	static const int TAG_nmc = 0;
	static const int TAG_ZONGSHIZHI = 1;
	static const int TAG_mktcap = 1;

	//量比,外内盘比
	static const int TAG_LIANGBI = 2;
	static const int TAG_NEIPAN = 3;
	static const int TAG_WAIPAN = 4;
	//成交量,成交额
	static const int TAG_TRADEVOL = 5;
	static const int TAG_RRADECAPTICAL = 6;
	//股本
	static const int TAG_LIUTONGGUBEN = 7;
	static const int TAG_ZONGGUBEN = 8;
	//评分
	static const int TAG_P1 = 9;
	static const int TAG_P2 = 10;
	static const int TAG_P3 = 11;
	static const int TAG_BID_P = 12;

	//龙虎榜
	static const int TAG_LHB_TYPE = 20;
	static const int TAG_LHB_DP = 21;

	//财务
	static const int TAG_PB = 26;
	static const int TAG_PE_D = 27;
	static const int TAG_PE_S = 28;
	static const int TAG_PE_TTM = 29;
	static const int TAG_DYR = 30;//Dividend Yield Ratio

	//买卖点
	static const int TAG_BS_ACTION = 40;
	static const int TAG_BS_BUY = 41;
	static const int TAG_BS_SALE = 42;

	static const int TAG_BETA = 50;

	//BS Data
	static const int TAG_BUY_DATE = 60;
	static const int TAG_SALE_DATE = 61;
	static const int TAG_BUY_PRICE = 62;
	static const int TAG_SALE_PRICE = 63;
	static const int TAG_SALE_TYPE = 64;
	static const int TAG_BS_HOLDDAY_COUNT = 65;

	//数据文件
	static const int TAG_DATA_FILE = 70;

	static const int TAG_OPEN = 80;
	static const int TAG_HIGH = 81;
	static const int TAG_LOW = 82;
	static const int TAG_CLOSE = 83;
	static const int TAG_VOL = 84;
	static const int TAG_SWING = 85;
	static const int TAG_TURNOVER = 86;
	static const int TAG_CAPITAL = 87;

	static const int TAG_TOP_VALUE = 88;
	static const int TAG_LOW_VALUE = 89;

	QString code;
	QString name;
	QString area;

	int score;

	unsigned int buy[5];
	unsigned int sale[5];

	QString industry;

	float turnover;
	float tradeVolume;
	float tradeCapital;

	double value;
	double privClose;
	double open;
	double top;
	double low;
	double current;
	double increase;
	double offset;
	float swing;
	float ma;

	QString state;
	QString date;
	QString timeToMarket;
	QString time;
	float p1;
	float p2;

	bool flag;
	QString type;
	QString jd;
	QString dp;

	float var;

	float getValueByIndex(int id)
	{
		float value = 0;
		switch (id)
		{
		case TAG_OPEN:
			value = open;
			break;
		case TAG_HIGH:
			value = top;
			break;
		case TAG_LOW:
			value = low;
			break;
		case TAG_CLOSE:
			value = current;
			break;
		case TAG_VOL:
			value = tradeVolume;
			break;
		case TAG_SWING:
			value = GET_INCREASE(top,low);
			break;
		default:
			value = current;
			break;
		}
		return value;
	}

	//date,OHLC,vol
	QString toString()
	{
		return QString("[%1,%2,%3,%4,%5,%6]").arg(date).arg(open).arg(current).arg(top).arg(low).arg(tradeVolume*0.01);
	}

	Stock()
	{
		memset(buy, 0, 5 * sizeof(unsigned int));
		memset(sale, 0, 5 * sizeof(unsigned int));

		score = 0;

		turnover = 0.0;
		tradeVolume = 0.0;

		value = 0.0;
		privClose = 0.0;
		open = 0.0;
		top = 0.0;
		low = 0.0;
		current = 0.0;
		increase = 0.0;
		offset = 0.0;
		swing = 0;
		ma = 0.0;

		p1 = 0.0;
		p2 = 0.0;

		flag = false;
		var = 0.0f;
	};

	float getMaxOC()const { return std::max<double>(open, current); }
	float getMinOC()const { return std::min<double>(open, current); }
	float getHead()const { return top - getMaxOC(); }
	float getBody()const { return getMaxOC() - getMinOC(); }
	float getFoot()const { return getMinOC() - low; }
	float getPriceCenter()const { return (getMaxOC() - getMinOC())*0.5 + getMinOC(); }

	bool isOneValue()const
	{
		return top-low < 0.01;
	}

	QMap<int,QString> data;
};

class SnailAddon : public QObject
{
	Q_OBJECT
public:
	virtual ~SnailAddon() {}
	virtual SnailAddonInfo addonInfo()const = 0;
	virtual QString tag()const = 0;
	virtual bool build(const QString& label,const QVector<double>& arguments) = 0;
	virtual bool load(const QVector<Stock>& stocks) = 0;
	virtual bool evalute(int pos,bool& fitness) = 0;
	virtual void clear() = 0;
};

Q_DECLARE_INTERFACE(SnailAddon,"com.snailstock.interface/1.0")


#endif
