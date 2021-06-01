#ifndef CANDLESTICK_VIEW_DIALOG_H
#define CANDLESTICK_VIEW_DIALOG_H
#include <qdialog.h>
#include <qmenu.h>
#include <qaction.h>
#include <QtCharts/QChart.h>
#include <QtCharts/qchartview.h>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QCandlestickSeries>
#include <qlineedit>
#include "config.h"
#include "Stock.h"

using namespace QtCharts;

class CanlestickArea : public QChartView
{
	Q_OBJECT
public:
	CanlestickArea(QWidget* parent = nullptr);
	~CanlestickArea();

	void setStocks(const QVector<Stock>& stocks);
private:
	QChart* chart;
	QCandlestickSeries* series;
};

class CanlestickViewButton : public QDialog
{
    Q_OBJECT
public:
	CanlestickViewButton(QWidget* parent = nullptr);

	QString code()const{ return stockCode; }
private slots:
    void onClick();
private:
	QLineEdit* edit;
	QString stockCode;
};

class CandlestickViewDialog : public QDialog
{
    Q_OBJECT
public:
	CandlestickViewDialog(QWidget* parent = nullptr);

	void contextMenuEvent(QContextMenuEvent* event);
private slots:
    void onContextMenuClick();
private:
	QMenu* menu;
	QAction* action;
	CanlestickArea* chartView;
};

#endif