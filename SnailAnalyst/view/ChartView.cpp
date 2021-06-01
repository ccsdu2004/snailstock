#include "chartview.h"

ChartView::ChartView(QWidget* parent):
QChartView(parent)
{
	chart = new Chart();
	setChart(chart);
}

void ChartView::setData(const QMap<int,double>& data)
{
	//chart->setData(data);
}