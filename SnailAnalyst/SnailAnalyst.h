#ifndef SNAILANALYST_H
#define SNAILANALYST_H
#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTimerEvent>
#include "config.h"
#include "StockThreadTarget.h"
#include "StockScriptEngine.h"
#include "common/Sigslot.h"
#include "widget/AutoTextEdit.h"

class SnailWidget : public QWidget,public SlotHolder
{
    Q_OBJECT
public:
	SnailWidget(QWidget* parent = nullptr);
	~SnailWidget();
public:
	void loadScript(const QString& script,bool run = false);
	void saveScript(const QString& script);

	void undo();
	void redo();
private slots:
    void clickButton();
	void clickView();
public slots:
    void onUpdateStock(int current,int all,int percent,const QString& code);
	void onUpdateFinished(const QString& error);
private:
	int stockFileCount;
	AutoTextEdit* edit;
	QProgressBar* progressBar;
	QLabel* stepLabel;
	QPushButton* button;
	QPushButton* view;
	QString scriptMode;
};

class SnailAnalyst : public QMainWindow
{
	Q_OBJECT
public:
	SnailAnalyst(const QString& script,QWidget* parent = 0);
	~SnailAnalyst();
private slots:
    void loadScript();
	void saveScript();
	void redo();
	void undo();
	void showStockList();
#ifdef SUPPORT_SCRIPT
	void showAddonList();
#endif 
	void showSeaScriptList();
	void about();
	void question();
	void buy();
	void help();
	void commandList();
	void loadStockListEdit();
	void edit();
#ifdef SUPPORT_BASIC
	void lookCandlestick();
#endif
#ifdef SUPPORT_UPDATE_STOCK
	void updateStockData();
#endif
#ifdef SUPPORT_TICKET
	void viewHistoryRealTime();
#endif
#ifdef SUPPORT_LEVEL2
	void viewStockLevel2();
#endif
	void viewStockCor();
#ifdef SUPPORT_PAIRS_TRADE
	void viewStockPoolCor();
#endif
#ifdef SUPPORT_VIEW_K
	void viewCandlestick();
#endif
#ifdef SUPPORT_STOCK_VALUATION
	void viewStockValuation();
#endif
#ifdef SUPPORT_STOCK_ANALYZE
	void viewStockAnalyze();
#endif
#ifdef SUPPORT_UPDATE_FINANCE_DATA
	void viewStockFinanceDownload();
#endif
#ifdef SUPPORT_STOCK_GRID_TRADE
	void viewGridTrade();
#endif
private:
	void loadScriptFileFromInputFile(const QString& script);
private:
	SnailWidget* widget;
};

#endif // SNAILANALYST_H
