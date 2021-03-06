#include "Indicator.h"

/*StockMACDScanner::StockMACDScanner()
{
}

StockMACDScanner::~StockMACDScanner()
{
}

int StockMACDScanner::check(const QString& stock,const QVector<Stock>& list,QStringList& desc)
{
	indicator.clear();
	if(list.size() <= 30)
		return -1;

	QVector<std::tuple<bool,float,float,float,float>> data;
	foreach(Stock stock,list)
		data.append(indicator.update(stock));

	int cursor = data.size()-1;
	if(std::get<2>(data[cursor-1]) < std::get<3>(data[cursor-1]) && 
	   std::get<2>(data[cursor]) > std::get<3>(data[cursor]))
	{
		desc.append("MACD金叉");
	    return 0;
	}
	if(std::get<2>(data[cursor-1]) > std::get<3>(data[cursor-1]) && std::get<2>(data[cursor]) < std::get<3>(data[cursor]))
	{
		desc.append("MACD死叉");
	    return 1;
	}
	if(std::get<2>(data[cursor]) - std::get<3>(data[cursor]) > std::get<2>(data[cursor-1]) - std::get<3>(data[cursor-1]) &&
	   std::get<2>(data[cursor-1]) - std::get<3>(data[cursor-1]) < std::get<2>(data[cursor-2]) - std::get<3>(data[cursor-2]) &&
	   std::get<2>(data[cursor]) > std::get<3>(data[cursor]) &&
	   std::get<2>(data[cursor-1]) > std::get<3>(data[cursor-1]) &&
	   std::get<2>(data[cursor-2]) > std::get<3>(data[cursor-2]))
	{
		desc.append("MACD支撑");
	    return 0;
	}
	return -1;
}*/