#include <algorithm>
#include <QTextStream>
#include "StockTradeWriter.h"
#include "StockScriptEngine.h"
#include "common/common.h"
#include "StockMapper.h"
#include "Config.h"

StockTradeWriter* StockTradeWriter::getInstance()
{
	static StockTradeWriter writer;
	return &writer;
}

double getIncrease(int end,const StockTradeItem& item,bool isend)
{
	if(end == item.index)
	{ 
		if(isend)
			return GET_INCREASE(item.sale,item.buy);
		return GET_INCREASE(dataProvider.stocks[end].current,item.buy);
	}
	if(isend)
		return GET_INCREASE(item.sale,dataProvider.stocks[end-1].current);
	return GET_INCREASE(dataProvider.stocks[end].current,dataProvider.stocks[end-1].current);
}

class StockTradeComparator
{
public:
	StockTradeComparator(const QString flag):
		tag(flag)
	{
		if (tag == "desc")
			tag = "voldesc";
		else if (tag == "asc")
			tag = "volasc";
		//if(tag == "bid.desc")
	}
	
	bool operator()(const StockTradeItem& left,const StockTradeItem& right)
	{
		if(tag == "voldesc")
		{
			if (left.vol > right.vol)
				return true;
			return false;
		}
		else if (tag == "voldasc")
		{
			if (left.vol < right.vol)
				return true;
			return false;
		}
		else if (tag == "increaseasc")
		{
			if (left.increase > right.increase)
				return true;
			return false;
		}
		else if (tag == "bid.desc")
		{
			if (left.p1 > right.p1)
				return true;
			return false;
		}
		if (left.increase < right.increase)
			return true;
		return false;
	}
private:
	QString tag;
};

StockTradeWriter::StockTradeWriter()
{
}

void StockTradeWriter::calcIncreaseList(int end)
{
	for(int i= currentTradeInfo.index;i<=end;i++)
	{
		bool isend = i == end;
		double increase = getIncrease(i,currentTradeInfo,isend);
		//currentTradeInfo.profits[dataProvider.stocks[i].date] = increase;
		double newinc = increase;
		if(isend)
			newinc -= config.tradeCost*0.1;
		currentTradeInfo.profits[dataProvider.stocks[i].date] = DataWrapper2<double,double>(increase,newinc);
	}
}

int StockTradeWriter::add(const QString& code,int pos,bool buy,bool sale,const QVector<float>& arguments)
{	
	int next = pos + 1;
	if(next >= dataProvider.stocks.size()) 
	{
		//当前有持股
		if(!currentTradeInfo.stock.isEmpty())
		{
			historyTradeInfo[currentTradeInfo.buyDate].push_back(currentTradeInfo);
			currentTradeInfo.clear();
			return 0;
		}

		if(!buy)
		{
			currentTradeInfo.clear();
			return 0;
		}

		currentTradeInfo.stock = code;
		currentTradeInfo.buy = 0.0;
		currentTradeInfo.buyDate = QString("%1下一日开盘买入").arg(dataProvider.stocks[dataProvider.stocks.size()-1].date);
		currentTradeInfo.holdDays = 1;
		currentTradeInfo.vol = 0;
		currentTradeInfo.increase = 0;
		currentTradeInfo.saleType = "待买入";
		currentTradeInfo.hasBS = false;
		currentTradeInfo.state = StockTradeItem::State_waitForBuy;
#ifdef SUPPORT_OUTPUT_ARGUMENT
		currentTradeInfo.arguments = arguments;
#endif
		historyTradeInfo[currentTradeInfo.buyDate].push_back(currentTradeInfo);
	    currentTradeInfo.clear();
	}
	else
		return addStock(code,pos,buy,sale,arguments);
    return 0;
}

int StockTradeWriter::buyStock(const QString& code,int pos,const QVector<float>& arguments)
{
	auto prev = dataProvider.stocks[pos];
	auto current = dataProvider.stocks[pos+1];
	double increase = GET_INCREASE(current.open,prev.current);
	double lowincrease = GET_INCREASE(current.low,prev.current);

	if (config.stockBuyPoint == "open")
	{
		if (increase <= config.maxAllowIncrease && increase >= config.maxAllowDecrease)
		{
			float buyprice = current.open;
#ifdef SUPPORT_BASIC
			buyprice = buyprice * (1.0 + config.moveBuyPointOffset*0.01f);
			if (buyprice < current.low)
				return 0;
#endif
			currentTradeInfo.stock = code;
			currentTradeInfo.buy = buyprice;
			currentTradeInfo.buyDate = current.date;
			currentTradeInfo.holdDays = 1;
			currentTradeInfo.vol = prev.tradeVolume;
			currentTradeInfo.increase = prev.increase;
			currentTradeInfo.saleType = "持股中";
			currentTradeInfo.hasBS = false;
			currentTradeInfo.index = pos + 1;
#ifdef SUPPORT_OUTPUT_ARGUMENT
			currentTradeInfo.arguments = arguments;
#endif
			return 0;
		}
		return 0;
	}
	else if (lowincrease <= config.maxAllowIncrease)
	{
		float buyprice = current.open;
		float topvalue = prev.current*(1+0.01*config.maxAllowIncrease);
		if (current.open > topvalue)
			buyprice = current.open;
		else
			buyprice = topvalue;

#ifdef SUPPORT_BASIC
		buyprice = buyprice * (1.0 + config.moveBuyPointOffset*0.01f);
		if (buyprice < current.low)
			return 0;
#endif
		currentTradeInfo.stock = code;
		currentTradeInfo.buy = buyprice;
		currentTradeInfo.buyDate = current.date;
		currentTradeInfo.holdDays = 1;
		currentTradeInfo.vol = prev.tradeVolume;
		currentTradeInfo.increase = prev.increase;
		currentTradeInfo.saleType = "持股中";
		currentTradeInfo.hasBS = false;
		currentTradeInfo.index = pos + 1;
#ifdef SUPPORT_OUTPUT_ARGUMENT
		currentTradeInfo.arguments = arguments;
#endif
		return 0;
	}
	return 0;
}

int StockTradeWriter::addStock(const QString& code,int pos,bool buy,bool sale,const QVector<float>& arguments)
{   
	//当前无持股
	if(currentTradeInfo.stock.isEmpty())
	{
		if(buy)
			return buyStock(code,pos,arguments);
		return 0;
	}
	//当前有持股
	if(sale)
		return saleStock(code,pos);
	return saleStockByRule(code,pos);
}

int StockTradeWriter::saleStock(const QString& stock,int pos)
{
	return saleStockImpl(pos,"卖点");
}

int StockTradeWriter::saleStockByRule(const QString& code,int pos)
{
	if(currentTradeInfo.stock.isEmpty())
	{
		currentTradeInfo.clear();
		return 0;
	}

	auto current = dataProvider.stocks[pos];
	auto next = dataProvider.stocks[pos+1];
	double increase = GET_INCREASE(current.current,currentTradeInfo.buy);
	double topIncrease = GET_INCREASE(current.top,currentTradeInfo.buy);

	//止盈
	if(config.stockSaleRapid == "yes" && topIncrease >= config.maxProfit)
	{
		currentTradeInfo.sale = (1+0.01*config.maxProfit)*currentTradeInfo.buy;
		currentTradeInfo.saleDate = current.date;
		currentTradeInfo.holdDays = pos - currentTradeInfo.index + 1;
		currentTradeInfo.saleType = QString("止盈");
		currentTradeInfo.hasBS = true;
		currentTradeInfo.state = StockTradeItem::State_done;
		calcIncreaseList(pos);
		historyTradeInfo[currentTradeInfo.buyDate].push_back(currentTradeInfo);
		currentTradeInfo.clear();
		return config.minExcludeDay;
	}
	else if(increase >= config.maxProfit)
	    return saleStockImpl(pos,"止盈");

	if(increase <= config.maxLost)
		return saleStockImpl(pos,"止损");
	
	else if(pos - currentTradeInfo.index + 1 >= config.maxHoldDay)
		return saleStockImpl(pos,"持股时间到期");
	return 0;
}

/*
bool StockTradeItemCompareByVol(const StockTradeItem& left,const StockTradeItem& right)
{
	if(config.stockBuyOrder == "desc")
	{
	    if(left.vol > right.vol)
			return true;
		return false;
	}
	return left.vol < right.vol;
}

bool StockTradeItemCompareByIncrease(const StockTradeItem& left, const StockTradeItem& right)
{
	if (config.stockBuyOrder == "desc")
	{
		if (left.increase > right.increase)
			return true;
		return false;
	}
	return left.increase < right.increase;
}*/

void StockTradeWriter::iterateTradeNode(const StockTradeItem& item)
{
	auto itr = item.profits.begin();
	while(itr != item.profits.end())
	{
		if(!profitList.contains(itr.key()))
		{
			QMap<QString,DataWrapper2<double,double>> unit;
			unit.insert(item.stock,itr.value());
			profitList.insert(itr.key(),unit);
		}
		else
		{
			QMap<QString,DataWrapper2<double,double>> unit = profitList[itr.key()];
			unit.insert(item.stock,itr.value());
			profitList.insert(itr.key(),unit);
		}
		itr ++;
	}
}

void StockTradeWriter::iterateTradeItme()
{
	auto itr = historyTradeInfo.begin();
	while(itr != historyTradeInfo.end())
	{
		QVector<StockTradeItem> list = itr.value();
		foreach(StockTradeItem item,list)
		{
			iterateTradeNode(item);
		}
		itr ++;
	}
}

void StockTradeWriter::reduce()
{
	QMap<QString,QVector<StockTradeItem>> output;
	int ii = 0;
	auto itr = historyTradeInfo.begin();
	while(itr != historyTradeInfo.end())
	{
		auto sub = itr.value();
		if(sub.size() <= config.maxOneDayBuyStockCount)
			output[itr.key()] = trimStockTradeItemByHS300(sub);
		else
		{
			QVector<StockTradeItem> leaf;

			StockTradeComparator comparator(config.stockBuyOrder);
		    std::sort(sub.begin(),sub.end(),comparator);
			for(int i=0;i<config.maxOneDayBuyStockCount;i++)
				leaf.push_back(sub[i]);
			output[itr.key()] = trimStockTradeItemByHS300(leaf);
		}
		ii++;
		itr ++;
	}   

	historyTradeInfo = output;
	
	iterateTradeItme();
	dumpProfit();
}

void StockTradeWriter::dumpProfit()
{
	StockProfitVar var1,var2;
	var1.color = QPen(Qt::red);
	var1.label = "normal";

	var2.color = QPen(Qt::green);
	var2.label = "cost";

	QVector<double> maxdown;

	DataWrapper2<double,double> k(1.0,1.0);
	auto itr1 = profitList.begin();
	while(itr1 != profitList.end())
	{
		auto itr2 = itr1.value().begin();
		DataWrapper2<double,double> avg(0.0,0.0);
		while(itr2 != itr1.value().end())
		{
			avg._1 += itr2.value()._1;
			avg._2 += itr2.value()._2;
			itr2 ++;
		}
	
#ifdef SUPPORT_BASIC
		if (config.userAvgProfitPerDay)
#endif
		{
			avg._1 /= (double)itr1.value().size();
			avg._2 /= (double)itr1.value().size();
		}

		k._1 *= (1.0+avg._1*0.01);
		k._2 *= (1.0+avg._2*0.01);

		maxdown.push_back(avg._2);
		var1.list.insert(itr1.key(),k._1);
		var2.list.insert(itr1.key(),k._2);
		itr1 ++;
	}
	
	var1.profit = k._1;
	var1.maxdown = maxDownValue(maxdown);
	var2.profit = k._2;
	var2.maxdown = maxDownValue(maxdown);
	profitByDate.push_back(var1);
	profitByDate.push_back(var2);
}

bool StockTradeWriter::isSameStock(const QString& stock,int pos)
{
	return 
		currentTradeInfo.stock == stock && currentTradeInfo.index < pos;
	return false;
}

QVector<StockTradeItem> StockTradeWriter::outputTradeItems(QMap<int,double>& ret)
{
	int count = 0;
	double avg = 0;
	double success = 0.0;
	double avg_profit = 0.0;
	int day = 0;
	
	QVector<StockTradeItem> output;
	auto itr = historyTradeInfo.begin();
	while(itr != historyTradeInfo.end())
	{
		QVector<StockTradeItem> list = itr.value();
		foreach(StockTradeItem item,list)
		{
			output.append(item);
			if(item.state == StockTradeItem::State_done)
			{
				avg += item.holdDays;
				if(item.sale >= item.buy)
					success += 1.0;
				double k = GET_INCREASE(item.sale,item.buy);
				avg_profit += k;
				count ++;
			}
		}
		day ++;
		itr ++;
	}

	if(count>0)
	{
		avg /= count;
		success /= count;
		success *= 100.0;
		avg_profit /= count;

		ret.insert(QUANT_TRADE_COUNT,count);
		ret.insert(QUANT_TRADE_AVG_HOLDDAY,avg);
		ret.insert(QUANT_TRADE_AVG_INCREASE,avg_profit);
		ret.insert(QUANT_TRADE_SUCCESS,success);
		ret.insert(QUANT_TRADE_PROFIT,(profitByDate[0].profit-1.0)*100.0);
		ret.insert(QUANT_TRADE_PROFIT_NET,(profitByDate[1].profit-1.0)*100.0);
		ret.insert(QUANT_TRADE_BUYDAY,day);
		
		double maxdown = 0.0;
		if(profitByDate.size() == 2)
			maxdown = profitByDate[1].maxdown;
		ret.insert(QUANT_TRADE_MAXDOWN,maxdown);
	}
	return output;
}

int StockTradeWriter::saleStockImpl(int pos,const QString& desc)
{
	if(config.stockSalePoint == "close" && currentTradeInfo.index != pos)
	{
	    auto current = dataProvider.stocks[pos];
		currentTradeInfo.sale = current.current;
	    currentTradeInfo.saleDate = current.date;
	    currentTradeInfo.holdDays = pos - currentTradeInfo.index+1;
	    currentTradeInfo.saleType = desc;
	    currentTradeInfo.hasBS = true;
	    currentTradeInfo.state = StockTradeItem::State_done;
		calcIncreaseList(pos);
	    historyTradeInfo[currentTradeInfo.buyDate].push_back(currentTradeInfo);
	    currentTradeInfo.clear();
	}
	else 
	{
		if(pos < dataProvider.stocks.size()-1)
		{
			auto current = dataProvider.stocks[pos+1];
			currentTradeInfo.sale = current.open;
			currentTradeInfo.saleDate = current.date;
			currentTradeInfo.holdDays = pos - currentTradeInfo.index+2;
			currentTradeInfo.saleType = desc;
			currentTradeInfo.hasBS = true;
			currentTradeInfo.state = StockTradeItem::State_done;

			calcIncreaseList(pos);
			historyTradeInfo[currentTradeInfo.buyDate].push_back(currentTradeInfo);
			currentTradeInfo.clear();	
		}
		else
		{
			currentTradeInfo.sale = 0.0;
			currentTradeInfo.saleDate = QString("%1下一交易日开盘").arg(dataProvider.stocks[pos].date);
			currentTradeInfo.holdDays = pos - currentTradeInfo.index+2;
			currentTradeInfo.saleType = desc;
			historyTradeInfo[currentTradeInfo.buyDate].push_back(currentTradeInfo);
			currentTradeInfo.clear();	
		}
	}
	
	return config.minExcludeDay;
}

QVector<StockTradeItem> StockTradeWriter::trimStockTradeItemByHS300(const QVector<StockTradeItem>& list)
{
    QVector<StockTradeItem> output;
	QString from,to;
	if(!dataProvider.hs300.isEmpty())
	{
		auto keys = dataProvider.hs300.keys();
		from = keys.at(0);
		to = keys.at(keys.size()-1);
	}

	foreach(auto item,list)
	{
		if(item.buyDate >= from && item.saleDate <= to)
			output.push_back(item);
	}

	return output;
}

TradeReport StockTradeWriter::getTradeItem()
{
	QMap<int,double> ret;
	auto list = outputTradeItems(ret);

    QString desc;
	if(list.isEmpty())
	{
		desc = "无符合策略的股票清单,谢谢";
	}
	else
	{
	    QString message("<html>平均持股天数:%1 策略成功率:%2 买入天数:%3<br>平均收益:%4 最大回撤:%5 毛盈利:%6 净收益:%7</html>");
		desc = message
			.arg(STR_CEIL(ret[QUANT_TRADE_AVG_HOLDDAY]))
			.arg(STR_CEIL(ret[QUANT_TRADE_SUCCESS]))
			.arg(STR_CEIL(ret[QUANT_TRADE_BUYDAY]))
			.arg(STR_CEIL(ret[QUANT_TRADE_AVG_INCREASE]))
			.arg(STR_CEIL(ret[QUANT_TRADE_MAXDOWN]))
			.arg(STR_CEIL(ret[QUANT_TRADE_PROFIT]))
			.arg(STR_CEIL(ret[QUANT_TRADE_PROFIT_NET]));  
	}

	QVector<double> k;
	k.push_back(ret[QUANT_TRADE_SUCCESS]);
	k.push_back(ret[QUANT_TRADE_AVG_INCREASE]);
	k.push_back(ret[QUANT_TRADE_MAXDOWN]);
	
	TradeReport report;
	report.desc = desc;
	report.item = list;
	report.profit = k;
	report.profitvar = profitByDate;
	return report;
}

bool StockTradeWriter::toCSV(const QString& filename,const QVector<TradeReport>& info,const QStringList& arguments)
{
#ifdef SUPPORT_BASIC
	if (!config.outputProfitToFile.isEmpty())
	{
		QFile file(config.outputProfitToFile);
		if (file.open(QIODevice::WriteOnly))
		{
			QTextStream stream(&file);
			foreach(auto item,info)
			{
			}
		}
	}

#endif


	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
		return false;

	QTextStream stream(&file);

	stream << QString::fromLocal8Bit("\"代码\",\"名称\",\"买入日期\",\"买入价格\",\"卖出日期\",\"卖出价格\",\"持有日期\",\"卖出类型\",\"涨跌幅\"");
#ifdef SUPPORT_OUTPUT_ARGUMENT
	stream << ",";
	stream << arguments.join(",");
#endif
	stream << "\n";

	auto itr = info.begin();
	while (itr != info.end())
	{
		QVector<StockTradeItem> list = itr->item;
		foreach(StockTradeItem item,list)
		{
			QString increase;
			if (item.saleDate.size() == 10)
				increase = STR_CEIL(GET_INCREASE(item.sale,item.buy));
			else
				increase = QString("N/A");

			QString name = config.stockList[item.stock].name;
			if (name.isEmpty())
				name = StockMapper::instance()->get(item.stock);

			stream << item.stock << ","
				<< name<<","
				<< item.buyDate << "," << item.buy << ","
				<< item.saleDate << "," << item.sale << ","
				<< item.holdDays << "," << item.saleType
			    << "," << increase;

#ifdef SUPPORT_OUTPUT_ARGUMENT
			foreach(auto argument,item.arguments)
				stream << "," << argument;

#endif // !SUPPORT_OUTPUT_ARGUMENT
			stream << "\n";
		}
		itr ++;
	}

	file.close();
	return true;
}
