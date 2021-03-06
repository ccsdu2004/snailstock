#include "indicator.h"

/*StockVolScanner::StockVolScanner():
vol5(5),
vol10(10),
vol60(60)
{
}

StockVolScanner::~StockVolScanner()
{
}

int StockVolScanner::check(const QString& stock,const QVector<Stock>& list,QStringList& desc)
{
    vol5.clear();
	vol10.clear();
	vol60.clear();

	if(list.size() < 20)
		return -1;

	QVector<std::tuple<bool,float,float,float,float>> vol5_ret,vol10_ret,vol60_ret;
	foreach(Stock stock,list)
	{
	    vol5_ret.append(vol5.update(stock));
	    vol10_ret.append(vol10.update(stock));
	    vol60_ret.append(vol60.update(stock));
	}

	int cursor = list.size()-1;

	//1 jx -1 sc 
	int vol0510 = 0;
	int vol1060 = 0;
#define CHECK_FLAG_JC(flag1,flag2)\
	std::get<0>(flag1[cursor]) && std::get<1>(flag1[cursor]) > std::get<1>(flag2[cursor]) && std::get<1>(flag1[cursor-1]) < std::get<1>(flag2[cursor-1])

#define CHECK_FLAG_SC(flag1,flag2)\
	std::get<0>(flag1[cursor]) && std::get<0>(flag1[cursor]) < std::get<0>(flag2[cursor]) && std::get<1>(flag1[cursor-1]) > std::get<1>(flag2[cursor-1])

	bool ok = CHECK_FLAG_JC(vol5_ret,vol10_ret);
	if(ok)
		vol0510 = 1;
	else 
	{
	    ok = CHECK_FLAG_SC(vol5_ret,vol10_ret);
		if(ok)
			vol0510 = -1;
	}

    ok = CHECK_FLAG_JC(vol10_ret,vol60_ret);
	if(ok)
		vol1060 = 1;
	else 
	{
	    ok = CHECK_FLAG_SC(vol10_ret,vol60_ret);
		if(ok)
			vol1060 = -1;
	}

	if(vol0510 == 1 && vol1060 == 1)
	{
	    desc.append("5,10日成交量金叉");
		desc.append("10,60日成交量金叉");
		return 0;
	}
	if(vol0510 == -1 && vol1060 == -1)
	{
	    desc.append("5,10日成交量死叉");
		desc.append("10,60日成交量死叉");
		return 1;
	}
	if(vol0510 == 1)
	{
	    desc.append("5,10日成交量金叉");
	    return 1;
	}
	if(vol1060 == 1)
	{
	    desc.append("10,60日成交量金叉");
	    return 1;
	}
	if(vol1060 == -1)
	{
	    desc.append("10,60日成交量死叉");
	    return 1;
	}

	if(stock_getAvg(list,cursor-20,cursor,true) <= 3.0f*stock_getAvg(list,cursor-40,cursor-20,true))
	{
		desc.append("近20日放量");
		return 1;
	}

	return -1;
}

//VolMaNIndicator
//0 平量
//1 缩放
//2 放量
int checkStockMA5Vol(const QString& code,const QVector<Stock>& list,QString& tag)
{
    if(list.size() <= 5)
		return -1;

	VolMaNIndicator vol(5);
	QVector<std::tuple<bool,float,float,float,float>> vol5;
	foreach(Stock stock,list)
		vol5.push_back(vol.update(stock));

	int cursor = list.size()-1;
	float volume = std::get<1>(vol5[cursor]);
	if(volume*0.86f > list[cursor].tradeVolume)
	{
		tag = "成交量缩量";
		return 1;
	}
	else if(volume*1.162f < list[cursor].tradeVolume)
	{
		tag = "成交量放量";
		return 2;
	}

	tag = "成交量平量";
	return 0;
}*/