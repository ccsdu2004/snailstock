#ifndef STOCK_PE_EVALUATION_WIDGET_H
#define STOCK_PE_EVALUATION_WIDGET_H
#include <QWidget>
#include <QScopedPointer>

class StockValuationWidget;
class StockPEEvaluationWidgetIMPL;

class StockPEEvaluationWidget : public QWidget
{
    Q_OBJECT
public:
	StockPEEvaluationWidget(StockValuationWidget* val,QWidget* parent = nullptr);
	~StockPEEvaluationWidget();
public slots:
	void invertCheckState();
	void loadData();
	void onCompute();
	void onClickRow(int i,int j);
private:
	StockValuationWidget* base;
	QScopedPointer<StockPEEvaluationWidgetIMPL> impl;
	bool selectAll = false;
};

#endif