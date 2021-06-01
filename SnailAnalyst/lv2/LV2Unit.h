#ifndef STOCK_LV2_UNIT_H
#define STOCK_LV2_UNIT_H
#include <cstdint>
#include <QString>
#include <QVector>
#include <QMap>
#include "config.h"
#include "zip/quazip.h"

#pragma execution_character_set("utf-8")

#define LV2_VALUE1_MIN 4
#define LV2_VALUE1_MAX 10

#define LV2_VALUE2_MIN 20
#define LV2_VALUE2_MAX 50

#define LV2_VALUE3_MIN 80
#define LV2_VALUE3_MAX 120

#define LV2_VALUE4_MIN 400
#define LV2_VALUE4_MAX 800

struct StockLV2Item
{
	QString id;
	QString buyid;
	QString saleid;
	QString time;
	float   price;
	int32_t vol;
};

struct StockLV2BuyUnit
{
	StockLV2BuyUnit()
	{
		vol = value = 0;
		positiveBuy = false;
		count = 0;
		price = 0.0f;
	}

	QString id;
	QString starttime;
	QString endtime;
	int64_t vol;
	int64_t value;
	int32_t count;
	float price;
	bool positiveBuy;
	QVector<StockLV2Item> items;
};

struct StockLV2SaleUnit
{
	StockLV2SaleUnit()
	{
		vol = value = 0;
		positiveSale = false;
		count = 0;
		price = 0.0;
	}

	QString id;
	QString starttime;
	QString endtime;
	int64_t vol;
	int64_t value;
	int32_t count;
	double price;
	bool positiveSale;
	QVector<StockLV2Item> items;
};

struct StockLV2
{
	void addBuyMap(uint32_t id,uint32_t value)
	{
		if(buyMap.find(id)==buyMap.end())
			buyMap.insert(id,0);
		buyMap[id] += value;
	}

	void addSaleMap(uint32_t id,uint32_t value)
	{
		if(saleMap.find(id)==saleMap.end())
			saleMap.insert(id,0);
		saleMap[id] += value;
	}

	void clear()
	{
		tickCount = 0;
		volume = 0;
		captical = 0;

		tinyBuy = 0;
		tinySale = 0;
		tinyBuyCount = 0;
		tinySaleCount = 0;

		smallBuy = 0;
		smallSale = 0;
		smallBuyCount = 0;
		smallSaleCount = 0;

		normalBuy = 0;
		normalBuyCount = 0;
		normalSale = 0;
		normalSaleCount = 0;

		bigBuy = 0;
		bigBuyCount = 0;
		bigSale = 0;
		bigSaleCount = 0;

		overBuy = 0;
		overBuyCount = 0;
		overSale = 0;
		overSaleCount = 0;

		price = 0;

		buyMap.clear();
		saleMap.clear();
		buyList.clear();
		saleList.clear();
	}

	bool netOut();
	bool netIn();

	QString code;
	QString date;

	uint32_t tickCount = 0;

	uint32_t volume = 0;
	uint32_t captical = 0;

	int32_t tinyBuy = 0;
	int32_t tinySale = 0;
	int32_t tinyBuyCount = 0;
	int32_t tinySaleCount = 0;

	int32_t smallBuy = 0;
	int32_t smallSale = 0;
	int32_t smallBuyCount = 0;
	int32_t smallSaleCount = 0;

	int32_t normalBuy = 0;
	int32_t normalBuyCount = 0;
	int32_t normalSale = 0;
	int32_t normalSaleCount = 0;

	int32_t bigBuy = 0;
	int32_t bigBuyCount = 0;
	int32_t bigSale = 0;
	int32_t bigSaleCount = 0;

	int32_t overBuy = 0;
	int32_t overBuyCount = 0;
	int32_t overSale = 0;
	int32_t overSaleCount = 0;

	bool valid = false;

	double price = 0;

	QMap<uint32_t,int32_t> buyMap;
	QMap<uint32_t,int32_t> saleMap;
	QMap<QString,StockLV2BuyUnit> buyList;
	QMap<QString,StockLV2SaleUnit> saleList;

	int buyCount()
	{
		return tinyBuyCount+smallBuyCount+normalBuyCount+bigBuyCount+overBuyCount;
	}

	int saleCount()
	{
	    return tinySaleCount+smallSaleCount+normalSaleCount+bigSaleCount+overSaleCount;
	}
	
	QMap<QString,QString> toMap();
};

struct StockI2Item
{
	uint32_t TranID;
	QString Time;
	double Price;
	uint32_t Volume;
	uint32_t SaleOrderVolume;
	uint32_t BuyOrderVolume;
	bool isBuy;
	uint32_t SaleOrderID;
	double SaleOrderPrice;
	uint32_t BuyOrderID;
	double BuyOrderPrice;
};

#endif