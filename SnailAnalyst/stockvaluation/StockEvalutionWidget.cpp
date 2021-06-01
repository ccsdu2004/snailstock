#include <QBoxLayout>
#include <QTabWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDir>
#include <algorithm>
#include "dialog/SnailStockMsgBox.h"
#include "StockValuationWidget.h"
#include "ConfigurationParser.h"
#include "common/common.h"
#include "widget/MapTableWidget.h"
#include "../common/FileUtils.h"
#include "StockPBREvaluationWidget.h"
#include "StockPEEvaluationWidget.h"
#include "per/StockPerData.h"

struct StockValuationWidgetIMPL
{
	QTabWidget* tabWidget;
	QLineEdit* stockCode;
	QComboBox* dateList;
};

StockTableWidget::StockTableWidget(StockValuationWidget* val,QWidget* parent):
QWidget(parent)
{
	base = val;

	QWidget* ui = loadWidgetFromUI(":ui/stockTable.ui");
	QBoxLayout* box = new QVBoxLayout();
	box->setMargin(0);
	box->addWidget(ui);
	setLayout(box);

	auto button = ui->findChild<QPushButton*>("load");
    tableWidget = ui->findChild<MapTableWidget*>("tableWidget");
	
	QList<QPair<QString, QString>> header;
	header << QPair<QString, QString>("code","代码");
	header << QPair<QString, QString>("name","名称");
	header << QPair<QString, QString>("trade","价格");
	header << QPair<QString, QString>("mktcap","市值(万)");
	header << QPair<QString, QString>("per_s","静态市盈率");
	header << QPair<QString, QString>("per","动态市盈率");
	header << QPair<QString, QString>("ttm","TTM");
	header << QPair<QString, QString>("pb","市净率");
	header << QPair<QString, QString>("dyr","股息率");
	header << QPair<QString, QString>("beta","Beta值");

	tableWidget->initTable(header,false);

	connect(button,SIGNAL(clicked()),this,SLOT(loadData()));
}

StockTableWidget::~StockTableWidget()
{
}

void StockTableWidget::loadData()
{
	tableWidget->clearTable();
	auto list = base->loadStockData();
	
	auto itr = list.begin();
	while (itr != list.end())
	{
		tableWidget->addData(itr.value());
		itr++;
	}
}

StockValuationWidget::StockValuationWidget(QWidget* parent):
QDialog(parent)
{
	setWindowTitle("股票估值器");
	setMinimumWidth(960);
	
	QWidget* ui = loadWidgetFromUI(":ui/stockEvalutionWidget.ui");

	impl = new StockValuationWidgetIMPL;
	impl->dateList = ui->findChild<QComboBox*>("dateList");
	impl->stockCode = ui->findChild<QLineEdit*>("stockCode");
	impl->stockCode->setText("sh600009");
	impl->tabWidget = ui->findChild<QTabWidget*>("tabWidget");

	impl->tabWidget->removeTab(1);
	impl->tabWidget->removeTab(0);

	QBoxLayout* box = new QVBoxLayout();
	box->setMargin(0);
	box->addWidget(ui);
	setLayout(box);

	QDir dir("per");
	auto pers = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
	
	foreach(auto per,pers)
		impl->dateList->addItem(per.baseName());

	impl->dateList->setCurrentIndex(impl->dateList->count()-1);

	impl->tabWidget->addTab(new StockTableWidget(this,this),"股票数据清单");

	StockPBREvaluationWidget* pbr = new StockPBREvaluationWidget(this,this);
	impl->tabWidget->addTab(pbr,"相对市净率估值");

	StockPEEvaluationWidget* pe = new StockPEEvaluationWidget(this,this);
	impl->tabWidget->addTab(pe,"相对市盈率估值");
}

StockValuationWidget::~StockValuationWidget()
{
	delete impl;
}

QString StockValuationWidget::getStockCode()
{
	return impl->stockCode->text().trimmed();
}

QMap<QString,QMap<QString,QString>> StockValuationWidget::loadStockData()
{
    QString date = impl->dateList->currentText();
	if(date == stockDataDate)
	{
		return stockData;
	}

	StockPerData perData;
	perData.load(date);
	return stockData = perData.toMap();
}

QString StockValuationWidget::evaluteValue(MapTableWidget* table,int colIndex,QStringList& refs,double& value)
{
	if(table)
	{
		int size = table->rowCount();

		QVector<double> valueTable;
		for (int i = 0; i < size; i++)
		{
			auto code = table->item(i, 0)->text();
			auto item = table->item(i, colIndex);

			auto v = 0.0;
			if(item)
				v = item->text().toDouble();
			
			auto checked = table->item(i,table->columnCount()-1)->checkState() == Qt::Checked;

			if (code != impl->stockCode->text().trimmed() && checked)
			{
				valueTable.append(v);
			    refs += table->item(i,1)->text();
			}
		}

		if(valueTable.isEmpty())
			return QString("无相对数据组");

		std::sort(valueTable.begin(),valueTable.end());
		value = std::accumulate(valueTable.begin(),valueTable.end(),0.0)/double(valueTable.size());
		return QString();
	}
	return QString("Table为nullptr");
}