#include <cmath>
#include "common/common.h"
#include "Indicator.h"
#include "SnailApplication.h"

WRIndicator::WRIndicator(int in):n(in)
{
}

WRIndicator::~WRIndicator()
{
}

DataWrapper5<bool,float,float,float,float> WRIndicator::update(const Stock& stock)
{
	datas.append(stock);

	if(datas.size() < n)
	    return DEFAULT_INDICATOR_RET;

	if(datas.size() > n)
		datas.remove(0);

	int toppos,lowpos;
	float low,high;
	stock_findStockPolar(datas,toppos,lowpos,high,low);

	float wr = (high - stock.current)/(high - low);
	return DataWrapper5<bool,float,float,float,float>(true,100.0*wr,0.0f,0.0f,0.0f);
}

