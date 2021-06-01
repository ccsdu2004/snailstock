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
#include "StockPEEvaluationWidget.h"
#include "common/common.h"
#include "dialog/SnailStockMsgBox.h"
#include "widget/MapTableWidget.h"

struct StockPEEvaluationWidgetIMPL
{
	QLineEdit* stockPool;
	QComboBox* computeMode;
	QPushButton* select;
	QPushButton* load;
	QPushButton* compute;
	MapTableWidget* tableWidget;
};

StockPEEvaluationWidget::StockPEEvaluationWidget(StockValuationWidget* val,QWidget* parent):
QWidget(parent),
impl(new StockPEEvaluationWidgetIMPL)
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
	impl->select = ui->findChild<QPushButton*>("select");

	QStringList items;
	items << "静态市盈率";
	items << "动态市盈率";
	items << "TTM市盈率";
	impl->computeMode->clear();
	impl->computeMode->addItems(items);

	auto completer = new QCompleter();
	completer->setFilterMode(Qt::MatchStartsWith);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	
	items = config.stockPool.keys();
	completer->setModel(new QStringListModel(items,this));
	impl->stockPool->setCompleter(completer);

	QList<QPair<QString,QString>> header;
	header << QPair<QString,QString>("code","代码");
	header << QPair<QString,QString>("name","名称");
	header << QPair<QString,QString>("trade","价格");
	header << QPair<QString,QString>("mktcap","市值");
	header << QPair<QString, QString>("per_s","静态市盈率");
	header << QPair<QString, QString>("per","动态市盈率");
	header << QPair<QString, QString>("ttm","TTM");

	impl->tableWidget->initTable(header,true);

	connect(impl->select,SIGNAL(clicked()),this,SLOT(invertCheckState()));
	connect(impl->load,SIGNAL(clicked()),this,SLOT(loadData()));
	connect(impl->tableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(onClickRow(int,int)));
	connect(impl->compute,SIGNAL(clicked()),this,SLOT(onCompute()));
}

StockPEEvaluationWidget::~StockPEEvaluationWidget()
{
}

void StockPEEvaluationWidget::invertCheckState()
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

void StockPEEvaluationWidget::onClickRow(int i,int j)
{
	j = impl->tableWidget->columnCount() - 1;
	QTableWidgetItem* checkItem = impl->tableWidget->item(i,j);
	if(checkItem->checkState() == Qt::Unchecked)
		checkItem->setCheckState(Qt::Unchecked);
	else
		checkItem->setCheckState(Qt::Checked);
}
	
void StockPEEvaluationWidget::loadData()
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

void StockPEEvaluationWidget::onCompute()
{
	double value = 0.0;
	int colIndex = impl->computeMode->currentIndex() + 4;

	QString key;
	if (colIndex == 4)
		key = "per_s";
	else if (colIndex == 5)
		key = "per";
	else
		key = "ttm";

	QStringList refs;
	auto str = base->evaluteValue(impl->tableWidget,colIndex,refs,value);
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

		if(!item.contains(key))
		{
			POP_MSGBOX("错误", QString("无%1项数据").arg(key), this)
			return;
		}

		double pb = item[key].toDouble();

		double profitPerStock = price/pb;
		value *= profitPerStock;

		QString content = QString(EVALUATION_STR).arg("相对市盈率估值")
			.arg(impl->computeMode->currentText())
			.arg(unicodeToUtf8(item["name"])).arg(item["code"]).arg(STR_CEIL(value)).arg(refs.join(","));

		OutputWidget outputWidget(content,this);
		outputWidget.setWindowTitle(QString("%1 - 相对市盈率估值报告").arg(unicodeToUtf8(item["name"])));
		outputWidget.exec();
	}
	else
	{
		POP_MSGBOX("错误",str,this)
	}
}
