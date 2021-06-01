#include <QFileInfo>
#include "interface/StockParser.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "zip/quazipfile.h"
#include "lv2/StockLV2Reader2019.h"
#include "per/StockPerData.h"
#include "DataProvider.h"
#include "StockScriptEngine.h"

DataProvider dataProvider;

bool DataProvider::loadStockFromFile(const QString& stockcode,const QString& filename,bool ref)
{	
	code = stockcode;

#ifdef SUPPORT_LEVEL2
	//StockLV2Reader2019::getInstance().clear();
#endif

	if (ref)
	{
		stocks = config.readStocksFromFile(filename);
		file = filename;
		return true;
	}

	if(code == config.ref300)
	{
		auto list = config.readStocksFromFile(filename);
		int i = 0;
		double increase = 1.0;
		double old = 0.0;
		foreach(auto item,list)
		{
			if(i == 0)
				old = item.current;
			else
			{
				if (item.date <= config.stockEndDate && item.date >= config.stockStartDate)
				{
					double inc = GET_INCREASE(item.current,old);
					increase *= (1.0 + inc * 0.01);
					hs300.insert(item.date,increase);
				}
				old = item.current;
		    }
			i ++;
		}
		loadedRef300 = true;
	    return false;
	}

	stocks = config.readStocksFromFile(filename);
	file = filename;
	return true;
}

#ifdef SUPPORT_TICKET
bool DataProvider::loadTickDataFromFileIMPL(const QString& stock,const QString& date)
{
	if (tickData.contains(date))
		return true;

	QString d(date);
	d = d.replace("-","");
	QString file = QString("%1/%2/%3").arg(config.stockTicketPath).arg(d).arg(stock);

	QFileInfo qinfo(file);
	if (!qinfo.isFile())
		return false;

	QString buffer = readAllBufferFromFile(file);
	auto info = config.readStockTicketFromFile(buffer);
	if (!info.flag)
		return false;
	this->date = date;
	tickData[date] = info;
	return true;
}

bool DataProvider::loadTickDataFromFile(const QString& stock,const QString& date)
{
	if (tickData.contains(date))
		return true;

	bool ok = loadTickDataFromFileIMPL(stock,date);
	if (ok)
		return true;

	ok = loadTickDataFromFileIMPL(stock.mid(2),date);
	if (ok)
		return true;

    return loadTickDataFromZip(stock,date);
}

bool DataProvider::loadTickDataFromZip(const QString& stock,const QString& date)
{
	QString d(date);
	d = d.replace("-","");
	d += ".zip";

	if(ticketZip->isOpen())
		ticketZip->close();

	QString zip = QString("%1/%2").arg(config.stockTicketPath).arg(d);
	ticketZip->setZipName(zip);

	if(!ticketZip->open(QuaZip::mdUnzip))
		return false;

	QString e(date);
	QString current = e.replace("-","") + "/" + stock;

	if(!ticketZip->setCurrentFile(current))
	{
		current = e.replace("-","") + "/" + stock.mid(2);
		if (!ticketZip->setCurrentFile(current))
			return false;
	}

    QuaZipFile qua(ticketZip);
    if(!qua.open(QIODevice::ReadOnly))
		return false;

	QString buffer = qua.readAll();
	auto info = config.readStockTicketFromFile(buffer);
	if(!info.flag)
		return false;
	this->date = date;
	tickData[date] = info;
	return true;
}
#endif 

bool DataProvider::loadStockBidData(const QString& date)
{
	if (bidData.contains(date))
		return true;

	QString d(date);
	d = d.remove("-");

	config.stockBidPath = "bid";
	QString filename = QString("%1//%2").arg(config.stockBidPath).arg(d);
	QString buffer = readAllBufferFromFile(filename);
	if (buffer.isEmpty())
		return false;

	QMap<QString,StockBidData> mapper;

	auto items = splitBuffer(buffer);
	foreach(auto item,items)
	{
		StockBidData bid = StockBidData::parse(item);
		mapper[bid.code] = bid;
	}

	bidData[date] = mapper;
	return true;
}

bool DataProvider::hasStockBidData(const QString& date,const QString& code)
{
	if (!bidData.contains(date))
		return false;

	auto p = bidData[date];
	return p.contains(code);
}

bool DataProvider::loadStockPerData(const QString& date)
{
	if (perStockData.contains(date))
		return true;

	//config.stockPerPath = "per";
	//QString filename = QString("%1//%2").arg(config.stockPerPath).arg(date);
	//QString buffer = readAllBufferFromFile(filename);
	//if (buffer.isEmpty())
	//	return false;

	StockPerData per;
	per.load(date);
	QMap<QString,Stock> mapper = per.toStock();
	perStockData[date] = mapper;
	perStockLimitR[date] = per.limitR;
	return true;
}

bool DataProvider::hasStockPerData(const QString& date,const QString& code)
{
	if (!perStockData.contains(date))
		return false;

	auto p = perStockData[date];
	return p.contains(code);
}

QString DataProvider::getStockCodeByFilePath(const QString& stockFile)
{
	QString code;
	if (config.stockFormat == "tdx")
	{
		code = stockFile.right(12);
		code = code.left(8);
	}
	else
	{
		QFileInfo info(stockFile);
		code = info.baseName();
	}
	return code;
}

void DataProvider::clear()
{
    code.clear();
	stocks.clear();
	file.clear();
	date.clear();
	tickData.clear();
	//tickStockData.clear();
	bidData.clear();
}

void DataProvider::reset()
{
	loadedRef300 = false;
	clear();
}