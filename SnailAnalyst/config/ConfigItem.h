#ifndef CONFIG_FACTORY_H
#define CONFIG_FACTORY_H
#include <QStringList>
#include <QList>
#include <QVariant>
#include "../common/common.h"

#define BUILD_CONFIG_UNIT(str,value,argument)\
	{\
		ConfigItemUnit unit(QString(str),QVariant(value));\
		argument.push_back(unit);\
	}

#define CHECK_CONFIG_VAR_DOUBLE(value,low,high,order)\
	QVariant var;\
	if(SnailContext::open()->getConfigFactory()->getVar(QString(order),var))\
	{\
		value = var.toDouble();\
		if(value < low)\
			value = low;\
		else if(mink > high)\
			value = high;\
	}

#define CHECK_CONFIG_VAR_INT(value,low,high,order)\
	QVariant var;\
	if(SnailContext::open()->getConfigFactory()->getVar(QString(order),var))\
	{\
		value = var.toInt();\
		if(value < low)\
			value = low;\
		else if(mink > high)\
			value = high;\
	}

struct ConfigItemUnit
{
	ConfigItemUnit()
	{
	}
    
	ConfigItemUnit(const QString& key,const QVariant& var):
	    tag(key),
		flag(var)
    {
    }

    bool parse(const QString& key,const QString& value,QVariant& var)
    {
        if(key == tag)
        {
			if(!strcmp(flag.typeName(),"int"))
				var.setValue<int>(value.toInt());
			else if(!strcmp(flag.typeName(),"double"))
				var.setValue<double>(value.toDouble());
			else if(!strcmp(flag.typeName(),"bool"))
				var.setValue<bool>(value == "true");
			else
				var.setValue<QString>(value);
			return true;
        }	
        return false;
    }
    
	QString tag;
    QVariant flag;
};

class ConfigItem
{
public:
	virtual QStringList name()const = 0;
	virtual QStringList tags()const = 0;
    virtual bool parse(const QString& key,const QString& value,QVariant& var) = 0;
	QString bind(const QString& config,const QVariant& var)
	{
	    QString output("%1=%2");
		if(var.type() == QVariant::Int)
			output = output.arg(config).arg(var.toInt());
		else if(var.type() == QVariant::Double)
			output = output.arg(config).arg(var.toDouble());
		else if(var.type() == QVariant::Bool)
			output = output.arg(config).arg(var.toBool() ? "true":"false");
		else
			output = output.arg(config).arg(var.toString());
		return output;
	}
};

class ConfigFactory : public ConfigItem
{
public:
	ConfigFactory();
	~ConfigFactory();

	QStringList name()const;
	QStringList tags()const;
	QStringList usage(); 

	void registerItem(ConfigItem* item);
    bool parse(const QString& item);
    bool parse(const QString& key,const QString& value,QVariant& var);
    bool getVar(const QString& key,QVariant& result);
private:
	QMap<QString,QVariant> configitem;
	QList<ConfigItem*> configTag;  
};

#endif