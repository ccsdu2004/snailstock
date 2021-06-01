#ifndef CONFIG_ITEM_FMA_CONFIG_H
#define CONFIG_ITEM_FMA_CONFIG_H
#include <QVector>
#include "config/ConfigItem.h"

#pragma execution_character_set("utf-8")

class ConfigItem_fma_config : public ConfigItem
{
public:
	ConfigItem_fma_config()
	{   
		BUILD_CONFIG_UNIT("config.unit.ma.fma.��С����б��",double(0.24),argument);
		BUILD_CONFIG_UNIT("config.unit.ma.fma.��С����",int(6),argument)
		BUILD_CONFIG_UNIT("config.unit.ma.fma.��С����",int(5),argument)
		BUILD_CONFIG_UNIT("config.unit.ma.fma.��С���",int(36),argument)
		BUILD_CONFIG_UNIT("config.unit.ma.fma.���ɽ���ƫ��",double(1.0),argument)
	}

	QStringList name()const{return QStringList(QString("unit.unit.ma.fma"));}
	
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