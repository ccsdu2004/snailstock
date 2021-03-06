#include <cmath>
#include "common/common.h"
#include "Indicator.h"

MacdIndicator::MacdIndicator(int ia,int ib,int ic)
{
	a = ia;
	b = ib;
	c = ic;
	clear();
}

MacdIndicator::~MacdIndicator()
{
}

DataWrapper5<bool,float,float,float,float> MacdIndicator::update(const Stock& stock)
{
	DataWrapper5<bool,float,float,float,float> d = DEFAULT_INDICATOR_RET;
	flag ++;
	if(!flag)
    {
	    m12 = stock.current;
        m26 = stock.current; 
    }
    else
    {
		m12 = m12 + (stock.current - m12) * 2.0f/float(a+1);
        m26 = m26 + (stock.current - m26) * 2.0f/float(b+1);
        diff = m12 - m26;
        if(flag == 1)
            dea = 0.2*diff;
        else
            dea = (dea*(c-1) + diff*2.0f)/(float)(1.0+c);
        bar = 2 * (-dea + diff);

		d = DataWrapper5<bool,float,float,float,float>(true,bar,diff,dea,0.0f);
    }
	
	return d;
}
/*
bool MacdIndicator::isMacdJX(const QVector<Stock>& stocks)
{
    MacdIndicator macd;
	QVector<std::tuple<bool,float,float,float,float>> macd_ret;
	foreach(Stock stock,stocks)
		macd_ret.append(macd.update(stock));
	if(stocks.size() < 2)
		return false;
	int current = stocks.size()-1;
	int prev = current - 1;
	if(std::get<0>(macd_ret[current]) && std::get<0>(macd_ret[prev]) && 
	   std::get<1>(macd_ret[current]) > 0 && std::get<1>(macd_ret[prev]) < 0)
		return true;
	return false;
}*/
