#ifndef CHARTVIEW_H
#define CHARTVIEW_H
#include <QtCharts/qchartview.h>
#include <QtCharts/qchart.h>
#include "Chart.h"

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
public:
	ChartView(QWidget* parent = 0);

	void setData(const QMap<int,double>& data);
private:
	QChart* chart;
};

#endif