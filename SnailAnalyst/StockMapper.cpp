#include <qfile.h>
#include <qtextstream.h>
#include "StockMapper.h"

StockMapper::StockMapper()
{

}

void StockMapper::open(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return;

	QStringList items;
	QTextStream stream(&file);
	stream.setCodec("gb2312");
	while (!stream.atEnd())
	{
		items = stream.readLine().split(",");
		if (items.size() >= 2)
		{
			codes.insert(items.at(0),items.at(1));
		}
	}
}

void StockMapper::add(const QString& code,const QString& name)
{
	codes.insert(code,name);
}

QString StockMapper::get(const QString& code)
{
	if (codes.find(code) == codes.end())
		return QString();
	return codes[code];
}

QStringList StockMapper::getHKStockList()
{
	QStringList output;
	auto itr = codes.begin();
	while (itr != codes.end())
	{
		if (itr.key().startsWith("hk"))
			output += itr.key();
		itr ++;
	}
	return output;
}

QVector<QPair<QString,QString>> StockMapper::getAHStockList()
{
	if (ahStockList.isEmpty())
	{
		QFile file(":data/ah");
		if (file.open(QIODevice::ReadOnly))
		{
			QStringList items;
			QTextStream stream(&file);
			stream.setCodec("gb2312");
			while (!stream.atEnd())
			{
				items = stream.readLine().split(",");
				if (items.size() == 2)
					ahStockList.push_back(QPair<QString,QString>(items[0],items[1]));
			}
		}
	}

	return ahStockList;
}