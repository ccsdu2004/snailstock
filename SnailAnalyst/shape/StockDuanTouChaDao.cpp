#include "StockShapeScanner.h"

StockDuanTouChaDaoScanner::StockDuanTouChaDaoScanner():
ma5(5),
ma10(10),
ma20(20)
{
}

StockDuanTouChaDaoScanner::~StockDuanTouChaDaoScanner()
{
}

int StockDuanTouChaDaoScanner::check(const QString& stock,const QVector<Stock>& list,QStringList& desc)
{
    ma5.clear();
	ma10.clear();
	ma20.clear();

	if(list.size() <= 25)
		return -1;

    QVector<DataWrapper5<bool,float,float,float,float>> m5ret,m10ret,m20ret;
	foreach(Stock stock,list)
	{
		m5ret.push_back(ma5.update(stock));
		m10ret.push_back(ma10.update(stock));
		m20ret.push_back(ma20.update(stock));
	}

	int cursor = list.size()-1;
	if(list[cursor].increase >= -3.0f)
		return -1;

#define GET(set,pos) set[cursor]._2

	if(list[cursor].current >= GET(m5ret,cursor) ||
	   list[cursor].current >= GET(m10ret,cursor) ||
	   list[cursor].current >= GET(m20ret,cursor))
	   return -1;

	if(list[cursor-1].current <= GET(m5ret,cursor-1) ||
	   list[cursor-1].current <= GET(m10ret,cursor-1) ||
	   list[cursor-1].current <= GET(m20ret,cursor-2))
	   return -1;

	output._1 = list[cursor].date;
	output._2 = list[cursor].current;
	output._7 = "断头铡刀";
	desc.append("断头铡刀");

	return 1;
}