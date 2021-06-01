#include <cmath>
#include "common/common.h"
#include "Indicator.h"

BOPStockIndicator::BOPStockIndicator()
{
}

BOPStockIndicator::~BOPStockIndicator()
{
}

DataWrapper5<bool,float,float,float,float> BOPStockIndicator::update(const Stock& stock)
{
	float value = 0.0;
	if (stock.top > stock.low)
		value = (stock.privClose - stock.open) / (stock.top - stock.low);
	return DataWrapper5<bool,float,float,float,float>(true,value,0,0,0);
}