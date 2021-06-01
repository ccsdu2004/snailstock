#include <algorithm>
#include "StockShapeScanner.h"
/*
StockUpwardTrendScanner::StockUpwardTrendScanner():
ma5(SHORT_TERM_MA),
ma20(LONG_TERM_MA)
{
	minDistance = 29;
	maxIncrease = 3.61f;
	maxDecrease = -3.2f;
	currentMaxIncrease = 4.8f;
	maxLap = 3;
}

StockUpwardTrendScanner::~StockUpwardTrendScanner()
{
}

int StockUpwardTrendScanner::check(const QString& stock,const QVector<Stock>& stocks,QStringList& desc)
{
    ma5.clear();
	ma20.clear();

	list = stocks;

	if(list.size() <= 25+LONG_TERM_MA)
		return -1;

	QVector<std::tuple<bool,float,float,float,float>> m5,m20;
	foreach(Stock stock,stocks)
	{
		m5.push_back(ma5.update(stock));
		m20.push_back(ma20.update(stock));
	}

	std::reverse(m5.begin(),m5.end());
	std::reverse(m20.begin(),m20.end());
	std::reverse(list.begin(),list.end());

	int lap = 0;

	for(int i=0;i<minDistance;i++)
	{
		if(std::get<1>(m5[i]) <= std::get<1>(m20[i]))
			return -1;
		if(i > 0 && list[i].increase > maxIncrease)
			return -1;
		if(i > 0 && list[i].increase < maxDecrease)
			return -1;
	}

	float price;
	int pos = stock_getLowPosition(list,0,minDistance,price,false);
	if(pos < minDistance*0.66+1)
		return -1;

	int lap = 0;

	QString tag;
	bool ok = StockKScanner::isKanZhang(list,tag,true);
	if(!ok)
		return -1;

    int cursor = list.size()-1;
	float low45 = 9999.99f;
	int pos45 = stock_getLowPosition(list,cursor-45,cursor-3,low45);
    
	float high21 = 0.0f;
	int pos21 = stock_getHighPosition(list,cursor-21,cursor-1,high21);

	if(pos45 >= pos21)
		return -1;

	float diff = high21-low45;
	if(diff/low45 < 0.2f)
		return -1;

	float current = qMin(list[cursor].open,list[cursor].current);
	diff = qMax(low45,current) - qMin(low45,current);
	diff /= qMin(low45,current);
	if(diff > 0.02f)
		return -1;

	output._1 = list[pos45].date;
	output._2 = list[pos45].current;
	output._3 = list[pos21].date;
	output._4 = list[pos21].current;
	output._5 = list[cursor].date;
	output._6 = current;
	output._7 = "中短期趋势向上";
	desc.append(output);
	return 0;
}*/