#include "Indicator.h"

OBVStockIndicator::OBVStockIndicator()
{
	obv = 0.0f;
}

OBVStockIndicator::~OBVStockIndicator()
{
}

DataWrapper5<bool,float,float,float,float> OBVStockIndicator::update(const Stock& stock)
{
	DataWrapper5<bool,float,float,float,float> ret = DEFAULT_INDICATOR_RET;
	ret._1 = true;

	if (stock.current > stock.privClose)
		obv += stock.tradeVolume;
	else if (stock.current < stock.privClose)
		obv -= stock.tradeVolume;

	ret._2 = obv;
	return ret;
}
