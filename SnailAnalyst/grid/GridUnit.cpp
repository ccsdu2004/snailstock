#include "GridTradeWidget.h"

QMap<QString,QString> GridUnit::toMap()
{
	QMap<QString, QString> map;
	map["buyDate"] = buyDate;
	map["buyPrice"] = STR_CEIL(buyPrice);
	map["saleDate"] = saleDate;
	map["lastPrice"] = STR_CEIL(lastPrice);
	
	if(!saleDate.isEmpty())
	{
		double increase = GET_INCREASE(salePrice,buyPrice);
		map["increase"] = STR_CEIL(increase);
		map["salePrice"] = STR_CEIL(salePrice);
	}
	else
	{
		map["increase"] = " - ";
		map["salePrice"] = "≥÷π…÷–..";
	}

	return map;
}