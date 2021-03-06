#include <cmath>
#include "Indicator.h"
#include "StockShapeScanner.h"

StockScanner::StockScanner()
{
}

StockScanner::~StockScanner()
{
}

LianDieNDaysScanner::LianDieNDaysScanner(int day):
days(day)
{
}

LianDieNDaysScanner::~LianDieNDaysScanner()
{
}

int LianDieNDaysScanner::check(const QString& stock,const QVector<Stock>& list,QStringList& desc)
{
    if(list.size() <= days)
		return -1;

	int count = 0;
	int pos = list.size()-1-days;
	for(;;pos--)
	{
		if(list[pos].current < list[pos].open && list[pos].current < list[pos-1].current)
			count ++;
		else
			return -1;
		if(count >= days)
		{
			desc.append(QString("连跌%1天反弹可期").arg(count));
			return 0;
		}
	}

	return -1;
}
/*
StockCommentFactory::~StockCommentFactory()
{
	foreach(StockScanner* scanner,scanners)
		delete scanner;
	scanners.clear();
}

StockCommentFactory::StockCommentFactory()
{
	scanners.append(new StockMaNScanner());
	scanners.append(new StockMACDScanner());

	scanners.append(new StockVolScanner());

	scanners.append(new StockKScanner());
	scanners.append(new PricePositionScanner());
	scanners.append(new BreachScanner());
#ifdef SNAIL_VERSION_ADV
	scanners.append(new StockGaoWeiHengPanScanner());
	scanners.append(new StockDiWeiHengPanScanner());
	scanners.append(new StockMShapeScanner());
	scanners.append(new StockDuanTouChaDaoScanner());
	scanners.append(new StockYiYangChuanSanXianScanner());
	scanners.append(new StockShangShengSanJiaoXingScanner());
	scanners.append(new StockWShapeScanner());
	scanners.append(new StockHengPanTuPoScanner());
#endif 
}

void StockCommentFactory::fillString(int id)
{
	if(output.find(id) == output.end())
		output.insert(id,QStringList());
}

int StockCommentFactory::check(const QString& code,const QVector<Stock>& list,QStringList& desc)
{
	desc.clear();
	output.clear();

	QStringList items;

	foreach(StockScanner* scanner,scanners)
	{
		QStringList item;
		int flag = scanner->check(code,list,item);
		if(flag != -1)
		{
			flag = scanner->type();
			fillString(flag);
			output[flag].append(item);
		}
	}

	QSet<int> key = output.uniqueKeys().toSet();
	foreach(int k,key)
	{
	    if(output[k].isEmpty())
			output.remove(k);
	}

	desc = items;
	return 1;
}
*/