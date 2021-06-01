#ifndef STOCK_HISTORY_CANDLESTICK_VIEW_H
#define STOCK_HISTORY_CANDLESTICK_VIEW_H
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "config.h"
#ifdef SUPPORT_TICKET

#include "candlestick/StockTimeDataWidget.h"

class StockHistoryCandlestickView : public QDialog
{
    Q_OBJECT
public:
	StockHistoryCandlestickView(QWidget* parent = nullptr);
	~StockHistoryCandlestickView();
private slots:
    void onShow();
private:
	StockTimeDataWidget* view;
	QLineEdit* input;
	QPushButton* button;
};

#endif
#endif