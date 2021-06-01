#ifndef STOCK_PBR_EVALUATION_WIDGET_H
#define STOCK_PBR_EVALUATION_WIDGET_H
#include <QWidget>
#include <QScopedPointer>

class StockValuationWidget;
class StockPBREvaluationWidgetIMPL;

class StockPBREvaluationWidget : public QWidget
{
    Q_OBJECT
public:
	StockPBREvaluationWidget(StockValuationWidget* val,QWidget* parent = nullptr);
	~StockPBREvaluationWidget();
public slots:
	void invertCheckState();
	void loadData();
	void onCompute();
	void onClickRow(int i,int j);
private:
	StockValuationWidget* base;
	QScopedPointer<StockPBREvaluationWidgetIMPL> impl;
	bool selectAll = false;
};

#endif