#include <qdebug.h>
#include <algorithm>
#include "math/math.h"
#include "GPDX.h"
#include "k/StockKAnalyzer.h"
#include "SnailContext.h"
#include "config/ConfigItem.h"
#include <iostream>
#ifdef SUPPORT_GPDX

/*
const double FibonacciList[] = { 0.382,0.5,0.618 };
const double FibonacciListExt[] = { 0.382,0.5,0.618,0.809 };

bool isThis_ChuiZi(const Stock& stock,float& value)
{
	value = stock.low;
	float head = stock.top - std::max(stock.open,stock.current);
	float body = std::max(stock.open,stock.current) - std::min(stock.open,stock.current);
	float foot = std::min(stock.open,stock.current) - stock.low;
	if (head >= body)
		return false;

	if (foot > 2.4*body && head*3.0 < foot && head*2.4 < body)
		return true;
	return false;
}

bool isThis_ChuanCi(const Stock& prev,const Stock& stock,float& value)
{
	if (prev.increase > -1.8 || stock.increase < 1.8)
		return false;
	if (stock.current <= stock.open)
		return false;
	if (prev.open <= prev.current)
		return false;
	if (prev.current < stock.open)
		return false;
	if (prev.open < stock.current)
		return false;
	value = stock.open;
	return true;
}

bool isThis_YangTunYin(const Stock& prev,const Stock& stock,float& value)
{
	if (prev.increase > 0 || stock.increase < 1.8)
		return false;
	if (stock.current <= stock.open)
		return false;
	if (prev.open < prev.current)
		return false;
	if (prev.increase > -1.8)
		return false;

	if (prev.top >= stock.top)
		return false;

	if (stock.open > prev.current)
		return false;
	value = stock.open;
	return true;
}

bool isThis_XuRiDongSheng(const Stock& prev,const Stock& stock,float& value)
{
	if (prev.increase > 0 || stock.increase < 1.8)
		return false;
	if (stock.current <= stock.open)
		return false;
	if (prev.open <= prev.current)
		return false;
	if (prev.increase > -1.8)
		return false;
	if (stock.open < prev.current)
		return false;
	if (stock.open > prev.open)
		return false;
	if (stock.current < prev.top)
		return false;

	value = stock.open;
	return true;
}

bool isThis_ZaoChengZhiXing(const Stock& prev2,const Stock& prev,const Stock& stock,float& value)
{
	if (prev2.increase > -1.8)
		return false;
	if (prev2.open <= prev2.increase)
		return false;

	if (stock.increase < 1.8)
		return false;
	if (stock.open >= stock.current)
		return false;

	if (prev.top >= stock.current || prev.top >= prev2.open)
		return false;

	if (prev.increase > 0.5)
		return false;

	if (prev.getPriceCenter() > stock.current || prev.getPriceCenter() > prev2.open)
		return false;

	value = stock.open;
	if (prev.getHead() > prev.getBody() || prev.getFoot() > prev.getBody())
		return true;

	return false;
}*/

bool isGPDX_spmr_impl(const QVector<Stock>& stocks)
{
	if (stocks.size() < 18)
		return false;

	QVariant var;

	auto list = SnailContext::open()->getConfigFactory()->usage();

#define GET_VAR(str)\
    SnailContext::open()->getConfigFactory()->getVar(QString(str),var);

	//[5,48]
	GET_VAR("config.unit.高抛低吸.升破买入.前低前高最小间距")
	int lowHighMinSpan = var.toInt();
	lowHighMinSpan = std::clamp<int>(lowHighMinSpan,6,12);

	GET_VAR("config.unit.高抛低吸.升破买入.前低前高最大间距")
	int lowHighMaxSpan = var.toInt();
	lowHighMaxSpan = std::clamp<int>(lowHighMaxSpan,36,60);

	GET_VAR("config.unit.高抛低吸.升破买入.前高反弹最小间距")
	int highCurrentMinSpan = var.toInt();
	highCurrentMinSpan = std::clamp<int>(highCurrentMinSpan,3,6);

	GET_VAR("config.unit.高抛低吸.升破买入.前高反弹最大间距")
	int highCurrentMaxSpan = var.toInt();
	highCurrentMaxSpan = std::clamp<int>(highCurrentMaxSpan,9,36);

	GET_VAR("config.unit.高抛低吸.升破买入.前高前低最小涨幅")
	double minLowHighIncrease = var.toDouble();
	minLowHighIncrease = std::clamp<double>(minLowHighIncrease,12,9999.99);

	GET_VAR("config.unit.高抛低吸.升破买入.双低最小涨幅")
	double minReLowHighIncrease = var.toDouble();
	minReLowHighIncrease = std::clamp<double>(minLowHighIncrease,6,3000.0);

	GET_VAR("config.unit.高抛低吸.升破买入.使用跳空缺口形态")
	bool useBearchAsK = var.toString() == "yes";

	GET_VAR("config.unit.高抛低吸.升破买入.使用大阳线形态")
	bool useBigSunAsK = var.toString() == "yes";

	int length = lowHighMaxSpan + highCurrentMaxSpan;
	if (stocks.size() <= length)
		return false;

	auto start = stocks.begin();
	std::advance(start,stocks.size() - length);

	auto end = stocks.end();
	std::advance(end,-1);

	auto highItr = std::max_element(start,end,[](const Stock& left,const Stock& right)->bool
	{
		return left.top < right.top;
	});

	auto lowItr = std::min_element(start,end,[](const Stock& left,const Stock& right)->bool
	{
		return left.current < right.current;
	});

	auto span = std::distance(lowItr,highItr);
	if (span < lowHighMinSpan || span > lowHighMaxSpan)
		return false;

	auto reLowItr = std::min_element(highItr,end,[](const Stock& left,const Stock& right)->bool
	{
		return left.current < right.current;
	});

	int highCurrentDistance = std::distance(highItr,end);
	if (highCurrentDistance < highCurrentMinSpan || highCurrentDistance > highCurrentMaxSpan)
		return false;

	double high = highItr->top;
	double low = lowItr->low;

	if (minLowHighIncrease > GET_INCREASE(high,low))
		return false;

	if (minReLowHighIncrease > GET_INCREASE(reLowItr->low,low))
		return false;

	int cursor = stocks.size() - 1;
	if (highItr->top <= stocks[cursor - 1].top)
		return false;

	double currentValue = stocks[cursor].current;

	if (useBearchAsK)
	{
		if (stocks[cursor].low > stocks[cursor - 1].top && currentValue > highItr->top)
			return true;
	}

	if (useBigSunAsK)
	{
		if (stocks[cursor].increase > 4.0 && currentValue > highItr->top)
			return true;
	}
	return false;
}

bool isGPDX_SPMR(const QVector<Stock>& stocks,int end)
{
	return isGPDX_spmr_impl(stocks.mid(0,end));
}

#endif