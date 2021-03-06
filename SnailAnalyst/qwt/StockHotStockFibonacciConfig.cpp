#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QSpacerItem>
#include "StockHotFibonacciWidget.h"
#include "SnailApplication.h"
#include "ConfigFile.h"

StockHotStockFibonacciConfig::StockHotStockFibonacciConfig(QWidget* parent):
QDialog(parent)
{
	QGridLayout* grid = new QGridLayout(this);
    grid->addWidget(new QLabel("最大偏差(0.5%-6.4%)"),0,0,1,1);

	{
		deltaSlider = new QSlider();
		deltaSlider->setRange(5,64);
		deltaSlider->setOrientation(Qt::Horizontal);
		grid->addWidget(deltaSlider,0,1,1,2);
	}
    
	grid->addWidget(new QLabel("均线周期(5-25)"),1,0,1,1);

	{
		spanSlider = new QSlider();
		spanSlider->setRange(5,25);
		spanSlider->setOrientation(Qt::Horizontal);
		grid->addWidget(spanSlider,1,1,1,2);
	}

    grid->addWidget(new QLabel("次新股上市范围(20-120)"),2,0,1,1);

	{
		newStockSlider = new QSlider();
		newStockSlider->setRange(20,120);
		newStockSlider->setOrientation(Qt::Horizontal);
		grid->addWidget(newStockSlider,2,1,1,2);
	}

    QSpacerItem* spacer = new QSpacerItem(137,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
    grid->addItem(spacer,3,0,1,2);

    QPushButton* ok = new QPushButton("确定");
	connect(ok,SIGNAL(clicked()),this,SLOT(onConfig()));
    grid->addWidget(ok,3,2,1,1);

	readConfig();
}

void StockHotStockFibonacciConfig::onConfig()
{
	snail->config.fibonacciDelta = deltaSlider->value();
	snail->config.fibonacciSpan = spanSlider->value();
	snail->config.fibonacciNewStock = newStockSlider->value();
	close();
}

void StockHotStockFibonacciConfig::readConfig()
{
	deltaSlider->setValue(snail->config.fibonacciDelta);
	spanSlider->setValue(snail->config.fibonacciSpan);
	newStockSlider->setValue(snail->config.fibonacciNewStock);
}