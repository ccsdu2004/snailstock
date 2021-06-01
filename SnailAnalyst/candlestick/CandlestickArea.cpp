#include <qdatetime.h>
#include <QtCharts/qcandlestickset.h>
#include "dialog/CandlestickViewDialog.h"
#include <iostream>

CanlestickArea::CanlestickArea(QWidget* parent):
QChartView(parent)
{
	setRenderHint(QPainter::Antialiasing);
	chart = new QChart();
	chart->legend()->setVisible(true);
	chart->legend()->setAlignment(Qt::AlignBottom);

	series = new QCandlestickSeries(this);
	series->setIncreasingColor(QColor(Qt::green));
	series->setDecreasingColor(QColor(Qt::red));
	setChart(chart);
}

CanlestickArea::~CanlestickArea()
{
}
 
void CanlestickArea::setStocks(const QVector<Stock>& stocks)
{
	int i = 0;
	foreach(auto item,stocks)
	{
		//QDateTime date = QDateTime::fromString(item.date,"yyyy-MM-dd");
		QCandlestickSet* unit = new QCandlestickSet(i++/*date.toTime_t()*/,series);
		unit->setOpen(item.open);
		unit->setHigh(item.top);
		unit->setLow(item.low);
		unit->setClose(item.current);
		series->append(unit);
	}
	//bindStockDataToCandlestickSeries(series,stocks);
	chart->createDefaultAxes();

	std::cout << "set stock" << std::endl;
}