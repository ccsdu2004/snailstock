#ifndef STOCK_PER_DATA_H
#define STOCK_PER_DATA_H
#include <QJsonObject>
#include "Stock.h"

class StockPerData
{
public:
	QString date;
	double limitR = 0.0;
	QMap<QString,QJsonObject> data;

	bool load(const QString& filename);

	QMap<QString,QMap<QString,QString>> toMap();
	QMap<QString,Stock> toStock();
private:
	QMap<QString,QString> toMap(const QString& code);
	Stock toStock(const QString& code);
};

#endif


