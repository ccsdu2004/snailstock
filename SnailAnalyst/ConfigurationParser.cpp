#include <QStringList>
#include <QTextStream>
#include <qfileinfo.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include "ConfigurationParser.h"
#include "StockScriptEngine.h"
#include "StockMapper.h"
#ifdef SUPPORT_LEVEL2
#include "lv2/StockLV2Reader2019.h"
#endif

Configuration config;
Configuration globalConfig;

ConfigurationParser configParser;

void parseConfiguration(const QString& buffer,Configuration& configuration)
{
	foreach(auto line,buffer.split("\n"))
		configuration.parse(line);
}

int Configuration::parse(const QString& line)
{
	QStringList items = line.split("=");
	if (items.size() != 2)
		return -1;

#define BUILD_STR(str,var)\
	if(items[0] == str)\
	{\
		var = items[1];\
		return 1;\
	}
#define BUILD(str,var)\
	if(items[0] == str)\
	{\
		var = items[1].toFloat();\
		return 1;\
	}

	BUILD_STR("config.source.market",stockMarket)
	BUILD_STR("config.source.k.dir",stockPath)
	BUILD_STR("config.source.k.format",stockFormat)
	BUILD_STR("config.trade.buy.point",stockBuyPoint)
	BUILD_STR("config.trade.sale.point",stockSalePoint)
	BUILD_STR("config.trade.buy.order",stockBuyOrder)
	BUILD_STR("config.trade.sale.rapid",stockSaleRapid)
	BUILD("config.trade.max.hold.day",maxHoldDay)
	BUILD("config.trade.min.exclude",minExcludeDay)
	BUILD("config.trade.tradecost",tradeCost)
	BUILD_STR("config.source.from",stockStartDate)
	BUILD_STR("config.source.to",stockEndDate)
	BUILD("config.source.minactive",minActiveStockValue)
	BUILD("config.source.maxactive",maxActiveStockValue)
	BUILD("config.trade.max.profit",maxProfit)
	BUILD("config.trade.max.lost",maxLost)
	BUILD("config.trade.max.count",maxOneDayBuyStockCount)
	BUILD("config.trade.max.allow.increase",maxAllowIncrease)
	BUILD("config.trade.max.allow.decrease",maxAllowDecrease)
	BUILD_STR("config.mode",runningMode)
	BUILD_STR("config.source.pool",stockPoolName)
	BUILD("config.trade.max.subnewstock",maxListDatePos)

#ifdef SUPPORT_BASIC
	BUILD_STR("config.source.ref.index",ref300)
	BUILD_STR("config.trade.output.profittofile",outputProfitToFile)
#endif 

#ifdef SUPPORT_REF
	BUILD_STR("config.source.ref.code",refStock)
#endif

	BUILD("config.trade.buy.movepoint",moveBuyPointOffset)
	BUILD_STR("config.source.bid.dir",stockBidPath)
	BUILD_STR("config.source.per.dir",stockPerPath)

#ifdef SUPPORT_TICKET
	BUILD_STR("config.source.ticket.dir",stockTicketPath)
	BUILD("config.source.ticket.maxtickloaded",maxTickFileLoaded)
#endif
#ifdef SUPPORT_LEVEL2
	BUILD_STR("config.source.lv2.dir",stockLevel2Path)
	BUILD("config.source.lv2.v1",stockLevel2V1)
	BUILD("config.source.lv2.v2",stockLevel2V2)
	BUILD("config.source.lv2.v3",stockLevel2V3)
	BUILD("config.source.lv2.v4",stockLevel2V4)
#endif

	BUILD_STR("config.boosting.order.by",boostingOrderBy)
	
	if ("config.candlestick.theme" == items[0])
	{
		auto str = items[1];
		QStringList values;
		values << "ChartThemeLight" << "ChartThemeBlueCerulean" << "ChartThemeDark"
			<< "ChartThemeBrownSand" << "ChartThemeBlueNcs" << "ChartThemeHighContrast"
			<< "ChartThemeBlueIcy" << "ChartThemeQt";

		if (values.contains(str))
			candlestickTheme = values.indexOf(str);
		else
			candlestickTheme = 1;
	}

	if ("config.trade.启用每日累加收益" == items[0])
	{
		userAvgProfitPerDay = !(items[1] == "yes");
	}

	if ("config.stock.download.count" == items[0])
	{
		stockDownloadDayCount = items[1].toInt();
	}
	
	return 0;
}

void Configuration::reset()
{
	stockMarket = "中国大陆";
	stockPath = "data";
	stockBidPath = "bid";
	stockPerPath = "per";
	stockFormat = "default";
	stockBuyPoint = "open";
	stockSalePoint = "close";
	stockStartDate = "1980-01-01";
	stockEndDate = "2099-12-30";
#ifdef SUPPORT_TICKET
	stockTicketPath = "ticket";
#endif
	
	boostingOrderBy = "default";
	candlestickTheme = 0;
	userAvgProfitPerDay = true;

	maxHoldDay = 60;
	minExcludeDay = 1;
	maxProfit = 72.0f;
	maxLost = -10.0f;
	maxOneDayBuyStockCount = 3;
	maxAllowIncrease = 3.6f;
	maxAllowDecrease = -3.6f;	
	stockBuyOrder = "desc";
    runningMode = "回测";
	tradeCost = 2.0f;
	moveBuyPointOffset = 0.0f;
	maxTickFileLoaded = 6;
	stockPoolName.clear();

#ifdef SUPPORT_LEVEL2
	stockLevel2Path = "level2";
	stockLevel2V1 = 5;
	stockLevel2V2 = 100;
	stockLevel2V3 = 500;
#endif

	minActiveStockValue = 0.0;
	maxActiveStockValue = 100.0;
	ref300 = "sh000300";
	refStock.clear();

	stockDownloadDayCount = 480;
}

bool ConfigurationParser::loadConfig()
{
	config.stockPoolName.clear();

	if (config.stockBuyPoint != "open")
		config.stockBuyPoint = "auto";
	
	if(config.stockSalePoint != "open")
		config.stockSalePoint = "close";

	if(config.stockSaleRapid != "yes")
		config.stockSaleRapid = "no";

	if(config.tradeCost > 5 || config.tradeCost < 0)
		config.tradeCost = 0.2;

	config.maxListDatePos = 99999;

	if(config.moveBuyPointOffset > 2.0f)
		config.moveBuyPointOffset = 0.0f;
	if(config.moveBuyPointOffset < -2.0f)
		config.moveBuyPointOffset = 0.0f;
	if (config.maxTickFileLoaded < 5)
		config.maxTickFileLoaded = 5;
	else if (config.maxTickFileLoaded > 10)
		config.maxTickFileLoaded = 10;

	configParser.loadStockList();
	config.stockDownloadDayCount = 480;
	
	QFile file("config/stockpool.json");
	if (file.open(QIODevice::ReadOnly))
	{
		QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
		if (doc.isArray())
		{
			auto lists = doc.array();
			foreach(auto item,lists)
			{
				auto object = item.toObject();
				QString name = object.value("name").toString();
				auto data = object.value("data").toArray();
				QStringList units;
				foreach(auto i,data)
					units += i.toString();

				StockPoolUnit slu;
				slu.items = units;
				slu.items.append(config.ref300);
				slu.name = name;
				config.stockPool.insert(name,slu);
			}
		}
	}

	return true;
}

ConfigurationParser::ConfigurationParser()
{
}

const QString ConfigurationParser::tag()const
{
	QStringList items;
	items << "config.candlestick.theme";
	items << "config.source.market";
	items << "config.source.k.dir";
	items << "config.source.k.format";
	items << "config.source.ref.code";
	items << "config.trade.max.hold.day";
	items << "config.trade.min.exclude";
	items << "config.trade.max.profit";
	items << "config.trade.max.lost";
	items << "config.trade.max.count";
	items << "config.trade.sale.point";
	items << "config.trade.max.allow.increase";
	items << "config.trade.max.allow.decrease";
	items.append(QString("config.trade.buy.order|config.trade.sale.rapid|config.trade.sale.point|config.source.ticket.dir|config.mode|config.source.pool").split("|"));
	items.append(QString("config.trade.buy.point"));
	items.append(QString("config.source.from|config.source.to|config.trade.tradecost|config.trade.buy.movepoint|config.trade.max.subnewstock").split("|"));
	items << "source.ticket.maxtickloaded";

	items.append("config.source.lv2.dir");
	items.append("config.source.lv2.v1");
	items.append("config.source.lv2.v2");
	items.append("config.source.lv2.v3");
	items.append("config.source.lv2.v4");

	items.append("config.source.bid.dir");
	items.append("config.source.per.dir");
	items.append("config.source.ref.index");

	items.append("config.source.minactive");
	items.append("config.source.maxactive");

	items << "config.trade.启用每日累加收益";

	return items.join("|");
}

int ConfigurationParser::parse(const QString& tag)
{
	QStringList items = tag.split("=");
	if(items.size() != 2)
		return -1;

#define BUILD_STR(str,var)\
	if(items[0] == str)\
	{\
		var = items[1];\
		return 1;\
	}
#define BUILD(str,var)\
	if(items[0] == str)\
	{\
		var = items[1].toFloat();\
		return 1;\
	}
		
	BUILD_STR("config.source.market",config.stockMarket)
	BUILD_STR("config.source.k.dir",config.stockPath)
	BUILD_STR("config.source.k.format",config.stockFormat)
	BUILD_STR("config.trade.buy.point",config.stockBuyPoint)
	BUILD_STR("config.trade.sale.point",config.stockSalePoint)
	BUILD_STR("config.trade.buy.order",config.stockBuyOrder)
	BUILD_STR("config.trade.sale.rapid",config.stockSaleRapid)
	BUILD("config.trade.max.hold.day",config.maxHoldDay)
	BUILD("config.trade.min.exclude",config.minExcludeDay)
	BUILD("config.trade.tradecost",config.tradeCost)
	BUILD_STR("config.source.from",config.stockStartDate)
	BUILD_STR("config.source.to",config.stockEndDate)
	BUILD("config.source.minactive",config.minActiveStockValue)
	BUILD("config.source.maxactive",config.maxActiveStockValue)
	BUILD("config.trade.max.profit",config.maxProfit)
	BUILD("config.trade.max.lost",config.maxLost)
	BUILD("config.trade.max.count",config.maxOneDayBuyStockCount)
	BUILD("config.trade.max.allow.increase",config.maxAllowIncrease)
	BUILD("config.trade.max.allow.decrease",config.maxAllowDecrease)
	BUILD_STR("config.mode",config.runningMode)
	BUILD_STR("config.source.pool",config.stockPoolName)
	BUILD("config.trade.max.subnewstock",config.maxListDatePos)

#ifdef SUPPORT_BASIC
	BUILD_STR("config.source.ref.index",config.ref300)
#endif 

#ifdef SUPPORT_REF
    BUILD_STR("config.source.ref.code",config.refStock)
#endif

	BUILD("config.trade.buy.movepoint",config.moveBuyPointOffset)
	BUILD_STR("config.source.bid.dir",config.stockBidPath)
	BUILD_STR("config.source.per.dir",config.stockPerPath)

#ifdef SUPPORT_TICKET
	BUILD_STR("config.source.ticket.dir",config.stockTicketPath)
	BUILD("config.source.ticket.maxtickloaded",config.maxTickFileLoaded)
#endif
#ifdef SUPPORT_LEVEL2
	BUILD_STR("config.source.lv2.dir",config.stockLevel2Path)
	BUILD("config.source.lv2.v1",config.stockLevel2V1)
	BUILD("config.source.lv2.v2",config.stockLevel2V2)
	BUILD("config.source.lv2.v3",config.stockLevel2V3)
	BUILD("config.source.lv2.v4",config.stockLevel2V4)
#endif
	return 0;
}

int ConfigurationParser::loadStockList()
{
    QString filename = config.stockPath+"/stocks";
    QFile file(filename);    
	if(!file.open(QIODevice::ReadOnly))
		return -1;

	QStringList items;
	QTextStream stream(&file);
	stream.setCodec("gb2312");
	while(!stream.atEnd())
	{
		items = stream.readLine().split(",");
		if(items.size() == 6)
		{
		    Stock stock;
			stock.code = items[0];
			stock.name = items[1];
			stock.data[Stock::TAG_PE_D] = items[2];
			stock.data[Stock::TAG_PB] = items[3];
			stock.data[Stock::TAG_mktcap] = items[4];
			stock.data[Stock::TAG_nmc] = items[5];
			
			QFileInfo fileInfo(config.stockPath + "/" + stock.code);
			bool is = fileInfo.isFile();
			if(is)
				stock.data[Stock::TAG_DATA_FILE] = "有";
			else
			    stock.data[Stock::TAG_DATA_FILE] = "无";
			config.stockList.insert(stock.code,stock);

			StockMapper::instance()->add(stock.code,stock.name);
		}
	}
	return 0;
}

void ConfigurationParser::reset()
{
	config.reset();
}