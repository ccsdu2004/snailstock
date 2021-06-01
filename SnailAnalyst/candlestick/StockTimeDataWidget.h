#ifndef STOCKTIMEDATAWIDGET_H
#define STOCKTIMEDATAWIDGET_H
#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QListWidget>
#include "QCustomPlot.h"
#include "Interface/StockParser.h"
#include "interface/StockRealTimeInfo.h"

class StockTimeDataWidget : public QWidget
{
    Q_OBJECT
public:
	StockTimeDataWidget(QWidget* parent = 0);
	~StockTimeDataWidget();
public:
	void loadData(const QString& code,const QString& p1,const QString& p2,const QVector<StockTimeLine>& stocks);
private:
	QWidget* createStockBox();
	void createKBar();
private:
    QCustomPlot* customPlot;
	QCPTextElement* stockText;
	
	QCPAxisRect* volumeBar;
    QCPBars* volumePos;
	QCPBars* volumeNeg;

	QString code;
    QVector<StockTimeLine> timeline;
};

#endif 