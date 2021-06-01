#include <qboxlayout.h>
#include <qpushbutton.h>
#include <QContextMenuEvent>
#include <QtUiTools/quiloader.h>
#include "../common/FileUtils.h"
#include "CandlestickViewDialog.h"
#include "ConfigurationParser.h"
#include <iostream>

CanlestickViewButton::CanlestickViewButton(QWidget* parent):
QDialog(parent)
{
	setWindowTitle("配置K线浏览数据");
	QUiLoader loader;
	QFile file(":ui/candlestickbutton.ui");
	file.open(QFile::ReadOnly);

	QWidget* ptr = loader.load(&file);
	QPushButton* button = ptr->findChild<QPushButton*>("button");
	if (button)
		connect(button,SIGNAL(clicked()),this,SLOT(onClick()));

	edit = ptr->findChild<QLineEdit*>("stockcode");

	QBoxLayout* vbox = new QVBoxLayout();
	vbox->setMargin(0);
	vbox->addWidget(ptr);
	setLayout(vbox);
}

void CanlestickViewButton::onClick()
{
	stockCode = edit->text();
	close();
}

CandlestickViewDialog::CandlestickViewDialog(QWidget* parent):
	QDialog(parent)
{
	auto box = new QVBoxLayout(this);
	box->setMargin(0);
	box->setSpacing(0);

	chartView = new CanlestickArea(this);
	box->addWidget(chartView);

	menu = new QMenu();
    action = new QAction(this);
	action->setText("修改");

	connect(action,&QAction::triggered,this,&CandlestickViewDialog::onContextMenuClick);
}

void CandlestickViewDialog::onContextMenuClick()
{
	CanlestickViewButton view(this);
	view.exec();
	QString code = view.code();
	
    auto file = getStockFileByPathAndFormatAndCode(globalConfig.stockMarket,globalConfig.stockPath,globalConfig.stockFormat,code);
	if (!file.isFile())
		return;
	if (!file.exists())
		return;

	auto items = globalConfig.readStocksFromFile(file.absoluteFilePath());
	if (items.isEmpty())
		return;

	std::cout << qPrintable(code) << std::endl;
	chartView->setStocks(items);
}

void CandlestickViewDialog::contextMenuEvent(QContextMenuEvent* event)
{
	menu->clear();
	menu->addAction(action);
    menu->exec(QCursor::pos());
	event->accept();
}
