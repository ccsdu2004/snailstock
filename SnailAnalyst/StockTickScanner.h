#ifndef TICK_SCANNER_H
#define TICK_SCANNER_H
#include <QVector>
#include <QStringList>
#include <QMap>
#include <functional>
#include "SnailAddonHost.h"

class StockTickAnalyzer
{
public:
	static MetaEvalutor* create(const QString& tag);
	static QStringList   keywords();
};

#endif 