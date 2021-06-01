#include "indicator.h"

StockWadIndicator::StockWadIndicator():
init(false),
sum(0.0f)
{
}

StockWadIndicator::~StockWadIndicator()
{
}

DataWrapper5<bool,float,float,float,float> StockWadIndicator::update(const Stock& stock)
{
	auto ret = DEFAULT_INDICATOR_RET;
	ret._1 = true;
	ret._2 = stock.current;
	if (init == false)
		init = true;
	else
	{
		if (stock.current > stock.privClose)
			sum += stock.current - std::min(stock.privClose,stock.low);
		else if (stock.current < stock.privClose)
			sum += stock.current - std::max(stock.privClose,stock.top);
	}

	return ret;
}
