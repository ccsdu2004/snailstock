#include "StockShapeScanner.h"
/*
StockYiYangChuanSanXianScanner::StockYiYangChuanSanXianScanner():
ma5(5),
ma10(10),
ma30(30)
{
}

StockYiYangChuanSanXianScanner::~StockYiYangChuanSanXianScanner()
{
}

int StockYiYangChuanSanXianScanner::check(const QString& stock,const QVector<Stock>& list,QStringList& desc)
{
	ma5.clear();
	ma10.clear();
	ma30.clear();

	if(list.size() <= 32)
		return -1;

	QVector<DataWrapper5<bool,float,float,float,float>> m5,m10,m30;
	foreach(Stock stock,list)
	{
		m5.push_back(ma5.update(stock));
		m10.push_back(ma10.update(stock));
		m30.push_back(ma30.update(stock));
	}

	int cursor = list.size()-1;
	if(std::get<1>(m5[cursor-1]) >= list[cursor-1].current || 
	   std::get<1>(m10[cursor-1]) >= list[cursor-1].current || 
	   std::get<1>(m30[cursor-1]) >= list[cursor-1].current)
		return -1;

	if(std::get<1>(m5[cursor]) >= list[cursor].current || 
	   std::get<1>(m10[cursor]) >= list[cursor].current || 
	   std::get<1>(m30[cursor]) >= list[cursor].current)
		return -1;

	float prevtop = qMax(std::get<1>(m5[cursor-1]),qMax(std::get<1>(m10[cursor-1]),std::get<1>(m30[cursor-1])));
	float prevlow = qMin(std::get<1>(m5[cursor-1]),qMin(std::get<1>(m10[cursor-1]),std::get<1>(m30[cursor-1])));

	if((prevtop-prevlow)/prevlow*100.0f > 1.5f)
		return -1;

	if(list[cursor].increase <= 4.0f)
		return -1;

	desc.append("一阳穿三线");
    return 0;
}*/