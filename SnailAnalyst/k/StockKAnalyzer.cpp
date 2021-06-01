#include <cmath>
#include "common/common.h"
#include "StockKAnalyzer.h"

bool checkBody(const Stock& current)
{   
	float body,head,foot;
	if(current.current >= current.open)
	{
		body = current.current - current.open;
		head = current.top - current.current;
		foot = current.open - current.low;
	}
	else
	{	
		body = current.open - current.current;
		head = current.top - current.open;
		foot = current.current - current.low;
	}

	if(head + foot >= body)
		return false;

	if(body <= 0.01)
		return false;

	double all = head + foot;
	if(all/body > 1.15)
		return false;

	return true;
}

inline float checkShadowRadio_(const Stock& current)
{   
	float body,head,foot;
	if(current.current >= current.open)
	{
		body = current.current - current.open;
		head = current.top - current.current;
		foot = current.open - current.low;
	}
	else
	{	
		body = current.open - current.current;
		head = current.top - current.open;
		foot = current.current - current.low;
	}

	if(body < 0.02)
	{
	    if(head+foot < 0.2)
			return 0;
	}

	return (head+foot)/body;
}

StockKAnalyzer* StockKAnalyzer::instance()
{
	static StockKAnalyzer analyzer;
	return &analyzer;
}

QStringList StockKAnalyzer::keywords()
{
	if(table.isEmpty())
		init();
	return table.keys();
}

bool StockKAnalyzer::isThisK(const QString& id,const QVector<Stock>& list)
{
	if(table.isEmpty())
		init();

	if(table.find(id) == table.end())
		return false;

	return table[id](list);
}

void StockKAnalyzer::init()
{
	table[QString("k.大实体短影")] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() <= 1)
			return false;

		int current = list.size()-1;
		auto stock = list[current];
		double body = std::max(stock.current,stock.open) - std::min(stock.current,stock.open);
		double head = stock.top - std::max(stock.current,stock.open); 
		double footer = std::min(stock.current,stock.open) - stock.low;
		if(body > 3.0*head && body > 3.0*footer)
			return true;
		return false;
	};

	table[QString("k.上缺口")] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() <= 1)
			return false;

		int current = list.size()-1;
		return list[current].top < list[current-1].low;
	};

	table[QString("k.下缺口")] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() <= 1)
			return false;

		int current = list.size()-1;
		return list[current].low > list[current-1].top;
	};

	table[QString("k.光头")] = [](const QVector<Stock>& list) -> bool
	{
		int current = list.size()-1;
		return isSameValue(std::max(list[current].current,list[current].open),list[current].top);
	};

	table[QString("k.光脚")] = [](const QVector<Stock>& list) -> bool
	{
		int current = list.size()-1;
		return isSameValue(std::min(list[current].current,list[current].open),list[current].low);
	};

	table[QString("k.光头阳线")] = [](const QVector<Stock>& list) -> bool
	{
		int current = list.size()-1;
		return isSameValue(list[current].current,list[current].top) && 
			list[current].open < list[current].current;
	};

	table["k.光身"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() <= 1)
			return false;
		int current = list.size()-1;
		return isSameValue(list[current].open,list[current].current);
	};

	table["k.光脚阳线"] = [](const QVector<Stock>& list) -> bool
	{
		int current = list.size()-1;
		return isSameValue(list[current].open,list[current].low) && 
			list[current].open < list[current].current;
	};

	table["k.光头光脚阳线"] = [](const QVector<Stock>& list) -> bool
	{
		int current = list.size()-1;
        return isSameValue(list[current].current,list[current].top) && 
			list[current].open < list[current].current &&
			isSameValue(list[current].open,list[current].low) && 
			list[current].open < list[current].current;
	};

	table["k.光头阴线"] = [](const QVector<Stock>& list) -> bool
	{
		int current = list.size()-1;
		return isSameValue(list[current].open,list[current].top) && 
			list[current].open > list[current].current;
	};

	table["k.光脚阴线"] = [](const QVector<Stock>& list) -> bool
	{
		int current = list.size()-1;
		return isSameValue(list[current].open,list[current].low) && 
			list[current].open > list[current].current;
	};

	table["k.光头光脚阴线"] = [](const QVector<Stock>& list) -> bool
	{
		int current = list.size()-1;
        return isSameValue(list[current].current,list[current].low) && 
			list[current].open > list[current].current &&
			isSameValue(list[current].open,list[current].top);
	};

	table["k.涨停"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() == 1)
			return false;
		int current = list.size()-1;
		Stock stock = list[current];
		double prev = list[current-1].current;
		if(!isSameValue(stock.current,stock.top))
			return false;
		
		return isIncreaseTop(prev,stock.current);
	};

	table["k.跌停"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.isEmpty())
			return false;
		int current = list.size()-1;
		Stock stock = list[current];
		double prev = list[current-1].current;
		if(!isSameValue(stock.low,stock.current))
			return false;
		return prev*0.9+0.006 >= stock.current && 
			stock.current >= prev*0.9-0.006;
	};

	table["k.十字星"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() == 0)
			return false;
		int current = list.size()-1;
		Stock stock = list[current];
		float body = std::max(stock.open,stock.current) - std::min(stock.open,stock.current);
		if(stock.top - std::max(stock.open,stock.current) > 1.5*body && 
			std::min(stock.open,stock.current) - stock.low > 1.5*body)
			return true;
		return false;
	};

	table["k.锤子线"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() == 0)
			return false;
		int current = list.size()-1;
		Stock stock = list[current];

		float head = stock.top - std::max(stock.open,stock.current);
		float body = std::max(stock.open,stock.current) - std::min(stock.open,stock.current);
		float foot = std::min(stock.open,stock.current) - stock.low;
		if(foot > 1.5*body && head*3.0 < foot && head*1.2 < body)
			return true;
		return false;
	};

	table["k.倒锤子线"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() == 0)
			return false;
		int current = list.size()-1;
		Stock stock = list[current];
		float head = stock.top - std::max(stock.open,stock.current);
		float body = std::max(stock.open,stock.current) - std::min(stock.open,stock.current);
		float foot = std::min(stock.open,stock.current) - stock.low;
		if(body*1.5 < head && foot*3.0 < head && foot*1.2 < body)
			return true;
		return false;
	};

	table["k.红三兵"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev = list[cursor-1];
		Stock prev2 = list[cursor-2];
		if(stock.current < stock.open || prev.current < prev.open || prev2.current < prev2.open)
			return false;
		if(stock.current <= prev.current || prev.current <= prev2.current)
			return false;
		return true;
	};

	table["k.黑三兵"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev = list[cursor-1];
		Stock prev2 = list[cursor-2];
		if(stock.current > stock.open || prev.current > prev.open || prev2.current > prev2.open)
			return false;
		if(stock.current >= prev.current || prev.current >= prev2.current)
			return false;
		return true;
	};

	table["k.旭日东升"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev = list[cursor-1];
		bool ok = checkBody(prev);
		if(!ok)
			return false;
		ok = checkBody(stock);
		if(!ok)
			return false;

		if(stock.current <= stock.open)
			return false;

		if(prev.current >= prev.open)
			return false;
		
		if(prev.current >= stock.open)
			return false;

		return true;
	};

	table["k.阳包阳"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev = list[cursor-1];

		bool ok = checkBody(prev);
	    if(!ok)
			return false;

		ok = checkBody(stock);
	    if(!ok)
			return false;

		if(stock.current <= stock.open || prev.current <= prev.open)
			return false;
		if(stock.low >= prev.low)
			return false;
		if(stock.top <= prev.top)
			return false;
		return true;
	};

	table["k.阴包阴"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev = list[cursor-1];

		bool ok = checkBody(prev);
	    if(!ok)
			return false;

		ok = checkBody(stock);
	    if(!ok)
			return false;

		if(stock.current >= stock.open || prev.current >= prev.open)
			return false;
		if(stock.top <= prev.top)
			return false;
		if(stock.low >= prev.low)
			return false;
		return true;
	};

	table["k.乌云盖顶"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev1 = list[cursor-1];
		Stock prev2 = list[cursor-2];
    
		bool ok = checkBody(prev1);
	    if(!ok)
			return false;

	    if(prev1.current <= prev1.open)
		    return false;
	    if(stock.open <= prev1.current)
			return false;
		if(stock.open <= stock.current)
			return false;
   
		ok = checkBody(stock);
	    if(!ok)
			return false;

	    if(prev1.top > stock.open)
			return false;

        return true;
	};

	table["k.涨势尽头"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev1 = list[cursor-1];
		Stock prev2 = list[cursor-2];
    
		bool ok = checkBody(prev1);
	    if(!ok)
			return false;

	    if(prev1.current <= prev1.open)
		    return false;

		if(stock.low <= prev1.current)
			return false;

		if(checkBody(stock))
			return false;

		if(std::min(stock.current,stock.open) < prev1.current)
			return false;
		if(std::max(stock.current,stock.open) < prev1.top)
			return true;
        return false;
	};

	table["k.倾盆大雨"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev1 = list[cursor-1];
		Stock prev2 = list[cursor-2];
    
        bool ok = checkBody(prev1);
	    if(!ok)
			return false;
		if(prev1.current <= prev1.open)
		    return false;
	    if(stock.open >= prev1.current)
		    return false;
	    ok = checkBody(stock);
	    if(!ok)
		    return false;
	    if(stock.current >= stock.open)
			return false;
	    if(stock.current >= prev1.open)
			return false;
		return true;
	};

	table["k.阳吞阴"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev1 = list[cursor-1];
		Stock prev2 = list[cursor-2];
    
        bool ok = checkBody(prev1);
		if(!ok)
			return false;

        ok = checkBody(stock);
		if(!ok)
			return false;

	    if(prev1.current >= prev1.open)
		    return false;
	    if(stock.current <= stock.open)
		    return false;
	    if(stock.current <= prev1.open)
		    return false;
		if(stock.open > prev1.current)
		    return false;
	    if(stock.current <= prev1.top)
		    return false;
        return true;
	};

	table["k.阴吞阳"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev1 = list[cursor-1];
		Stock prev2 = list[cursor-2];
    
        bool ok = checkBody(prev1);
		if(!ok)
			return false;

        ok = checkBody(stock);
		if(!ok)
			return false;

	    if(prev1.current <= prev1.open)
		    return false;
	    if(stock.current >= stock.open)
		    return false;
	    if(stock.current > prev1.open)
		    return false;
		if(stock.open < prev1.top)
		    return false;
        return true;
	};

	table["k.穿刺"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev1 = list[cursor-1];

        bool ok = checkBody(prev1);
		if(!ok)
			return false;

	    if(prev1.increase >= -2.0f)
			return false;

	    if(prev1.current >= prev1.open)
			return false;

	    if(stock.current <= stock.open)
			return false;

	    if(stock.open >= prev1.current)
			return false;   

		if(stock.top >= prev1.open)
			return false;

	    ok = checkBody(stock);
		if(!ok)
			return false;

	    if(stock.current >= prev1.open)
			return false;
		return true;
	};

	table["k.尖三兵"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev1 = list[cursor-1];
		Stock prev2 = list[cursor-2];
    
        bool ok = checkBody(prev2);
		if(!ok)
			return false;

        ok = checkBody(stock);
		if(!ok)
			return false;

        ok = checkBody(prev1);
		if(ok)
			return false;

		if(stock.increase < 3.6f || stock.current < stock.open)
			return false;

		if(stock.current <= prev1.top)
			return false;

		if(prev2.current <= prev2.open)
			return false;

		if(prev1.low <= prev2.current)
			return false;

		float head = prev1.top - std::max(prev1.open,prev1.current);
		float body = std::max(prev1.open,prev1.current) - std::min(prev1.open,prev1.current);
		if(head*0.5<body)
			return false;

		return true;
	};

	table["k.看涨孕育线"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock stock = list[cursor];
		Stock prev1 = list[cursor-1];
    
        bool ok = checkBody(stock);
		if(!ok)
			return false;

        ok = checkBody(prev1);
		if(!ok)
			return false;

		if(stock.increase <= 0.0)
			return false;

		if(prev1.increase >= 0.0)
			return false;

		if(stock.top >= prev1.open || stock.open <= prev1.current)
			return false;

		if(stock.current <= stock.open || prev1.open <= prev1.current)
			return false;

		return true;
	};

	table["k.清晨之星"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock current = list[cursor];
		Stock prev1 = list[cursor-1];
		Stock prev2 = list[cursor-2];
    
		if(prev2.current >= prev2.open)
			return false;

		double increase = GET_INCREASE(prev2.current,prev2.open);
		if(increase > -2.2)
			return false;

		if(prev1.top > prev2.current || prev1.top > current.open)
			return false;

		if(prev2.low <= std::max(prev1.current,prev1.open))
			return false;

		if(current.low <= prev1.low)
			return false;

		float ratio = checkShadowRadio_(prev1);
		if(ratio < 2.4f)
			return false;

		increase = GET_INCREASE(current.current,current.open);
		if(increase <= 2.3)
			return false;

		auto ok = checkBody(current);
		if(!ok)
			return false;

		if(current.current < prev2.current + 0.5*(prev2.open - prev2.current))
			return false;
	    return true;
	};

	table["k.黄昏之星"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 4)
			return false;
		int cursor = list.size()-1;
		Stock current = list[cursor];
		Stock prev1 = list[cursor-1];
		Stock prev2 = list[cursor-2];

		if(prev2.increase <= 4.0)
			return false;
    
        bool ok = checkBody(prev2);
	    if(!ok)
			return false;

		ok = checkBody(current);
	    if(!ok)
			return false;

		ok = checkBody(prev1);
	    if(ok)
			return false;

	    if(prev2.current <= prev2.open)
			return false;

		if(current.current >= current.open)
			return false;

	    float ratio = checkShadowRadio_(prev1);
	    if(ratio < 3.0f)
			return false;

		if(prev1.open < prev2.current)
			return false;

		if(current.open > prev1.current)
			return false;

		if(current.current < prev2.current)
			return true;

	    return true;
	};

	table["k.双飞乌鸦"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock current = list[cursor];
		Stock prev = list[cursor-1];

        bool ok = checkBody(prev);
	    if(!ok)
			return false;

	    if(prev.current >= prev.open || current.current >= current.open)
			return false;
        
		if(current.open < prev.current)
			return false;
		if(current.current >= prev.low)
			return false;
		if(current.top >= prev.open)
			return false;

		return true;
	};

	table["k.多方炮"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;
		int cursor = list.size()-1;
		Stock current = list[cursor];
		Stock prev = list[cursor-1];
		Stock prev2 = list[cursor-2];

        bool ok = checkBody(current);
	    if(!ok)
			return false;

        ok = checkBody(prev2);
	    if(!ok)
			return false;

		if(current.current <= current.open)
			return false;

		if(prev2.current <= prev2.open)
			return false;

		if(current.increase < 3.0 || prev2.increase < 3.0)
			return false;

		if(current.current < prev.top)
			return false;

		if(prev2.current < prev2.top)
			return false;

		if(prev.low <= prev2.current)
			return false;

		if(prev.low <= current.current)
			return false;

		if(prev.increase >= -1.0)
			return false;

		return true;
	};

	table["k.好友反攻"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 3)
			return false;

		int cursor = list.size()-1;
		Stock current = list[cursor];
		Stock prev = list[cursor-1];
		Stock prev2 = list[cursor-2];

		if(prev.increase > -3.0f)
			return false;

		if(prev.current >= prev.open)
			return false;

		if(current.open >= prev.current)
			return false;

		if(current.open >= current.current)
			return false;

		if(GET_INCREASE(current.current,current.open) < 3.0f)
			return false;

		if(!isEqual(current.current,prev.current,prev.current*0.005))
			return false;

		return true;
	};

	table["k.上升三法"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 5)
			return false;

		int cursor = list.size()-1;
		Stock current = list[cursor];
		Stock prev1 = list[cursor-1];
		Stock prev2 = list[cursor-2];
		Stock prev3 = list[cursor-3];
		Stock prev4 = list[cursor-4];

	    bool ok = checkBody(prev4);
		if(!ok)
			return false;

		ok = checkBody(current);
		if(!ok)
			return false;

	    if(prev4.current < prev4.open || prev4.increase <= 3.0f)
		    return false;
	
	    //连续3天价格下跌且不超出实体
	    if(prev3.current >= prev3.open &&
	        prev2.current >= prev2.open &&
	        prev3.current >= prev1.open)
	        return false;

	    //最后一天大涨
	    if(current.current <= current.open || current.current <= prev4.current)
		    return false;

	    if(prev1.low < prev4.low || 
	       prev2.low < prev4.low || 
	       prev3.low < prev4.low)
		    return false;

	    //连续下跌
	    if(prev4.current < prev3.current || prev3.current < prev2.current || prev2.current < prev1.current)
	        return false;
		return true;
	};

	table["k.下降三法"] = [](const QVector<Stock>& list) -> bool
	{
		if(list.size() < 5)
			return false;

		int cursor = list.size()-1;
		Stock current = list[cursor];
		Stock prev1 = list[cursor-1];
		Stock prev2 = list[cursor-2];
		Stock prev3 = list[cursor-3];
		Stock prev4 = list[cursor-4];

        bool ok = checkBody(current);
	    if(!ok)
		    return false;

		ok = checkBody(prev4);
	    if(!ok)
		    return false;

        ok = checkBody(current);
	    if(!ok)
		    return false;

	    if(current.current >= current.open)
		    return false;

		if(current.increase >= -3.0f)
			return false;

	    if(prev1.current <= prev1.open || prev2.current <= prev2.open || prev3.current <= prev3.open)
		    return false;

	    if(prev4.current >= prev4.open)
		    return false;

	    if(prev1.current >= prev4.current || prev2.current >= prev4.current || prev3.current >= prev4.current)
		    return false;

	    if(prev1.low < prev2.low || prev2.low < prev3.low || prev3.low < prev4.low)
		    return false;

	    if(current.current > prev4.current)
		    return false;

	    if(prev1.top > prev4.open || prev2.top > prev3.open || prev3.top > prev4.open)
		    return false;
		return true;
	};
}



