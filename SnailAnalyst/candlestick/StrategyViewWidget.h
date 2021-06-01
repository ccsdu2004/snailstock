#ifndef STRATEGY_VIEW_WIDGET_H
#define STRATEGY_VIEW_WIDGET_H
#include <QDialog>
#include <QTabWidget>
#include "../common/common.h"
#include "candlestick/StockProfitView.h"
#include "tradewriter/StockTradeWriter.h"
#include "StockListView.h"

class StrategyViewWidget : public QDialog
{
    Q_OBJECT
public:
	StrategyViewWidget(const QVector<TradeReport>& list,const QString& mode,const QString& baseScript,QWidget* parent = nullptr);
private:
	QWidget* createTab(int count,const TradeReport& unit,const QString& mode,const QString& baseScript);
	void setStockList(StockListView* listView,const QVector<StockTradeItem>& list);

	QTabWidget* tab;
};

#endif 