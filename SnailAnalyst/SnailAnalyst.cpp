#include <QGridLayout>
#include <QFileInfo>
#include <QMenuBar>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <qprocess.h>
#include <qsettings.h>
#include <qcompleter.h>
#include <qstringlistmodel.h>
#include "ConfigurationParser.h"
#include "StockListView.h"
#include "SnailAnalyst.h"
#include "candlestick/StrategyViewWidget.h"
#include "common/HighLighter.h"
#include "SnailApplication.h"
#include "dialog/SnailStockMsgBox.h"
#include "interface/StockRealTimeInfo.h"
#include "StockScriptEngine.h"
#include "dialog/SnailAboutDlg.h"
#include "candlestick/StockRealTimeView.h"
#include "script/ScriptManager.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "sea/SeaScriptDialog.h"
#include "config/GlobalConfigDialog.h"
#include "config/ConfigItem.h"
#include "SnailContext.h"
#include "Sea/SeaScript.h"
#include "dialog/StockCorDialog.h"
#include "dialog/StockPoolCorDialog.h"
#include "dialog/CandlestickViewDialog.h"
#include "stockvaluation/StockValuationWidget.h"
#include "analyze/StockNatureAnalyzer.h"
#include "grid/GridTradeWidget.h"
#include <cstdlib>

#define TEXT_CONFIG_FILE "config/edit.ini"

SnailWidget::SnailWidget(QWidget* parent):
QWidget(parent)
{
	setMinimumSize(960,640);
    auto grid = new QGridLayout(this);
    
	edit = new AutoTextEdit(this);
	auto completer = new QCompleter(this);
	completer->setModel(new QStringListModel(SnailContext::getIntelligentPrompts(),this));
	completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(false);
	edit->setCompleter(completer);

    grid->addWidget(edit,0,0,1,4);
    QTextEdit::ExtraSelection highlight;
    highlight.cursor = edit->textCursor();
    highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
    highlight.format.setBackground(Qt::gray);
	scriptMode = "script";

	QSettings config(TEXT_CONFIG_FILE,QSettings::IniFormat);
	int textcolor = config.value("edit/textcolor",0xFF0000).toUInt();
	int bgcolor = config.value("edit/bgcolor",0x00FF00).toUInt();
	int fontsize = config.value("edit/fontsize",8).toInt();
	int fontIndex = config.value("edit/font",0).toInt();
	QString fontname = config.value("edit/fontname",QString()).toString();

	auto pen = edit->palette();
	pen.setColor(QPalette::Base,IntToQColor(bgcolor));
	edit->setPalette(pen);
	edit->setTextColor(IntToQColor(textcolor));

    QList<QTextEdit::ExtraSelection> extras;
    extras << highlight;
    edit->setExtraSelections(extras);
	QFont font(fontname);
	font.setPointSize(fontsize);
	font.setFixedPitch(true);
	edit->setFont(font);

    progressBar = new QProgressBar(this);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);
	progressBar->hide();
	grid->addWidget(progressBar,1,0,1,3);

	stepLabel = new QLabel("-/-");
	grid->addWidget(stepLabel,1,3,1,1);
	stepLabel->hide();

	auto horizontalSpacer = new QSpacerItem(217,20,QSizePolicy::Expanding,QSizePolicy::Minimum);
	grid->addItem(horizontalSpacer,2,0,1,1);

    button = new QPushButton(this);
	button->setText("执行");
    grid->addWidget(button,2,1,1,1);

    view = new QPushButton(this);
	view->setText("查看");
    grid->addWidget(view,2,2,1,2);

#if defined(SUPPORT_PAIRS_TRADE)
#ifndef SUPPORT_BASIC
	QString buffer = readAllBufferFromFile(":qrc/pairtrade.script");
#else
	QString buffer = readAllBufferFromFile(":qrc/default.script");
#endif
#else
	QString buffer = readAllBufferFromFile(":qrc/default.script");
#endif

	buffer = buffer.replace("VERSION#",SNAIL_VERSION);
	edit->setText(buffer);

	connect(button,SIGNAL(clicked()),this,SLOT(clickButton()));
	connect(view,SIGNAL(clicked()),this,SLOT(clickView()));

	view->setEnabled(false);
	QStringList keywords = StockScriptList::GetInstance().getScriptEngine()->keywords();
	new Highlighter(keywords,edit->document());

	StockScriptList::GetInstance().setTargetObject(this);
}

SnailWidget::~SnailWidget()
{
}

void SnailWidget::undo()
{
    edit->undo();
}

void SnailWidget::redo()
{
	edit->redo();
}

void SnailWidget::clickButton()
{
	stockFileCount = 0;
	StockTradeWriter::getInstance()->clear();

	button->setEnabled(false);
	progressBar->setVisible(true);
	stepLabel->setVisible(true);

	QString script = edit->toPlainText();

	SeaScriptManager::getInstance()->reset();
	StockScriptList::GetInstance().loadAndRunScripts(script,scriptMode);
}

void SnailWidget::clickView()
{
	if (!dataProvider.loadedRef300)
	{
		POP_MSGBOX("警告","策略文件不存在或缺少股票数据",this);
	}
	else
	{
		auto list = StockScriptList::GetInstance().getStockTradeItems();
		auto arguments = StockScriptList::GetInstance().getScriptEngine()->getArguments();
		auto basescript = StockScriptList::GetInstance().getBaseScript();
		StockTradeWriter::getInstance()->toCSV("output.csv",list,arguments);
		StockScriptList::GetInstance().clearStockTradeItems();
		StrategyViewWidget widget(list,scriptMode,basescript,nullptr);
		widget.exec();
	}
}

void SnailWidget::onUpdateStock(int current,int all,int percent,const QString& code)
{
	progressBar->setValue(percent);
	QString text = QString("%1-%2").arg(current).arg(all);
	stepLabel->setText(text);
}

void SnailWidget::onUpdateFinished(const QString& error)
{
	progressBar->setVisible(false);
	stepLabel->hide();
	button->setEnabled(true);

	if(!error.isEmpty())
	{
	    POP_MSGBOX("错误",error,this)
	    return;
	}
	
	clickView();
	return;
}

void SnailWidget::loadScript(const QString& script,bool run)
{
	QString buffer = readAllBufferFromFile(script);
	if(buffer.size() > 20)
		edit->setText(buffer);

	if(script.endsWith("batch-script"))
		scriptMode = "batch-script";
	else if(script.endsWith("boosting-script"))
		scriptMode = "boosting-script";
	else 
		scriptMode = "script";

	if(run)
		clickButton();
}

void SnailWidget::saveScript(const QString& script)
{
	QString buffer = edit->toPlainText();
	saveToFile(script,buffer.toLocal8Bit());
}

SnailAnalyst::SnailAnalyst(const QString& script,QWidget* parent): 
QMainWindow(parent)
{
#if defined(SUPPORT_PAIRS_TRADE)
#ifndef SUPPORT_BASIC
	QString app = QString("蜗牛股票量化分析软件(配对交易专版) %1").arg(SNAIL_VERSION);
#else
	QString app = QString("蜗牛股票量化分析软件 %1").arg(SNAIL_VERSION);
#endif
#else
	QString app = QString("蜗牛股票量化分析软件 %1").arg(SNAIL_VERSION);
#endif

	setWindowIcon(QIcon(":logo/snail.png"));
	widget = new SnailWidget();
	setCentralWidget(widget);

#define SET_ACTION(node,tag,action,iconfile)\
	{\
	    QAction* load = new QAction(tag,this);\
        QString icon(iconfile);\
        if(!icon.isEmpty())\
            load->setIcon(QIcon(QString(icon)));\
	    connect(load,SIGNAL(triggered()),this,SLOT(action()));\
		node->addAction(load);\
	}

	{
	    QMenu* menu = menuBar()->addMenu("文件");
        SET_ACTION(menu,"导入脚本",loadScript,":icon/open.png")
		SET_ACTION(menu,"导出脚本",saveScript,":icon/save.png")
		SET_ACTION(menu,"关闭",close,":icon/close.png")
    }
	{
	    QMenu* menu = menuBar()->addMenu("编辑");
        SET_ACTION(menu,"撤销",undo,":icon/undo.png")
		SET_ACTION(menu,"重做",redo,":icon/redo.png")
    }
	{
	    QMenu* menu = menuBar()->addMenu("查看");
		SET_ACTION(menu, "股票清单", showStockList, ":icon/list.png")
#ifdef SUPPORT_BASIC
			//SET_ACTION(menu,"查看股票K线",lookCandlestick)
#endif
#ifdef SUPPORT_TICKET
			SET_ACTION(menu, "查看历史分时图", viewHistoryRealTime, ":icon/view.png")
#endif 
#ifdef SUPPORT_SCRIPT
			//SET_ACTION(menu,"查看插件清单",showAddonList,"")
#endif
#ifdef SUPPORT_SEA_SCRIPT
			SET_ACTION(menu, "查看Sea清单", showSeaScriptList, ":icon/script.png")
#endif
#ifdef SUPPORT_LEVEL2
			SET_ACTION(menu, "查看Level2分析", viewStockLevel2, "")
#endif
			SET_ACTION(menu, "查看股票相关度", viewStockCor, "")
#ifdef SUPPORT_PAIRS_TRADE
			SET_ACTION(menu, "查看股票池相关度",viewStockPoolCor, "")
#endif
#ifdef SUPPORT_VIEW_K
			SET_ACTION(menu, "查看股票K线", viewCandlestick, "")
#endif
#ifdef SUPPORT_STOCK_VALUATION
			SET_ACTION(menu, "股票估值器", viewStockValuation, "")
#endif
#ifdef SUPPORT_STOCK_ANALYZE
			SET_ACTION(menu, "股性分析",viewStockAnalyze,"");
#endif
#ifdef SUPPORT_STOCK_GRID_TRADE
		    SET_ACTION(menu, "网格交易", viewGridTrade,"");
#endif
    }

	{
        QMenu* menu = menuBar()->addMenu("配置");
		SET_ACTION(menu,"编辑配置",edit,":icon/edit.png")
		SET_ACTION(menu,"股票池",loadStockListEdit,":icon/note.png")
#ifdef SUPPORT_UPDATE_STOCK
		SET_ACTION(menu,"更新数据",updateStockData,":icon/update.png")
#endif
#ifdef SUPPORT_UPDATE_FINANCE_DATA
		SET_ACTION(menu,"更新财务数据",viewStockFinanceDownload,":icon/update.png")
#endif
	}
	{
        QMenu* menu = menuBar()->addMenu("帮助");
		SET_ACTION(menu,"疑问",question,":icon/question.png")
		SET_ACTION(menu,"命令清单",commandList,"")
	    SET_ACTION(menu,"购买",buy,":icon/buy.png")
		SET_ACTION(menu,"帮助",help,":icon/help.png")
		SET_ACTION(menu,"关于",about,":icon/info.png")
	}

    ConfigurationParser::loadConfig();

	loadScriptFileFromInputFile(script);
}

SnailAnalyst::~SnailAnalyst()
{
	SnailContext::open()->shutdown();
}

void SnailAnalyst::redo()
{
    widget->redo();
}

void SnailAnalyst::undo()
{
	widget->undo();
}

void SnailAnalyst::loadScript()
{
#if defined(SUPPORT_BATCH_SCRIPT) || defined(SUPPORT_BOOSTING_ALGO)
	QString file = QFileDialog::getOpenFileName(this,"导入脚本",
                    QString(),
                    "Script (*.script);;Batch Script(*.batch-script);;Boosting Script(*.boosting-script)");
	if(!file.isEmpty())
		widget->loadScript(file);
#else
#ifndef SUPPORT_BATCH_SCRIPT
	QString file = QFileDialog::getOpenFileName(this,"导入脚本",
                    QString(),
                    "Script (*.script)");
	if(!file.isEmpty())
		widget->loadScript(file);
#else
	QString file = QFileDialog::getOpenFileName(this,"导入脚本",
                    QString(),
                    "Script (*.script);;Batch Script(*.batch-script)");
	if(!file.isEmpty())
		widget->loadScript(file);
#endif
#endif
}

void SnailAnalyst::saveScript()
{
#if defined(SUPPORT_BATCH_SCRIPT) || defined(SUPPORT_BOOSTING_ALGO)
    QString fileName = QFileDialog::getSaveFileName(this,
		"导出脚本","",
		"Script (*.script);;Batch Script (*.batch-script);;Boosting Script (*.boosting-script)");

    if(!fileName.isNull())
		widget->saveScript(fileName);
#else
	#ifndef SUPPORT_BATCH_SCRIPT
		QString fileName = QFileDialog::getSaveFileName(this,
			"导出脚本","",
			"Script (*.script)");

		if(!fileName.isNull())
			widget->saveScript(fileName);
	#else
		QString fileName = QFileDialog::getSaveFileName(this,
			"导出脚本","",
			"Script (*.script);;Batch Script (*.batch-script)");

		if(!fileName.isNull())
			widget->saveScript(fileName);
	#endif
#endif
}

void SnailAnalyst::showStockList()
{
	QMap<QString,Stock> list = config.stockList;
    StockListDialog dlg(StockListViewEnum_SnailStockList,list,this);
	dlg.exec();
}

void SnailAnalyst::about()
{
	SnailAboutDlg dlg;
	dlg.exec();
}

void SnailAnalyst::help()
{
	QDesktopServices::openUrl(QUrl(QString("http://snail007.com/archives/685")));
}

void SnailAnalyst::question()
{
	QDesktopServices::openUrl(QUrl(QString("http://snail007.com/archives/699")));
}

void SnailAnalyst::buy()
{
    QDesktopServices::openUrl(QUrl(QString("https://sharesoft.taobao.com/")));
}

#ifdef SUPPORT_TICKET
void SnailAnalyst::viewHistoryRealTime()
{
	StockHistoryCandlestickView view(this);
	view.exec();
}
#endif 

#ifdef SUPPORT_UPDATE_STOCK
void SnailAnalyst::updateStockData()
{
	//数据下载不开源!
}
#endif

#ifdef SUPPORT_BASIC
void SnailAnalyst::lookCandlestick()
{
	//不需要提供蜡烛图
}
#endif

void SnailAnalyst::loadStockListEdit()
{
	QProcess::startDetached("edit.exe");
}

void SnailAnalyst::commandList()
{
	QDesktopServices::openUrl(QUrl(QString("http://snail007.com/archives/817")));    
}

#ifdef SUPPORT_SCRIPT
void SnailAnalyst::showAddonList()
{
    ScriptAddonView view(this);
	view.exec();
}
#endif 

#ifdef SUPPORT_LEVEL2
void SnailAnalyst::viewStockLevel2()
{
	//!开源版本不提供LV2分析功能
}
#endif

void SnailAnalyst::loadScriptFileFromInputFile(const QString& script)
{
	if(script.isEmpty())
		return;

	if(script.endsWith(".batch-script"))
	{
#ifdef SUPPORT_BATCH_SCRIPT
		widget->loadScript(script,true);
#endif
	}
	else
	{
		widget->loadScript(script,true);
	}
}

void SnailAnalyst::showSeaScriptList()
{
	SeaScriptDialog dialog(this);
	dialog.exec();
}

void SnailAnalyst::edit()
{
	GlobalConfigDialog dlg(this);
	dlg.exec();
}

void SnailAnalyst::viewStockCor()
{
	StockCorDialog dlg(this);
	dlg.exec();
}

#ifdef SUPPORT_PAIRS_TRADE

void SnailAnalyst::viewStockPoolCor()
{
	StockPoolCorDialog dlg(this);
	dlg.exec();
}

#endif

#ifdef SUPPORT_STOCK_VALUATION
void SnailAnalyst::viewStockValuation()
{
	StockValuationWidget dlg(this);
	dlg.exec();
}
#endif

#ifdef SUPPORT_STOCK_ANALYZE
void SnailAnalyst::viewStockAnalyze()
{
	StockNatrueAnalyzer dlg(this);
	dlg.exec();
}
#endif

#ifdef SUPPORT_STOCK_GRID_TRADE
void SnailAnalyst::viewGridTrade()
{
	GridTradeWidget dlg(this);
	dlg.exec();
}
#endif

#ifdef SUPPORT_UPDATE_FINANCE_DATA
void SnailAnalyst::viewStockFinanceDownload()
{
	//开源不提供数据下载功能
}
#endif

#ifdef SUPPORT_VIEW_K
void SnailAnalyst::viewCandlestick()
{
	CandlestickViewDialog dlg(this);
	dlg.exec();
}

#endif 