#include <cmath>
#include "common/common.h"
#include "Indicator.h"

RSIIndicator::RSIIndicator(int in):
n(in),up(0.0f),down(0.0f)
{
}

RSIIndicator::~RSIIndicator()
{
}

DataWrapper5<bool,float,float,float,float> RSIIndicator::update(const Stock& stock)
{
	data.append(stock);

	if(data.size() < n)
	{
		int size = data.size();
		if(size > 1)
		{
		    if(data[size-1].current > data[size-2].current)
				up += data[size-1].current - data[size-2].current;

		    if(data[size-1].current < data[size-2].current)
				down += data[size-2].current - data[size-1].current;
		}
	    return DEFAULT_INDICATOR_RET;
	}
	else if(data.size() == n)
	{
		int size = n;
		if(data[size-1].current > data[size-2].current)
			up += data[size-1].current - data[size-2].current;

		if(data[size-1].current < data[size-2].current)
			down += data[size-2].current - data[size-1].current;
	    up /= (float)n;
		down /= (float)n;
		float rsi = 100*up/(up+down); 
		return DataWrapper5<bool,float,float,float,float>(true,rsi,0.0f,0.0f,0.0f);
	}
	else
		data.remove(0);

	float upward = 0.0;
	float downward = 0.0;
	if(data[n-1].current - data[n-2].current > 0)
		upward = data[n-1].current - data[n-2].current;
	else if(data[n-1].current < data[n-2].current)
		downward = data[n-2].current - data[n-1].current;

	up = (upward-up)/(float)n + up;
	down = (downward-down)/(float)n + down;
	float rsi = 100*up/(up+down); 
	return DataWrapper5<bool,float,float,float,float>(true,rsi,0.0f,0.0f,0.0f);
}
