#include "indicator.h"

ADStockIndicator::ADStockIndicator()
{
	sum = 0.0f;
}

ADStockIndicator::~ADStockIndicator()
{
}

DataWrapper5<bool,float,float,float,float> ADStockIndicator::update(const Stock& stock)
{
	DataWrapper5<bool,float,float,float,float> ret = DEFAULT_INDICATOR_RET;
	ret._1 = true;
    
	if (isEqual(stock.top,stock.low) > 0.01f)
	{
		sum += (2 * stock.current - stock.open - stock.top) / (stock.top-stock.low) * stock.tradeVolume;
		ret._2 = sum;
	}

	return ret;
}
