#include "SnailApplication.h"
#include "GridTradeWidget.h"
#include <QDebug>

GridTrader::GridTrader(QObject* parent):
QObject(parent)
{
}

bool GridTrader::sale(const Stock& stock,double& lastprice)
{
	const auto config = SnailApplication::getInstance()->gridTradeConfig;
	
	for(int i=0;i<items.size();i++)
	{
		GridUnit unit = items[i];
		if(!unit.saleDate.isEmpty())
			continue;

		auto perfectPrice = unit.buyPrice*(1 + config.saleProfit);
		
		/*if(unit.buyDate == "2017-12-01")
		
			qDebug() << stock.date << " " << unit.buyPrice << " " << config.saleProfit << " " <<
			perfectPrice << " " << stock.low;*/

		if(perfectPrice <= stock.top)
		{
			if (perfectPrice < stock.low)
				perfectPrice = stock.low;
	
			unit.salePrice = perfectPrice;
			unit.saleDate = stock.date;
			unit.percent = config.firstBuyPercent;
			unit.price = perfectPrice;
			items[i] = unit;
			lastprice = perfectPrice;
		}
	}

	return true;
}

int GridTrader::hasHoldings()
{
	int count = 0;
	for(int i = 0; i < items.size(); i++)
	{
		if (items[i].saleDate.isEmpty())
			count++;
	}
	return count;
}

double GridTrader::holdLowBuyPrice()
{
	double value = std::numeric_limits<double>::max();
	for(int i = 0; i < items.size(); i++)
	{
		if(items[i].saleDate.isEmpty())
		{
			value = std::min<double>(value,items[i].buyPrice);
		}
	}
	return value;
}

bool GridTrader::buy(const Stock& stock,double buyPrice,double& price)
{
	int holds = hasHoldings()+1;
	const auto config = SnailApplication::getInstance()->gridTradeConfig;
	if(config.firstBuyPercent*holds > 1.0)
		return false;

	GridUnit unit;
	unit.buyDate = stock.date;
	unit.buyPrice = buyPrice;
	unit.price = unit.buyPrice;
	unit.lastPrice = stock.open;
	items.append(unit);
	price = unit.buyPrice;
	return true;
}

void GridTrader::trade(int pos,const QVector<Stock>& stocks)
{
	items.clear();

	const auto config = SnailApplication::getInstance()->gridTradeConfig;
	double operatePrice = 0.1;

	double lastBuyPrice = -1;
	int lastBuyPos = -1;

	for(int i = pos; i < stocks.size(); i++)
	{
		Stock current = stocks.at(i);
		if(items.isEmpty())
		{
			buy(current, current.open, operatePrice);
		    lastBuyPos = i;
		}
		else if(hasHoldings() == 0)
		{
			GridTradeWidget* widget = dynamic_cast<GridTradeWidget*>(parent());
			i = widget->getNextPosition(i);

			if(i >= stocks.size() || i < 0)
				break;

			current = stocks.at(i);
			buy(current,current.open,operatePrice);
			lastBuyPos = i;
		}
		else
		{
			double down = operatePrice*(1.0-config.decreaseBuy);
			sale(current,operatePrice);

			if(down >= current.low && down <= current.top)
			{
				buy(current,down,operatePrice);
				lastBuyPos = i;
			}
			else
			{
				GridTradeWidget* widget = dynamic_cast<GridTradeWidget*>(parent());
				bool ok = widget->check(i);

				if(ok)
				{
					double prevBuyPrice = holdLowBuyPrice();

					current = stocks.at(i);
					
					double value = prevBuyPrice*(1-config.decreaseBuy);
					if(current.low < value && current.top >= value)
					{
						buy(current,value,operatePrice);
						lastBuyPrice = operatePrice;
						lastBuyPos = i;
					}
				}
			}
		}
	}
}

 