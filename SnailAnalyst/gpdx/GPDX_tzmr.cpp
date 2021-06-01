#include <qdebug.h>
#include <algorithm>
#include "math/math.h"
#include "GPDX.h"
#include "k/StockKAnalyzer.h"
#include "SnailContext.h"
#include "config/ConfigItem.h"
#include <iostream>
#ifdef SUPPORT_GPDX

const double FibonacciList[] = {0.382,0.5,0.618};
const double FibonacciListExt[] = {0.382,0.5,0.618,0.809};

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
}

bool isGPDX_FTMR_impl(const QVector<Stock>& stocks)
{
	if (stocks.size() < 18)
		return false;

	QVariant var;

	auto list = SnailContext::open()->getConfigFactory()->usage();

#define GET_VAR(str)\
    SnailContext::open()->getConfigFactory()->getVar(QString(str),var);

	//[5,48]
	GET_VAR("config.unit.高抛低吸.调整买入.前低前高最小间距")
	int lowHighMinSpan = var.toInt();
	lowHighMinSpan = std::clamp<int>(lowHighMinSpan,6,12);

	GET_VAR("config.unit.高抛低吸.调整买入.前低前高最大间距")
	int lowHighMaxSpan = var.toInt();
	lowHighMaxSpan = std::clamp<int>(lowHighMaxSpan,36,60);

	GET_VAR("config.unit.高抛低吸.调整买入.前高反弹最小间距")
	int highCurrentMinSpan = var.toInt();
	highCurrentMinSpan = std::clamp<int>(highCurrentMinSpan,3,6);

	GET_VAR("config.unit.高抛低吸.调整买入.前高反弹最大间距")
	int highCurrentMaxSpan = var.toInt();
	highCurrentMaxSpan = std::clamp<int>(highCurrentMaxSpan,9,36);

	GET_VAR("config.unit.高抛低吸.调整买入.是否采用最低价作为前低")
	bool useLowAsLow = var.toString() == "yes";

	GET_VAR("config.unit.高抛低吸.调整买入.是否采用最高价作为前高")
	bool useHighAsHigh = var.toString() == "yes";

	GET_VAR("config.unit.高抛低吸.调整买入.前高前低最小涨幅")
	double minLowHighIncrease = var.toDouble();
	minLowHighIncrease = std::clamp<double>(minLowHighIncrease,12,9999.99);

	GET_VAR("config.unit.高抛低吸.调整买入.斐波那契最大误差")
	double maxErrorDelta = var.toDouble();
	maxErrorDelta = std::min<double>(maxErrorDelta,3.6);

	GET_VAR("config.unit.高抛低吸.调整买入.使用扩展斐波那契数列")
	bool useFibonacciExt = var.toString() == "yes";

	int length = lowHighMaxSpan + highCurrentMaxSpan;
	if (stocks.size() <= length)
		return false;

	auto start = stocks.begin();
	std::advance(start,stocks.size()-length);

	auto highItr = std::max_element(start,stocks.end(),[&useHighAsHigh](const Stock& left,const Stock& right)->bool
	    {
		    if (useHighAsHigh)
			    return left.top < right.top;
			return left.getMaxOC() < right.getMaxOC();
	    });

	auto lowItr = std::min_element(start,stocks.end(),[&useLowAsLow](const Stock& left,const Stock& right)->bool
	{
		if (useLowAsLow)
			return left.low < right.low;
		return left.getMinOC() < right.getMinOC();
	});

	auto span = std::distance(lowItr,highItr);
	if (span < lowHighMinSpan || span > lowHighMaxSpan)
		return false;

	int highCurrentDistance = std::distance(highItr,stocks.end());
	if (highCurrentDistance < highCurrentMinSpan || highCurrentDistance > highCurrentMaxSpan)
		return false;

	double high = highItr->top;
	double low = lowItr->low;

	if (minLowHighIncrease > GET_INCREASE(high,low))
		return false;

	int cursor = stocks.size() - 1;
	double currentValue = stocks[cursor].current;

	bool isGoodK = false;

	bool c = GET_VAR("config.unit.高抛低吸.调整买入.使用穿刺形态")
	{
		bool has = var.toString() == "yes";
		if (has)
		{
			float v;
			bool ok = isThis_ChuanCi(stocks[cursor - 1],stocks[cursor],v);
			currentValue = v;
			isGoodK = ok;
			if (ok)
				goto FLAG;
			return false;
		}
	}

	c = GET_VAR("config.unit.高抛低吸.调整买入.使用旭日东升形态")
	{
		bool has = var.toString() == "yes";
		if (has)
		{
			float v;
			bool ok = isThis_XuRiDongSheng(stocks[cursor - 1],stocks[cursor],v);
			currentValue = v;
			isGoodK = ok;
			if (ok)
				goto FLAG;
			return false;
		}
	}

	c = GET_VAR("config.unit.高抛低吸.调整买入.使用阳吞阴形态")
	{
		bool has = var.toString() == "yes";
		if (has)
		{
			float v;
			bool ok = isThis_YangTunYin(stocks[cursor - 1],stocks[cursor],v);
			currentValue = v;
			isGoodK = ok;
			if (ok)
				goto FLAG;
			return false;
		}
	}

	c = GET_VAR("config.unit.高抛低吸.调整买入.使用锤子形态")
	{
		bool has = var.toString() == "yes";
		if (has)
		{
			float v;
			bool ok = isThis_ChuiZi(stocks[cursor],v);
			currentValue = v;
			isGoodK = ok;
			if (ok)
				goto FLAG;
			return false;
		}
	}

	c = GET_VAR("config.unit.高抛低吸.调整买入.使用早晨之星形态")
	{
		bool has = var.toString() == "yes";
		if (has)
		{
			float v;
			bool ok = isThis_ZaoChengZhiXing(stocks[cursor-2],stocks[cursor-1],stocks[cursor],v);
			currentValue = v;
			isGoodK = ok;
			if (ok)
				goto FLAG;
			return false;
		}
	}

FLAG:
	if (isGoodK)
	{
		if (!useFibonacciExt)
		{
			double list[3] = {0};
			for (int i = 0;i < 3;i++)
				list[i] = FibonacciList[i] * (high - low) + low;

			auto itr = std::find_if(list,list + 3,[&maxErrorDelta,&currentValue](double v)->bool
			    {
				    double error = GET_INCREASE(std::max(currentValue,v),std::min(currentValue,v));
					return error < maxErrorDelta;
			    }
			);

			if (itr == list + 3)
				return false;
		}
		else
		{
			double list[4] = {0};
			for (int i = 0;i < 4;i++)
				list[i] = FibonacciListExt[i] * (high-low) + low;

			auto itr = std::find_if(list,list + 4,[&maxErrorDelta,&currentValue](double v)->bool
			{
				double error = GET_INCREASE(std::max(currentValue,v),std::min(currentValue,v));
				return error < maxErrorDelta;
			}
			);

			if (itr == list + 4)
				return false;
		}
	}

	return isGoodK;
}

bool isGPDX_FTMR(const QVector<Stock>& stocks,int end)
{
	return isGPDX_FTMR_impl(stocks.mid(0,end));
}

#endif