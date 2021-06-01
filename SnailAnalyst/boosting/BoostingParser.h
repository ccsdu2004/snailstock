#ifndef BOOSTING_PARSER_H
#define BOOSTING_PARSER_H
#include <QString>
#include <QPair>
#include <QVector>
#include <QMap>
#include "config.h"

#pragma execution_character_set("utf-8")

#ifdef SUPPORT_BOOSTING_ALGO

struct BoostingVar
{
	static BoostingVar* create(const QString& tag);
	virtual ~BoostingVar();
	virtual bool evalute(const QString& key,const QString& value) = 0;
	QString name;
	QStringList values;
};

struct BoostingConfig
{
	BoostingConfig():longSuccess(60.0),
		longRetreats(-32.0),
		longProfit(1.2),
		minTradeCount(5),
		showAll(false),
		forEachStock(false)
	{
	}
    double longSuccess;
	double longRetreats;
	double longProfit;
	int    minTradeCount;
	bool   showAll;
	bool   forEachStock;
};

class BoostingParser
{
public:
	static QStringList keywords();
	static BoostingConfig boostConfig;
	static void parse(const QString& name,const QString& value);
	static bool show(int count,double success,double avg,double maxdown);

	BoostingParser();
	bool parse(const QString& buffer);
	QVector<QString> getScripts()const{return scriptList;}
private:
	bool buildScripts();

	QVector<QString> scriptList;
	QVector<QPair<QString,QString>> items;

	QStringList varKeys;
	QVector<QStringList> varValues;

	QString stockPool;
	QStringList stocksInStockPool;
};

#endif

#endif

