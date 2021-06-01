#include "ObjectFactory.h"
#include "DataProvider.h"
#include "bid/StockBidData.h"
#include "expr/MetaEvalutor.h"
#include "ConfigurationParser.h"

struct MetaEvalator_StockBid : public MetaEvalutor
{
	int offset;
	QVector<Stock> stocks;
	QString flag;

	MetaEvalator_StockBid()
	{
		offset = 0;
	}

	MetaEvalator_StockBid(Span spn):MetaEvalutor(spn)
	{
		offset = 0;
	}

	bool returnBool()const{ return false; }
	const QStringList tag()const
	{
		QStringList key;
		key << "bid.买1" << "bid.买2" << "bid.买3" << "bid.买4" << "bid.买5";
		key << "bid.卖1" << "bid.卖2" << "bid.卖3" << "bid.卖4" << "bid.卖5";
		key << "bid.成交量" << "bid.成交额" << "bid.五档卖单和" << "bid.五档买单和" << "bid.价格" << "bid.涨幅";
		return key;
	}
	bool build(const QString& label,const QVector<double>& arguments)
	{
		flag = label;

		if (arguments.size() != 1)
			return false;

		offset = arguments[0];
		if (offset > 0)
			return false;
		return tag().contains(flag);
	}

	bool evalute(int pos,ResultType& output)
	{
		//current 为量化下一日
		int current = pos + offset+1;
		if (current <= 0 || current >= stocks.size())
			return false;

		auto data = stocks;

		if (span != normal)
			return false;

		if (current < 0)
			return false;

		QString code = dataProvider.code;
		QString date = data[current].date;

		dataProvider.loadStockBidData(date);
		bool has = dataProvider.hasStockBidData(date,code);
		if (!has)
			return false;

		auto bid = dataProvider.bidData[date][code];

		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = 0.0;
		if (flag == "bid.买1")
			output.double_ = bid.buy[0];
		else if (flag == "bid.买2")
			output.double_ = bid.buy[1];
		else if (flag == "bid.买3")
			output.double_ = bid.buy[2];
		else if (flag == "bid.买4")
			output.double_ = bid.buy[3];
		else if (flag == "bid.买5")
			output.double_ = bid.buy[4];
		else if (flag == "bid.卖1")
			output.double_ = bid.sale[0];
		else if (flag == "bid.卖2")
			output.double_ = bid.sale[1];
		else if (flag == "bid.卖3")
			output.double_ = bid.sale[2];
		else if (flag == "bid.卖4")
			output.double_ = bid.sale[3];
		else if (flag == "bid.卖5")
			output.double_ = bid.sale[4];
		else if (flag == "bid.五档买单和")
		{
			for (int i = 0;i < 5;i++)
				output.double_ += bid.buy[i];
		}
		else if (flag == "bid.五档卖单和")
		{
			for (int i = 0;i < 5;i++)
				output.double_ += bid.sale[i];
		}
		else if (flag == "bid.成交量")
		{
			output.double_ = bid.vol;
		}
		else if (flag == "bid.成交额")
		{
			output.double_ += bid.capital;
		}
		else if (flag == "bid.涨幅")
		{
			output.double_ = bid.increase;
		}
		else if (flag == "bid.价格")
			output.double_ = bid.price;
		else
			return false;
		return true;
	}

	bool loadData()
	{
		stocks = dataProvider.stocks;
		return true;
	}

	void clear()
	{
		stocks.clear();
	}

	static ObjectRegister<MetaEvalutor,MetaEvalator_StockBid,QString> dummy;
};

ObjectRegister<MetaEvalutor,MetaEvalator_StockBid,QString> MetaEvalator_StockBid::dummy("bid");