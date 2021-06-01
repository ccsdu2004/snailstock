#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QBoxLayout>
#include <QCompleter>
#include <QStringListModel>
#include "widget/OutputWidget.h"
#include "StockValuationWidget.h"
#include "ConfigurationParser.h"
#include "StockPBREvaluationWidget.h"
#include "common/common.h"
#include "dialog/SnailStockMsgBox.h"
#include "widget/MapTableWidget.h"

struct StockPBREvaluationWidgetIMPL
{
	QLineEdit* stockPool;
	QComboBox* computeMode;
	QPushButton* select;
	QPushButton* load;
	QPushButton* compute;
	MapTableWidget* tableWidget;
};

StockPBREvaluationWidget::StockPBREvaluationWidget(StockValuationWidget* val,QWidget* parent):
QWidget(parent),
impl(new StockPBREvaluationWidgetIMPL)
{
	base = val;

	QWidget* ui = loadWidgetFromUI(":ui/pb.ui");
	QBoxLayout* box = new QVBoxLayout();
	box->setMargin(0);
	box->addWidget(ui);
	setLayout(box);

	impl->stockPool = ui->findChild<QLineEdit*>("stockPool");
	impl->compute = ui->findChild<QPushButton*>("compute");
	impl->load = ui->findChild<QPushButton*>("load");
	impl->tableWidget = ui->findChild<MapTableWidget*>("tableWidget");
	impl->computeMode = ui->findChild<QComboBox*>("computeMode");
	impl->computeMode->clear();
	impl->computeMode->addItem("均值");
	impl->select = ui->findChild<QPushButton*>("select");

	auto completer = new QCompleter();
	completer->setFilterMode(Qt::MatchStartsWith);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	QStringList items = config.stockPool.keys();
	completer->setModel(new QStringListModel(items,this));
	impl->stockPool->setCompleter(completer);

	QList<QPair<QString,QString>> header;
	header << QPair<QString,QString>("code","代码");
	header << QPair<QString,QString>("name","名称");
	header << QPair<QString,QString>("trade","价格");
	header << QPair<QString,QString>("mktcap","市值");
	header << QPair<QString,QString>("pb","市净率");

	impl->tableWidget->initTable(header,true);

	connect(impl->select,SIGNAL(clicked()),this,SLOT(invertCheckState()));
	connect(impl->load,SIGNAL(clicked()),this,SLOT(loadData()));
	connect(impl->tableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(onClickRow(int,int)));
	connect(impl->compute,SIGNAL(clicked()),this,SLOT(onCompute()));
}

StockPBREvaluationWidget::~StockPBREvaluationWidget()
{
}

void StockPBREvaluationWidget::invertCheckState()
{
	selectAll = !selectAll;
	if (selectAll)
		impl->select->setText("全不选");
	else
		impl->select->setText("全选");

	int colIndex = impl->tableWidget->columnCount() - 1;

	for(int i = 0; i < impl->tableWidget->rowCount();i++)
	{
		QTableWidgetItem* checkItem = impl->tableWidget->item(i,colIndex);
		if(checkItem)
			checkItem->setCheckState(selectAll ? Qt::Checked : Qt::Unchecked);
	}
}

void StockPBREvaluationWidget::onClickRow(int i,int j)
{
	j = impl->tableWidget->columnCount() - 1;
	QTableWidgetItem* checkItem = impl->tableWidget->item(i,j);
	if(checkItem->checkState() == Qt::Unchecked)
		checkItem->setCheckState(Qt::Unchecked);
	else
		checkItem->setCheckState(Qt::Checked);
}
	
void StockPBREvaluationWidget::loadData()
{
	impl->tableWidget->clearTable();

	auto code = base->getStockCode();
	auto list = base->loadStockData();
	auto pool = impl->stockPool->text().trimmed();

	QStringList stocks;
	if(!pool.isEmpty())
	{
		if (!config.stockPool.contains(pool))
		{
			POP_MSGBOX("警告", QString("%1股票池不存在").arg(pool),this)
			return;
		}

		stocks = config.stockPool[pool].items;
		stocks.append(code);

		auto items(list);
		list.clear();

		foreach(auto stock,stocks)
			if(!stock.isEmpty())
			    list.insert(stock,items[stock]);
	}

	auto itr = list.begin();
	while(itr != list.end())
	{
		impl->tableWidget->addData(itr.value());
		itr++;
	}
}

void StockPBREvaluationWidget::onCompute()
{
	double value = 0.0;

	QStringList refs;
	auto str = base->evaluteValue(impl->tableWidget,4,refs,value);
	if(str.isEmpty())
	{
		auto code = base->getStockCode();
		auto list = base->loadStockData();

		if(!list.contains(code))
		{
			POP_MSGBOX("错误", "指定股票无数据", this)
			return;
		}

		auto item = list[code];
		double price = item["trade"].toDouble();
		double pb = item["pb"].toDouble();

		double netValue = price/pb;
		value = netValue * value;

		QString content = QString(EVALUATION_STR).arg("相对市净率估值")
			.arg(impl->computeMode->currentText())
			.arg(unicodeToUtf8(item["name"])).arg(item["code"]).arg(STR_CEIL(value)).arg(refs.join(","));

		OutputWidget outputWidget(content,this);
		outputWidget.setWindowTitle(QString("%1 - 相对市净率估值报告").arg(unicodeToUtf8(item["name"])));
		outputWidget.exec();
	}
	else
	{
		POP_MSGBOX("错误",str,this)
	}
}