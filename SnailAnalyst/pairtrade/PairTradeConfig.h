#ifndef CONFIG_ITEM_PAIR_TRADE_CONFIG_H
#define CONFIG_ITEM_PAIR_TRAFE_CONFIG_H
#include <QVector>
#include "config.h"
#include "config/ConfigItem.h"

#pragma execution_character_set("utf-8")

#ifdef SUPPORT_PAIRS_TRADE

class ConfigItemPairTradeConfig : public ConfigItem
{
public:
	ConfigItemPairTradeConfig()
	{
		BUILD_CONFIG_UNIT("config.unit.pairtrade.mincor",double(-100.0),argument);
		BUILD_CONFIG_UNIT("config.unit.pairtrade.maxcor",double(100.0),argument);
		BUILD_CONFIG_UNIT("config.unit.pairtrade.stock1",QString("sz000858"),argument);
		BUILD_CONFIG_UNIT("config.unit.pairtrade.stock2",QString("sh600276"),argument);
		BUILD_CONFIG_UNIT("config.unit.pairtrade.最少股票数据量",int(60),argument);
		BUILD_CONFIG_UNIT("config.unit.pairtrade.长久持股",bool(false),argument);
		//[-20-20]
		BUILD_CONFIG_UNIT("config.unit.pairtrade.中轴偏差",double(0.0),argument);
		//[0-50]
		BUILD_CONFIG_UNIT("config.unit.pairtrade.带宽",double(25.0),argument);
		//K值
		BUILD_CONFIG_UNIT("config.unit.pairtrade.k",double(0.0),argument);
	}

	QStringList name()const{ return QStringList(QString("unit.pairtrade.config")); }

	QStringList tags()const
	{
		QStringList output;
		foreach(auto item,argument)
			output += item.tag;
		return output;
	}

	bool parse(const QString& key,const QString& value,QVariant& var)
	{
		foreach(auto item,argument)
		{
			if (item.parse(key,value,var))
				return true;
		}
		return false;
	}
private:
	QStringList label;
	QVector<ConfigItemUnit> argument;
};

#endif
#endif