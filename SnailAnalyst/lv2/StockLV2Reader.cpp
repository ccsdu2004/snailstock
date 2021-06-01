#include <QDir>
#include <QBuffer>
#include <QTextStream>
#include "common/FileMapper.h"
#include "common/FileUtils.h"
#include "zip/quazipfile.h"
#include "LV2/StockLV2Reader2019.h"
#include "SnailApplication.h"

StockLV2Reader::StockLV2Reader():
zip(new QuaZip())
{
}

StockLV2Reader::~StockLV2Reader()
{
	if(zip->isOpen())
		zip->close();
}

bool StockLV2Reader::load(const QString& zipfile)
{
	zip->setZipName(zipfile);
	return zip->open(QuaZip::mdUnzip);
}

QStringList StockLV2Reader::files()
{
	return zip->getFileNameList();
}

bool StockLV2Reader::read(const QString& file)
{
	static QString tags =
		"TranID,Time,Price,Volume,SaleOrderVolume,BuyOrderVolume,Type,SaleOrderID,SaleOrderPrice,BuyOrderID,BuyOrderPrice";

	if(!zip->setCurrentFile(file))
		return false;

	QuaZipFile qua(zip.data());
	if(!qua.open(QIODevice::ReadOnly))
		return false;

	lv2.clear();
	lv2.valid = true;

	LV2Config config = SnailApplication::getInstance()->lv2Config;

	QVector<StockI2Item*> items;

	uint count = 0;
	QString line;
	QStringList str;

	while(!qua.atEnd())
	{
		line = qua.readLine().trimmed();
		if(count==0&&line!=tags)
			return false;
		else
			count ++;

		str = line.split(",");
		if(str.size()!=11)
			return false;

		StockI2Item* info = new StockI2Item();
		info->TranID = str[0].toLong();
		info->Time = str[1];
		info->Price = str[2].toDouble();

		//TranID,Time,Price
		info->Volume = str[3].toLong();
		info->SaleOrderVolume = str[4].toLong();
		info->BuyOrderVolume = str[5].toLong();
		info->isBuy = str[6]=="B"?true:false;
		//Volume,SaleOrderVolume,BuyOrderVolume,Type,
		info->SaleOrderID = str[7].toLong();
		info->SaleOrderPrice = str[8].toFloat();
		info->BuyOrderID = str[9].toLong();
		info->BuyOrderPrice = str[10].toFloat();
		//SaleOrderID,SaleOrderPrice,BuyOrderID,BuyOrderPrice

		lv2.addBuyMap(info->BuyOrderID,info->Price*info->Volume);
		lv2.addSaleMap(info->SaleOrderID,info->Price*info->Volume);

		lv2.volume += info->Volume;
		lv2.captical += info->Price*info->Volume;

		lv2.price = info->Price;
		items.push_back(info);

		if(lv2.buyList.find(str[9])==lv2.buyList.end())
		{
			StockLV2BuyUnit unit;
			unit.starttime = str[1];
			unit.id = str[9];
			unit.endtime = unit.starttime;
			unit.positiveBuy = false;
			unit.vol = str[3].toLong();
			unit.value = unit.vol*str[2].toDouble();
			unit.price = str[10].toFloat();
			unit.count = 1;
			lv2.buyList.insert(str[9],unit);
		}
		else
		{
			auto unit = lv2.buyList[str[9]];
			unit.endtime = str[1];
			unit.value += str[3].toLong()*str[2].toDouble();
			unit.vol += str[3].toLong();
			unit.count ++;
			lv2.buyList[str[9]] = unit;
		}

		if(lv2.saleList.find(str[7])==lv2.saleList.end())
		{
			StockLV2SaleUnit unit;
			unit.starttime = str[1];
			unit.id = str[7];
			unit.endtime = unit.starttime;
			unit.positiveSale = false;
			unit.vol = str[3].toLong();
			unit.value = unit.vol*str[2].toDouble();
			unit.count = 1;
			lv2.saleList.insert(str[7],unit);
		}
		else
		{
			auto unit = lv2.saleList[str[7]];
			unit.endtime = str[1];
			unit.value += str[3].toLong()*str[2].toDouble();
			unit.vol += str[3].toLong();
			unit.count ++;
			lv2.saleList[str[7]] = unit;
		}
	}

	lv2.tickCount = items.size();

	auto itr = lv2.buyMap.begin();
	while(itr!=lv2.buyMap.end())
	{
		if(itr.value()<=config._1)
		{
			lv2.tinyBuy += itr.value();
			lv2.tinyBuyCount ++;
		}
		else if(itr.value()<=config._2)
		{
			lv2.smallBuy += itr.value();
			lv2.smallBuyCount ++;
		}
		else if(itr.value()<=config._3)
		{
			lv2.normalBuy += itr.value();
			lv2.normalBuyCount ++;
		}
		else if(itr.value()<=config._4)
		{
			lv2.bigBuy += itr.value();
			lv2.bigBuyCount ++;
		}
		else
		{
			lv2.overBuy += itr.value();
			lv2.overBuyCount ++;
		}

		itr ++;
	}

	itr = lv2.saleMap.begin();
	while(itr!=lv2.saleMap.end())
	{
		if(itr.value()<=config._1)
		{
			lv2.tinySale += itr.value();
			lv2.tinySaleCount ++;
		}
		else if(itr.value()<=config._2)
		{
			lv2.smallSale += itr.value();
			lv2.smallSaleCount ++;
		}
		else if(itr.value()<=config._3)
		{
			lv2.normalSale += itr.value();
			lv2.normalSaleCount ++;
		}
		else if(itr.value()<=config._4)
		{
			lv2.bigSale += itr.value();
			lv2.bigSaleCount ++;
		}
		else
		{
			lv2.overSale += itr.value();
			lv2.overSaleCount ++;
		}
		itr ++;
	}

	qDeleteAll(items);
	items.clear();

	return true;
}
 