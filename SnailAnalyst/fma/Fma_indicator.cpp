#include "Fma_indicator.h"
#include "../config/ConfigItem.h"
#include "../ConfigurationParser.h"
#include "../SnailContext.h"

inline int isInFMABox(const Stock& stock,const DataWrapper5<bool,float,float,float,float>& ma,double tor)
{
	if(!ma._1)
		return 1;
    
	double std;
	if(stock.low > ma._2)
		std = stock.low;
	else
		std = std::min(stock.current,stock.open);
	
	double increase = GET_INCREASE(std,ma._2);
	if(increase > tor && std > ma._2)
		return 1;
	if(increase > tor)
		return -1;
	return 0;
}

bool isFMA(const QVector<Stock>& stocks,QVector<DataWrapper5<bool,float,float,float,float>>& flags)
{
    if(stocks.size() != flags.size())
		return false;
	int count = stocks.size();
	if(count <= 15)
		return false;

#define GET_VAR(str)\
    SnailContext::open()->getConfigFactory()->getVar(QString(str),var);

	QVariant var;

	double mink = .2;
	{
		CHECK_CONFIG_VAR_DOUBLE(mink,0.02,5.0,"config.unit.ma.fma.最小上升斜率")
	}

	int mincount = 4;
	{
		CHECK_CONFIG_VAR_INT(mincount,3,5,"config.unit.ma.fma.最小点数")
	}

	int span = 36;
	{
		CHECK_CONFIG_VAR_INT(span,24,96,"config.unit.ma.fma.最小跨度")
	}

	int interval = 5;
	{
		CHECK_CONFIG_VAR_INT(span,3,36,"config.unit.ma.fma.最小点间距")
	}

	double tor = 2.0;
	{
		CHECK_CONFIG_VAR_DOUBLE(tor,0.02,9.0,"config.unit.ma.fma.最大可接受偏差")
	}

	int points = 0;
	int vps = 99999;
	double to = stocks[count-1].current;

	for(int i=count-1;i>=0;i--)
	{
		int ok = isInFMABox(stocks[i],flags[i],tor);
	    if(i == count-1)
		{
		    if(ok != 0)
			{
				return false;
			}
			else
			{
			    points ++;
				vps = i;
			}
		}
		else
		{
			if(ok == 0 && vps - i >= interval-1)
			{
				vps = i;
				points ++;

				if(points >= mincount &&
					stocks[i].current*(1+mink) < to)
				{
					if(count-1-vps >= span)
						return true;
					return false;
			    }
			}
			//else if(ok == -1)
				//return false;

			else if(ok == -1 && vps - i >= interval-1)
			{
				vps = i;
				points ++;

				if(points >= mincount && stocks[i].current*1.02 < to)
					return true;
				else
					return false;
			}
		}
	}
    return false;
}