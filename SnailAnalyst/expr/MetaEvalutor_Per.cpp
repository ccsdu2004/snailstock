#include "ObjectFactory.h"
#include "DataProvider.h"
#include "Stock.h"
#include "expr/MetaEvalutor.h"
#include "ConfigurationParser.h"

int checkStockIndex(const QList<Stock>& stocks,const QString& code)
{
	int i = 0;
	foreach(auto item,stocks)
	{
		if (item.code == code)
			return i;
		i ++;
	}
	return -1;
};

struct MetaEvalator_StockPer : public MetaEvalutor
{
	int offset;
	QVector<Stock> stocks;
	QString flag;

	MetaEvalator_StockPer()
	{
		offset = 0;
	}

	MetaEvalator_StockPer(Span spn):MetaEvalutor(spn)
	{
		offset = 0;
	}

	bool returnBool()const{ return false; }
	const QStringList tag()const
	{
		QStringList items;
		items.append("per.������");
		items.append("per.����");
		items.append("per.low");
		items.append("per.high");
		items.append("per.open");
		items.append("per.close");
		items.append("per.�ɽ���");
		items.append("per.�ɽ���");
		items.append("per.���");
		items.append("per.�Ƿ�");
		items.append("per.��ͨ��ֵ");
		items.append("per.����ֵ");
		items.append("per.per");
		items.append("per.pb");
		items.append("per.������ǰ50");
		items.append("per.������ǰ100");
		items.append("per.�Ƿ�ǰ50");
		items.append("per.�Ƿ�ǰ100");
		items.append("per.���ǰ50");
		items.append("per.�Ƿ�ǰ100");
		items.append("per.�ɽ���ǰ50");
		items.append("per.�ɽ���ǰ100");
		items.append("per.�ɽ���ǰ50");
		items.append("per.�ɽ���ǰ100");
		items.append("per.�߼�ǰ50");
		items.append("per.�߼�ǰ100");
		items.append("per.�ͼ�ǰ50");
		items.append("per.�ͼ�ǰ100");
		items.append("per.С��ֵǰ50");
		items.append("per.С��ֵǰ100");
		items.append("per.����ֵǰ50");
		items.append("per.����ֵǰ100");
		items.append("per.��ͣ��");
		return items;
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
		int current = pos + offset;
		if (current <= 0 || current >= stocks.size())
			return false;

		auto data = stocks;

		if (span != normal)
			return false;

		if (current < 0)
			return false;

		QString code = dataProvider.code;
		QString date = data[current].date;

		dataProvider.loadStockPerData(date);
		bool has = dataProvider.hasStockPerData(date,code);
		if (!has)
			return false;

		auto per = dataProvider.perStockData[date][code];

		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = 0.0;
		if (flag == "per.������")
			output.double_ = per.turnover;
		else if (flag == "per.����")
			output.double_ = per.privClose;
		else if (flag == "per.open")
			output.double_ = per.open;
		else if (flag == "per.low")
			output.double_ = per.low;
		else if (flag == "per.high")
			output.double_ = per.top;
		else if (flag == "per.close")
			output.double_ = per.current;
		else if (flag == "per.�ɽ���")
			output.double_ = per.tradeCapital;
		else if (flag == "per.�ɽ���")
			output.double_ = per.tradeVolume;
		else if (flag == "per.���")
			output.double_ = GET_INCREASE(per.top,per.low);
		else if (flag == "per.�Ƿ�")
			output.double_ = per.increase;
		else if (flag == "per.��ͨ��ֵ")
			output.double_ = per.data[Stock::TAG_LIUTONGSHIZHI].toDouble();
		else if (flag == "per.����ֵ")
			output.double_ = per.data[Stock::TAG_ZONGSHIZHI].toDouble();
		else if (flag == "per.pb")
			output.double_ = per.data[Stock::TAG_PB].toDouble();
		else if (flag == "per.per")
			output.double_ = per.data[Stock::TAG_PE_D].toDouble();
		else if (flag == "per.������ǰ50")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.turnover > right.turnover;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 50)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.������ǰ100")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.turnover > right.turnover;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 100)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.���ǰ50")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				float d1 = GET_INCREASE(left.top,left.low);
				float d2 = GET_INCREASE(right.top,right.low);
				return d1 > d2;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 50)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.���ǰ100")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				float d1 = GET_INCREASE(left.top,left.low);
				float d2 = GET_INCREASE(right.top,right.low);
				return d1 > d2;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 100)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.�Ƿ�ǰ50")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.increase > right.increase;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 50)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.�Ƿ�ǰ100")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.increase > right.increase;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 100)
				output.double_ = 1.0;
			else
				return false;
		}

		else if (flag == "per.�ɽ���ǰ50")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.tradeVolume > right.tradeVolume;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 50)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.�ɽ���ǰ100")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.tradeVolume > right.tradeVolume;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 100)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.�ɽ���ǰ50")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.tradeCapital > right.tradeCapital;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 50)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.�ɽ���ǰ100")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.tradeCapital > right.tradeCapital;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 100)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.�߼�ǰ50")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.current > right.current;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 50)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.�߼�ǰ100")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.current > right.current;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 100)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.�ͼ�ǰ50")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.current < right.current;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 50)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.�ͼ�ǰ100")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.current < right.current;
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 100)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.С��ֵǰ50")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.data[Stock::TAG_ZONGSHIZHI] < right.data[Stock::TAG_ZONGSHIZHI];
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 50)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.С��ֵǰ100")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.data[Stock::TAG_ZONGSHIZHI] < right.data[Stock::TAG_ZONGSHIZHI];
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 50)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.����ֵǰ50")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.data[Stock::TAG_ZONGSHIZHI] > right.data[Stock::TAG_ZONGSHIZHI];
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 50)
				output.double_ = 1.0;
			else
				return false;
		}
		else if (flag == "per.����ֵǰ100")
		{
			auto info = dataProvider.perStockData[date];
			QList<Stock> stocks = info.values();

			std::sort(stocks.begin(),stocks.end(),[](const Stock& left,const Stock& right)
			{
				return left.data[Stock::TAG_ZONGSHIZHI] > right.data[Stock::TAG_ZONGSHIZHI];
			});

			int index = checkStockIndex(stocks,code);
			if (index >= 0 && index < 100)
				output.double_ = 1.0;
			else
				return false;
		}
		else if(flag=="per.��ͣ��")
		{
		    output.double_ = dataProvider.perStockLimitR[date];
		}

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

	static ObjectRegister<MetaEvalutor,MetaEvalator_StockPer,QString> dummy;
};

ObjectRegister<MetaEvalutor,MetaEvalator_StockPer,QString> MetaEvalator_StockPer::dummy("per");