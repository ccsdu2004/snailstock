#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include "StockScriptEngine.h"
#include "StrategyViewWidget.h"
#include "ConfigurationParser.h"
#include "StockMapper.h"
#include "common/common.h"
#include "style/WidgetStyle.h"
#include "boosting/BoostingParser.h"

StrategyViewWidget::StrategyViewWidget(const QVector<TradeReport>& list,const QString& mode,const QString& baseScript,QWidget* parent):
QDialog(parent)
{
	setWindowTitle(QString("蜗牛股票量化分析软件%1").arg(SNAIL_VERSION));
	setWindowIcon(QIcon(":logo/snail.png"));

	auto box = new QVBoxLayout(this);

	if(list.isEmpty())
	{
		auto label = new QLabel("<h2><font color=red>无满足策略的股票清单</font></h2>");
		label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		box->addWidget(label);
		setFixedSize(480,280);
		return;
	}

	if(list.size() == 1)
	{	
		auto it = createTab(1,list.at(0),mode,baseScript);
		box->addWidget(it);
	}
	else
	{
	    tab = new QTabWidget(this);
		{
			QVector<TradeReport> sortList(list);
			if(config.boostingOrderBy == "success.desc")
				std::sort(sortList.begin(),sortList.end(),[](const TradeReport& l,const TradeReport& r)->bool
				{
					return l.profit[2] <= r.profit[2];
				}
				);
			else if(config.boostingOrderBy == "success.asc")
				std::sort(sortList.begin(),sortList.end(),[](const TradeReport& l,const TradeReport& r)->bool
				{
				return l.profit[2] >= r.profit[2];
				}
				);
			else if (config.boostingOrderBy == "maxdown.asc")
				std::sort(sortList.begin(),sortList.end(),[](const TradeReport& l,const TradeReport& r)->bool
				{
				return l.profit[0] <= r.profit[0];
				}
				);
			else if (config.boostingOrderBy == "maxdown.desc")
				std::sort(sortList.begin(),sortList.end(),[](const TradeReport& l,const TradeReport& r)->bool
				{
				return l.profit[0] >= r.profit[0];
				}
				);
			else if (config.boostingOrderBy == "tradecount.asc")
				std::sort(sortList.begin(),sortList.end(),[](const TradeReport& l,const TradeReport& r)->bool
				{
				return l.item.size() >= r.item.size();
				}
				);
			else if (config.boostingOrderBy == "tradecount.desc")
				std::sort(sortList.begin(),sortList.end(),[](const TradeReport& l,const TradeReport& r)->bool
				{
				return l.item.size() <= r.item.size();
				}
				);

			foreach(auto item,sortList)
				tab->addTab(createTab(list.size(),item,mode,baseScript),item.scriptName);
		}

		if(tab->count() == 0)
		{
			tab->deleteLater();
			box->addWidget(new QLabel("<h2><font color=red>无满足策略的股票清单</font></h2>"));
		}
		else
			box->addWidget(tab);
	}
}

void StrategyViewWidget::setStockList(StockListView* listView,const QVector<StockTradeItem>& list)
{
	listView->clearTable();
	foreach(auto item,list)
	{
	    Stock stock;
		stock.code = item.stock;
		QString name = config.stockList[item.stock].name;
		if (name.isEmpty())
			stock.name = StockMapper::instance()->get(item.stock);
		else
			stock.name = name;
		stock.data[Stock::TAG_BUY_DATE] = item.buyDate;
		stock.data[Stock::TAG_SALE_DATE] = item.saleDate;
		stock.data[Stock::TAG_BUY_PRICE] = QString("%1").arg(item.buy);
		stock.data[Stock::TAG_SALE_PRICE] = QString("%1").arg(item.sale);
		stock.data[Stock::TAG_BS_HOLDDAY_COUNT] = QString("%1").arg(item.holdDays);
		stock.data[Stock::TAG_SALE_TYPE] = QString("%1").arg(item.saleType);
		listView->addStock(stock);
    }
}

QWidget* StrategyViewWidget::createTab(int count,const TradeReport& unit,const QString& mode,const QString& baseScript)
{
	QWidget* widget = new QWidget(this);
	QVBoxLayout* box = new QVBoxLayout(widget);

	auto view = new StockProfitView(this);
	view->setFixedHeight(180);
	box->addWidget(view);
	
	auto stockList = StockListView::createStockListView(StockListViewEnum_Strategy);
	box->addWidget(stockList);
	setMinimumWidth(660);
	QVector<StockProfitVar> vec;
	StockProfitVar item;
	item.color = QPen(Qt::blue);
	item.label = QString::fromLocal8Bit("参考指数");
	item.list = dataProvider.hs300;
	vec.push_back(item);
	vec += unit.profitvar;
	view->loadData(unit.desc,vec);
	setStockList(stockList,unit.item);

    QTextEdit* edit = new QTextEdit();
	setTextEditStyle(edit);
	if (count > 1)
		edit->setText(unit.scriptContent);
	else
	{
		if(baseScript.isEmpty())
			edit->setText(unit.scriptContent);
		else
		    edit->setText(baseScript);
	}
	box->addWidget(edit);
	return widget;
}
