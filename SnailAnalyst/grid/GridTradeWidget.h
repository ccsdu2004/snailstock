#ifndef GRID_TRADE_WIDGET_DIALOG
#define GRID_TRADE_WIDGET_DIALOG
#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QTabWidget>
#include <QVector>
#include <QPlainTextEdit>
#include <QMap>
#include "Stock.h"
#include "common/common.h"
#include "widget/MapTableWidget.h"

struct GridTradeConfig
{
	static QString configFile;

	double cost = 0.008;
	double firstBuyPercent = 0.05;
	double saleProfit = 0.05;
	double decreaseBuy = 0.05;
	bool buyInOpen = true;
	bool saleInClose = true;

	bool loadFromFile(const QString& file);
	bool save(const QString& file);
};

class GridTradeConfigDialog : public QDialog
{
    Q_OBJECT
public:
	GridTradeConfigDialog(QWidget* parent = nullptr);
	~GridTradeConfigDialog();
public slots:
	void onSave();
private:
	QDoubleSpinBox* cost;
	QDoubleSpinBox* firstBuyPercent;
	QDoubleSpinBox* saleProfit;
	QDoubleSpinBox* decreaseBuy;
	//QCheckBox* buyInOpen;
	//QCheckBox* saleInClose;
	QPushButton* button;
};

class GridTradeItemWidget : public QWidget
{
    Q_OBJECT
public:
	GridTradeItemWidget(QWidget* parent = nullptr):
	    QWidget(parent) 
	{
	}

	virtual ~GridTradeItemWidget()
	{
	}

	virtual void init(const QVector<Stock>& stocks) {}

	virtual int execute(const QVector<Stock>& stocks,int offset,QString* error = nullptr) 
	{ 
		return -1;
	}

	virtual bool check(const QVector<Stock>& stocks,int pos)
	{
		return false;
	}
};

struct GridUnit
{
	QString buyDate;
	QString saleDate;
	double buyPrice;
	double price;
	double salePrice = -1;
	double percent;
	double lastPrice;
	QMap<QString,QString> toMap();
};

class GridTrader : public QObject
{
	Q_OBJECT
public:
	GridTrader(QObject* parent = nullptr);

	void trade(int pos,const QVector<Stock>& stocks);
	QVector<GridUnit> getItems()const { return items; }
private:
	bool sale(const Stock& stock,double& price);
	int  hasHoldings();
	double holdLowBuyPrice();
	bool buy(const Stock& stock,double buyPrice,double& price);
private:
	QVector<GridUnit> items;
};

class GridTradeWidget : public QDialog
{
	Q_OBJECT
public:
	GridTradeWidget(QWidget* parent = nullptr);
	~GridTradeWidget();
public slots:
	void onConfig();
	void onAnalyze();
public:
	int getNextPosition(int pos);
	bool check(int pos);
private:
	QLineEdit* code;
	QTabWidget* tabWidget;
	QPushButton* config;
	QPushButton* button;

	QVector<GridTradeItemWidget*> tradeItemWidgets;
	GridTrader* gridTrader;

	QVector<Stock> stocks;
	GridTradeItemWidget* tradeItem;
};

class GridTradeOutput : public QDialog
{
	Q_OBJECT
public:
	GridTradeOutput(GridTrader* tradeInfo,const Stock& target,QWidget* parent = nullptr);
	~GridTradeOutput();
public slots:
	void exportCvs();
private:
	GridTrader* trader;
	Stock stock;

	MapTableWidget* table;
	QPlainTextEdit* output;
	QPushButton* cvs;
	QPushButton* button;
};

#endif
