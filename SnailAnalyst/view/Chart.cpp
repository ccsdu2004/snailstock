#include <qgraphicsanchorlayout.h>
#include <QtCharts/QValueAxis>
#include <QtCharts/QAbstractAxis>
#include "chart.h"

Chart::Chart(QGraphicsItem* parent,Qt::WindowFlags wFlags):
QChart(QChart::ChartTypeCartesian,parent,wFlags)
{
	setTheme(QtCharts::QChart::ChartThemeBlueCerulean);
	setBackgroundRoundness(0);
	layout()->setContentsMargins(0,0,0,0);
	legend()->hide();
	series = new QLineSeries(this);
}

Chart::~Chart()
{
}

void Chart::setData(const QMap<int,double>& data)
{
	series->clear();
	auto itr = data.begin();
	while (itr != data.end())
	{
		series->append(itr.key(),itr.value());
		itr ++;
	}
}
