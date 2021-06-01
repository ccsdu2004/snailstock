#include <QtUiTools/quiloader.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <cassert>
#include <qboxlayout.h>
#include "math/math.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "StockCorDialog.h"
#include "ConfigurationParser.h"
#include "interface/StockParser.h"

StockCorDialog::StockCorDialog(QWidget* parent):
QDialog(parent)
{
	setWindowTitle("股票相关度");
	setFixedSize(210,148);

	QUiLoader loader;
	QFile file(":ui/stockcor.ui");
	file.open(QFile::ReadOnly);
	auto widget = loader.load(&file,this);

	QPushButton* button = widget->findChild<QPushButton*>("button");
	stock1 = widget->findChild<QLineEdit*>("stock1");
	stock2 = widget->findChild<QLineEdit*>("stock2");
	value  = widget->findChild<QLineEdit*>("value");

	connect(button,SIGNAL(clicked()),this,SLOT(setValue()));

	auto box = new QVBoxLayout(this);
	box->addWidget(widget);
}

void StockCorDialog::setValue()
{
	QString code1 = stock1->text();
	QString code2 = stock2->text();
	if (code1 == code2)
		return;

	if (code1.isEmpty() || code2.isEmpty())
		return;

#ifndef SUPPORT_HK_MARKET
	if (code1.startsWith("hk") || code1.size() == 7)
		return;
	if (code2.startsWith("hk") || code2.size() == 7)
		return;
#endif

	QVector<Stock> stock1,stock2;
	auto msg = getPairTradeStockData(globalConfig.stockMarket,globalConfig.stockPath,globalConfig.stockFormat,code1,code2,stock1,stock2);
	if(!msg.isEmpty())
	{
		value->setText(msg);
		return;
	}

	if (stock1.isEmpty())
		return;

	double cor = pearsonStock(stock1,stock2);
	QString strvalue = STR_CEIL(cor*100);
	value->setText(strvalue);
}