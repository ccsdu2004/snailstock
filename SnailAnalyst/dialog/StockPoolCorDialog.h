#ifndef STOCK_POOL_COR_DIALOG
#define STOCK_POOL_COR_DIALOG
#include <qdialog.h>
#include <qspinbox.h>
#include <qthread.h>
#include <qpushbutton.h>
#include <qprogressbar.h>
#include <qtablewidget.h>
#include <QtCharts/QChart.h>
#include <QtCharts/qchartview.h>
#include <QtCharts/QLineSeries>
#include <QtCharts/qdatetimeaxis.h>
#include "SnailAddonHost.h"
#include "view/ChartView.h"

#pragma execution_character_set("utf-8")
QT_CHARTS_USE_NAMESPACE

class QLineEdit;

class StockPoolCorThread : public QThread
{
    Q_OBJECT
public:
	StockPoolCorThread(QObject* parent):
		QThread(parent),flag(true)
	{
	}

	void setData(const QVector<QPair<QString,QString>>& items,bool one)
	{
		list = items; 
		firstIsSame = one; 
	}
	
	void run();
	void stop(){ flag = false; }
signals:
	void finish();
	void value(int percent,const QString& stock1,const QString& stock2,double cor,double r);
private:
	bool flag;
	bool firstIsSame;
	QVector<QPair<QString,QString>> list;
};

class StockCorChart : public QWidget
{
    Q_OBJECT
public:
	StockCorChart(QWidget* parent = nullptr);
	void setData(const QVector<Stock>& stock1,const QVector<Stock>& stock2);
private:
	QChartView* chartView1;
	QChart* chart1;

	QLineSeries* series1;
	QLineSeries* series2;

	QDateTimeAxis* axisX;
	QChartView* chartView2;
	QChart* chart2;

	QLineSeries* series3;
	QLineSeries* series4;

};

class StockPoolCorDialog : public QDialog
{
	Q_OBJECT
public:
	StockPoolCorDialog(QWidget* parent = nullptr);
	~StockPoolCorDialog();
private slots:
	void onButtonClick();
	void onValue(int percent,const QString& stock1,const QString& stock2,double cor,double r);
	void onThreadFinished();
	void onTableClick(int r,int c);
private:
	void initTable();
private:
	QLineEdit* basicStock;
	QLineEdit* stockPool;
	QDoubleSpinBox* minValue;
	QDoubleSpinBox* maxValue;
	QPushButton* button;
	QProgressBar* progressBar;
	QTableWidget* table;
	StockCorChart* corChart;

	StockPoolCorThread* asyncThread;
};

#endif