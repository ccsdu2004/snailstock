#include <QApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include "common/common.h"
#include "common/FileUtils.h"
#include "StockPerData.h"

bool StockPerData::load(const QString& filename)
{
	QString file = QApplication::applicationDirPath() + "//per//" + filename;
	auto buffer = readAllBufferFromFile(file);
	if(buffer.isEmpty())
		return false;

	date = filename;

	QJsonDocument doc;
	QJsonParseError error;
	doc = QJsonDocument::fromJson(buffer.toLocal8Bit(),&error);
	
	if(error.error == QJsonParseError::NoError)
	{
		auto object = doc.object();
		limitR = object["limitR"].toDouble();

		auto list = object["list"].toArray();
		for(int i = 0; i<list.count(); i++)
		{
			auto item = list[i].toObject();
			data.insert(item["code"].toString(),item);
		}
		return true;
	}
	return false;
}

QMap<QString,QString> StockPerData::toMap(const QString& code)
{
	QMap<QString,QString> output;
	if(data.contains(code))
	{
		output.insert("code",code);
		auto object = data[code];
		auto keys = object.keys();
		foreach(auto key,keys)
		{
			if(object[key].type() == QJsonValue::Type::String)
				output.insert(key,object[key].toString());
			else
			{
				QString str = STR_CEIL(object[key].toDouble());
				output.insert(key,str);
			}
		}
	}
	return output;
}

Stock StockPerData::toStock(const QString& code)
{
    Stock stock;
	if(data.contains(code))
	{
		stock.code = code;
		auto object = data[code];

		if(object.contains("code"))
			stock.code = object["code"].toString();
		if(object.contains("amount"))
			stock.tradeVolume = object["amount"].toDouble();
		if(object.contains("changepercent"))
			stock.increase = object["changepercent"].toDouble();
		if(object.contains("high"))
			stock.top = object["high"].toDouble();
		if(object.contains("low"))
			stock.low = object["low"].toDouble();
		if(object.contains("open"))
			stock.open = object["open"].toDouble();
		if(object.contains("volume"))
			stock.tradeVolume = object["volume"].toDouble();
		if(object.contains("turnover"))
			stock.turnover = object["turnover"].toDouble();
		if(object.contains("trade"))
			stock.current = object["trade"].toDouble();
		if(object.contains("settlement"))
			stock.privClose = object["settlement"].toDouble();
		if(object.contains("mktcap"))
			stock.data[Stock::TAG_ZONGSHIZHI] = object["mktcap"].toDouble();
		if(object.contains("nmc"))
			stock.data[Stock::TAG_LIUTONGSHIZHI] = object["nmc"].toDouble();
		if(object.contains("pb"))
			stock.data[Stock::TAG_PB] = object["pb"].toDouble();
		if(object.contains("per"))
			stock.data[Stock::TAG_PE_D] = object["per"].toDouble();

		stock.code = buildStockCodeByDigitStockCode(stock.code);
	}
	return stock;
}

QMap<QString,QMap<QString,QString>> StockPerData::toMap()
{
	QMap<QString,QMap<QString,QString>> output;
	auto itr = data.begin();
	while(itr!=data.end())
	{
		QString code = itr.key();
		output.insert(code,toMap(code));
		itr ++;
	}

	return output;
}

QMap<QString,Stock> StockPerData::toStock()
{
	QMap<QString,Stock> output;
	auto itr = data.begin();
	while(itr!=data.end())
	{
		QString code = itr.key();
		output.insert(code,toStock(code));
		itr ++;
	}

	return output;
}