#include <cmath>
#include "common/common.h"
#include "Indicator.h"
#include "SnailApplication.h"

KDJIndicator::KDJIndicator()
{
	clear();
}

KDJIndicator::~KDJIndicator()
{
}

void KDJIndicator::findMinAndMax(float& min,float& max)
{
	foreach(Stock stock,datas)
	{
	    if(stock.top > max)
			max = stock.top;
		if(stock.low < min)
			min = stock.low;
	}
}

DataWrapper5<bool,float,float,float,float> KDJIndicator::update(const Stock& stock)
{
	datas.push_back(stock);
	if(datas.size() < 9)
		return DEFAULT_INDICATOR_RET;
	if(datas.size() > 9)
		datas.remove(0);
    
	float minv = 999999.99f;
	float maxv = 0;
	findMinAndMax(minv,maxv);
	float rsv = (stock.current - minv)/(maxv-minv)*100.0f;

    float oldk = k;
    float oldd = d;

    k = (rsv + 2*oldk)/3.0;
    d = (k + 2*oldd)/3.0;

#define min(a,b) ((a < b) ? a : b)
#define max(a,b) ((a > b) ? a : b)

    k = min(100,max(k,0));
    d = min(100,max(d,0));
    j = 3*k - 2*d;
    j = min(100,max(j,0));

#undef min
#undef max
	return DataWrapper5<bool,float,float,float,float>(true,k,d,j,0.0f);
}
