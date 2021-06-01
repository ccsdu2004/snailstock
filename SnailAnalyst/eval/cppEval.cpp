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
		desc = "sum函数参数有误";
		return 0.0;
	}
	return std::accumulate(input.begin(),input.end(),0.0);
}

double s_max(const std::vector<double>& input,std::string& desc)
{
	if(input.empty())
	{
		desc = "max函数参数有误";
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
		desc = "min函数参数有误";
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
		desc = "cos函数参数有误";
		return 0.0;
	}
	return cos(input[0]);
}

double s_sin(const std::vector<double>& input,std::string& desc)
{
	if(input.size() != 1)
	{
		desc = "sin函数参数有误";
		return 0.0;
	}
	return sin(input[0]);
}

double s_mean(const std::vector<double>& input,std::string& desc)
{
	if(input.empty())
	{
		desc = "函数mean参数有误";
		return 0.0;
	}
	
	double total = std::accumulate(input.begin(),input.end(),0.0);
	return total / (double)input.size();
}

double s_stdevp(const std::vector<double>& input, std::string& desc)
{
	if(input.empty())
	{
		desc = "函数stdevp参数有误";
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
		desc = "open函数参数有误";
		return 0.0;
	}
	if(input[0] > 0)
	{
		desc = "open函数参数带未来函数";
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
		desc = "close函数参数有误";
		return 0.0;
	}
	if(input[0] > 0)
	{
		desc = "close函数参数带未来函数";
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
		desc = "high函数参数有误";
		return 0.0;
	}
	if(input[0] > 0)
	{
		desc = "high函数参数带未来函数";
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
		desc = "low函数参数有误";
		return 0.0;
	}
	if(input[0] > 0)
	{
		desc = "low函数参数带未来函数";
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
		desc = "vol函数参数有误";
		return 0.0;
	}
	if(input[0] > 0)
	{
		desc = "vol函数参数带未来函数";
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
		desc = "maxvol函数参数有误";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "maxvol第一个参数范围必须在[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "maxVol第二个参数必须小于等于1";
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
		desc = "minVol函数参数有误";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "minVol第一个参数范围必须在[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "minVol第二个参数必须小于等于1";
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
		desc = "maxHigh函数参数有误";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "maxHigh第一个参数范围必须在[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "maxHigh第二个参数必须小于等于1";
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
		desc = "minHigh函数参数有误";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "minHigh第一个参数范围必须在[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "minHigh第二个参数必须小于等于1";
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
		desc = "maxLow函数参数有误";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "maxLow第一个参数范围必须在[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "maxLow第二个参数必须小于等于1";
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
		desc = "minLow函数参数有误";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "minLow第一个参数范围必须在[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "minLow第二个参数必须小于等于1";
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
		desc = "maxClose函数参数有误";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "maxClose第一个参数范围必须在[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "maxClose第二个参数必须小于等于1";
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
		desc = "minClose函数参数有误";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "minClose第一个参数范围必须在[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "minClose第二个参数必须小于等于1";
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
		desc = "maxOpen函数参数有误";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "maxOpen第一个参数范围必须在[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "maxOpen第二个参数必须小于等于1";
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
		desc = "minOpen函数参数有误";
		return 0.0;
	}

	if (input[0] < 0 || input[0] > 60)
	{
		desc = "minOpen第一个参数范围必须在[1,60]";
		return 0.0;
	}

	if (input[1] > 0)
	{
		desc = "minOpen第二个参数必须小于等于1";
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
		desc = "increase函数参数有误";
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