#ifndef STOCKHOTFIBOWIDGET_H
#define STOCKHOTFIBOWIDGET_H
#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QMap>
#include <QComboBox>
#include <QSpinBox>
#include <QDialog>
#include <QSlider>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include "common.h"
#include "Stock.h"
#include "indicator.h"
#include "StockThreadTarget.h"
#include "CandlestickWidget.h"

class QGroupBox;

class StockHotStockFibonacciScanner : public StockScanner
{
	Q_OBJECT
public:
	const static int TAG_HOTSTOCK_FIBONACCI_TIAOZHENGMAIRU = 0;
	const static int TAG_HOTSTOCK_FIBONACCI_SHENGPOMAIRU = 1;
	const static int TAG_HOTSTOCK_FIBONACCI_FANTANMAICHU = 2;
	const static int TAG_HOTSTOCK_FIBONACCI_DIEPOMAICHU = 3;
    static StockHotStockFibonacciScanner* createScanner(int id);
public:
	StockHotStockFibonacciScanner();
	virtual ~StockHotStockFibonacciScanner();
public:
	int type()const{return SCANNER_HOT;}
    //1 for ok
	virtual int check(const QString& code,const QVector<Stock>& list,QStringList& desc){return -1;}
public:
	CandlestickLine clstTop,clstLow,clstCur;
};

class StockHotStockFibonacciConfig : public QDialog
{
    Q_OBJECT
public:
    StockHotStockFibonacciConfig(QWidget* parent = 0);
public slots:
	void onConfig();
private:
	void readConfig();
private:
	QSlider* spanSlider;
	QSlider* deltaSlider;
	QSlider* newStockSlider;
};

class StockHotFibonacciWidget : public QWidget
{
    Q_OBJECT
public:
    StockHotFibonacciWidget(QWidget* parent = 0);
	~StockHotFibonacciWidget();
private slots:
    void onScan();
	void onClick(QListWidgetItem* item);
	void onClickRadioButton1();
	void onClickRadioButton2();
	void onClickRadioButton3();
	void onClickRadioButton4();
	void onUpdateResult(int percent,const QString& code,int flag);
	void onScannFinished();
	void onConfig();
private:
	QGroupBox* createGroupBox();
private:
	CandlestickWidget* candlestick;
	QListWidget* listWidget;
    QProgressBar* progressBar;
	QPushButton* button;

	QRadioButton* lineMode[4];

	StockThreadTarget* threadTarget;
};

#endif 