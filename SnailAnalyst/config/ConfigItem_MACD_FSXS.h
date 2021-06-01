#ifndef CONFIG_ITEM_MACD_FSXS_H
#define CONFIG_ITEM_MACD_FSXS_H
#include <QVector>
#include "ConfigItem.h"

class ConfigItem_MACD_FSXS : public ConfigItem
{
public:
	ConfigItem_MACD_FSXS()
	{   
		BUILD_CONFIG_UNIT("config.unit.macd.��������.macd����",QString("12,26,9"),argument);
		BUILD_CONFIG_UNIT("config.unit.macd.��������.���Ͻ��",bool(false),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.��������.������С���",int(3),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.��������.���������",int(36),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.��������.������С���",int(3),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.��������.���������",int(36),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.��������.�ص���С����",double(0.8),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.��������.���߳�������",bool(false),argument)
		BUILD_CONFIG_UNIT("config.unit.macd.��������.��׼��̬",bool(false),argument)
	}

	QStringList name()const{return QStringList(QString("unit.macd.��������"));}
	
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