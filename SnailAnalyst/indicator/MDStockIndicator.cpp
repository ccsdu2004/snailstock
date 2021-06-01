#include <cmath>
#include "common/common.h"
#include "Indicator.h"

/*
MDStockIndicator::MDStockIndicator(int m):
n(m),
sum(0.0f)
{
}

MDStockIndicator::~MDStockIndicator()
{
}

DataWrapper5<bool,float,float,float,float> MDStockIndicator::update(const Stock& stock)
{
	sum += stock.current;
	datas.append(stock);

	if (datas.size() < n)
		return DEFAULT_INDICATOR_RET;

	if (datas.size() > n)
	{
		sum -= datas[0].current;
		datas.remove(0);
	}

	float avg = sum / (float)n;

	if (datas.size() > n - 1)
	{
		float acc = 0.0f;

	}

	float avg,sum = 0;

	for (int i = 0;i<datas.size();i++)
		sum += datas.at(i).current;

	avg = sum / (float)n;
	return DataWrapper5<bool,float,float,float,float>(true,avg,0.0f,0.0f,0.0f);
}
*/