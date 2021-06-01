#ifndef CONFIGURATION_PARSER_H
#define CONFIGURATION_PARSER_H
#include <QString>
#include <QVector>
#include <QMap>
#include "config.h"
#include "Stock.h"
#include "candlestick/StockRealTimeView.h"

struct StockPoolUnit
{
    QString name;
	QStringList items;
};

class ConfigurationParser
{
public:
	static bool loadConfig();
public:
	ConfigurationParser();
	const QString tag()const;
	int parse(const QString& tag);
    int loadStockList();
	void reset();
};

struct Configuration
{
    Configuration()
	{
        reset();
	}

	int parse(const QString& line);

    QString stockPath;
#ifdef SUPPORT_TICKET
	QString stockTicketPath;
#endif
#ifdef SUPPORT_LEVEL2
	QString stockLevel2Path;
	int stockLevel2V1;
	int stockLevel2V2;
	int stockLevel2V3;
	int stockLevel2V4;
	int stockLevel2V5;
#endif
	QString stockMarket;
	QString stockBidPath;
	QString stockPerPath;

	QString stockFormat;
	QString stockBuyPoint;
	QString stockSalePoint;
	QString stockSaleRapid;
	QString stockStartDate;
    QString stockEndDate;
	float minActiveStockValue;
	float maxActiveStockValue;

	int maxHoldDay;
	int minExcludeDay;
	float maxProfit;
	float maxLost;
	int maxOneDayBuyStockCount;
	float maxAllowIncrease;
	float maxAllowDecrease;
	float moveBuyPointOffset;
	int maxListDatePos;
	int maxTickFileLoaded;
	QString stockBuyOrder;
    QString runningMode;
	QMap<QString,Stock> stockList;
	float tradeCost;

	QString boostingOrderBy;
	int candlestickTheme;

	QString outputProfitToFile;

	bool userAvgProfitPerDay;
	QString stockPoolName;
	QMap<QString,StockPoolUnit> stockPool;

	QString ref300;
	QString refStock;

	int stockDownloadDayCount;

	QVector<Stock> readStocksFromFile(const QString& file);
#ifdef SUPPORT_TICKET
	StockRealTimeInfo readStockTicketFromFile(const QString& buffer);
#endif

	void reset();
private:
};

void parseConfiguration(const QString& buffer,Configuration& configuration);

extern Configuration config;
extern Configuration globalConfig;
extern ConfigurationParser configParser;

#endif 