#include <QDir>
#include "zip/quazipfile.h"
#include "LV2/StockLV2Reader.h"

#define MAX_LV2_CACHE_SIZE 150

#define R 0.333

StockLV2Reader StockLV2Reader::reader;

StockLV2Reader::StockLV2Reader()
{
    zip = new QuaZip();
}

StockLV2Reader::~StockLV2Reader()
{
	qDeleteAll(items);
	items.clear();
	delete zip;
}

StockLV2Reader& StockLV2Reader::getInstance()
{
	return reader;
}
    
bool StockLV2Reader::readFileFromZip(const QString& zipfile,const QString& dir,const QString& stock)
{
	foreach(auto item,items)
		delete item;
	items.clear();
	lv2.clear();

	if (zip->isOpen())
		zip->close();

	zip->setZipName(zipfile);
	if (!zip->open(QuaZip::mdUnzip))
		return false;

	QString filename = QString("%1/%2.csv").arg(dir,stock);

	if (!zip->setCurrentFile(filename))
	{
		if (stock.size() != 8)
			return false;

		filename = QString("%1/%2.csv").arg(dir,stock.mid(2));
		if (!zip->setCurrentFile(filename))
			return false;
	}

	QuaZipFile qua(zip);
	if (!qua.open(QIODevice::ReadOnly))
		return false;

	int count = 0;
	QString line;
	QStringList lines;
	while (!qua.atEnd())
	{
		line = qua.readLine().trimmed();
		lines = line.split(",");
		if (lines.size() != 11)
			break;

		StockI2Item* info = new StockI2Item();
		info->TranID = lines[0].toLong();
		info->Time = lines[1];
		info->Price = lines[2].toFloat();
		//TranID,Time,Price,
		info->Volume = lines[3].toLong();
		info->SaleOrderVolume = lines[4].toLong();
		info->BuyOrderVolume = lines[5].toLong();
		info->isBuy = lines[6] == "B" ? true : false;
		//Volume,SaleOrderVolume,BuyOrderVolume,Type,
		info->SaleOrderID = lines[7].toLong();
		info->SaleOrderPrice = lines[8].toFloat();
		info->BuyOrderID = lines[9].toLong();
		info->BuyOrderPrice = lines[10].toFloat();
		//SaleOrderID,SaleOrderPrice,BuyOrderID,BuyOrderPrice

		lv2.volume += info->Volume;
		lv2.addBuyMap(info->BuyOrderID,info->Price*info->Volume);
		lv2.addSaleMap(info->SaleOrderID,info->Price*info->Volume);

		lv2.price = info->Price;
		items.push_back(info);

		if (lv2.buyList.find(lines[9]) == lv2.buyList.end())
		{
			StockLV2BuyUnit unit;
			unit.starttime = lines[1];
			unit.id = lines[9];
			unit.endtime = unit.starttime;
			unit.positiveBuy = false;
			unit.vol = lines[3].toLong();
			unit.value = unit.vol*lines[2].toDouble();
			unit.price = lines[10].toFloat();
			unit.count = 1;
			lv2.buyList.insert(lines[9],unit);
		}
		else
		{
			auto unit = lv2.buyList[lines[9]];
			unit.endtime = lines[1];
			unit.value += lines[3].toLong()*lines[2].toDouble();
			unit.vol += lines[3].toLong();
			unit.count ++;
			lv2.buyList[lines[9]] = unit;
		}

		if (lv2.saleList.find(lines[7]) == lv2.saleList.end())
		{
			StockLV2SaleUnit unit;
			unit.starttime = lines[1];
			unit.id = lines[7];
			unit.endtime = unit.starttime;
			unit.positiveSale = false;
			unit.vol = lines[3].toLong();
			unit.value = unit.vol*lines[2].toDouble();
			unit.count = 1;
			lv2.saleList.insert(lines[7],unit);
		}
		else
		{
			auto unit = lv2.saleList[lines[7]];
			unit.endtime = lines[1];
			unit.value += lines[3].toLong()*lines[2].toDouble();
			unit.vol += lines[3].toLong();
			unit.count ++;
			lv2.saleList[lines[7]] = unit;
		}

		count ++;
	}

	lv2.tickCount = items.size();

	auto itr = lv2.buyMap.begin();
	while(itr!=lv2.buyMap.end())
	{
		if(itr.value()<=tinyValue)
		{
		   lv2.tinyBuy += itr.value()/10000;
		   lv2.tinyBuyCount ++;
	    }
	    else if(itr.value()<=value)
		{
			lv2.normalBuy += itr.value() / 10000;
			lv2.normalBuyCount ++;
		}
		else if (itr.value() <= bigValue)
		{
			lv2.bigBuy += itr.value() / 10000;
			lv2.bigBuyCount ++;
		}
		else
		{
			lv2.overBuy += itr.value() / 10000;
			lv2.overBuyCount ++;
		}

		lv2.captical += itr.value();
		itr ++;
	}

	itr = lv2.saleMap.begin();
	while (itr != lv2.saleMap.end())
	{
		if(itr.value()<=tinyValue)
		{
			lv2.tinySale += itr.value()/10000;
			lv2.tinySaleCount ++;
		}
		else if(itr.value()<=value)
		{
			lv2.normalSale += itr.value() / 10000;
			lv2.normalSaleCount ++;
		}
		else if (itr.value() <= bigValue)
		{
			lv2.bigSale += itr.value() / 10000;
			lv2.bigSaleCount ++;
		}
		else
		{
			lv2.overSale += itr.value() / 10000;
			lv2.overSaleCount ++;
		}
		itr ++;
	}

	int sale = 0.25*lv2.normalSale;
	int bs = R*mainForce;
	sale = std::min(sale,bs);
	mainForce = mainForce - sale + lv2.bigBuy + lv2.overBuy - lv2.bigSale - lv2.overSale;
	if (mainForce < 0)
		mainForce = 0;

	lvData[dir] = lv2;
	if (lvData.size() > MAX_LV2_CACHE_SIZE)
	{
		lvData.erase(lvData.begin());
	}

	return true;
}

QStringList StockLV2Reader::getHeader()const
{
    QStringList items;
	items << "日期"<<"成交金额"<<"中买单"<<"中买单数"<<"中卖单"<<"中卖单数"<<"大买单"<<"大买单数"<<"大卖单"<<"大卖单数"<<"超大买单"<<"超大买单数"<<"超大卖单"<<"超大卖单数"<<"散买单"<<"散卖单"<<"股价"<<"散买%"<<"散卖/散买%";
	return items;
}

QStringList StockLV2Reader::getStockLV2FlowList()const
{
	QStringList items;
	items << "类型" << "开始时间" << "结束时间" << "成交量(手)" << "成交金额(万)" << "吞单量"<<"均价";
	return items;
}