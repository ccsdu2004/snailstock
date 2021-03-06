#include "StockShapeScanner.h"

StockShapeScanner::StockShapeScanner()
{
}

StockShapeScanner::~StockShapeScanner()
{
}

int StockShapeScanner::check(const QString& stock,const QVector<Stock>& list,QStringList& desc)
{
    return -1;
}

StockMShapeScanner::StockMShapeScanner():
ma5(5),
ma10(10),
ma20(20)
{
}

StockMShapeScanner::~StockMShapeScanner()
{
}

int StockMShapeScanner::checkTopPolar(float& price,int from,int to)
{
	int ret = -1;
	if(to < from)
		to = list.size();

	float d = 0.0f;
	for(int i=from;;i++)
	{
		if(i>=to)
			return -1;

		if(list[i].current > d)
		{
			d = list[i].current;
			ret = i;
		}
	}
	return ret;
}

int StockMShapeScanner::checkLowPolar(float& price,int from,int to)
{
	int ret = -1;
	float d = 99999.99f;
	for(int i=from;i<=to;i++)
	{
	    if(list[i].current < d)
		{
			d = list[i].current;
			ret = d;
		}
	}
	return ret;
}

int StockMShapeScanner::check(const QString& stock,const QVector<Stock>& stocks,QStringList& desc)
{
    ma5.clear();
	ma10.clear();
	ma20.clear();

	list = stocks;

	if(list.size() <= 60)
		return -1;

	/*bool ok = StockKScanner::isKanDie(list);
	if(!ok)
		return -1;*/

    int cursor = list.size()-1;
	float high60 = 0.0f;
	int pos60 = stock_getHighPosition(list,cursor-60,cursor,high60);
    
	float high20 = 0.0f;
	int pos20 = stock_getHighPosition(list,cursor-30,cursor,high20);

	if(pos60 != pos20)
		return -1;

	int jcpos1 = 0;
	int jcpos2 = 0;
	int count = 0;
	QVector<DataWrapper5<bool,float,float,float,float>> ma5_ret,ma10_ret,ma20_ret;
	foreach(Stock stock,list)
	{
		ma5_ret.push_back(ma5.update(stock));
		ma10_ret.push_back(ma10.update(stock));
		ma20_ret.push_back(ma20.update(stock));
		int size = ma5_ret.size()-1;
		if(ma5_ret.size() > 2 && 
		   ma5_ret[size]._2 > ma10_ret[size]._2 &&
		   ma5_ret[size-1]._2 < ma10_ret[size-1]._2)
		{
		    jcpos2 = jcpos1;
		    jcpos1 = count;
		}
		count ++;
	}

	if(ma5_ret[cursor]._2 < ma10_ret[cursor]._2)
		return -1;

	if(ma5_ret[cursor]._2 < ma20_ret[cursor]._2)
		return -1;

	float price1;
	int pos1 = stock_getHighPosition(list,jcpos2,jcpos1,price1);
	if(pos1 == -1)
		return -1;
    
	float price2;
	int pos2 = stock_getHighPosition(list,jcpos1,list.size()-1,price2);
	if(pos2 == -1)
		return -1;

	float low;
	int lowpos = stock_getLowPosition(list,pos1,pos2,low,false);

	if(price1 < low*1.09f || price2 < low*1.09f)
		return -1;

	float top_ = qMax(price1,price2);
	float low_ = qMin(price1,price2);

	if(top_ - low_ > low_*1.11f)//1.045f)
		return -1;

	output._1 = list[pos2].date;
	output._2 = list[pos2].current;
	output._3 = list[pos1].date;
	output._4 = list[pos1].current;
	output._5 = list[lowpos].date;
	output._6 = list[lowpos].current;
	output._7 = "双顶形态";
	desc.append("M形态");
	return 1;
}