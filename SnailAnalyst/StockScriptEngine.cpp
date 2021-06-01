#include <QStringList>
#include <QDir>
#include <qdebug.h>
#include "math/math.h"
#include "indicator.h"
#include "expr/VarMeta.h"
#include "expr/MetaEvalutor.h"
#include "expr/StatementObject.h"
#include "ConfigurationParser.h"
#include "StockScriptEngine.h"
#include "k/StockKAnalyzer.h"
#include "StockTickScanner.h"
#include "SnailApplication.h"
#include "dialog/SnailStockMsgBox.h"
#include "config.h"
#include "interface/StockParser.h"
#include "common/common.h"
#include "common/FileUtils.h"
#include "script/ScriptManager.h"
#include "boosting/BoostingParser.h"
#include "sea/SeaScript.h"
#include "lv2/StockLV2Reader2019.h"
#include "config/ConfigItem.h"
#include "SnailContext.h"
#include "ObjectFactory.h"
#include "interface/StockParser.h"
#include "pairtrade/PairTradeAlgo.h"
#include <iostream>

#define CHECK_AND_BUILD(str,list)\
if(str.split("|").size() > 1)\
{\
StatementObject* evalutor = new StatementObjectNode();\
bool ok = evalutor->build(str);\
if (!ok)\
{\
delete evalutor;\
return -1;\
}\
list.push_back(evalutor);\
}\
else\
{\
StatementObject* evalutor = new StatementObject();\
bool ok = evalutor->build(str);\
if (!ok)\
{\
delete evalutor;\
return -1;\
}\
list.push_back(evalutor);\
}

QVector<Stock> Configuration::readStocksFromFile(const QString& file)
{
	QString code = dataProvider.getStockCodeByFilePath(file);
	QVector<Stock> output;

	StockParser* parser = nullptr;

	if(stockFormat == "default")
	{
		parser = ObjectFactory<StockParser,QString>::create("tencent");	
	}
	else if (stockFormat == "tdx")
	{
		parser = ObjectFactory<StockParser,QString>::create("tdx");
	}
#ifdef SUPPORT_TUSHARE
	else if(stockFormat == "tushare")
	{
		parser = ObjectFactory<StockParser,QString>::create("tushare");
	}
#endif 
#ifdef SUPPORT_BASIC 
	else if(stockFormat == "auto")
	{
		parser = ObjectFactory<StockParser,QString>::create("auto");
	}
#endif
	if (parser)
	{
		output = parser->parseContent(file);
		delete parser;
	}
	return output;
}

#ifdef SUPPORT_TICKET

StockRealTimeInfo Configuration::readStockTicketFromFile(const QString& input)
{
	StockRealTimeInfo info;
    QString buffer = input;

	if(buffer.startsWith("{\"code\""))
	{
	    StockTimeDataInterface face;
		info = face.parseContent(buffer);
	}
	else
	{
	    StockTuShareTimeDataInterface face;
		info = face.parseContent(buffer);
	}
	return info;
}

#endif 

StockScriptEngine::StockScriptEngine()
{
}

StockScriptEngine::~StockScriptEngine()
{
	clearEvalutor();
}

void StockScriptEngine::clearEvalutor()
{
	foreach(auto ptr,buyEvalutor)
		delete ptr;
	buyEvalutor.clear();
	foreach(auto ptr,buyRefEvalutor)
		delete ptr;
	buyRefEvalutor.clear();
	foreach(auto ptr,saleEvalutor)
		delete ptr;
	saleEvalutor.clear();
	foreach(auto ptr,saleRefEvalutor)
		delete ptr;
	saleRefEvalutor.clear();
#ifdef SUPPORT_BASIC
    foreach(auto ptr,varMapper)
		delete ptr;
	varMapper.clear();
	foreach(auto ptr,storeMapper)
		delete ptr;
	storeMapper.clear();
#endif 

	refBSSet.clear();
}

QStringList StockScriptEngine::keywords()
{
	auto list = MetaEvalutor::keywords();
	list.append(configParser.tag().split("|"));
	list += StockKAnalyzer::instance()->keywords();
#ifdef SUPPORT_BASIC
	list += VarMeta::keywords();
	list += "not";
#endif 

#ifdef SUPPORT_IMPLICIT_VAR
	list += "@IMPLICIT_VAR1";
	list += "@IMPLICIT_VAR2";
	list += "@IMPLICIT_VAR3";
	list += "@IMPLICIT_VAR4";
	list += "@IMPLICIT_VAR5";
	list += "@IMPLICIT_VAR6";
	list += "@IMPLICIT_VAR7";
	list += "@IMPLICIT_VAR8";
	list += "@IMPLICIT_VAR9";
	list += "@IMPLICIT_VAR10";
	list += "@IMPLICIT_VAR11";
	list += "@IMPLICIT_VAR12";
	list += "@IMPLICIT_VAR13";
	list += "@IMPLICIT_VAR14";
	list += "@IMPLICIT_VAR15";
	list += "@IMPLICIT_VAR16";
	list += "@IMPLICIT_VAR17";
	list += "@IMPLICIT_VAR18";
	list += "@IMPLICIT_VAR19";
	list += "@IMPLICIT_VAR20";
#endif 

#ifdef SUPPORT_BIG_SPAN
	list << "周线" << "半月线" << "月线";
#endif
#ifdef SUPPORT_TICKET
	list << "分时5分钟" << "分时10分钟" << "分时15分钟"<<"分时30分钟"<<"分时60分钟";
#endif
#ifdef SUPPORT_SCRIPT
	list += ScriptManager::instance().keywords();
#endif
#ifdef SUPPORT_BATCH_SCRIPT
	list += "batch.path";
	list += "batch.allInOn";
	list += "batch.items";
#endif
#ifdef SUPPORT_SEA_SCRIPT
	foreach(auto item,SeaScriptManager::getInstance()->getSeaScriptKeys())
		list += QString("sea.%1").arg(item);
#endif
#ifdef SUPPORT_BOOSTING_ALGO
    list += BoostingParser::keywords();
#endif 

#ifdef SUPPORT_PER
	list += "per.最高换手率50";
	list += "per.最高换手率100";
	list += "per.最高成交量50";
	list += "per.最高成交量100";
	list += "per.最高成交额50";
	list += "per.最高成交额100";
	list += "per.涨幅50";
	list += "per.涨幅100";
	list += "per.跌幅50";
	list += "per.涨幅100";
	list += "per.最高价50";
	list += "per.最高价100";
	list += "per.最低价50";
	list += "per.最低价100";
	list += "per.振幅50";
	list += "per.振幅100";
#endif 

#ifdef SUPPORT_REF
	list += "ref";
#endif

	list += SnailContext::open()->getConfigFactory()->name();
	list += SnailContext::open()->getConfigFactory()->tags();
	return list;
}

bool StockScriptEngine::prepare(const QString& script,QString& error)
{
	dataProvider.reset();
	configParser.loadConfig();
	clearEvalutor();
	arguments.clear();
	config.reset();

	int cur = 1;
	QStringList statements = script.split("\n");
	QStringList list;
	
	foreach(auto item,statements)
	    list += SeaScriptManager::getInstance()->buildScripts(item);

	foreach(QString statement,list)
	{
		auto item = statement.trimmed();
		if(item.startsWith("config.unit."))
		{
			QStringList cmdline = item.split("=");
			if(cmdline.size() != 2)
			{
				error = QString("脚本第%1行格式错误").arg(cur);
				return false;
			}
			bool ok = SnailContext::open()->getConfigFactory()->parse(item);
			if(!ok)
			{
				error = QString("脚本第%1行内容%2格式错误").arg(cur).arg(item);
				return false;
			}
		}
		else if(item.startsWith("config."))
		{
			config.parse(item);
			int ret = configParser.parse(item);
			if(ret == -1)
			{
				error = QString("脚本第%1行内容%2格式错误").arg(cur).arg(item);
				return false;
			}
		}
		else 
		{
			int ok = parseLine(item);
			if(ok != 1)
			{
				error = QString("脚本第%1行内容%2解析错误").arg(cur).arg(item);
				return false;
			}
		}
		cur ++;
	}

	if(buyEvalutor.isEmpty() && config.runningMode != "pairtrade")
	{
		error = QString("策略没有指定买点");
		return false;
	}

	return afterPrepare(error);
}

bool StockScriptEngine::afterPrepare(QString& error)
{
	if (config.minActiveStockValue < 0.0)
		config.minActiveStockValue = 0.0;
	if (config.maxActiveStockValue > 100.0)
		config.maxActiveStockValue = 100.0;

#ifdef SUPPORT_LEVEL2
	if(config.stockLevel2V1 > LV2_VALUE1_MAX)
		config.stockLevel2V1 = LV2_VALUE1_MAX;
	else if(config.stockLevel2V1 < LV2_VALUE1_MIN)
		config.stockLevel2V1 = LV2_VALUE1_MIN;

	if(config.stockLevel2V2 > LV2_VALUE2_MAX)
		config.stockLevel2V2 = LV2_VALUE2_MAX;
	else if(config.stockLevel2V2 < LV2_VALUE1_MIN)
		config.stockLevel2V2 = LV2_VALUE2_MIN;

	if(config.stockLevel2V3 < LV2_VALUE3_MIN)
		config.stockLevel2V3 = LV2_VALUE3_MIN;
	else if(config.stockLevel2V3 > LV2_VALUE3_MAX)
		config.stockLevel2V3 = LV2_VALUE3_MAX;

	config.stockLevel2V4 = std::clamp<int>(config.stockLevel2V4,LV2_VALUE4_MIN,LV2_VALUE4_MAX);

	config.stockLevel2V1*= 10000;
	config.stockLevel2V2*= 10000;
	config.stockLevel2V3*= 10000;
	config.stockLevel2V4*= 10000;

	auto lv2Config = &SnailApplication::getInstance()->lv2Config;
	lv2Config->dataPath = config.stockLevel2Path;
	lv2Config->_1 = config.stockLevel2V1;
	lv2Config->_2 = config.stockLevel2V2;
	lv2Config->_3 = config.stockLevel2V3;
	lv2Config->_4 = config.stockLevel2V4;
#endif

#ifdef SUPPORT_PAIRS_TRADE
	if (config.runningMode == "pairtrade")
	{
#define GET_VAR(str)\
    SnailContext::open()->getConfigFactory()->getVar(QString(str),var);

		QString code1,code2;
		QVariant var;
		GET_VAR("config.unit.pairtrade.stock1")
		code1 = var.toString();
		GET_VAR("config.unit.pairtrade.stock2")
		code2 = var.toString();
		config.stockPoolName = QString("%1,%2").arg(code1).arg(code2);
	}
#endif

	QString pl = config.stockPoolName;

	if(isStockCode(config.stockMarket,pl))
	{
		StockPoolUnit pool;
		pool.name = config.stockPoolName;
		pool.items.push_back(pl);
		pool.items.push_back(config.ref300);
		config.stockPool.insert(config.stockPoolName,pool);
	}
	else if(config.stockPoolName.startsWith("per."))
	{
	}
	else
	{
		auto list = config.stockPoolName.split(",");
		if(list.size() > 1)
		{
		    StockPoolUnit pool;
		    pool.name = config.stockPoolName;
		    pool.items.push_back(config.ref300);
			foreach(auto item,list)
			{
				if (isStockCode(config.stockMarket,item))
				{
					pool.items.push_back(item);
				}
			}
				 
			config.stockPool.insert(config.stockPoolName,pool);
		}
		else
		{
			QList<QFileInfo> list = getStockFilesByPathAndFormat(config.stockMarket,config.stockPath,config.stockFormat);
#ifdef SUPPORT_BASIC
			if (config.stockPoolName == "随机50")
				list = getRandomFileInList(list,50);
			else if (config.stockPoolName == "随机100")
				list = getRandomFileInList(list,100);
			else if (config.stockPoolName == "随机300")
				list = getRandomFileInList(list,300);
			else if (isStockCode(config.stockMarket,config.stockPoolName))
				list = getStockFilesByStockCode(config.stockMarket,list,config.stockPoolName,config.stockFormat);
			else
#endif
			{
				QString name = config.stockPoolName;
				list = getStockFilesByRegex(config.stockMarket,list,name,config.stockFormat);
			}

			if(!list.isEmpty())
			{
				StockPoolUnit pool;
				pool.items.push_back(config.ref300);
				foreach(auto item,list)
				{
					QString base = getStockCodeByFileName(config.stockMarket,item,config.stockFormat);
					if(isStockCode(config.stockMarket,base))
						pool.items.push_back(base);
				}
				config.stockPool.insert(config.stockPoolName,pool);
			}
		}
	}

#ifdef SUPPORT_REF
	if(!config.refStock.isEmpty())
	{
		if (!isStockCode(config.stockMarket,config.refStock))
		{
			error = QString("索引股票%1不存在或格式错误").arg(config.refStock);
			return false;
		}
	}
#endif

#ifdef SUPPORT_PAIRS_TRADE
	if (config.runningMode == "pairtrade")
	{
		PairTraderAlgo algo;
		algo.setConfiguration(&config);
		pairTradeStocks.clear();
		pairTradeStocks << algo.getStock1() << algo.getStock2();
	}
#endif

	config.candlestickTheme = std::clamp<int>(config.candlestickTheme,0,7);
	return true;
}

int StockScriptEngine::parseLine(const QString& statement)
{
	if(statement.startsWith("#"))
		return 1;
	else
	{
		if(statement.startsWith("@"))
		{
#ifdef SUPPORT_BASIC
			if(statement.startsWith("@@"))
			{
				QStringList items = statement.split("=");
				if(items.size() != 2)
					return -1;

				StoreMeta* meta = new StoreMeta();
				bool ok = meta->build(items[0],items[1]);
				if(!ok)
				{
					delete meta;
					return -1;
				}

				if(storeMapper.find(items[0]) != storeMapper.end())
				{
				    delete meta;
					return -1;
				}

				storeMapper.insert(items[0],meta);
				return 1;
			}
			else
			{
				VarMeta::Meta meta;
				if(!VarMeta::parseMeta(statement,meta))
					return -1;

				auto object = ObjectFactory<VarMeta,QString>::create(meta.operation); 
				if(!object)
					return -1;

				if(!object->build(meta))
				{
					delete object;
					return -1;
				}

				if(varMapper.find(meta.var) != varMapper.end())
				{
					delete object;
					return -1;
				}
				varMapper.insert(meta.var,object);
			}
#endif 
			return 1;
		}
        else if(statement.startsWith("buy."))
		{
			QString block = statement.mid(4);
			if(block.startsWith("ref."))
			{
				block = block.mid(4);
				CHECK_AND_BUILD(block,buyRefEvalutor)
			}
			else
			{
				CHECK_AND_BUILD(block,buyEvalutor)
			}
		}
	    else if(statement.startsWith("sale."))
		{	
			QString block = statement.mid(5);
			if (block.startsWith("ref."))
			{
				block = block.mid(4);
				CHECK_AND_BUILD(block,saleRefEvalutor)
			}
			else
			{
				CHECK_AND_BUILD(block,saleEvalutor)
			}
		}
		else
			return -1;
	}

	return 1;
}

void StockScriptEngine::checkRefStock(const QString& stock)
{
	if (dataProvider.stocks.size() < 6)
		return;

#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->clearImplicitVaraible();
#endif
	reloadStockData(stock);

	int lastEnd = dataProvider.stocks.size() - 1;
	QVector<float> arguments;

	int from = 5;
	int to = dataProvider.stocks.size();
	if (from > to)
		return;

	refBSSet.clear();
	StockTradeWriter::getInstance()->clear();

	QPair<QString,QString> node;

	for (int i = from;i<to;i++)
	{
		dataProvider.currentStockCursor = i;
		dataProvider.date = dataProvider.stocks[i].date;
		QString date = dataProvider.stocks[i].date;
		QString nextDate;
		if (i <= lastEnd-1)
			nextDate = dataProvider.stocks[i + 1].date;

		bool buy = isRefBuyPoint(i,arguments);
		bool sale = isRefSalePoint(i);

		if (buy && !sale)
			reclacStoreMapper(i);

		StockTradeWriter::getInstance()->add(stock,i,buy,sale,arguments);
		/*if (buy)
		{
			node.first = date;
			refBSSet += node;
		}
		
		if (sale)
		{
			node.second = date;
			refBSSet += node;
		}*/
	}

	auto ret = StockTradeWriter::getInstance()->getTradeItem();
	auto items = ret.item;

	foreach(auto item,items)
	{
		refBSSet += QPair<QString,QString>(item.buyDate,item.saleDate);
	}
	StockTradeWriter::getInstance()->clear();
}

void StockScriptEngine::checkStock(const QString& stock)
{
	if(dataProvider.stocks.size() < 6)
		return;

	if (isRefMode() && stock == config.refStock)
		return;

	if (config.runningMode == "pairtrade")
	{
		if(!pairTradeStocks.contains(stock))
			return;

		checkPairTradeStock(stock);
		return;
	}

#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->clearImplicitVaraible();
#endif
    reloadStockData(stock);

	int lastEnd = dataProvider.stocks.size()-1;

	if (config.runningMode == "选股")
	{
		QVector<float> arguments;
		bool buy = isBuyPoint(lastEnd,arguments);
		QString date = dataProvider.stocks[lastEnd].date;
		if (buy)
		{
			if((isRefMode() && containInRefSet(date)) || !isRefMode() )
				StockTradeWriter::getInstance()->add(stock,lastEnd,buy,false,arguments);
		}
	}
	else
	{
		QVector<float> arguments;
		int from = 5;
		int to = dataProvider.stocks.size();
		if (from > to)
			return;

		auto value = getStockActivteValue(dataProvider.stocks);
		if (value < config.minActiveStockValue || value > config.maxActiveStockValue)
			return;

		for (int i = from;;i++)
		{
			if (i >= to)
				return;

			dataProvider.currentStockCursor = i;
			dataProvider.date = dataProvider.stocks[i].date;
			QString date = dataProvider.stocks[i].date;

			bool buy = isBuyPoint(i,arguments);
			bool sale = isSalePoint(i);

			if (buy)
			{
				if(isRefMode())
				    if (!containInRefSet(date))
					    buy = false;
			}

			if(buy && !sale)
				reclacStoreMapper(i);

			i += StockTradeWriter::getInstance()->add(stock,i,buy,sale,arguments);

			if(i > config.maxListDatePos)
			{
				StockTradeWriter::getInstance()->clearCurrentTradeInfo();
				break;
			}
		}
	}
}

void StockScriptEngine::checkPairTradeStock(const QString& stock)
{
	if (dataProvider.stocks.size() < 6)
		return;

	if (isRefMode() && stock == config.refStock)
		return;

	reloadStockData(stock);

	PairTraderAlgo algo;
	algo.setConfiguration(&config);
	algo.run();

	QSet<QString> b1,s1,b2,s2;
	algo.getStockTradeInfo(b1,s1,b2,s2);

	QSet<QString> buyset,saleset;
	if (algo.getStock1() == stock)
	{
		buyset = b1;
		saleset = s1;
	}
	else
	{
		buyset = b2;
		saleset = s2;
	}

	int lastEnd = dataProvider.stocks.size() - 1;

	QVector<float> arguments;
	int from = 5;
	int to = dataProvider.stocks.size();
	if (from > to)
		return;

	for (int i = from;;i++)
	{
		if (i >= to)
			return;

		dataProvider.currentStockCursor = i;
		dataProvider.date = dataProvider.stocks[i].date;
		QString date = dataProvider.stocks[i].date;

		bool buy = isBuyPoint(i,arguments) && buyset.contains(date);
		bool sale = isSalePoint(i) || (saleset.contains(date));

		if (buy)
		{
			if (isRefMode())
				if (!(isRefMode() && containInRefSet(date)))
					buy = false;
		}

		if (buy && !sale)
			reclacStoreMapper(i);

		i += StockTradeWriter::getInstance()->add(stock,i,buy,sale,arguments);

		if (i > config.maxListDatePos)
		{
			StockTradeWriter::getInstance()->clearCurrentTradeInfo();
			break;
		}
	}
}

void StockScriptEngine::reloadStockData(const QString& stock)
{
#ifdef SUPPORT_BASIC
	StockScriptList::GetInstance().getVarMetaManager()->clear();
#endif

    foreach(auto evalutor,buyEvalutor)
	{
		evalutor->clear();
		evalutor->loadData(dataProvider.stocks);
	}

    foreach(auto evalutor,saleEvalutor)
	{	
		evalutor->clear();
		evalutor->loadData(dataProvider.stocks);
    }

	foreach(auto evalutor,buyRefEvalutor)
	{
		evalutor->clear();
		evalutor->loadData(dataProvider.stocks);
	}

	foreach(auto evalutor,saleRefEvalutor)
	{
		evalutor->clear();
		evalutor->loadData(dataProvider.stocks);
	}

#ifdef SUPPORT_BASIC
	foreach(auto ptr,varMapper)
	{	
		ptr->clear();
		ptr->loadData();
	}

	foreach(auto ptr,storeMapper)
	{
	    ptr->clear();
		ptr->loadData();
	}
#endif
}

void StockScriptEngine::reclacStoreMapper(int pos)
{
#ifdef SUPPORT_BASIC
	auto itr = storeMapper.begin();
	while(itr != storeMapper.end())
	{
		itr.value()->execute(pos);
		itr ++;
	}
#endif
}

QStringList StockScriptEngine::getStockTicketFileList(const QString& stock)
{
	QStringList items;
	QDir dir(QString("ticket/%1").arg(stock));
    if(!dir.exists())
        return items;
 
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

	QFileInfoList list = dir.entryInfoList();
    foreach(QFileInfo info,list)
    {
		QString file = info.absoluteFilePath();
		items.append(file);
    }
	return items;
}

#ifdef SUPPORT_BASIC
VarMeta* StockScriptEngine::getVarMetaByVar(const QString& var)
{
	if(varMapper.find(var) == varMapper.end())
		return nullptr;
	return varMapper[var];
}

StoreMeta* StockScriptEngine::getStoreMetaByVar(const QString& var)
{
	if(storeMapper.find(var) == storeMapper.end())
		return nullptr;
	return storeMapper[var];   
}
#endif

bool StockScriptEngine::isBuyPoint(int pos,QVector<float>& arguments)
{
	QVector<float> output;
	foreach(auto ptr,buyEvalutor)
	{
		if (!ptr->check(pos,arguments))
			return false;
		else
		{
			output += arguments;
		}
	}
	arguments = output;
	return true;
}

bool StockScriptEngine::isRefBuyPoint(int pos,QVector<float>& arguments)
{
	QVector<float> output;
	foreach(auto ptr,buyRefEvalutor)
	{
		if (!ptr->check(pos,arguments))
			return false;
		else
		{
			output += arguments;
		}
	}
	arguments = output;
	return true;
}

bool StockScriptEngine::isSalePoint(int pos)
{
	QVector<float> arguments;
	foreach(auto ptr,saleEvalutor)
	{
		if (ptr->check(pos,arguments))
			return true;
	}
	return false;
}

bool StockScriptEngine::isRefSalePoint(int pos)
{
	QVector<float> arguments;
	foreach(auto ptr,saleRefEvalutor)
	{
		if (!ptr->check(pos,arguments))
			return true;
	}
	return false;
}

void StockScriptEngine::appendArgument(const QString& argument)
{
	arguments.append(QString("\"%1\"").arg(argument)); 
}

bool StockScriptEngine::isRefMode()
{
#ifdef SUPPORT_REF
	if (!config.refStock.isEmpty())
	{
		if (isStockCode(config.stockMarket,config.refStock))
			return true;
	}
#endif
	return false;
}

bool StockScriptEngine::containInRefSet(const QString& date)
{
	int maxDay = config.maxHoldDay;
	auto itr = refBSSet.begin();
	while (itr != refBSSet.end())
	{
		QString endDate = itr->second;
		/*if (endDate.isEmpty())
		{
			QDate d = QDate::fromString(itr->first,"yyyy-MM-dd");
			d = d.addDays(maxDay);
			endDate = d.toString("yyyy-MM-dd");
		}*/

		if (date >= itr->first && date < itr->second)
			return true;
		itr ++;
	}
	return false;
}