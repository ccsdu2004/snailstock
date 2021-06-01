#include "Indicator.h"

PVIStockIndicator::PVIStockIndicator()
{
	clear();
}

PVIStockIndicator::~PVIStockIndicator()
{
}

DataWrapper5<bool,float,float,float,float> PVIStockIndicator::update(const Stock& stock)
{
	if (init == false)
	{
		prev = stock;
		init = true;
		return DataWrapper5<bool,float,float,float,float>(true,pvi,0,0,0);
    }
	
    if (stock.tradeVolume > prev.tradeVolume)
	{
		pvi += (stock.current - prev.current) / prev.current * pvi;
	}
	prev = stock;
	auto ret = DEFAULT_INDICATOR_RET;
	ret._2 = pvi;
	ret._1 = true;
	return ret;
}