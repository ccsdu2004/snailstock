#include <cmath>
#include "common/common.h"
#include "Indicator.h"
#include "SnailApplication.h"

MaNIndicator::MaNIndicator(int in):n(in)
{
}

MaNIndicator::~MaNIndicator()
{
}

DataWrapper5<bool,float,float,float,float> MaNIndicator::update(const Stock& stock)
{
	datas.append(stock);

	if(datas.size() < n)
	    return DEFAULT_INDICATOR_RET;

	if(datas.size() > n)
		datas.remove(0);

	float avg,sum = 0;   

	for(int i=0;i<datas.size();i++)
		sum += datas.at(i).current;

	avg = sum/(float)n;
	return DataWrapper5<bool,float,float,float,float>(true,avg,0.0f,0.0f,0.0f);
}
