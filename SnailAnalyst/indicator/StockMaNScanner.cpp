#include "Indicator.h"

/*StockMaNScanner::StockMaNScanner():
ma5(5),
ma10(10)
{
}

StockMaNScanner::~StockMaNScanner()
{
}

int StockMaNScanner::check(const QString& stock,const QVector<Stock>& list,QStringList& desc)
{
	ma5.clear();
	ma10.clear();

	if(list.size() < 20)
		return -1;

	QVector<std::tuple<bool,float,float,float,float>> ma5_ret,ma10_ret;
	foreach(Stock stock,list)
	{
	    ma5_ret.push_back(ma5.update(stock));
		ma10_ret.push_back(ma10.update(stock));
	}

	int cursor = list.size();
	if(std::get<1>(ma5_ret[cursor]) > std::get<1>(ma10_ret[cursor]) && 
	   std::get<1>(ma5_ret[cursor-1]) < std::get<1>(ma10_ret[cursor-1]))
	{
		desc.append("5,10日均线金叉");
	    return 0;
	}
	if(std::get<1>(ma5_ret[cursor-1]) > std::get<1>(ma10_ret[cursor-1]) && 
		std::get<1>(ma5_ret[cursor]) < std::get<1>(ma10_ret[cursor]))
	{
		desc.append("5,10日均线死叉");
	    return 1;
	}

    return -1;
}*/