#include <cmath>
#include "common/common.h"
#include "Indicator.h"
#include "SnailApplication.h"

float StockIndicator::getTrueRange(const Stock& prev,const Stock& current)
{
	float value = 0.0f;
	float d1 = fabs(current.top - prev.privClose);
	float d2 = fabs(current.low - prev.privClose);
	float d3 = current.top - current.low;
	return std::max(d1,std::max(d2,d3));
}

int PricePositionScanner::check(const QString& code,const QVector<Stock>& list,QStringList& desc)
{
	int count = list.size();
	if(count < 60)
		return -1;
	float price = 0.0f;
	int pos = stock_getHighPosition(list,count-60-1,count-1,price,true);
	if(pos == count-1)
	{
		pos = stock_getHighPosition(list,count-60-1,count-2,price,true);
		QString tag("60日新高,前60日最高:%1");
		tag = tag.arg(price);
		desc.append(tag);
	    return 0;
	}

    price = 9999999.99f;
	pos = stock_getLowPosition(list,count-60-1,count-1,price,true);
	if(pos == count-1)
	{
		pos = stock_getLowPosition(list,count-60-1,count-2,price,true);
		QString tag("60日新低,前60日最低:%1");
		tag = tag.arg(price);
	    return 0;
	}
	return -1;
}

bool isNDaysTop(const QVector<Stock>& stocks,int n,int pos)
{
	if(stocks.size() <= n+pos)
		return false;
	Stock stock = stocks.at(stocks.size()-1);

	for(int i=stocks.size()-pos-1;i<stocks.size()-pos-1-n;i--)
		if(stock.current <= stocks[i].open || stock.current <= stocks[i].current)
			return false;
	return true;
}

bool isDiKaiDiZou(const QVector<Stock>& stocks)
{
    if(stocks.size() < 2)
		return false;

	int cursor = stocks.size()-1;
	if(stocks[cursor].current < stocks[cursor].open && 
	   stocks[cursor].open < stocks[cursor].privClose &&
	   stocks[cursor].top <= stocks[cursor].current)
	   return true;
	return false;
}

bool checkNDaysOverBollTop(const QVector<std::tuple<bool,float,float,float,float>>& datas,float value,int n)
{
    if(datas.size() < n)
		return false;

	int count = 0;
	for(int i=datas.size()-1;i>=datas.size()-n;i--)
	{
	    if(std::get<1>(datas[i]) + 2*std::get<2>(datas[i]) < value)
	        count ++;
	}

	if(count == n)
		return true;
	return false;
}

/*DMIStockIndicator::DMIStockIndicator(int n):
span(n)
{
	clear();
}

DMIStockIndicator::~DMIStockIndicator()
{
}

void DMIStockIndicator::clear()
{
	dataN.clear();
	prevDpi = 0.0f;
	prevMdi = 0.0f;
	prevAdx = 0.0f;
	prevAdxr = 0.0f;
}

std::tuple<bool,float,float,float,float> DMIStockIndicator::update(const Stock& stock)
{
	std::tuple<bool,float,float,float,float> output(false,0,0,0,0);
	dataN.push_back(stock);
	if(dataN.size() > span)
		dataN.remove(0);
	else if(dataN.size() < span)
		return output;

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

		float tr0 = fabs(dataN[i].top - dataN[i].open);
	    float tr1 = fabs(dataN[i].top - dataN[i-1].current);
	    float tr2 = fabs(dataN[i].low - dataN[i-1].current);
	    float tr_ = qMax(tr0,qMax(tr1,tr2));
		tr += tr_;
	}

	float a = 2.0f/(1.0f+span);

	float pdi = hp_all*100.0f/tr;
	prevDpi = a*pdi + (1-a)*prevDpi;

	float mdi = lp_all*100.0f/tr;
	prevMdi = a*mdi + (1-a)*prevMdi;

	float adx = 100.0f*fabs(pdi-mdi)/(pdi+mdi);
	//adx = a*adx + (1-a)*prevAdx;

    //prevAdx = adx + prevAdx;
	//prevAdx *= 0.5f;
	float adxr = a*adx + (1-a)*prevAdxr;
	prevAdx = adx;
	prevAdxr = adxr;
	return std::tuple<bool,float,float,float,float>(true,pdi,mdi,adx,adxr);
}*/