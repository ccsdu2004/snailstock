#include "CandlestickView.h"

#define BINSIZE 3600*24

CandlestickArea::CandlestickArea(QCustomPlot* plot,int row,int col):
QCPAxisRect(plot)
{
	plot->plotLayout()->addElement(row,col,this);

	setMaximumSize(QSize(QWIDGETSIZE_MAX,128));
	axis(QCPAxis::atBottom)->setLayer("axes");
	axis(QCPAxis::atBottom)->grid()->setLayer("grid");
	axis(QCPAxis::atBottom)->setVisible(false);
	axis(QCPAxis::atLeft)->setVisible(false);
	
	setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
	setMargins(QMargins(0,0,0,0));

	plot->plotLayout()->setRowSpacing(0);

	QCPMarginGroup* group = new QCPMarginGroup(plot);
	plot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight,group);
	setMarginGroup(QCP::msLeft|QCP::msRight,group);
	 
	connect(plot->xAxis,SIGNAL(rangeChanged(QCPRange)),axis(QCPAxis::atBottom),SLOT(setRange(QCPRange)));
	connect(axis(QCPAxis::atBottom),SIGNAL(rangeChanged(QCPRange)),plot->xAxis,SLOT(setRange(QCPRange)));

	volumePos = new QCPBars(axis(QCPAxis::atBottom),axis(QCPAxis::atLeft));
	volumeNeg = new QCPBars(axis(QCPAxis::atBottom),axis(QCPAxis::atLeft));

	volumePos->setWidth(BINSIZE*0.9);
	volumePos->setPen(Qt::NoPen);
	volumePos->setBrush(QColor(180,90,90));
	volumeNeg->setWidth(BINSIZE*0.9);
	volumeNeg->setPen(Qt::NoPen);
	volumeNeg->setBrush(QColor(100,180,110));
}

CandlestickArea::~CandlestickArea()
{
}

void CandlestickArea::setStockCount(int n)
{
    count = n;
}

void CandlestickArea::setStocks(const QVector<Stock>& list)
{
	if(list.isEmpty())
		return;

	volumeNeg->clear();
	volumePos->clear();

	QVector<Stock> stocks(list);

	double offset = 0;

	int i = 0;
	foreach(Stock stock,stocks)
	{
		offset += BINSIZE;
		if(stock.current < stock.open)
            volumeNeg->addData(offset,stock.tradeVolume/100000.0f);
		else
			volumePos->addData(offset,stock.tradeVolume/100000.0f);
		i ++;
	}    
	axis(QCPAxis::atLeft)->rescale();
}