#ifndef SNALL_STOCK_CHART_H
#define SNAIL_STOCK_CHART_H
#include <qmap.h>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

class Chart : public QChart
{
	Q_OBJECT
public:
	explicit Chart(QGraphicsItem* parent = 0,Qt::WindowFlags wFlags = 0);
	~Chart();
    
	void setData(const QMap<int,double>& data);
private:
	QLineSeries* series;
};

#endif