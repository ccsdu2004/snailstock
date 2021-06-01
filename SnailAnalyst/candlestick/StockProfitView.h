#ifndef STOCKPROFIT_VIEW_H
#define STOCKPROFIT_VIEW_H
#include <QWidget>
#include <QMap>
#include <QVector>
#include "../tradewriter/StockTradeWriter.h"

class StockProfitViewPriv;

class StockProfitView : public QWidget
{
    Q_OBJECT
public:
	StockProfitView(QWidget* parent = 0);
	~StockProfitView();
public:
	void loadData(const QString& title,const QVector<StockProfitVar>& list);
private:
	void loadData(const StockProfitVar& profit);
private:
	StockProfitViewPriv* priv;
};

#endif 