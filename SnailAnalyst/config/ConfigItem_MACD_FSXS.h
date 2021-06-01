#ifndef CONFIG_ITEM_MACD_FSXS_H
#define CONFIG_ITEM_MACD_FSXS_H
#include <QVector>
#include "ConfigItem.h"

class ConfigItem_MACD_FSXS : public ConfigItem
{
public:
	ConfigItem_MACD_FSXS()
	{   
		BUILD_CONFIG_UNIT("config.unit.macd.佛手向上.macd参数",QString("12,26,9"),argument);
		BUILD_CONFIG_UNIT("config.unit.macd.佛手向上.线上金叉",bool(false),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.佛手向上.上行最小间隔",int(3),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.佛手向上.上行最大间隔",int(36),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.佛手向上.下行最小间隔",int(3),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.佛手向上.下行最大间隔",int(36),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.佛手向上.回调最小倍率",double(0.8),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.佛手向上.慢线持续上行",bool(false),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.佛手向上.标准形态",bool(false),argument)
	}

	QStringList name()const{return QStringList(QString("unit.macd.佛手向上"));}
	
	QStringList tags()const
	{
		QStringList output;
		foreach(auto item, argument)
		{
			output += item.tag;
	    }
		return output;
	}

    bool parse(const QString& key,const QString& value,QVariant& var)
    {
		foreach(auto item,argument)
		{
			if(item.parse(key,value,var))
				return true;
		}
		return false;
    }
private:
    QStringList label;
    QVector<ConfigItemUnit> argument;
};

#endif