#include "ConfigItem.h"
#include "../config.h"
#include "ConfigItem_MACD_FSXS.h"
#include "fma/Config_FMA_config.h"
#include "pairtrade/PairTradeConfig.h"
#include "gpdx/GPDX_config.h"

ConfigFactory::ConfigFactory()
{
#ifdef SUPPORT_hyperbola_algo
	//registerItem(new ConfigItem_MACD_FSXS());
	registerItem(new ConfigItem_fma_config());
#endif

#ifdef SUPPORT_PAIRS_TRADE
	registerItem(new ConfigItemPairTradeConfig());
#endif

#ifdef SUPPORT_GPDX
	registerItem(new ConfigItemGPDX_TZMR_Config());
	registerItem(new ConfigItemGPDX_SPMR_Config());
#endif
}

ConfigFactory::~ConfigFactory()
{
    foreach(auto item,configTag)    
        delete item;
	configTag.clear();
}

QStringList ConfigFactory::name()const
{
	QStringList output;
    foreach(auto item,configTag)    
        output += item->name();
    return output;
}

QStringList ConfigFactory::tags()const
{
	QStringList output;
	foreach(auto item,configTag)
	{
		auto list = item->tags();
		output += list;
    }
    return output;
}

QStringList ConfigFactory::usage()
{
	QStringList output;
    auto itr = configitem.begin();
	while(itr != configitem.end())
	{
		if(itr.value().type() == QVariant::Int)
			output += QString("%1=%2").arg(itr.key()).arg(itr.value().toInt());
		else if(itr.value().type() == QVariant::Double)
			output += QString("%1=%2").arg(itr.key()).arg(itr.value().toDouble());
		else if(itr.value().type() == QVariant::Bool)
			output += QString("%1=%2").arg(itr.key()).arg(itr.value().toBool() ? "true" : "false");
		else
			output += QString("%1=%2").arg(itr.key()).arg(itr.value().toString());
		itr ++;
	}
    return output;
}

void ConfigFactory::registerItem(ConfigItem* item)
{
    configTag.push_back(item);
}
   
bool ConfigFactory::parse(const QString& line)
{
    QStringList keyvalue = line.split("=");
    if(keyvalue.size() != 2)
    	return false;

	QVariant var;
	bool ok = parse(keyvalue[0],keyvalue[1],var);
	if(ok)
	{
		configitem.insert(keyvalue[0],var);
	}
	return ok;
}

bool ConfigFactory::parse(const QString& key,const QString& value,QVariant& var)
{
    foreach(auto item,configTag)
	{
		if(item->parse(key,value,var))
			return true;
	}
	return false;
}

bool ConfigFactory::getVar(const QString& key,QVariant& var)
{
    if(configitem.find(key) == configitem.end())
		return false;
	var = configitem[key];
	return true;
}