#include <QColor>
#include <QMargins>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include "StockTimeDataWidget.h"

/*
QCPItemText *textLabel = new QCPItemText(customPlot);
customPlot->addItem(textLabel);
textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
textLabel->position->setCoords(0.5, 0); // place position at center/top of axis rect
textLabel->setText("Text Item Demo");
textLabel->setFont(QFont(font().family(), 16)); // make font a bit larger
textLabel->setPen(QPen(Qt::black)); // show black border around text
*/

StockTimeDataWidget::StockTimeDataWidget(QWidget* parent):
QWidget(parent)
{
	customPlot = new QCustomPlot();
	//customPlot->xAxis->setRange(-1,241);
	customPlot->setBackground(QBrush(Qt::black));
	customPlot->setLocale(QLocale(QLocale::Chinese,QLocale::China));
	customPlot->axisRect()->setupFullAxesBox();
	customPlot->yAxis->setTickLabels(false);
	createKBar();

	QVBoxLayout* box = new QVBoxLayout(this);
	box->addWidget(customPlot);
	
	stockText = new QCPTextElement(customPlot);
	stockText->setTextColor(QColor(244,244,244));
	QFont font;
	font.setPointSize(12);
	stockText->setFont(font);
	stockText->setText("000000");
    customPlot->plotLayout()->insertRow(0);  
    customPlot->plotLayout()->addElement(0,0,stockText); 
}

StockTimeDataWidget::~StockTimeDataWidget()
{
}

void StockTimeDataWidget::createKBar()
{
	volumeBar = new QCPAxisRect(customPlot);
	volumeBar->setMaximumSize(QSize(QWIDGETSIZE_MAX,100));
	customPlot->plotLayout()->addElement(1,0,volumeBar);

	volumeBar->axis(QCPAxis::atBottom)->setLayer("axes");
	volumeBar->axis(QCPAxis::atBottom)->grid()->setLayer("grid");
	volumeBar->axis(QCPAxis::atBottom)->setVisible(false);
	volumeBar->axis(QCPAxis::atLeft)->setVisible(false);
	
	volumeBar->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
	volumeBar->setMargins(QMargins(0,0,0,0));

	customPlot->plotLayout()->setRowSpacing(0);

	QCPMarginGroup* group = new QCPMarginGroup(customPlot);
	customPlot->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight,group);
	volumeBar->setMarginGroup(QCP::msLeft|QCP::msRight,group);
	 
	connect(customPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),volumeBar->axis(QCPAxis::atBottom),SLOT(setRange(QCPRange)));
	connect(volumeBar->axis(QCPAxis::atBottom),SIGNAL(rangeChanged(QCPRange)),customPlot->xAxis,SLOT(setRange(QCPRange)));

	volumePos = new QCPBars(volumeBar->axis(QCPAxis::atBottom),volumeBar->axis(QCPAxis::atLeft));
	volumeNeg = new QCPBars(volumeBar->axis(QCPAxis::atBottom),volumeBar->axis(QCPAxis::atLeft));

	volumePos->setWidth(0.9);
	volumePos->setPen(Qt::NoPen);
	volumePos->setBrush(QColor(180,90,90));
	volumeNeg->setWidth(0.9);
	volumeNeg->setPen(Qt::NoPen);
	volumeNeg->setBrush(QColor(100,180,110));

	customPlot->addGraph("ma")->setPen(QPen(Qt::yellow));
	customPlot->addGraph("price")->setPen(QPen(Qt::green));
}

void StockTimeDataWidget::loadData(const QString& stock,const QString& p1,const QString& p2,const QVector<StockTimeLine>& input)
{
	code = stock;
    timeline = input;
	QString title = stock;
	//QString title = QString("%1,%2,%3").arg(stock).arg(p1).arg(p2);
	stockText->setText(title);

	customPlot->graph("ma")->clear();
    customPlot->graph("price")->clear();
	volumePos->clear();
	volumeNeg->clear();

	double pos = 0.0;
	float price = 0.0;
	foreach(auto item,timeline)
	{
		if(item.price >= price)
			volumePos->addData(pos,item.volume);
		else
			volumeNeg->addData(pos,item.volume);
		customPlot->graph("ma")->addData(pos,item.ma);
		customPlot->graph("price")->addData(pos,item.price);
		price = item.price;
	    pos = pos + 1.0;
	}

    customPlot->rescaleAxes();
    customPlot->replot();
}
