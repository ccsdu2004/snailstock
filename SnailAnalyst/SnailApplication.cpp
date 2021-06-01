#include <QDebug>
#include <qfile.h>
#include <qfileinfo.h>
#include "SnailApplication.h"
#include "ObjectFactory.h"
#include "StockMapper.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "expr/MetaEvalutor.h"
#include "expr/VarMeta.h"
#include "dialog/SnailStockMsgBox.h"
#include "interface/StockParser.h"

SnailApplication* SnailApplication::getInstance()
{
	return dynamic_cast<SnailApplication*>(qApp);
}

SnailApplication::SnailApplication(int argc,char** argv):
	QApplication(argc,argv)
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
	setApplicationName(app);

	QFileInfo file("config/config.conf");
	if (!file.exists())
	{
		auto config = readAllBufferFromFile(":qrc/config.conf");
		saveToFile("config/config.conf",config.toLatin1());
		parseConfiguration(config,globalConfig);
	}
	else
	{
		auto buffer = readAllBufferFromFile("config/config.conf");
		parseConfiguration(buffer,globalConfig);
	}

	StockMapper::instance()->open(":data/stocks");


	gridTradeConfig.loadFromFile(GridTradeConfig::configFile);
	lv2Config.load();

	pythonScript = new PythonScript(this);
}

SnailApplication::~SnailApplication()
{
	ObjectFactory<MetaEvalutor,QString>::shutdown();
#ifdef SUPPORT_BOOSTING_ALGO
	ObjectFactory<VarMeta,QString>::shutdown();
#endif
	ObjectFactory<StockParser,QString>::shutdown();

	gridTradeConfig.save(GridTradeConfig::configFile);
	lv2Config.save();
}
