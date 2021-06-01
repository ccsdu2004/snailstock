#include "Indicator.h"

TRIndicator::TRIndicator():
init(false)
{
}

TRIndicator::~TRIndicator()
{
}

void TRIndicator::clear()
{
	init = false;
}

DataWrapper5<bool,float,float,float,float> TRIndicator::update(const Stock& stock)
{
	DataWrapper5<bool,float,float,float,float> value = DEFAULT_INDICATOR_RET;
	value._1 = true;
	if (!init)
	{
		init = true;
		value._2 = stock.top - stock.low;
	}
	else
		value._2 = StockIndicator::getTrueRange(prev,stock);

	prev = stock;
	return value;
}
