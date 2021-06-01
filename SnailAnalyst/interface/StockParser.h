#ifndef STOCK_PARSER_H
#define STOCK_PARSER_H
#include <qstring.h>
#include <qvector.h>
#include "Stock.h"

class StockParser
{
public:
	const static int FLAG_TARGET_DIR = 0;
	const static int FLAG_TARGET_FUQUAN = 1;
	const static int FLAG_TARGET_DAY = 2;
	const static int FLAG_TARGET_COUNT = 3;

	const static int FLAG_TARGET_PASE_CURRENT_INFO = 99;
public:
	StockParser();
	virtual ~StockParser();
public:
	static  float rand(){ return qrand() / (float)INT_MAX; }
	virtual void  onError(const QString& err);
	virtual QStringList buildLinks(const QStringList& list) = 0;
	virtual QVector<Stock> parseContent(const QString& filename,const QString& code = QString()) = 0;
	virtual void setParam(int flag,const QString& value);
private:
};


#endif
