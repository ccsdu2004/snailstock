#include "indicator.h"

EMAIndicator::EMAIndicator(int in):
n(in)
{
}

EMAIndicator::~EMAIndicator()
{
}

void EMAIndicator::clear()
{
    dataN.clear();
	ema = 0;
}

DataWrapper5<bool,float,float,float,float> EMAIndicator::update(const Stock& stock)
{
	DataWrapper5<bool,float,float,float,float> output(false,0,0,0,0);
	dataN.push_back(stock);
	if(dataN.size() < n)
	{
		ema = stock.current;
		return output;
	}
	else if(dataN.size() > n)
		dataN.remove(0);

	float alpha = 2.0f/(1.0f+n);
	ema = alpha*stock.current + (1.0f-alpha)*ema;
	return DataWrapper5<bool,float,float,float,float>(true,ema,0.0f,0.f,0.0f);
}