#include <QBoxLayout>
#include "SnailApplication.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "dialog/SnailStockMsgBox.h"
#include "GridTradeWidget.h"

GridTradeOutput::GridTradeOutput(GridTrader* tradeInfo,const Stock& target,QWidget* parent) :
	QDialog(parent)
{
	setWindowTitle("�����ױ���");
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
	header << QPair<QString, QString>("buyDate", "��������");
	header << QPair<QString, QString>("buyPrice", "����۸�");
	header << QPair<QString, QString>("saleDate", "��������");
	header << QPair<QString, QString>("salePrice", "�����۸�");
	header << QPair<QString, QString>("increase", "ӯ��(%)");
	header << QPair<QString, QString>("lastPrice", "��׼�۸�");

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

	QString message = QString("��Ʊ���: %1\n����������� %2��,ë�� %3%,���� %4%,��ǰ���� %5��")
		.arg(stock.code).arg(success).arg(profit).arg(profitNet).arg(holds);

	output->appendPlainText(message);

	message = QString("��������:\n���׳ɱ�:%1\n���������λ:%2\nֹӯ:%3\n�Ӳֵ���:%4")
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
	QString file = QString("report//%1-�����ױ���.csv").arg(stock.code);
	bool ok = exportToCSV(file,table);
	if(ok)
	{
		POP_MSGBOX("�ɹ�",QString("��������ɹ�-�ļ�·��:%1").arg(file),this)
	}
}