#include "indicator.h"

BiasIndicator::BiasIndicator(int n)
{
    maIndicator = new MaNIndicator(n);
	init = false;
}

BiasIndicator::~BiasIndicator()
{
    delete maIndicator;
}

void BiasIndicator::clear()
{
    maIndicator->clear();
}

DataWrapper5<bool,float,float,float,float> BiasIndicator::update(const Stock& stock)
{
	DataWrapper5<bool,float,float,float,float> ret = maIndicator->update(stock);
	if(ret._1)// && init == true)
		ret._2 = (stock.current - ret._2)/ret._2*100.0;
	return ret;
}
