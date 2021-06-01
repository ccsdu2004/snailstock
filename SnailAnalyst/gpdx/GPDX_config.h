#ifndef CONFIG_GPDX_CONFIG_H
#define CONFIG_GPDX_CONFIG_H
#include <QVector>
#include "config.h"
#include "config/ConfigItem.h"

#pragma execution_character_set("utf-8")

#ifdef SUPPORT_GPDX

class ConfigItemGPDX_TZMR_Config : public ConfigItem
{
public:
	ConfigItemGPDX_TZMR_Config()
	{
		//[5-48]
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.前低前高最小间距",int(6),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.前低前高最大间距",int(60),argument);

		//[2-12]
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.前高反弹最小间距",int(3),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.前高反弹最大间距",int(12),argument);
		
		//yes,no
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.是否采用最低价作为前低",QString("yes"),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.是否采用最高价作为前高",QString("yes"),argument);

		//[20.0-360.0]
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.前高前低最小涨幅",double(24.0),argument);

		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.使用穿刺形态",QString("no"),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.使用阳吞阴形态",QString("no"),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.使用旭日东升形态",QString("no"),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.使用锤子形态",QString("no"),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.使用早晨之星形态",QString("no"),argument);

		//0-3.2
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.斐波那契最大误差",double(1.2),argument);

		//斐波那契数列类型,扩展包含0.809
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.调整买入.使用扩展斐波那契数列",QString("false"),argument);
	}

	QStringList name()const{ return QStringList(QString("unit.高抛低吸.调整买入.config")); }

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

class ConfigItemGPDX_SPMR_Config : public ConfigItem
{
public:
	ConfigItemGPDX_SPMR_Config()
	{
		//[5-48]
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.升破买入.前低前高最小间距",int(6),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.升破买入.前低前高最大间距",int(60),argument);

		//[2-12]
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.升破买入.前高反弹最小间距",int(3),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.升破买入.前高反弹最大间距",int(12),argument);

		//yes,no
		//BUILD_CONFIG_UNIT("config.unit.高抛低吸.升破买入.是否采用最低价作为前低",QString("yes"),argument);
		//BUILD_CONFIG_UNIT("config.unit.高抛低吸.升破买入.是否采用最高价作为前高",QString("yes"),argument);

		//[20.0-360.0]
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.升破买入.前高前低最小涨幅",double(24.0),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.升破买入.双低最小涨幅",double(6.0),argument);

		BUILD_CONFIG_UNIT("config.unit.高抛低吸.升破买入.使用跳空缺口形态",QString("no"),argument);
		BUILD_CONFIG_UNIT("config.unit.高抛低吸.升破买入.使用大阳线形态",QString("no"),argument);

		//0-3.2
		//BUILD_CONFIG_UNIT("config.unit.调整买入.斐波那契最大误差",double(1.2),argument);

		//斐波那契数列类型,扩展包含0.809
		//BUILD_CONFIG_UNIT("config.unit.调整买入.使用扩展斐波那契数列",QString("false"),argument);
	}

	QStringList name()const{ return QStringList(QString("unit.高抛低吸.升破买入.config")); }

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