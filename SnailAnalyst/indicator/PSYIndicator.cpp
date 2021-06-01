#include "indicator.h"

PSYIndicator::PSYIndicator(int in):
n(in)
{
}

PSYIndicator::~PSYIndicator()
{
}

void PSYIndicator::clear()
{
    datas.clear();
}

DataWrapper5<bool,float,float,float,float> PSYIndicator::update(const Stock& stock)
{
	DataWrapper5<bool,float,float,float,float> output(false,0,0,0,0);
	datas.push_back(stock);
	if(datas.size() < n)
		return output;
	else if(datas.size() > n)
		datas.remove(0);

	int count = 0;
	foreach(auto stock,datas)
	{
		if(stock.current > stock.privClose)
			count ++;
	}

	double value = count;
	value /= (double)n;
	return DataWrapper5<bool,float,float,float,float>(true,value*100.0,0.0f,0.f,0.0f);
}