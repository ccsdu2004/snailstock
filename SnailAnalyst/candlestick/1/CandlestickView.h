#ifndef CANDLESTICKVIEW_H
#define CANDLESTICKVIEW_H
#include <QWidget>
#include <QSignalMapper>
#include <QScopedPointer>
#include <QLine>
#include <QColor>
#include "QCustomPlot.h"
#include "Stock.h"
#include "Indicator.h"
#include "qwt/qwt_text.h"

class CandlestickArea : public QCPAxisRect
{
    Q_OBJECT
public:
    CandlestickArea(QCustomPlot* plot,int r,int c);
	~CandlestickArea();
public:
	void setStockCount(int n);
	void setStocks(const QVector<Stock>& stocks);
private:
    QCPBars* volumePos;
	QCPBars* volumeNeg;
	int count;
	QVector<Stock> stocks;
};

class CandlestickView : public QWidget
{
    Q_OBJECT
public:
	CandlestickView(QWidget* paernt = 0);
	~CandlestickView();
public:
	void setText(const QString& text,bool rich = false);
	void setTitle(const QString& title);
	void updateChart(int count,const QString& code,int offset,const QVector<Stock>& stocks);
private slots:
	void onMouseClick(QMouseEvent* event);
private:
	void createCandlestick();
	void createMA();
	void createTicker();
	void updateAuxData(const QVector<Stock>& stocks);
private:
	QCustomPlot* customPlot;
    QCPTextElement* titleElement;
	QwtText* qwtText;
	QCPFinancial* candlesticks;

	QString code;
	QScopedPointer<MaNIndicator> ma5;
	QScopedPointer<MaNIndicator> ma10;
	QScopedPointer<MaNIndicator> ma20;

    CandlestickArea* vol;
};

#endif 