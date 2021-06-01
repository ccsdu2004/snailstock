#include <numeric>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/qdatetimeaxis.h>
#include <QGraphicsLayout>
#include "StockProfitView.h"
#include "ConfigurationParser.h"
#include "common/FileUtils.h"

QT_CHARTS_USE_NAMESPACE

class StockProfitViewPriv
{
public:
	StockProfitViewPriv(QBoxLayout* box)
	{
		chart = new QChart();
		auto theme = static_cast<QChart::ChartTheme>(config.candlestickTheme);
		chart->setTheme(theme);
		chart->setBackgroundRoundness(0);
		chart->layout()->setContentsMargins(0, 0, 0, 0);
		chart->legend()->hide();

		axisX = new QDateTimeAxis;
		axisX->setTickCount(12);
		axisX->setFormat("yyyy-MM-dd");
		chart->addAxis(axisX,Qt::AlignBottom);

		yAxis = new QValueAxis();
		yAxis->setTickCount(3);
		yAxis->setVisible(true);
		//yAxis->setLabelFormat("%i");
		chart->addAxis(yAxis,Qt::AlignLeft);

		view = new QChartView(chart);
		view->setRenderHint(QPainter::Antialiasing);
		box->addWidget(view);
	}

	void setTitle(const QString title)
	{
		chart->setTitle(title);
	}

	void resetAxis()
	{
		yAxis->setMax(yMax * 1.1);
		yAxis->setMin(yMin * 0.9);
	}

	QChart* chart;
	QChartView* view;
	QDateTimeAxis* axisX;
	QValueAxis* yAxis;
	double yMin, yMax;
};

StockProfitView::StockProfitView(QWidget* parent):
QWidget(parent)
{
	QVBoxLayout* box = new QVBoxLayout(this);
	box->setSpacing(0);
	box->setMargin(0);
	priv = new StockProfitViewPriv(box);
}

StockProfitView::~StockProfitView()
{
	delete priv;
}

void StockProfitView::loadData(const StockProfitVar& profit)
{
	QLineSeries* series = new QLineSeries();
	series->setPen(profit.color);

	int i = 0;
	auto itr = profit.list.begin();
	while(itr != profit.list.end())
	{   		
		series->append(i,itr.value());
		priv->yMin = std::min(itr.value(),priv->yMin);
		priv->yMax = std::max(itr.value(),priv->yMax);

		i++;
		itr ++;
    }

	priv->chart->addSeries(series);
	series->attachAxis(priv->yAxis);
}

void StockProfitView::loadData(const QString& title,const QVector<StockProfitVar>& list)
{
	if (list.isEmpty())
		return;

	priv->setTitle(title);
	priv->chart->removeAllSeries();

	priv->yMin = std::numeric_limits<double>::max();
	priv->yMax = std::numeric_limits<double>::min();

	QVector<QLineSeries*> series;
	foreach(auto item,list)
	{
		auto serie = new QLineSeries();
		serie->setPen(item.color);
		series.push_back(serie);
	}

	QStringList content;

	int i = 0;
	int j = 1;
	auto itr = list[0].list.begin();
	while (itr != list[0].list.end())
	{
		auto key = itr.key();
		QDateTime datetime = QDateTime::fromString(key,"yyyy-MM-dd");
		auto time = datetime.toMSecsSinceEpoch();

		series[0]->append(time,itr.value());
		priv->yMin = std::min<double>(itr.value(),priv->yMin);
		priv->yMax = std::max<double>(itr.value(),priv->yMax);

		double v1,v2;
		v1 = 0;
		v2 = 0;

		bool flag = false;

		if (list[1].list.find(itr.key()) != list[1].list.end())
		{
			v1 = list[1].list[itr.key()];
			series[1]->append(time,v1);
			priv->yMin = std::min<double>(v1,priv->yMin);
			priv->yMax = std::max<double>(v1,priv->yMax);
			flag = true;
		}

		if (list[2].list.find(itr.key()) != list[2].list.end())
		{
			v2 = list[2].list[itr.key()];
			series[2]->append(time,v2);
			priv->yMin = std::min<double>(v2,priv->yMin);
			priv->yMax = std::max<double>(v2,priv->yMax);
			flag = !flag;
		}

		if((!flag) && !isSameValue(v1,0) && !isSameValue(v2,0))
		    content += QString("%1,%2,%3").arg(key).arg(v1).arg(v2);

		i ++;
		itr ++;
	}

	saveToFile("profittable.csv",content.join("\n").toLatin1());

	foreach(auto item,series)
	{
		priv->chart->addSeries(item);
	    item->attachAxis(priv->axisX);
		item->attachAxis(priv->yAxis);
    }
    priv->resetAxis();
}
