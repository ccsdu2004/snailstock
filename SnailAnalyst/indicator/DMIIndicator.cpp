#include "indicator.h"

DMIStockIndicator::DMIStockIndicator(int n,int m):
spanN(n+1),
spanM(m)
{
	clear();
}

DMIStockIndicator::~DMIStockIndicator()
{
}

void DMIStockIndicator::clear()
{
	dataN.clear();
	dxM.clear();
	adxM.clear();
	prevMAdx = 0.0;
	prevAdx = 0.0f;
}

DataWrapper5<bool,float,float,float,float> DMIStockIndicator::update(const Stock& stock)
{
	DataWrapper5<bool,float,float,float,float> output(false,0,0,0,0);
	dataN.push_back(stock);
	if(dataN.size() > spanN)
	{
		dataN.remove(0);
	}
	else if(dataN.size() < spanN)
	{	
		return output;
	}

	float hp_all = 0.0f;
	float lp_all = 0.0f;
	float tr = 0.0f;

	for(int i=1;i<dataN.size();i++)
	{
	    float hp = dataN[i].top - dataN[i-1].top;
		float lp = dataN[i-1].low - dataN[i].low;
		if(hp > lp && hp > 0)
			hp_all += hp;

		if(lp > hp && lp > 0)
			lp_all += lp;

		float tr0 = fabs(dataN[i].top - dataN[i].low);
		float tr1 = fabs(dataN[i].top - dataN[i-1].current);
	    float tr2 = fabs(dataN[i].low - dataN[i-1].current);
	    float tr_ = qMax(tr0,qMax(tr1,tr2));
		tr += tr_;
	}

	float pdi = hp_all*100.0f/tr;
	float mdi = lp_all*100.0f/tr;
	float dx = 100.0f*fabs(pdi-mdi)/(pdi+mdi);
	dxM.push_back(dx);

	if(dxM.size() > spanM)
		dxM.remove(0);

	float adx = 0.0;
	foreach(double i,dxM)
		adx += i;
	adx /= (double)dxM.size();

	float adx6 = 0.0;
	adxM.push_back(adx);
	if(adxM.size() > spanM)
	{
		adx6 = adxM[0];
		adxM.remove(0);
	}

	float adxr = adx6 + adx;
	return DataWrapper5<bool,float,float,float,float>(true,pdi,mdi,adx,adxr*0.5);
}