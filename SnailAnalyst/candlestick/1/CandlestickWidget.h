#ifndef CANDLESTICK_WDIGET_H
#define CANDLESTICK_WIDGET_H
#include <qdialog.h>
#include <qlistwidget.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include "CandlestickView.h"

class CandlestickWidget : public QDialog
{
    Q_OBJECT
public:
    CandlestickWidget(QWidget* parent = nullptr);
	~CandlestickWidget();

private:
	QListWidget* stockList;
	CandlestickView* candlestickView;
	QLineEdit* searchEdit;
	QPushButton* button;
};

#endif