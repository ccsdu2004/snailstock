#include <QDateTimeEdit>
#include <QBoxLayout>
#include <QSpinBox>
#include "ObjectFactory.h"
#include "GridTradeWidget.h"
#include "../addon/Addon_Boll_C.h"

class GridTradeItemWidgetByDate : public GridTradeItemWidget
{
public:
	GridTradeItemWidgetByDate(QWidget* parent = nullptr) :
		GridTradeItemWidget(parent)
	{
		date = new QDateEdit(this);
		date->setObjectName(QStringLiteral("date"));
		date->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
		date->setDate(QDate::currentDate().addDays(-360));

		auto box = new QVBoxLayout(this);
		box->addWidget(date);

		setObjectName(QString("指定日期买入"));
	}

	~GridTradeItemWidgetByDate()
	{
	}

	int execute(const QVector<Stock>& stocks,int offset,QString* error = nullptr)
	{
		int id = -1;
		if (offset < stocks.size())
		{
			QString str = date->text();
			for (int i = offset; i < stocks.size(); i++)
			{
				if (str == stocks[i].date)
					id = i;
			}
		}
		
		if(id == -1 && !error)
			*error = QString("无指定日期股票数据");
		return id;
	}
private:
	QDateTimeEdit* date;
	static ObjectRegister<GridTradeItemWidget,GridTradeItemWidgetByDate,QString> dummy;
};

ObjectRegister<GridTradeItemWidget,GridTradeItemWidgetByDate,QString> 
    GridTradeItemWidgetByDate::dummy("date");

class GridTradeItemWidgetByMa : public GridTradeItemWidget
{
public:
	GridTradeItemWidgetByMa(QWidget* parent = nullptr) :
		GridTradeItemWidget(parent)
	{
		ma = new QSpinBox();
		ma->setRange(6,240);
		ma->setValue(30);

		auto box = new QVBoxLayout(this);
		box->addWidget(ma);

		setObjectName(QString("低于均线买入"));
	}

	~GridTradeItemWidgetByMa()
	{
	}

	void init(const QVector<Stock>& stocks)
	{
		list.clear();

		MaNIndicator indicator(ma->value());
		foreach(Stock stock,stocks)
			list.push_back(indicator.update(stock));
	}

	int execute(const QVector<Stock>& stocks,int offset,QString* error = nullptr)
	{
		int id = -1;
		if (offset < stocks.size())
		{
			for(int i = offset; i < stocks.size(); i++)
			{
				auto value = stocks.at(i).current;
				auto indicator = list.at(i);
				if (indicator._1 && indicator._2 > value)
				{
					id = i + 1;
					if (id >= stocks.size())
						id = -1;
					break;
				}
			}
		}

		if(id == -1 && error)
			*error = QString("无指定日期股票数据");
		return id;
	}

	bool check(const QVector<Stock>& stocks, int pos)
	{
		if(pos < 0 || stocks.size() <= pos)
			return false;

		auto value = stocks.at(pos).current;
		auto node = list.at(pos);
		return node._1 && node._2 > value;
	}
private:
	QSpinBox* ma;
	IndicList list;
	static ObjectRegister<GridTradeItemWidget,GridTradeItemWidgetByMa,QString> dummy;
};

ObjectRegister<GridTradeItemWidget,GridTradeItemWidgetByMa,QString>
GridTradeItemWidgetByMa::dummy("ma");

class GridTradeItemWidgetByBoll : public GridTradeItemWidget
{
public:
	GridTradeItemWidgetByBoll(QWidget* parent = nullptr) :
		GridTradeItemWidget(parent)
	{
		ma = new QSpinBox();
		ma->setRange(10,30);
		ma->setValue(20);

		ma2 = new QDoubleSpinBox();
		ma2->setRange(1.0,3.0);
		ma2->setDecimals(2);
		ma2->setValue(2.0);
		ma2->setSingleStep(0.1);

		auto box = new QVBoxLayout(this);
		box->addWidget(ma);
		box->addWidget(ma2);

		setObjectName(QString("跌破布林线下轨买入"));
	}

	~GridTradeItemWidgetByBoll()
	{
	}

	void init(const QVector<Stock>& stocks)
	{
		list.clear();

		BollIndicator indicator(ma->value(), ma2->value());
		foreach(Stock stock, stocks)
			list.push_back(indicator.update(stock));
	}

	int execute(const QVector<Stock>& stocks,int offset,QString* error = nullptr)
	{
		int id = -1;
		if (offset < stocks.size())
		{
			for(int i = offset; i < stocks.size(); i++)
			{
				auto value = stocks.at(i).current;
				auto indicator = list.at(i);
				if (indicator._1 && indicator._2 - indicator._3 > value)
				{
					id = i + 1;
					if(id >= stocks.size())
						id = -1;
					break;
				}
			}
		}

		if ((id == -1 || id >= stocks.size()) && error)
			*error = QString("无指定日期股票数据");
		return id;
	}

	bool check(const QVector<Stock>& stocks, int pos)
	{
		if (pos < 0 || stocks.size() <= pos)
			return false;

		auto value = stocks.at(pos).current;
		auto node = list.at(pos);
		return node._1 && (node._2 - node._3)*ma2->value()*0.5 > value;
	}
private:
	QSpinBox* ma;
	QDoubleSpinBox* ma2;
	IndicList list;
	static ObjectRegister<GridTradeItemWidget, GridTradeItemWidgetByBoll, QString> dummy;
};

ObjectRegister<GridTradeItemWidget,GridTradeItemWidgetByBoll,QString>
GridTradeItemWidgetByBoll::dummy("boll");

class GridTradeItemWidgetByAlgo : public GridTradeItemWidget
{
public:
	GridTradeItemWidgetByAlgo(QWidget* parent = nullptr) :
		GridTradeItemWidget(parent)
	{
		algo = new QLineEdit();

		p1 = new QSpinBox();
		p1->setRange(0,100);
		p1->setValue(20);

		p2 = new QSpinBox();
		p2->setRange(0,100);
		p2->setValue(20);

		p3 = new QSpinBox();
		p3->setRange(0,100);
		p3->setValue(0);

		auto box = new QVBoxLayout(this);
		box->addWidget(algo);
		box->addWidget(p1);
		box->addWidget(p2);
		box->addWidget(p3);

		setObjectName(QString("指定算法买入"));
	}

	~GridTradeItemWidgetByAlgo()
	{
	}

	int execute(const QVector<Stock>& stocks,int offset,QString* error = nullptr)
	{
		if(algo->text() != "bollC")
		{
			if (error)
				*error = QString("不支持指定算法");
			return -1;
		}

		if (stocks.size() < 20)
		{
			if (error)
				*error = QString("股票样点数据过少无法计算");
			return -1;
		}

		BollIndicator indicator(p1->value(),p2->value());
		BUILD_INDICATOR_LIST(indicator,stocks);

		int id = -1;
		if (offset < stocks.size())
		{
			for (int i = offset; i < list.size(); i++)
			{
				bool ok = isFit(stocks.mid(0, i), list.mid(0, i), p3->value());
				if (ok)
				{
					id = i;
					break;
				}
			}
		}

		if ((id == -1 || id >= stocks.size()) && error)
			*error = QString("无指定日期股票数据");
		return id;
	}
private:
	QLineEdit* algo;
	QSpinBox* p1;
	QSpinBox* p2;
	QSpinBox* p3;
	static ObjectRegister<GridTradeItemWidget, GridTradeItemWidgetByAlgo, QString> dummy;
};

ObjectRegister<GridTradeItemWidget, GridTradeItemWidgetByAlgo, QString>
GridTradeItemWidgetByAlgo::dummy("algo");