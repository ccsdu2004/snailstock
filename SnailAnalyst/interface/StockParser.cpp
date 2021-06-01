#include <qdebug>
#include <qfile>
#include <qdatastream>
#include <qjsondocument>
#include <qjsonobject>
#include <qjsonarray.h>
#include "StockParser.h"
#include "ObjectFactory.h"
#include "../common/common.h"
#include "../common/FileUtils.h"

StockParser::StockParser()
{
}

StockParser::~StockParser()
{
}

void StockParser::onError(const QString& error)
{
	qDebug() << error << "\n";
}

void StockParser::setParam(int flag,const QString& value)
{
}

class StockTuShareParser : public StockParser
{
public:
	QStringList buildLinks(const QStringList& list)
	{
		return QStringList();
	}

	QVector<Stock> parseContent(const QString& filename,const QString& code = QString())
	{
		auto content = readAllBufferFromFile(filename);

		double close = 0.0;
		QVector<Stock> output;
		foreach(QString item,content.split("\n"))
		{
			Stock stock;
			stock.code = code;
			QStringList cs = item.split(",");
			if (cs.size() != 7)
				return output;
			stock.date = cs[0];
			stock.open = cs[1].toFloat();
			stock.current = cs[2].toFloat();
			stock.low = cs[3].toFloat();
			stock.top = cs[4].toFloat();
			stock.tradeVolume = cs[5].toDouble()*0.0001f;
			stock.tradeCapital = cs[6].toDouble()*0.0001f;
			if (close > 0.01)
				stock.increase = (stock.current - close) / close * 100.0f;
			stock.privClose = close;
			output.push_front(stock);
			close = stock.current;
		}

		std::reverse(output.begin(),output.end());
		return output;
	}

	static ObjectRegister<StockParser,StockTuShareParser,QString> dummy;
};

ObjectRegister<StockParser,StockTuShareParser,QString> StockTuShareParser::dummy("tushare");

class StockTDXParser : public StockParser
{
public:
	QStringList buildLinks(const QStringList& list)
	{
		return QStringList();
	}

	QVector<Stock> parseContent(const QString& filename,const QString& code = QString())
	{
		QVector<Stock> stocks;

		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly))
			return stocks;

		QDataStream stream(&file);
		quint32 date;
		qint32 data;

		char message[32] = { 0 };

		float close = 0.0f;
		Stock stock;
		stock.code = code;

		while (!stream.atEnd())
		{
			int len = stream.readRawData(message,32);
			if (len != 32)
				break;

			memcpy(&date,message,4);
			stock.date = QString("%1").arg(date);
			stock.date = stock.date.insert(6,"-");
			stock.date = stock.date.insert(4,"-");

			memcpy(&data,message + 4,4);
			stock.open = data * 0.01;

			memcpy(&data,message + 8,4);
			stock.top = data * 0.01;

			memcpy(&data,message + 12,4);
			stock.low = data * 0.01;

			memcpy(&data,message + 16,4);
			stock.current = data * 0.01;

			memcpy(&data,message + 20,4);
			stock.tradeCapital = data * 0.00001f;

			memcpy(&data,message + 24,4);
			stock.tradeVolume = data * 0.01f;
			stock.privClose = close;
			if (close < 0.01f)
				stock.increase = 0.0;
			else
				stock.increase = GET_INCREASE(stock.current,stock.privClose);
			close = stock.current;

			stocks.push_back(stock);
		}

		return stocks;
	}

	static ObjectRegister<StockParser,StockTDXParser,QString> dummy;
};

ObjectRegister<StockParser,StockTDXParser,QString> StockTDXParser::dummy("tdx");

class StockiFengParser : public StockParser
{
	QString targetDir;
public:
	void setParam(int flag,const QString& value)
	{
		if (StockParser::FLAG_TARGET_DIR == flag)
			targetDir = value;
	}

	QStringList buildLinks(const QStringList& list)
	{
		const QString temp = "http://api.finance.ifeng.com/akdaily/?code=%1&type=last";
		QStringList output;
		foreach(QString item,list)
		{
			/*if(item.startsWith("6"))
			item = "sh" + item;
			else
			item = "sz" + item;*/
			QString link = QString(temp).arg(item);
			output.append(link);
		}

		return output;
	}

	QVector<Stock> parseContent(const QString& filename,const QString& code)
	{
		QString content = readAllBufferFromFile(filename);
		QString buffer = content;
		buffer = buffer.remove('"');
		buffer = buffer.mid(buffer.indexOf("[[") + 1);
		buffer = buffer.mid(0,buffer.lastIndexOf("]]"));
		QStringList items = buffer.split("],[");
		QVector<Stock> output;
		foreach(QString item,items)
		{
			QStringList list = item.split(",");
			if (list.size() == 17)
			{
				Stock stock;
				stock.code = code;
				//stock.name = stocks[code].name;
				//stock.industry = stocks[code].industry;
				//stock.area = stocks[code].area;
				stock.date = list[0];
				stock.open = list[1].toFloat();
				stock.top = list[2].toFloat();
				stock.current = list[3].toFloat();
				stock.low = list[4].toFloat();
				stock.tradeVolume = list[5].toFloat();
				stock.offset = list[6].toFloat();
				stock.increase = list[7].toFloat();
				/*stock.ma5 = list[8].toFloat();
				stock.ma10 = list[9].toFloat();
				stock.ma20 = list[10].toFloat();
				stock.v_ma5 = list[11].toFloat();
				stock.v_ma10 = list[12].toFloat();
				stock.v_ma20 = list[13].toFloat();*/
				/*if(stocks[code].outstanding > 0)
				stock.turnover = (100.0f*stock.tradeVolume)/(float)stocks[code].outstanding;
				else
				stock.turnover = 0.0f;*/
				output.append(stock);
			}
		}

		if ((!code.isEmpty()) && (!targetDir.isEmpty()))
		{
			saveToFile(targetDir + code,content.toLatin1());
		}

		return output;
	}

	static ObjectRegister<StockParser,StockiFengParser,QString> dummy;
};

ObjectRegister<StockParser,StockiFengParser,QString> StockiFengParser::dummy("ifeng");
