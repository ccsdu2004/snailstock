#include <QBoxLayout>
#include "SnailApplication.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "dialog/SnailStockMsgBox.h"
#include "GridTradeWidget.h"

GridTradeOutput::GridTradeOutput(GridTrader* tradeInfo,const Stock& target,QWidget* parent) :
	QDialog(parent)
{
	setWindowTitle("网格交易报告");
	setMinimumWidth(740);

	auto ui = loadWidgetFromUI(":ui/gridTradeOutput.ui");
	auto box = new QVBoxLayout(this);
	box->setMargin(0);
	box->addWidget(ui);

	output = ui->findChild<QPlainTextEdit*>("output");
	table = ui->findChild<MapTableWidget*>("table");

	cvs = ui->findChild<QPushButton*>("export");
	button = ui->findChild<QPushButton*>("button");

	connect(button, &QPushButton::clicked, [&]() {close(); });
	connect(cvs, SIGNAL(clicked()), this, SLOT(exportCvs()));

	trader = tradeInfo;
	stock = target;

	QList<QPair<QString, QString>> header;
	header << QPair<QString, QString>("buyDate", "买入日期");
	header << QPair<QString, QString>("buyPrice", "买入价格");
	header << QPair<QString, QString>("saleDate", "卖出日期");
	header << QPair<QString, QString>("salePrice", "卖出价格");
	header << QPair<QString, QString>("increase", "盈利(%)");
	header << QPair<QString, QString>("lastPrice", "基准价格");

	table->initTable(header, false, false);

	QFont font;
	font.setPointSize(12);
	output->setFont(font);

	const auto config = SnailApplication::getInstance()->gridTradeConfig;

	auto items = trader->getItems();
	double profit = 0.0;
	double profitNet = 0.0;
	int holds = 0;
	int success = 0;
	foreach(auto item, items)
	{
		table->addData(item.toMap());
		if (item.saleDate.isEmpty())
		{
			holds++;
		}
		else
		{
			auto increase = GET_INCREASE(item.salePrice,item.buyPrice);
			profit += STR_CEIL(increase).toDouble();
			profitNet += STR_CEIL(increase*(1 - config.cost)).toDouble();
			success++;
		}
	}

	QString message = QString("股票标的: %1\n共完成网格交易 %2笔,毛利 %3%,净利 %4%,当前持有 %5笔")
		.arg(stock.code).arg(success).arg(profit).arg(profitNet).arg(holds);

	output->appendPlainText(message);

	message = QString("网格配置:\n交易成本:%1\n单笔买入仓位:%2\n止盈:%3\n加仓跌幅:%4")
		.arg(config.cost)
		.arg(config.firstBuyPercent)
		.arg(config.saleProfit)
		.arg(config.decreaseBuy);

	output->appendPlainText(message);
}

GridTradeOutput::~GridTradeOutput()
{
}

void GridTradeOutput::exportCvs()
{
	QString file = QString("report//%1-网格交易报告.csv").arg(stock.code);
	bool ok = exportToCSV(file,table);
	if(ok)
	{
		POP_MSGBOX("成功",QString("导出报告成功-文件路径:%1").arg(file),this)
	}
}