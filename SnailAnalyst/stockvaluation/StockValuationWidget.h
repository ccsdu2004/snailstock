#ifndef STOCK_VALUATION_WIDGET_H
#define STOCK_VALUATION_WIDGET_H
#include <QDialog>
#include "Stock.h"

class MapTableWidget;
class StockValuationWidgetIMPL;

#pragma execution_character_set("utf-8")
#define EVALUATION_STR "���� %1 �㷨 %2\n��Ʊ: %3 ����:%4 ��ֵ�۸�Ϊ %5\n\n�ο���ƱΪ:\n%6"

class StockValuationWidget : public QDialog
{
    Q_OBJECT
public:
	StockValuationWidget(QWidget* parent = nullptr);
	~StockValuationWidget();
public:
	QString getStockCode();
	QMap<QString,QMap<QString,QString>> loadStockData();
	QString evaluteValue(MapTableWidget* table,int colIndex,QStringList& refs,double& value);
private:
	StockValuationWidgetIMPL* impl = nullptr;

	QString stockDataDate;
	QMap<QString, QMap<QString, QString>> stockData;
};

class StockTableWidget : public QWidget
{
	Q_OBJECT
public:
	StockTableWidget(StockValuationWidget* val,QWidget* parent = nullptr);
	~StockTableWidget();
public slots:
	void loadData();
private:
	StockValuationWidget* base;
	MapTableWidget* tableWidget;
};

#endif
