#include <qdir.h>
#include <qdebug.h>
#include "StockScriptEngine.h"
#include "DataProvider.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "sea/SeaScript.h"
#include "boosting/BoostingParser.h"
#include "pairtrade/PairTradeAlgo.h"
#include <iostream>

StockScriptList& StockScriptList::GetInstance()
{
	static StockScriptList instance;
	return instance;
}

StockScriptList::StockScriptList()
{
	showInOne = false;
	engine = new StockScriptEngine();
	target = nullptr;

	thread = new StockThreadTarget(StockTargetCallbackImpl,0);
	thread->flagFinishedScript.connect(this,&StockScriptList::receivedFinishSignal);
#ifdef SUPPORT_BASIC
	varMetaManager = new VarMetaManager(this);
#endif
}

StockScriptList::~StockScriptList()
{
	DELETE_PTR(engine)
	thread->deleteLater();
}

void StockScriptList::receivedFinishSignal(QString error)
{
	if(error.isEmpty() && showInOne && !(scriptMode == "boosting-script"))
	{
		StockTradeWriter::getInstance()->reduce();
		auto value = StockTradeWriter::getInstance()->getTradeItem();
		value.scriptName = currentScriptName;
		value.scriptContent = currentScriptContent;
		stockTradeItems.push_back(value);
		StockTradeWriter::getInstance()->clear();
    }

	emit onFinish(error);
}

void StockScriptList::setTargetObject(QObject* object)
{
    target = object;
	connect(this,SIGNAL(onUpdate(int,int,int,const QString&)),target,SLOT(onUpdateStock(int,int,int,const QString&)));
	connect(this,SIGNAL(onFinish(const QString&)),target,SLOT(onUpdateFinished(const QString&)));
}

bool StockScriptList::loadAndRunScripts(const QString& buffer,const QString& scriptmode)
{
	scripts.clear();
	path.clear();
	config.reset();
	showInOne = true;

	scriptMode = scriptmode;

	bool ok = false;

	if(scriptMode == "script")
	{
		baseScriptContent = buffer;
		scripts.push_back(DataWrapper2<QString,QString>("当前",buffer));
	}
#ifdef SUPPORT_BOOSTING_ALGO
	else if(scriptMode == "boosting-script")
	{
		baseScriptContent = buffer;
		BoostingParser parser;
		parser.parse(buffer);
		doAfter();
		auto list = parser.getScripts();
		for(int i=0;i<list.size();i++)
		{
			QString buffer = list[i];
		    scripts.push_back(DataWrapper2<QString,QString>(QString("%1").arg(i+1),buffer));
		}
		ok = true;
	}
#endif
	else
	{
		baseScriptContent = buffer;
		ok = parse(buffer);
	}

	scriptIndex = 0;

	execute();
	return ok;
}

void StockScriptList::doLine(const QString& tag,const QString& value)
{
	if(scriptMode == "boosting-script")
	{
	}
	else
	{
		if(tag == "batch.path")
			path = value;
		else if(tag == "batch.allInOne")
			showInOne = value == "yes";
		else if(tag == "batch.items")
		{
			if(value == "all")
			{
			    QFileInfoList scriptList = QDir(path).entryInfoList(QDir::Files | QDir::NoSymLinks);
				foreach(auto file,scriptList)
				{
					if(file.absoluteFilePath().endsWith(".script"))
					{
						QString buffer = readAllBufferFromFile(file.absoluteFilePath());
				        if(!buffer.isEmpty())
							scripts.push_back(DataWrapper2<QString,QString>(file.baseName(),buffer));
					}
				}
			}
			else
			{
				QString filename = path + "//" + value;
				QString buffer = readAllBufferFromFile(filename);
				if(!buffer.isEmpty())
					scripts.push_back(DataWrapper2<QString,QString>(filename,buffer));
			}
		}
	}
}

void StockScriptList::doAfter()
{
}

void StockScriptList::execute()
{
    thread->setData(scripts);
	thread->start();
}

void StockScriptList::callbackScript(const QString& desc,const QString& script,QString& error)
{
	DELETE_PTR(engine)
    engine = new StockScriptEngine();

	currentScriptContent = script;

    bool status = engine->prepare(script,error);
    if(!status)
    {
		scripts.clear();
        return;
    }

	QFileInfo sinfo(desc);
	currentScriptName = sinfo.baseName();

#ifdef SUPPORT_REF
	if (config.refStock.isEmpty() == false)
		checkRefOneStock(config.refStock);
#endif

	QFileInfoList stockFiles = getStockFilesByPathAndFormat(config.stockMarket,config.stockPath,config.stockFormat);

	int pos = 0;
	int count = stockFiles.size();
	foreach(auto file,stockFiles)
	{
		checkOneStock(file.absoluteFilePath());
		emit onUpdate(scriptIndex,scripts.size(),pos * 100 / count,file.baseName());
		pos ++;
	}
	
	scriptIndex ++;

#ifdef SUPPORT_BOOSTING_ALGO
	if(scriptMode == "boosting-script" || !showInOne)
	{
		StockTradeWriter::getInstance()->reduce();
		auto value = StockTradeWriter::getInstance()->getTradeItem();
		value.scriptName = currentScriptName;
		value.scriptContent = currentScriptContent;

		if(scriptMode != "boosting-script")
			stockTradeItems.push_back(value);
		else
		{
			double withdraw = value.profit[0];
			double profit = value.profit[1];
			double success = value.profit[2];
			bool ok = BoostingParser::show(value.item.size(),success,profit,withdraw);
			if(ok)
				stockTradeItems.push_back(value);
		}

		StockTradeWriter::getInstance()->clear();
    }
#endif
}

void StockScriptList::checkOneStock(const QString& stockFile)
{
	QString stockName = dataProvider.getStockCodeByFilePath(stockFile);
    QString pool = config.stockPoolName;

    if( !pool.isEmpty())
    {
		if(pool.startsWith("per."))
		{
			if (!dataProvider.loadStockFromFile(stockName,stockFile))
			{
				dataProvider.clear();
				return;
			}
			engine->checkStock(stockName);
			return;
		}
		else
		{
			auto poolObject = config.stockPool[pool];
			bool has = poolObject.items.contains(stockName);
			if (has)
			{
				if (!dataProvider.loadStockFromFile(stockName,stockFile))
				{
					dataProvider.clear();
					return;
				}
				engine->checkStock(stockName);
			}
		}
		return;
    }

    if(!dataProvider.loadStockFromFile(stockName,stockFile))
    {
        dataProvider.clear();
        return;
    }
    
    engine->checkStock(stockName);
}

void StockScriptList::checkRefOneStock(const QString& stockcode)
{
	auto stockFile = getStockFileByStockCode(stockcode,config.stockPath,config.stockFormat);

	QString stockName = dataProvider.getStockCodeByFilePath(stockFile);
	if (!dataProvider.loadStockFromFile(stockName,stockFile,true))
	{
		qDebug() << "加载参考股票数据失败," << stockName << "," << stockFile;
		dataProvider.clear();
		return;
	}

	std::cout << "ref check stock:" << qPrintable(stockcode) << std::endl;
	engine->checkRefStock(stockcode);
}
