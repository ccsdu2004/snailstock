#include <QBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "dialog/SnailStockMsgBox.h"
#include "ObjectFactory.h"
#include "SnailApplication.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "interface/StockParser.h"
#include "GridTradeWidget.h"

QString GridTradeConfig::configFile = "config/gridTradeConfig.json";

bool GridTradeConfig::loadFromFile(const QString& file)
{
	auto buffer = readAllBufferFromFile(file);
	auto doc = QJsonDocument::fromJson(buffer.toLatin1());
	if(doc.isNull())
		return false;

	auto object = doc.object();

	cost = object["cost"].toDouble();
	firstBuyPercent = object["firstBuyPercent"].toDouble();
	saleProfit = object["saleProfit"].toDouble();
	decreaseBuy = object["decreaseBuy"].toDouble();
	buyInOpen = object["buyInOpen"].toBool();
	saleInClose = object["saleInClose"].toBool();
	return true;
}

bool GridTradeConfig::save(const QString& file)
{
	QJsonObject object;
	object.insert("cost",cost);
	object.insert("firstBuyPercent",firstBuyPercent);
	object.insert("saleProfit",saleProfit);
	object.insert("decreaseBuy",decreaseBuy);
	object.insert("buyInOpen",buyInOpen);
	object.insert("saleInClose",saleInClose);

	QJsonDocument doc;
	doc.setObject(object);
	auto json = doc.toJson(QJsonDocument::JsonFormat::Indented);
	saveToFile(file,json);
	return true;
}

GridTradeConfigDialog::GridTradeConfigDialog(QWidget* parent):
QDialog(parent)
{
	setWindowTitle("网格交易配置");
	auto box = new QVBoxLayout(this);
	box->setMargin(0);

	QWidget* ui = loadWidgetFromUI(":ui/gridTradeConfig.ui");
	box->addWidget(ui);

	cost = ui->findChild<QDoubleSpinBox*>("cost");
	firstBuyPercent = ui->findChild<QDoubleSpinBox*>("firstBuyPercent");
	saleProfit = ui->findChild<QDoubleSpinBox*>("saleProfit");
	decreaseBuy = ui->findChild<QDoubleSpinBox*>("decreaseBuy");

	//buyInOpen = ui->findChild<QCheckBox*>("buyInOpen");
	//saleInClose = ui->findChild<QCheckBox*>("saleInClose");

	button = ui->findChild<QPushButton*>("button");

	connect(button, SIGNAL(clicked()), this, SLOT(onSave()));

	cost->setValue(SnailApplication::getInstance()->gridTradeConfig.cost);
	firstBuyPercent->setValue(SnailApplication::getInstance()->gridTradeConfig.firstBuyPercent);
	saleProfit->setValue(SnailApplication::getInstance()->gridTradeConfig.saleProfit);
	decreaseBuy->setValue(SnailApplication::getInstance()->gridTradeConfig.decreaseBuy);
	//buyInOpen->setChecked(SnailApplication::getInstance()->gridTradeConfig.buyInOpen);
	//saleInClose->setChecked(SnailApplication::getInstance()->gridTradeConfig.saleInClose);
}

GridTradeConfigDialog::~GridTradeConfigDialog()
{
}

void GridTradeConfigDialog::onSave()
{
	auto app = SnailApplication::getInstance();

	//app->gridTradeConfig.buyInOpen = buyInOpen->isChecked();
	//app->gridTradeConfig.saleInClose = saleInClose->isChecked();
	app->gridTradeConfig.cost = cost->value();
	app->gridTradeConfig.decreaseBuy = decreaseBuy->value();
	app->gridTradeConfig.firstBuyPercent = firstBuyPercent->value();
	app->gridTradeConfig.saleProfit = saleProfit->value();
	close();
}

GridTradeWidget::GridTradeWidget(QWidget* parent):
QDialog(parent)
{
	setWindowTitle("网格交易分析");
	gridTrader = new GridTrader(this);

	auto box = new QVBoxLayout(this);
	box->setMargin(0);
	auto ui = loadWidgetFromUI(":ui/gridTrade.ui");
	box->addWidget(ui);

	code = ui->findChild<QLineEdit*>("code");
	code->setPlaceholderText("股票代码前需要sh/sz");

	code->setText("sh000300");

	config = ui->findChild<QPushButton*>("config");
	connect(config,SIGNAL(clicked()),this,SLOT(onConfig()));

	config->setFixedWidth(72);
	
	button = ui->findChild<QPushButton*>("button");
	connect(button,SIGNAL(clicked()),this,SLOT(onAnalyze()));

	button->setFixedWidth(72);

	tabWidget = ui->findChild<QTabWidget*>("tabWidget");
	tabWidget->clear();

#define ADD_ITEM(flag)\
	{\
		auto item = ObjectFactory<GridTradeItemWidget, QString>::create(flag);\
		tradeItemWidgets.append(item);\
		tabWidget->addTab(item,item->objectName());\
	}

	//ADD_ITEM("date")
	ADD_ITEM("ma")
	ADD_ITEM("boll")
	//ADD_ITEM("algo")
}

GridTradeWidget::~GridTradeWidget()
{
}

void GridTradeWidget::onConfig()
{
	GridTradeConfigDialog config(this);
	config.exec();
}

void GridTradeWidget::onAnalyze()
{
	auto stockCode = code->text();
	bool hk = stockCode.startsWith("hk");
	if((!hk) && (stockCode.size() != 8 || 
		(stockCode.startsWith("sh") == false && stockCode.startsWith("sz") == false)))
	{
		POP_MSGBOX("错误", "股票格式不对", this)
		return;
	}
	else if(hk && stockCode.size()!=7)
	{
		POP_MSGBOX("错误","股票格式不对",this)
		return;
	}

	stocks.clear();

	StockParser* p = ObjectFactory<StockParser,QString>::create("tdx");
	QSharedPointer<StockParser> pointer(p);

	QString file = QString("data//%1").arg(code->text());
	QFileInfo info(file);

	if (info.lastModified().toString("yyyy-MM-dd") <
		QDateTime::currentDateTime().toString("yyyy-MM-dd"))
	{
		pointer->setParam(StockParser::FLAG_TARGET_COUNT,"540");
		pointer->setParam(StockParser::FLAG_TARGET_FUQUAN,"bfq");

#ifdef SUPPORT_STOCK_GRID_TRADE_UPDATE_STOCK
		/*auto links = pointer->buildLinks(QStringList(code->text()));
		auto buffer = DataLoader::getText(links[0]);
		saveToFile(file,buffer,code->text());*/
#endif
		stocks = p->parseContent(file);
	}
	else
		stocks = p->parseContent(file);

	if(stocks.isEmpty())
	{
		POP_MSGBOX("错误","无对应股票数据",this)
		return;
	}

	tradeItem = tradeItemWidgets[tabWidget->currentIndex()];

	tradeItem->init(stocks);

	QString error;
	auto pos = tradeItem->execute(stocks,0,&error);

	if(pos < 0)
	{
		POP_MSGBOX("错误",error,this)
		return;
	}

	if (pos >= stocks.size())
	{
		POP_MSGBOX("错误","无指定日期股票数据",this)
		return;
	}

	gridTrader->trade(pos,stocks);

	auto stock = stocks.constLast();
	stock.code = code->text();

	GridTradeOutput dlg(gridTrader,stock,this);
	dlg.exec();
}

int GridTradeWidget::getNextPosition(int pos)
{
	if (stocks.size() <= pos)
		return -1;
	return tradeItem->execute(stocks,pos);
}

bool GridTradeWidget::check(int pos)
{
	if (stocks.size() <= pos)
		return false;
	return tradeItem->check(stocks, pos);
}