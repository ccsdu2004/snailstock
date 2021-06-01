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
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ǰ��ǰ����С���",int(6),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ǰ��ǰ�������",int(60),argument);

		//[2-12]
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ǰ�߷�����С���",int(3),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ǰ�߷��������",int(12),argument);
		
		//yes,no
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.�Ƿ������ͼ���Ϊǰ��",QString("yes"),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.�Ƿ������߼���Ϊǰ��",QString("yes"),argument);

		//[20.0-360.0]
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ǰ��ǰ����С�Ƿ�",double(24.0),argument);

		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ʹ�ô�����̬",QString("no"),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ʹ����������̬",QString("no"),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ʹ�����ն�����̬",QString("no"),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ʹ�ô�����̬",QString("no"),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ʹ���糿֮����̬",QString("no"),argument);

		//0-3.2
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.쳲�����������",double(1.2),argument);

		//쳲�������������,��չ����0.809
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ʹ����չ쳲���������",QString("false"),argument);
	}

	QStringList name()const{ return QStringList(QString("unit.���׵���.��������.config")); }

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
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ǰ��ǰ����С���",int(6),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ǰ��ǰ�������",int(60),argument);

		//[2-12]
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ǰ�߷�����С���",int(3),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ǰ�߷��������",int(12),argument);

		//yes,no
		//BUILD_CONFIG_UNIT("config.unit.���׵���.��������.�Ƿ������ͼ���Ϊǰ��",QString("yes"),argument);
		//BUILD_CONFIG_UNIT("config.unit.���׵���.��������.�Ƿ������߼���Ϊǰ��",QString("yes"),argument);

		//[20.0-360.0]
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ǰ��ǰ����С�Ƿ�",double(24.0),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.˫����С�Ƿ�",double(6.0),argument);

		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ʹ������ȱ����̬",QString("no"),argument);
		BUILD_CONFIG_UNIT("config.unit.���׵���.��������.ʹ�ô�������̬",QString("no"),argument);

		//0-3.2
		//BUILD_CONFIG_UNIT("config.unit.��������.쳲�����������",double(1.2),argument);

		//쳲�������������,��չ����0.809
		//BUILD_CONFIG_UNIT("config.unit.��������.ʹ����չ쳲���������",QString("false"),argument);
	}

	QStringList name()const{ return QStringList(QString("unit.���׵���.��������.config")); }

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