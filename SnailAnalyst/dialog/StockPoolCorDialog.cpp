#include <QtUiTools/quiloader.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <cassert>
#include <algorithm>
#include <qboxlayout.h>
#include <qstringlistmodel.h>
#include <qcompleter.h>
#include <qgraphicsanchorlayout.h>
#include "math/math.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "widget/LineDelegate.h"
#include "StockPoolCorDialog.h"
#include "ConfigurationParser.h"
#include "interface/StockParser.h"
#include "StockMapper.h"
#include "dialog/SnailStockMsgBox.h"

#define CELL(i,value) (i = new QTableWidgetItem(QString("%1").arg(value)))
#define CELL_ORDER(i,value) (i = new OrderTableWidgetItem(QString("%1").arg(value)))

#define WIDGET_WIDTH 360

void StockPoolCorThread::run()
{
	int count = list.size();
	for (int i = 0;i < count;i++)
	{
		if (list[i].first == list[i].second)
			continue;
		int progress = i * 100 / count;
		QVector<Stock> s1,s2;
		auto msg = getPairTradeStockData(globalConfig.stockMarket,globalConfig.stockPath,globalConfig.stockFormat,list[i].first,list[i].second,s1,s2);
		if (firstIsSame && !msg.isEmpty())
			break;

		if (msg.isEmpty() && s1.size() == s2.size() && !s1.isEmpty())
		{
			double cor = pearsonStock(s1,s2);
			double avg1 = stock_mean(s1,Stock::TAG_CLOSE);
			double avg2 = stock_mean(s2,Stock::TAG_CLOSE);
			if(!std::isnan<double>(cor))
				emit value(progress,list[i].first,list[i].second,cor,avg1/avg2);
		}

		if (!flag)
			break;
	}

	emit finish();
}

StockPoolCorDialog::StockPoolCorDialog(QWidget* parent):
	QDialog(parent)
{
	setWindowTitle("股票池相关度");

	QUiLoader loader;
	QFile file(":ui/stockcorlist.ui");
	file.open(QFile::ReadOnly);
	auto widget = loader.load(&file,this);

	basicStock = widget->findChild<QLineEdit*>("basicstock");
	stockPool = widget->findChild<QLineEdit*>("stockpool");
	auto completer = new QCompleter();
	completer->setFilterMode(Qt::MatchStartsWith);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	QStringList items = config.stockPool.keys();
	completer->setModel(new QStringListModel(items,this));
	stockPool->setCompleter(completer);

	minValue = widget->findChild<QDoubleSpinBox*>("minValue");
	minValue->setValue(90.00);
	maxValue = widget->findChild<QDoubleSpinBox*>("maxValue");
	maxValue->setValue(99.99);
	progressBar = widget->findChild<QProgressBar*>("progressBar");
	progressBar->hide();
	table = widget->findChild<QTableWidget*>("table");
	
	initTable();

	button = widget->findChild<QPushButton*>("button");

	connect(button,SIGNAL(clicked()),this,SLOT(onButtonClick()));

	auto box = new QHBoxLayout(this);
	box->addWidget(widget);
	box->addWidget(corChart = new StockCorChart(this));
	corChart->hide();

	asyncThread = new StockPoolCorThread(this);
	connect(asyncThread,SIGNAL(value(int,const QString&,const QString&,double,double)),this,SLOT(onValue(int,const QString&,const QString&,double,double)));
	connect(asyncThread,SIGNAL(finish()),this,SLOT(onThreadFinished()));

	setFixedWidth(320);
}

StockPoolCorDialog::~StockPoolCorDialog()
{
	asyncThread->quit();
}

void StockPoolCorDialog::initTable()
{
	QStringList header;
	table->setColumnCount(4);
	header << "股票1" << "股票2" << "K" << "相关度";

	table->setColumnWidth(0,72);
	table->setColumnWidth(1,72);
	table->setColumnWidth(2,72);
	table->setColumnWidth(3,72);
	table->setHorizontalHeaderLabels(header);

	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);

	table->verticalHeader()->setVisible(false);
	table->horizontalHeader()->setVisible(true);

	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->setEditTriggers(QTableView::NoEditTriggers);
	table->setAlternatingRowColors(true);

	table->setShowGrid(false);
	table->setItemDelegate(new LineDelegate(table));
	table->setSortingEnabled(true);

	connect(table,SIGNAL(cellClicked(int,int)),this,SLOT(onTableClick(int,int)));
}

void StockPoolCorDialog::onButtonClick()
{
	table->clearContents();
	table->setRowCount(0);

	auto poolName = stockPool->text();
	if (poolName.isEmpty())
		return;

	auto stdstock = basicStock->text();
	QVector<QPair<QString,QString>> stockpair;

	if (poolName != "ah")
	{
		auto items = getStocksByInput(poolName);
		poolName = "___pairtrade.stockpool";
		StockPoolUnit unit;
		unit.items = items;
		unit.name = poolName;
		config.stockPool.insert(poolName,unit);

		if (!config.stockPool.contains(poolName))
		{
			POP_MSGBOX("警告",QString("%1股票池不存在").arg(poolName),this)
				return;
		}

		auto pool = config.stockPool[poolName];
		
		if (stdstock.isEmpty())
		{
			auto list = getCombinationVector(pool.items.size());
			if (list.isEmpty())
				return;
			for (auto itr = list.begin();itr != list.end();itr ++)
			{
				stockpair += QPair<QString,QString>(pool.items.at(itr->first),pool.items.at(itr->second));
			}
		}
		else
		{
			foreach(auto item,pool.items)
			{
				stockpair += QPair<QString,QString>(stdstock,item);
			}
		}
	}
	else
		stockpair = StockMapper::instance()->getAHStockList();

	corChart->hide();

	setFixedWidth(320);
	button->setEnabled(false);
	progressBar->setVisible(true);
	asyncThread->setData(stockpair,!stdstock.isEmpty());
	asyncThread->start();
}

void StockPoolCorDialog::onValue(int percent,const QString& stock1,const QString& stock2,double cor,double r)
{
	progressBar->setValue(percent);

	if (cor*100.0 < minValue->value() || cor*100.0 > maxValue->value() + 0.01)
		return;

	int row = table->rowCount();
	table->insertRow(row);

	QTableWidgetItem* i = 0;
	table->setItem(row,0,CELL(i,stock1));
	table->setItem(row,1,CELL(i,stock2));
	table->setItem(row,3,CELL_ORDER(i,STR_CEIL(100*cor)));
	table->setItem(row,2,CELL(i,STR_CEIL(r)));
}

void StockPoolCorDialog::onThreadFinished()
{
	progressBar->setValue(0);
	progressBar->hide();

	corChart->setVisible(true);
	setFixedWidth(760);
	button->setEnabled(true);
}

void StockPoolCorDialog::onTableClick(int r,int c)
{
	QString code1 = table->item(r,0)->text();
	QString code2 = table->item(r,1)->text();

	QVector<Stock> s1,s2;
	
	auto msg = getPairTradeStockData(globalConfig.stockMarket,globalConfig.stockPath,globalConfig.stockFormat,code1,code2,s1,s2);
	if (msg.isEmpty() && s1.size() == s2.size() && !s1.isEmpty())
	{
		double avg1 = stock_mean(s1,Stock::TAG_CLOSE);
		double avg2 = stock_mean(s2,Stock::TAG_CLOSE);
		double v = avg1 / avg2;
		std::for_each(s2.begin(),s2.end(),[v](Stock& stock){stock.current *= v;});
		corChart->setData(s1,s2);
	}
}

StockCorChart::StockCorChart(QWidget* parent)
{
	auto box = new QVBoxLayout(this);
	box->setMargin(0);
	box->setSpacing(0);

	chartView1 = new QChartView();
	chartView1->setRenderHint(QPainter::Antialiasing);

	chartView2 = new QChartView();
	chartView2->setRenderHint(QPainter::Antialiasing);

	chart1 = new QChart();
	chart1->legend()->hide();
	chart2 = new QChart();
	chart2->legend()->hide();

	auto axisX = new QDateTimeAxis;
	axisX->setTickCount(12);
	axisX->setFormat("yyyy-MM-dd");
	chart2->addAxis(axisX,Qt::AlignBottom);

	chartView1->setChart(chart1);
	chartView2->setChart(chart2);	
	
	auto theme = static_cast<QChart::ChartTheme>(config.candlestickTheme);
	chart1->setTheme(theme);
	chart1->setBackgroundRoundness(0);
	chart1->layout()->setContentsMargins(0,0,0,0);

	chart2->setTheme(theme);
	chart2->setBackgroundRoundness(0);
	chart2->layout()->setContentsMargins(0,0,0,0);

	box->addWidget(chartView1,2);
	box->addWidget(chartView2,1);

	series1 = new QLineSeries(this);
	series2 = new QLineSeries(this);
	chart1->addSeries(series1);
	chart1->addSeries(series2);

	series3 = new QLineSeries(this);
	chart2->addSeries(series3);

	series4 = new QLineSeries(this);
	chart2->addSeries(series4);

	series4->setColor(QColor(100,200,0));

	axisX = new QDateTimeAxis(this);
	axisX->setTickCount(120);
	axisX->setFormat("MM-dd");

	chart1->addAxis(axisX,Qt::AlignBottom);
	//series1->attachAxis(timeAxis);
	//series2->attachAxis(timeAxis);
	//series3->attachAxis(timeAxis);
}

void StockCorChart::setData(const QVector<Stock>& stock1,const QVector<Stock>& stock2)
{
	series1->clear();
	series2->clear();
	series3->clear();
	series4->clear();

	chart1->removeSeries(series1);
	chart1->removeSeries(series2);
	chart2->removeSeries(series3);
	chart2->removeSeries(series4);

	QDateTime date;

	int index = 0;
	foreach(auto stock,stock1)
	{
		date = QDateTime::fromString(stock.date,"yyyy-MM-dd");
		series1->append(index++,stock.current);
	}

	index = 0;

	foreach(auto stock,stock2)
	{
		date = QDateTime::fromString(stock.date,"yyyy-MM-dd");
		series2->append(index++,stock.current);
	}

	chart1->addSeries(series1);
	chart1->addSeries(series2);

	index = 0;

	for (int i = 0;i < stock1.size();i++)
	{
		date = QDateTime::fromString(stock1[i].date,"yyyy-MM-dd");
		auto time = date.toMSecsSinceEpoch();

		series3->append(index,stock1[i].current/stock2[i].current);
		series4->append(index,0.618*std::sinf(i / 10.0));
		index ++;
	}

	chart2->addSeries(series3);
	chart2->addSeries(series4);

	//chart2->addAxis(timeAxis,Qt::AlignBottom);
	//series1->attachAxis(axisX);
	//series2->attachAxis(axisX);
	//series3->attachAxis(timeAxis);
}
