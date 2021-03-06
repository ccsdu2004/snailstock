#include <cmath>
#include "common/common.h"
#include "Indicator.h"

BollIndicator::BollIndicator(int in,double im):
n(in),
m(im)
{
}

DataWrapper5<bool,float,float,float,float> BollIndicator::update(const Stock& stock)
{
	datas.append(stock);

	if(datas.size() < n)
	    return DEFAULT_INDICATOR_RET;

	if(datas.size() > n)
		datas.remove(0);

	float avg,sum = 0;

	for(int i=0;i<datas.size();i++)
		sum += datas.at(i).current;

	avg = sum/(double)n;

	sum = 0.0f;
	for(int i=0;i<datas.size();i++)
		sum += (datas[i].current - avg)*(datas[i].current - avg);

	sum /= (double)n;
	float delta = sqrtf(sum);

	DataWrapper5<bool,float,float,float,float> unit(true,avg,m*delta,n,m);
	return unit;
}
