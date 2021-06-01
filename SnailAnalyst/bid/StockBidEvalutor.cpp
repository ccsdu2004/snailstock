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
		key << "bid.��1" << "bid.��2" << "bid.��3" << "bid.��4" << "bid.��5";
		key << "bid.��1" << "bid.��2" << "bid.��3" << "bid.��4" << "bid.��5";
		key << "bid.�ɽ���" << "bid.�ɽ���" << "bid.�嵵������" << "bid.�嵵�򵥺�" << "bid.�۸�" << "bid.�Ƿ�";
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
		//current Ϊ������һ��
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
		if (flag == "bid.��1")
			output.double_ = bid.buy[0];
		else if (flag == "bid.��2")
			output.double_ = bid.buy[1];
		else if (flag == "bid.��3")
			output.double_ = bid.buy[2];
		else if (flag == "bid.��4")
			output.double_ = bid.buy[3];
		else if (flag == "bid.��5")
			output.double_ = bid.buy[4];
		else if (flag == "bid.��1")
			output.double_ = bid.sale[0];
		else if (flag == "bid.��2")
			output.double_ = bid.sale[1];
		else if (flag == "bid.��3")
			output.double_ = bid.sale[2];
		else if (flag == "bid.��4")
			output.double_ = bid.sale[3];
		else if (flag == "bid.��5")
			output.double_ = bid.sale[4];
		else if (flag == "bid.�嵵�򵥺�")
		{
			for (int i = 0;i < 5;i++)
				output.double_ += bid.buy[i];
		}
		else if (flag == "bid.�嵵������")
		{
			for (int i = 0;i < 5;i++)
				output.double_ += bid.sale[i];
		}
		else if (flag == "bid.�ɽ���")
		{
			output.double_ = bid.vol;
		}
		else if (flag == "bid.�ɽ���")
		{
			output.double_ += bid.capital;
		}
		else if (flag == "bid.�Ƿ�")
		{
			output.double_ = bid.increase;
		}
		else if (flag == "bid.�۸�")
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