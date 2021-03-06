#include <QVBoxLayout>
#include <QIcon>
#include <QDateTime>
#include <QDate>
#include <QBrush>
#include "SnailApplication.h"
#include "CandlestickView.h"
#include "Indicator.h"
#include "qwt/qwt_text.h"

#define BINSIZE 3600*24

CandlestickView::CandlestickView(QWidget* parent):
QWidget(parent),
ma5(new MaNIndicator(5)),
ma10(new MaNIndicator(10)),
ma20(new MaNIndicator(20))
{
	setWindowTitle("股票日K");
	setWindowIcon(QIcon(":logo/snail.png"));
    
	QVBoxLayout* box = new QVBoxLayout();
	setLayout(box);
    customPlot = new QCustomPlot();
	customPlot->setBackground(QBrush(Qt::black));
	customPlot->setLocale(QLocale(QLocale::Chinese,QLocale::China));
	customPlot->axisRect()->setupFullAxesBox();
	box->addWidget(customPlot);

	titleElement = new QCPTextElement(customPlot);
	titleElement->setTextColor(QColor(244,244,244));
	QFont font;
	font.setPointSize(12);
	titleElement->setFont(font);
	titleElement->setText("---");
    customPlot->plotLayout()->insertRow(0);  
    customPlot->plotLayout()->addElement(0,0,titleElement); 

    createCandlestick();
	createMA();

	vol = new CandlestickArea(customPlot,2,0);

	customPlot->rescaleAxes();
	customPlot->xAxis->scaleRange(1.025,customPlot->xAxis->range().center());
	customPlot->yAxis->scaleRange(1.1,customPlot->yAxis->range().center());

	connect(customPlot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(onMouseClick(QMouseEvent*)));

	qwtText = new QwtText(customPlot);
	qwtText->setAntialiased(true);
	qwtText->setColor(QColor(184,26,26));
	font.setPointSize(15);
	qwtText->setFont(font);
}

CandlestickView::~CandlestickView()
{
	delete vol;
}

void CandlestickView::setText(const QString& text,bool rich)
{
	qwtText->setText(text,rich ? QwtText::RichText : QwtText::AutoText);
}

void CandlestickView::setTitle(const QString& title)
{
	titleElement->setText(title);
}

void CandlestickView::createCandlestick()
{
	candlesticks = new QCPFinancial(customPlot->xAxis,customPlot->yAxis);
	candlesticks->setName("Candlestick");
	candlesticks->setChartStyle(QCPFinancial::csCandlestick);
	candlesticks->setWidth(BINSIZE*0.9);
	candlesticks->setTwoColored(true);
	candlesticks->setBrushPositive(QColor(180,90,90));
	candlesticks->setBrushNegative(QColor(100,180,110));
	candlesticks->setPenPositive(QPen(QColor(180,90,90)));
	candlesticks->setPenNegative(QPen(QColor(100,180,110)));
	customPlot->yAxis->setTickLabels(false);
}

void CandlestickView::createMA()
{
	customPlot->addGraph("ma5")->setPen(QPen(Qt::yellow));
	customPlot->addGraph("ma10")->setPen(QPen(Qt::cyan));
	customPlot->addGraph("ma20")->setPen(QPen(Qt::gray));
}

void CandlestickView::onMouseClick(QMouseEvent* event)
{
	event->accept();
}

void CandlestickView::updateChart(int count,const QString& code,int offset,const QVector<Stock>& stocks)
{
	if(code.isEmpty() || stocks.isEmpty())
		return;

	customPlot->xAxis->setRange(0,BINSIZE*(count+1));

	customPlot->graph("ma5")->clear();
	customPlot->graph("ma10")->clear();
	customPlot->graph("ma20")->clear();

	ma5->clear();
	ma10->clear();
	ma20->clear();

	bool init = false;
	auto list = stocks;

	QVector<DataWrapper5<bool,float,float,float,float>> m5ret,m10ret,m20ret;
	foreach(Stock stock,list)
	{
		m5ret.append(ma5->update(stock));
		m10ret.append(ma10->update(stock));
		m20ret.append(ma20->update(stock));
	}

	int span = list.size() > offset ? offset : 0;
	list = list.mid(span,count);
	
	m5ret  = m5ret.mid(span,count);
	m10ret = m10ret.mid(span,count);
	m20ret = m20ret.mid(span,count);

	int start = 0;

    QVector<double> key,open,high,low,close;
	for(int i=0;i<list.size();i++)
	{
		Stock stock = list[i];
		start += BINSIZE;
		key.append(start);
		open.append(stock.open);
		high.append(stock.top);
		low.append(stock.low);
		close.append(stock.current);

		if(m5ret[i]._1)
			customPlot->graph("ma5")->addData(start,m5ret[i]._2);
		if(m10ret[i]._1)
			customPlot->graph("ma10")->addData(start,m10ret[i]._2);
		if(m20ret[i]._1)
			customPlot->graph("ma20")->addData(start,m20ret[i]._2);
	}

	setTitle(code);

	vol->setStockCount(count);
	vol->setStocks(list);
	candlesticks->setData(key,open,high,low,close);
	customPlot->rescaleAxes(true);
    customPlot->replot();
	setWindowTitle(code);
}
 