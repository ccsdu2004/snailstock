#include <algorithm>
#include "cppeval.h"
#include "../DataProvider.h"
#include "expr/VarManager.h"
#include "StockScriptEngine.h"
#include <iostream>

double s_sum(const std::vector<double>& input,std::string& desc)
{
	if(input.empty())
	{
		desc = "sum������������";
		return 0.0;
	}
	return std::accumulate(input.begin(),input.end(),0.0);
}

double s_max(const std::vector<double>& input,std::string& desc)
{
	if(input.empty())
	{
		desc = "max������������";
		return 0.0;
	}
	
	double ret = std::numeric_limits<double>::min();
	auto itr = input.begin();
	while(itr != input.end())
	{
		ret = std::max(ret,*itr);
	    itr ++;
	}
	return ret;
}

double s_min(const std::vector<double>& input,std::string& desc)
{
	if(input.empty())
	{
		desc = "min������������";
		return 0.0;
	}
	
	double ret = std::numeric_limits<double>::max();
	auto itr = input.begin();
	while(itr != input.end())
	{
		ret = std::min(ret,*itr);
	    itr ++;
	}
	return ret;
}

double s_cos(const std::vector<double>& input,std::string& desc)
{
	if(input.size() != 1)
	{
		desc = "cos������������";
		return 0.0;
	}
	return cos(input[0]);
}

double s_sin(const std::vector<double>& input,std::string& desc)
{
	if(input.size() != 1)
	{
		desc = "sin������������";
		return 0.0;
	}
	return sin(input[0]);
}

double s_mean(const std::vector<double>& input,std::string& desc)
{
	if(input.empty())
	{
		desc = "����mean��������";
		return 0.0;
	}
	
	double total = std::accumulate(input.begin(),input.end(),0.0);
	return total / (double)input.size();
}

double s_stdevp(const std::vector<double>& input, std::string& desc)
{
	if(input.empty())
	{
		desc = "����stdevp��������";
		return 0.0;
	}

	double avg = s_mean(input,desc);
	if(!desc.empty())
		return avg;

	double evp = 0.0;
	auto itr = input.begin();
	while(itr != input.end())
	{
		double diff = *itr - avg;
		diff *= diff;
		evp += diff;
	}

	evp /= input.size();
	return sqrt(evp);
}

double s_stdev(const std::vector<double>& input, std::string& desc)
{
	double avg = s_mean(input,desc);
	if(!desc.empty())
		return avg;

	double evp = 0.0;
	auto itr = input.begin();
	while(itr != input.end())
	{
		double diff = *itr - avg;
		diff *= diff;
		evp += diff;
	}

	evp /= (input.size()-1);
	return sqrt(evp);
}

double s_open(const std::vector<double>& input,std::string& desc)
{
	if(input.size() != 1)
	{
		desc = "open������������";
		return 0.0;
	}
	if(input[0] > 0)
	{
		desc = "open����������δ������";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;
	int offset = input[0];
	int current = dataProvider.currentStockCursor + offset;
	if(current < 0 || current > dataProvider.stocks.size()-1)
		return std::numeric_limits<double>::max();

	return dataProvider.stocks[current].open;
}

double s_close(const std::vector<double>& input,std::string& desc)
{
	if(input.size() != 1)
	{
		desc = "close������������";
		return 0.0;
	}
	if(input[0] > 0)
	{
		desc = "close����������δ������";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;
	int offset = input[0];
	int current = dataProvider.currentStockCursor + offset;
	if(current < 0 || current > dataProvider.stocks.size()-1)
		return std::numeric_limits<double>::max();

	return dataProvider.stocks[current].current;
}

double s_high(const std::vector<double>& input,std::string& desc)
{
	if(input.size() != 1)
	{
		desc = "high������������";
		return 0.0;
	}
	if(input[0] > 0)
	{
		desc = "high����������δ������";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;
	int offset = input[0];
	int current = dataProvider.currentStockCursor + offset;
	if(current < 0 || current > dataProvider.stocks.size()-1)
		return std::numeric_limits<double>::max();

	return dataProvider.stocks[current].top;
}

double s_low(const std::vector<double>& input,std::string& desc)
{
	if(input.size() != 1)
	{
		desc = "low������������";
		return 0.0;
	}
	if(input[0] > 0)
	{
		desc = "low����������δ������";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;
	int offset = input[0];
	int current = dataProvider.currentStockCursor + offset;
	if(current < 0 || current > dataProvider.stocks.size()-1)
		return std::numeric_limits<double>::max();

	return dataProvider.stocks[current].low;
}

double s_vol(const std::vector<double>& input,std::string& desc)
{
	if(input.size() != 1)
	{
		desc = "vol������������";
		return 0.0;
	}
	if(input[0] > 0)
	{
		desc = "vol����������δ������";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;
	int offset = input[0];
	int current = dataProvider.currentStockCursor + offset;
	if(current < 0 || current > dataProvider.stocks.size()-1)
		return std::numeric_limits<double>::max();

	return dataProvider.stocks[current].tradeVolume;
}

double s_maxVol(const std::vector<double>& input,std::string& desc)
{
	if (input.size() != 2)
	{
		desc = "maxvol������������";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "maxvol��һ��������Χ������[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "maxVol�ڶ�����������С�ڵ���1";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;

	int count = input[0];
	int offset = input[1];

	int start = dataProvider.currentStockCursor + offset - count+1;
	int end = dataProvider.currentStockCursor + offset + 1;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	auto itr = std::max_element(dataProvider.stocks.data() + start,dataProvider.stocks.data() + end,
		[](const Stock& left,const Stock& right)->bool{return left.tradeVolume < right.tradeVolume;});


	int index = std::distance(dataProvider.stocks.begin(),itr);
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->addRegisterVar(QPair<double,QString>(index,"maxVol"));
#endif	
	return itr->tradeVolume;
	/*double maxVol = 0;
	for (int i = start;i <= end;i++)
	maxVol = std::max<double>(dataProvider.stocks[i].tradeVolume,maxVol);
	return maxVol;*/
}

double s_minVol(const std::vector<double>& input,std::string& desc)
{
	if (input.size() != 2)
	{
		desc = "minVol������������";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "minVol��һ��������Χ������[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "minVol�ڶ�����������С�ڵ���1";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;

	int count = input[0];
	int offset = input[1];

	int start = dataProvider.currentStockCursor + offset - count + 1;
	int end = dataProvider.currentStockCursor + offset + 1;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	auto itr = std::min_element(dataProvider.stocks.data() + start,dataProvider.stocks.data() + end,
		[](const Stock& left,const Stock& right)->bool{return left.tradeVolume < right.tradeVolume;});

	int index = std::distance(dataProvider.stocks.begin(),itr);
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->addRegisterVar(QPair<double,QString>(index,"minVol"));
#endif	
	return itr->tradeVolume;
}

double s_maxHigh(const std::vector<double>& input,std::string& desc)
{
	if (input.size() != 2)
	{
		desc = "maxHigh������������";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "maxHigh��һ��������Χ������[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "maxHigh�ڶ�����������С�ڵ���1";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;

	int count = input[0];
	int offset = input[1];

	int start = dataProvider.currentStockCursor + offset - count + 1;
	int end = dataProvider.currentStockCursor + offset + 1;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	auto itr = std::max_element(dataProvider.stocks.data() + start,dataProvider.stocks.data() + end,
		[](const Stock& left,const Stock& right)->bool{return left.top < right.top;});

	int index = std::distance(dataProvider.stocks.begin(),itr);
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->addRegisterVar(QPair<double,QString>(index,"maxHigh"));
#endif	
	return itr->top;
}

double s_minHigh(const std::vector<double>& input,std::string& desc)
{
	if (input.size() != 2)
	{
		desc = "minHigh������������";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "minHigh��һ��������Χ������[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "minHigh�ڶ�����������С�ڵ���1";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;

	int count = input[0];
	int offset = input[1];

	int start = dataProvider.currentStockCursor + offset - count + 1;
	int end = dataProvider.currentStockCursor + offset + 1;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	auto itr = std::min_element(dataProvider.stocks.data() + start,dataProvider.stocks.data() + end,
		[](const Stock& left,const Stock& right)->bool{return left.top < right.top;});

	int index = std::distance(dataProvider.stocks.begin(),itr);
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->addRegisterVar(QPair<double,QString>(index,"minHigh"));
#endif	
	return itr->top;
}

double s_maxLow(const std::vector<double>& input,std::string& desc)
{
	if (input.size() != 2)
	{
		desc = "maxLow������������";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "maxLow��һ��������Χ������[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "maxLow�ڶ�����������С�ڵ���1";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;

	int count = input[0];
	int offset = input[1];

	int start = dataProvider.currentStockCursor + offset - count + 1;
	int end = dataProvider.currentStockCursor + offset + 1;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	auto itr = std::max_element(dataProvider.stocks.data() + start,dataProvider.stocks.data() + end,
		[](const Stock& left,const Stock& right)->bool{return left.low < right.low;});

	int index = std::distance(dataProvider.stocks.begin(),itr);
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->addRegisterVar(QPair<double,QString>(index,"maxLow"));
#endif	
	return itr->low;
}

double s_minLow(const std::vector<double>& input,std::string& desc)
{
	if (input.size() != 2)
	{
		desc = "minLow������������";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "minLow��һ��������Χ������[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "minLow�ڶ�����������С�ڵ���1";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;

	int count = input[0];
	int offset = input[1];

	int start = dataProvider.currentStockCursor + offset - count + 1;
	int end = dataProvider.currentStockCursor + offset + 1;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	auto itr = std::min_element(dataProvider.stocks.data() + start,dataProvider.stocks.data() + end,
		[](const Stock& left,const Stock& right)->bool{return left.low < right.low;});

	int index = std::distance(dataProvider.stocks.begin(),itr);
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->addRegisterVar(QPair<double,QString>(index,"minLow"));
#endif	
	return itr->low;
}

double s_maxClose(const std::vector<double>& input,std::string& desc)
{
	if (input.size() != 2)
	{
		desc = "maxClose������������";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "maxClose��һ��������Χ������[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "maxClose�ڶ�����������С�ڵ���1";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;

	int count = input[0];
	int offset = input[1];

	int start = dataProvider.currentStockCursor + offset - count + 1;
	int end = dataProvider.currentStockCursor + offset + 1;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	auto itr = std::max_element(dataProvider.stocks.data() + start,dataProvider.stocks.data() + end,
		[](const Stock& left,const Stock& right)->bool{return left.current < right.current;});

	int index = std::distance(dataProvider.stocks.begin(),itr);
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->addRegisterVar(QPair<double,QString>(index,"maxClose"));
#endif	
	return itr->current;
}

double s_minClose(const std::vector<double>& input,std::string& desc)
{
	if (input.size() != 2)
	{
		desc = "minClose������������";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "minClose��һ��������Χ������[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "minClose�ڶ�����������С�ڵ���1";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;

	int count = input[0];
	int offset = input[1];

	int start = dataProvider.currentStockCursor + offset - count + 1;
	int end = dataProvider.currentStockCursor + offset + 1;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	auto itr = std::min_element(dataProvider.stocks.data() + start,dataProvider.stocks.data() + end,
		[](const Stock& left,const Stock& right)->bool{return left.current < right.current;});

	int index = std::distance(dataProvider.stocks.begin(),itr);
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->addRegisterVar(QPair<double,QString>(index,"minClose"));
#endif	
	return itr->current;
}

double s_maxOpen(const std::vector<double>& input,std::string& desc)
{
	if (input.size() != 2)
	{
		desc = "maxOpen������������";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "maxOpen��һ��������Χ������[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "maxOpen�ڶ�����������С�ڵ���1";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;

	int count = input[0];
	int offset = input[1];

	int start = dataProvider.currentStockCursor + offset - count + 1;
	int end = dataProvider.currentStockCursor + offset + 1;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	auto itr = std::max_element(dataProvider.stocks.data() + start,dataProvider.stocks.data() + end,
		[](const Stock& left,const Stock& right)->bool{return left.open < right.open;});

	int index = std::distance(dataProvider.stocks.begin(),itr);
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->addRegisterVar(QPair<double,QString>(index,"maxOpen"));
#endif	
	return itr->open;
}

double s_minOpen(const std::vector<double>& input,std::string& desc)
{
	if (input.size() != 2)
	{
		desc = "minOpen������������";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "minOpen��һ��������Χ������[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "minOpen�ڶ�����������С�ڵ���1";
		return 0.0;
	}

	auto stocks = dataProvider.stocks;

	int count = input[0];
	int offset = input[1];

	int start = dataProvider.currentStockCursor + offset - count + 1;
	int end = dataProvider.currentStockCursor + offset + 1;

	if (start < 0)
		start = 0;

	if (end < 0)
		end = 0;

	auto itr = std::min_element(dataProvider.stocks.data() + start,dataProvider.stocks.data() + end,
		[](const Stock& left,const Stock& right)->bool{return left.open < right.open;});

	int index = std::distance(dataProvider.stocks.begin(),itr);
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->addRegisterVar(QPair<double,QString>(index,"minOpen"));
#endif	
	return itr->open;
}

double s_increase(const std::vector<double>& input,std::string& desc)
{
	if(input.size() != 2)
	{
		desc = "increase������������";
		return 0.0;
	}

	double low = std::min(input[0],input[1]);
	double high = std::max(input[0],input[1]);
	return 100.0*(high-low)/low;
}

SFuntionFactory::SFuntionFactory()
{
	registerFunction("sum",s_sum);
	registerFunction("cos",s_cos);
	registerFunction("sin",s_sin);
	registerFunction("max",s_max);
	registerFunction("min",s_min);
    registerFunction("mean",s_mean);
    registerFunction("stdevp",s_stdevp);
    registerFunction("stdev",s_stdev);

	registerFunction("open",s_open);
	registerFunction("close",s_close);
	registerFunction("high",s_high);
	registerFunction("low",s_low);
	registerFunction("vol",s_vol);

	registerFunction("maxVol",s_maxVol);
	registerFunction("maxOpen",s_maxOpen);
	registerFunction("maxClose",s_maxClose);
	registerFunction("maxHigh",s_maxHigh);
	registerFunction("maxLow",s_maxLow);
	registerFunction("minVol",s_minVol);
	registerFunction("minOpen",s_minOpen);
	registerFunction("minClose",s_minClose);
	registerFunction("minLow",s_minLow);
	registerFunction("minHigh",s_minHigh);

	registerFunction("increase",s_increase);
}