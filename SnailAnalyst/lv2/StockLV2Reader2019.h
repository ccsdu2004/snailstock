#ifndef STOCK_LV2_READER_2019_H
#define STOCK_LV2_READER_2019_H
#include <QCache>
#include <QScopedPointer>
#include "LV2Unit.h"
#include "LV2Config.h"

class StockLV2Reader
{
public:
	StockLV2Reader();
	~StockLV2Reader();
public:
	bool load(const QString& zip);
	QStringList files();
	StockLV2 data()const{return lv2;}
	bool read(const QString& file);
private:
	StockLV2 lv2;
	QScopedPointer<QuaZip> zip;
};

class StockLV2Reader2019
{
public:
	static StockLV2Reader2019& getInstance();

	void clear(){cache.clear();}
	StockLV2* data(const QString& date)const{return cache.contains(date) ? cache[date]:nullptr;}
	
	bool readFileFromZip(const QString& zipfile,const QString& stock);
	QStringList getCodesFromZip(const QString& zipfile);
private:
	bool readFileFromZip2(const QString& zipfile,const QString& stock);

	StockLV2Reader2019();
	~StockLV2Reader2019();

	QCache<QString,StockLV2> cache;
};

#endif