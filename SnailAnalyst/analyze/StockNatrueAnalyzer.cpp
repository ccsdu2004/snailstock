#include <algorithm>
#include <qcompleter.h> 
#include <QStringListModel>
#include <QGridLayout>
#include <QScopedPointer>
#include <qdir.h>
#include <QDebug>
#include "ObjectFactory.h"
#include "math/math.h"
#include "../Indicator.h"
#include "dialog/SnailStockMsgBox.h"
#include "interface/StockParser.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "StockMapper.h"
#include "ConfigurationParser.h"
#include "StockNatureAnalyzer.h"
#include "per/StockPerData.h"

StockNatrueAnalyzer::StockNatrueAnalyzer(QWidget* parent):
QDialog(parent)
{
	setWindowTitle("股性分析器");
    
	auto ui = loadWidgetFromUI(":/ui/stockAnalyzeWithMapTableWidget.ui");
	auto box = new QHBoxLayout(this);
	box->setMargin(0);
	box->addWidget(ui);

	stockPoolEdit = ui->findChild<QLineEdit*>("stockPoolEdit");
	auto completer = new QCompleter();
	completer->setFilterMode(Qt::MatchStartsWith);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	QStringList items = config.stockPool.keys();
	completer->setModel(new QStringListModel(items,this));
	stockPoolEdit->setCompleter(completer);

	button = ui->findChild<QPushButton*>("button");
	clearButton = ui->findChild<QPushButton*>("clearButton");
	tableWidget = ui->findChild<MapTableWidget*>("mapTableWidget");

	connect(button,SIGNAL(clicked()),this,SLOT(onAnalyze()));
	connect(clearButton,SIGNAL(clicked()),this,SLOT(onClear()));

	auto exportButton = ui->findChild<QPushButton*>("exportButton");
	connect(exportButton,SIGNAL(clicked()),this,SLOT(onExport()));

	QList<QPair<QString,QString>> header;
	header << QPair<QString, QString>("code","代码");
	header << QPair<QString, QString>("name","名称");
	header << QPair<QString, QString>("beta","Beta值");
	header << QPair<QString, QString>("p","指数相关度");
	header << QPair<QString, QString>("dyr","股息率");
	header << QPair<QString, QString>("vix","波动率");
	header << QPair<QString, QString>("swing","平均振幅");

	header << QPair<QString, QString>("<-5.0","超跌%");
	header << QPair<QString, QString>(">5.0","超涨%");

	header << QPair<QString, QString>("ma5","5日均线乖离");
	header << QPair<QString, QString>("ma15","15日均线乖离");
	header << QPair<QString, QString>("ma30","30日均线乖离");

	header << QPair<QString, QString>("vix_10","10日最大波动");
	header << QPair<QString, QString>("vix_20_down","20日最大跌幅");
	header << QPair<QString, QString>("vix_20_up","20日最大涨幅");
	header << QPair<QString, QString>("vix_30_down","30日最大跌幅");
	header << QPair<QString, QString>("vix_30_up","30日最大涨幅");
	header << QPair<QString, QString>("vix_60_down","60日最大跌幅");
	header << QPair<QString, QString>("vix_60_up","60日最大涨幅");
	header << QPair<QString, QString>("vix_increase","振幅波动");

	tableWidget->initTable(header,false,true);
	setMinimumWidth(540);

	thread = new StockAnalezeThread(this);
	connect(thread,SIGNAL(updateData(const QString&)),this,SLOT(writeData(const QString&)));
}

StockNatrueAnalyzer::~StockNatrueAnalyzer()
{
}

void StockNatrueAnalyzer::insert(const QString& code,const QMap<QString,QString>& unit)
{
    data.insert(code,unit);
}

void StockNatrueAnalyzer::onClear()
{
	data.clear();
	tableWidget->clearTable();
}

void StockNatrueAnalyzer::onAnalyze()
{
	if(thread->isRunning())
		return;

	if(per.isEmpty())
	{
		auto dir = QDir("per");
		{
			auto files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
			if(files.isEmpty())
			{
				POP_MSGBOX(QString("警告"), QString("缺少per数据,无法分析"), this);
				return;
			}

			std::sort(files.begin(), files.end(), [](const QFileInfo& l, const QFileInfo& r)
			{
				return l.lastModified() > r.lastModified();
			}
			);

			auto fileInfo = files.at(0);
			StockPerData perData;
			perData.load(fileInfo.baseName());
			per = perData.toMap();
		}
	}

	if(per.isEmpty())
	{
		POP_MSGBOX(QString("警告"), QString("缺少per数据,无法分析"),this);
		return;
	}

	if(ref.isEmpty())
	{
		QScopedPointer<StockParser> stockParser(ObjectFactory<StockParser,QString>::create("tdx"));
		ref = stockParser->parseContent("data/sh000001");
	}

	if (ref.isEmpty())
	{
		POP_MSGBOX(QString("警告"), QString("缺少上证指数数据文件,无法分析"), this);
		return;
	}

	auto input = stockPoolEdit->text();
	QStringList items;
	if(input.isEmpty())
		items = per.keys();
	else
		items = getStocksByInput(input);

	thread->setData(items,ref,per);
	thread->start();
}

void StockNatrueAnalyzer::writeData(const QString& code)
{
	tableWidget->addData(data[code]);
}

void StockNatrueAnalyzer::onExport()
{
    bool ok = exportToCSV("report/natrue.csv",tableWidget);
	if(ok)
	{
		POP_MSGBOX("成功","导出文件到report/natrue.csv",this);
	}
}

StockAnalezeThread::StockAnalezeThread(QObject* parent):
QThread(parent),
stockParser(ObjectFactory<StockParser,QString>::create("tdx"))
{
}

StockAnalezeThread::~StockAnalezeThread()
{
}

void StockAnalezeThread::setData(const QStringList& items,const QVector<Stock>& refs,const QMap<QString,QMap<QString,QString>>& pers)
{
	stocks = items;
	ref = refs;
	per = pers;
}

void StockAnalezeThread::run()
{
	foreach(auto item,stocks)
	{
		processInThread(item);
	}
}

void StockAnalezeThread::processInThread(const QString& code)
{
	if(code == "sh000001")
		return;

	QString file = QString("data//%1").arg(code);
	if(!QFile::exists(file))
		return;

	auto items = stockParser->parseContent(file);
	if(items.size() < 120)
		return;

	QMap<QString,QString> unit;
	unit["code"] = code;
	unit["name"] = StockMapper::instance()->get(code);
	
	double cor = pearsonStock(ref,items);
	unit["p"] = QString("%1").arg(cor*100.0);

	if(per.contains(code) && per[code].contains("beta"))
		unit["beta"] = per[code]["beta"];
	else
		unit["beta"] = "0.0";

	if (per.contains(code) && per[code].contains("dyr"))
		unit["dyr"] = per[code]["dyr"];
	else
		unit["dyr"] = "0.0";

	double swing = 0.0;
	foreach(auto item,items)
	{
		swing += 100.0*(item.top - item.low) / item.getPriceCenter();
	}

	swing /= (double)items.size();
	//平均振幅
	unit["swing"] = QString("%1").arg(swing);

	double minPrice = std::numeric_limits<double>::max();
	double maxPrice = std::numeric_limits<double>::min();
	
	int increase5 = 0;
	int decrease5 = 0;
	
	foreach(auto item,items)
	{
		minPrice = std::min(item.low,minPrice);
		maxPrice = std::max(item.top,maxPrice);
		
		if (item.increase > 5.0)
			increase5++;
		else if(item.increase < -5.0)
			decrease5++;
	}

	//波动率
	unit["vix"] = QString("%1").arg(GET_INCREASE(maxPrice,minPrice));
	unit["<-5.0"] = QString("%1").arg(decrease5*100.0/(double)items.size());
	unit[">5.0"] = QString("%1").arg(increase5*100.0/(double)items.size());

	MaNIndicator ma5(5),ma15(15),ma30(30);
	IndicList ima5,ima15,ima30;

	foreach(auto item,items)
	{
	    ima5.append(ma5.update(item));
		ima15.append(ma15.update(item));
		ima30.append(ma30.update(item));
	}

	double v5 = 0.0;
	int i5 = 0;

	double v15 = 0.0;
	int i15 = 0;
	
	double v30 = 0.0;
	int i30 = 0;

	for(int i = 0;i<items.size();i++)
	{
		double close = items.at(i).current;
		if(ima5[i]._1)
		{
			v5 += std::fabs(GET_INCREASE(close,ima5[i]._2));
			i5 ++;
		}
		if(ima15[i]._1)
		{
			v15 += std::fabs(GET_INCREASE(close,ima15[i]._2));
			i15 ++;
		}
		if(ima30[i]._1)
		{
			v30 += std::fabs(GET_INCREASE(close,ima30[i]._2));
			i30 ++;
		}
	}

	if(i5 == 0)
		unit["ma5"] = "-";
	else
		unit["ma5"] = QString("%1").arg(v5/(double)i5);
	
	if(i15 == 0)
		unit["ma15"] = "-";
	else
		unit["ma15"] = QString("%1").arg(v15/(double)i15);
	
	if(i30 == 0)
	    unit["ma30"] = "-";
	else
		unit["ma30"] = QString("%1").arg(v30/(double)i30);

	double vix = 0.0;
	
#define CLAC_VALUE(n,name)\
    vix = 0.0;\
	for(int i = 0;i<items.size()-n;i++)\
	{\
		double low,high;\
		bool ok = stock_getMinMax(items,i,i+n,low,high,false);\
        if(ok){\
	        double  v = GET_INCREASE(high,low);\
		    vix = std::max<double>(vix,v);\
        }\
	}\
	unit[name] = QString("%1").arg(vix);

#define CLAC_VALUE_DOWN(n,name)\
   {\
    vix = 0.0;bool seq = true;\
	for (int i = 0; i < items.size() - n; i++)\
	{\
		double low, high; \
		bool ok = stock_getMinMax(items, i, i + n, low, high, true,&seq); \
		if (ok && !seq) {\
			\
				double  v = GET_INCREASE(high, low); \
				vix = std::max<double>(vix, v); \
		}\
	}\
		unit[name] = QString("%1").arg(vix); \
	}

#define CLAC_VALUE_UP(n,name)\
    {vix = 0.0;bool seq = true;\
	for(int i = 0;i<items.size()-n;i++)\
	{\
		double low,high;\
		bool ok = stock_getMinMax(items,i,i+n,low,high,true,&seq);\
        if(ok && seq){\
	        double  v = GET_INCREASE(high,low);\
		    vix = std::max<double>(vix,v);\
        }\
	}\
	unit[name] = QString("%1").arg(vix);}

	CLAC_VALUE(10, "vix_10")
	CLAC_VALUE_DOWN(20, "vix_20_down")
	CLAC_VALUE_DOWN(30, "vix_30_down")
	CLAC_VALUE_DOWN(60, "vix_60_down")

	CLAC_VALUE_UP(20, "vix_20_up")
	CLAC_VALUE_UP(30, "vix_30_up")
	CLAC_VALUE_UP(60, "vix_60_up")

	//vix_increase
	QVector<double> increases;
	double sum = 0.0;
	
	for (int i = 1; i < items.size(); i++)
	{
		sum += items[i].increase;
		increases += items[i].increase;
	}

	double avg = sum/(double)items.size();
	sum = 0.0;

	for(int i = 0; i < increases.size(); i++)
		sum += (increases[i] - avg)*(increases[i] - avg);

	sum /= (double)increases.size();
	double vix_increase = sqrt(sum);
	unit["vix_increase"] = QString("%1").arg(vix_increase);

	StockNatrueAnalyzer* target = dynamic_cast<StockNatrueAnalyzer*>(parent());
	target->insert(code,unit);
	emit updateData(code);
}