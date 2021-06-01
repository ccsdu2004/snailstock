#ifndef STOCK_KANALYZER
#define STOCK_KANALYZER
#include <QVector>
#include <QStringList>
#include <QMap>
#include <functional>
#include "SnailAddonHost.h"

class StockKAnalyzer
{
public:
	static StockKAnalyzer* instance();
	QStringList keywords();
	bool isThisK(const QString& id,const QVector<Stock>& list);
private:
	void init();
	QMap<QString,std::function<bool(const QVector<Stock>&)>> table;
};

#endif 