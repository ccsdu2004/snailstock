#ifndef STOCK_LV2_READER_H
#define STOCK_LV2_READER_H
#include "LV2Unit.h"

class StockLV2Reader
{
public:
	static StockLV2Reader& getInstance();

	QStringList getHeader()const;
	QStringList getStockLV2FlowList()const;

	void setConfig(int v1,int v2,int v3)
	{
	    tinyValue = v1*10000;
		value = v2*10000;
		bigValue = v3*10000;
	}

	QMap<QString,StockLV2> getLV2Data()const{return lvData;}
	void clear()
	{
		lvData.clear();
		mainForce = 0;
	}

	bool readFileFromZip(const QString& zipfile,const QString& dir,const QString& stock);
private:
	StockLV2Reader();
	~StockLV2Reader();

	static StockLV2Reader reader;

	int tinyValue,value,bigValue;

	QuaZip* zip;
	QVector<StockI2Item*> items;

	StockLV2 lv2;
	QMap<QString,StockLV2> lvData;
	int32_t mainForce;
};

#endif