#ifndef STOCK_MAPPER_H
#define STOCK_MAPPER_H
#include <qstring.h>
#include <qmap.h>
#include <qpair.h>
#include <qvector.h>

class StockMapper
{
public:
	static StockMapper* instance()
	{
		static StockMapper mgr;
		return &mgr;
	}

	void open(const QString& file);
	void add(const QString& code,const QString& name);
	QString get(const QString& code);
	QStringList getHKStockList();
	QVector<QPair<QString,QString>> getAHStockList();
private:
	StockMapper();
	StockMapper(const StockMapper&){}

	QMap<QString,QString> codes;
	QVector<QPair<QString,QString>> ahStockList;
};

#endif 