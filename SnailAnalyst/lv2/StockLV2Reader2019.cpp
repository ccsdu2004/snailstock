#include <QDir>
#include <QBuffer>
#include <QTextStream>
#include "common/FileMapper.h"
#include "common/FileUtils.h"
#include "zip/quazipfile.h"
#include "LV2/StockLV2Reader2019.h"
#include "SnailApplication.h"

QString getFileName(const QStringList& files,const QString& code)
{
	QString stock = code + ".csv";
	foreach(auto file,files)
	{
        if(file.endsWith(stock))
			return file;
	}

	return QString();
}

StockLV2Reader2019::StockLV2Reader2019()
{
}

StockLV2Reader2019::~StockLV2Reader2019()
{
}

StockLV2Reader2019& StockLV2Reader2019::getInstance()
{
	static StockLV2Reader2019 reader;
	return reader;
}

QStringList StockLV2Reader2019::getCodesFromZip(const QString& zipfile)
{
	QScopedPointer<QuaZip> zip(new QuaZip());
	zip->setAutoClose(true);

	zip->setZipName(zipfile);
	if(!zip->open(QuaZip::mdUnzip))
		return QStringList();

	return zip->getFileNameList();
}

bool StockLV2Reader2019::readFileFromZip(const QString& zipfile,const QString& stock)
{
	QFileInfo info(zipfile);
	QString date = info.baseName();
	if(date > "2018-07-13" && date < "2019-03-01")
		return readFileFromZip2(zipfile,stock);

	QScopedPointer<QuaZip> zip(new QuaZip());
	zip->setAutoClose(true);

	zip->setZipName(zipfile);
	if(!zip->open(QuaZip::mdUnzip))
	{
		StockLV2* unit = new StockLV2();
		cache.insert(date,unit);
		return false;
	}

	QString filename = QString("%1/%2.csv").arg(info.baseName(),stock);

	if(!zip->setCurrentFile(filename))
	{
		if(stock.size()!=8)
		{
			StockLV2* unit = new StockLV2();
			cache.insert(date,unit);
			return false;
		}

		filename = QString("%1/%2.csv").arg(info.baseName(),stock.mid(2));
		if(!zip->setCurrentFile(filename))
		{
			StockLV2* unit = new StockLV2();
			cache.insert(date,unit);
			return false;
		}
	}

	QuaZipFile qua(zip.data());
	if(!qua.open(QIODevice::ReadOnly))
	{
		StockLV2* unit = new StockLV2();
		cache.insert(date,unit);
		return false;
	}

	StockLV2* lv2 = new StockLV2();
	lv2->code = stock;
	lv2->date = info.baseName();
	
	QVector<StockI2Item*> items;
	LV2Config config = SnailApplication::getInstance()->lv2Config;

	uint count = 0;
	QString line;
	QStringList str;

	static QString tags =
		"TranID,Time,Price,Volume,SaleOrderVolume,BuyOrderVolume,Type,SaleOrderID,SaleOrderPrice,BuyOrderID,BuyOrderPrice";

	while(!qua.atEnd())
	{
		line = qua.readLine().trimmed();
		if(count==0 && line!=tags)
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

		lv2->addBuyMap(info->BuyOrderID,info->Price*info->Volume);
		lv2->addSaleMap(info->SaleOrderID,info->Price*info->Volume);

		lv2->volume += info->Volume;
		lv2->captical += info->Price*info->Volume;

		lv2->price = info->Price;
		items.push_back(info);

		if(lv2->buyList.find(str[9])==lv2->buyList.end())
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
			lv2->buyList.insert(str[9],unit);
		}
		else
		{
			auto unit = lv2->buyList[str[9]];
			unit.endtime = str[1];
			unit.value += str[3].toLong()*str[2].toDouble();
			unit.vol += str[3].toLong();
			unit.count ++;
			lv2->buyList[str[9]] = unit;
		}

		if(lv2->saleList.find(str[7])==lv2->saleList.end())
		{
			StockLV2SaleUnit unit;
			unit.starttime = str[1];
			unit.id = str[7];
			unit.endtime = unit.starttime;
			unit.positiveSale = false;
			unit.vol = str[3].toLong();
			unit.value = unit.vol*str[2].toDouble();
			unit.count = 1;
			lv2->saleList.insert(str[7],unit);
		}
		else
		{
			auto unit = lv2->saleList[str[7]];
			unit.endtime = str[1];
			unit.value += str[3].toLong()*str[2].toDouble();
			unit.vol += str[3].toLong();
			unit.count ++;
			lv2->saleList[str[7]] = unit;
		}
	}

	lv2->tickCount = items.size();

	auto itr = lv2->buyMap.begin();
	while(itr!=lv2->buyMap.end())
	{
		if(itr.value()<=config._1)
		{
			lv2->tinyBuy += itr.value();
			lv2->tinyBuyCount ++;
		}
		else if(itr.value()<=config._2)
		{
			lv2->smallBuy += itr.value();
			lv2->smallBuyCount ++;
		}
		else if(itr.value()<=config._3)
		{
			lv2->normalBuy += itr.value();
			lv2->normalBuyCount ++;
		}
		else if(itr.value()<=config._4)
		{
			lv2->bigBuy += itr.value();
			lv2->bigBuyCount ++;
		}
		else
		{
			lv2->overBuy += itr.value();
			lv2->overBuyCount ++;
		}

		itr ++;
	}

	itr = lv2->saleMap.begin();
	while(itr!=lv2->saleMap.end())
	{
		if(itr.value()<=config._1)
		{
			lv2->tinySale += itr.value();
			lv2->tinySaleCount ++;
		}
		else if(itr.value()<=config._2)
		{
			lv2->smallSale += itr.value();
			lv2->smallSaleCount ++;
		}
		else if(itr.value()<=config._3)
		{
			lv2->normalSale += itr.value();
			lv2->normalSaleCount ++;
		}
		else if(itr.value()<=config._4)
		{
			lv2->bigSale += itr.value();
			lv2->bigSaleCount ++;
		}
		else
		{
			lv2->overSale += itr.value();
			lv2->overSaleCount ++;
		}
		itr ++;
	}

	lv2->valid = true;

	qDeleteAll(items);
	items.clear();

	cache.insert(info.baseName(),lv2);
	return true;
}


bool StockLV2Reader2019::readFileFromZip2(const QString& zipfile,const QString& stock)
{
	QFileInfo info(zipfile);
	QString date = info.baseName();

	QScopedPointer<QuaZip> zip(new QuaZip());
	zip->setAutoClose(true);

	zip->setZipName(zipfile);
	if(!zip->open(QuaZip::mdUnzip))
	{
		StockLV2* unit = new StockLV2();
		cache.insert(date,unit);
		return false;
	}

	auto list = zip->getFileNameList();
	auto filename = getFileName(list,stock);

	if(filename.isEmpty())
	{
		StockLV2* unit = new StockLV2();
		cache.insert(date,unit);
		return false;
	}

	if(!zip->setCurrentFile(filename))
	{
		StockLV2* unit = new StockLV2();
		cache.insert(date,unit);
		return false;
	}

	QuaZipFile qua(zip.data());
	if(!qua.open(QIODevice::ReadOnly))
	{
		StockLV2* unit = new StockLV2();
		cache.insert(date,unit);
		return false;
	}

	StockLV2* lv2 = new StockLV2();
	lv2->code = stock;
	lv2->date = info.baseName();

	QVector<StockI2Item*> items;
	LV2Config config = SnailApplication::getInstance()->lv2Config;

	uint count = 0;
	QString line;
	QStringList str;

	while(!qua.atEnd())
	{
		line = qua.readLine().trimmed();
        count ++;

		if(count == 1)
			continue;

		str = line.split(",");
		if(str.size()!=12)
			return false;

		StockI2Item* info = new StockI2Item();

		QString time = str[0].right(8);
		QString price = str[1];
		QString vol = str[2];
		QString flag = str[3];
		QString buyPrice = str[4];
		QString buyVol = str[5];
		QString buyID = str[6];
		QString salePrice = str[8];
		QString saleVol = str[9];
		QString saleID = str[10];

		info->TranID = count;
		info->Time = time;
		info->Price = price.toDouble();
		info->Volume = vol.toLong()*100;
		info->SaleOrderVolume = saleVol.toLong();
		info->BuyOrderVolume = buyVol.toLong();
		info->isBuy = flag=="B"?true:false;
		info->SaleOrderID = saleID.toLong();
		info->SaleOrderPrice = salePrice.toFloat();
		info->BuyOrderID = buyID.toLong();
		info->BuyOrderPrice = buyPrice.toFloat();

		lv2->addBuyMap(info->BuyOrderID,info->Price*info->Volume);
		lv2->addSaleMap(info->SaleOrderID,info->Price*info->Volume);

		lv2->volume += info->Volume;
		lv2->captical += info->Price*info->Volume;

		lv2->price = info->Price;
		items.push_back(info);

		if(lv2->buyList.find(buyID)==lv2->buyList.end())
		{
			StockLV2BuyUnit unit;
			unit.starttime = time;
			unit.id = buyID;
			unit.endtime = unit.starttime;
			unit.positiveBuy = false;
			unit.vol = vol.toLong();
			unit.value = info->Price*info->Volume;
			unit.price = buyPrice.toDouble();
			unit.count = 1;
			lv2->buyList.insert(buyID,unit);
		}
		else
		{
			auto unit = lv2->buyList[buyID];
			unit.endtime = time;
			unit.value += info->Price*info->Volume;
			unit.vol += vol.toLong();
			unit.price = unit.value/(double)unit.vol;
			unit.count ++;
			lv2->buyList[buyID] = unit;
		}

		if(lv2->saleList.find(saleID)==lv2->saleList.end())
		{
			StockLV2SaleUnit unit;
			unit.starttime = time;
			unit.id = saleID;
			unit.endtime = unit.starttime;
			unit.positiveSale = false;
			unit.vol = vol.toLong();
			unit.value = info->Price*info->Volume;
			unit.count = 1;
			lv2->saleList.insert(saleID,unit);
		}
		else
		{
			auto unit = lv2->saleList[saleID];
			unit.endtime = time;
			unit.value += info->Price*info->Volume;
			unit.vol += info->Volume;
			unit.count ++;
			lv2->saleList[saleID] = unit;
		}
	}

	lv2->tickCount = items.size();

	auto itr = lv2->buyMap.begin();
	while(itr!=lv2->buyMap.end())
	{
		if(itr.value()<=config._1)
		{
			lv2->tinyBuy += itr.value();
			lv2->tinyBuyCount ++;
		}
		else if(itr.value()<=config._2)
		{
			lv2->smallBuy += itr.value();
			lv2->smallBuyCount ++;
		}
		else if(itr.value()<=config._3)
		{
			lv2->normalBuy += itr.value();
			lv2->normalBuyCount ++;
		}
		else if(itr.value()<=config._4)
		{
			lv2->bigBuy += itr.value();
			lv2->bigBuyCount ++;
		}
		else
		{
			lv2->overBuy += itr.value();
			lv2->overBuyCount ++;
		}

		itr ++;
	}

	itr = lv2->saleMap.begin();
	while(itr!=lv2->saleMap.end())
	{
		if(itr.value()<=config._1)
		{
			lv2->tinySale += itr.value();
			lv2->tinySaleCount ++;
		}
		else if(itr.value()<=config._2)
		{
			lv2->smallSale += itr.value();
			lv2->smallSaleCount ++;
		}
		else if(itr.value()<=config._3)
		{
			lv2->normalSale += itr.value();
			lv2->normalSaleCount ++;
		}
		else if(itr.value()<=config._4)
		{
			lv2->bigSale += itr.value();
			lv2->bigSaleCount ++;
		}
		else
		{
			lv2->overSale += itr.value();
			lv2->overSaleCount ++;
		}
		itr ++;
	}

	lv2->valid = true;

	qDeleteAll(items);
	items.clear();

	cache.insert(info.baseName(),lv2);
	return true;
}