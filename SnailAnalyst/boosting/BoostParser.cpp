#include <QStringList>
#include <algorithm>
#include "BoostingParser.h"
#include "ConfigurationParser.h"
#include "common/common.h"
#include "math/math.h"
#include "expr/VarMeta.h"
#include "dialog/SnailStockMsgBox.h"
#include <qdebug.h>

#define MAX_BOOSTING_COUNT 99999999

#ifdef SUPPORT_BOOSTING_ALGO

const static QSet<QString> varKeyMap = 
{ 
	"@BOOSTING_VAR1","@BOOSTING_VAR2","@BOOSTING_VAR3",
	"@BOOSTING_VAR4","@BOOSTING_VAR5","@BOOSTING_VAR6",
	"@BOOSTING_VAR7","@BOOSTING_VAR8","@BOOSTING_VAR9" 
};

class ArrPool
{
public:
    void update(const QStringList& items);
    auto get()const->QVector<QStringList>
    {
        return list;
    }
private:
    QVector<QStringList> list;
};

void ArrPool::update(const QStringList& items)
{
    if(list.empty())
    {
        auto itr = items.begin();
        while (itr != items.end())
        {
            QStringList item;
            item.push_back(*itr);
            list.push_back(item);
            itr++;
        }
    }
    else
    {
        int count = items.size();
        auto newlist = list;
        list.clear();
        auto itr = newlist.begin();
        while (itr != newlist.end())
        {
            auto item = *itr;
            for (int i = 0; i < count; i++)
                list.push_back(item);
            itr++;
        }

        itr = list.begin();
        int i = 0;
        while (itr != list.end())
        {
            itr->push_back(items[i%count]);
            i++;
            itr++;
        }
    }
}

BoostingConfig BoostingParser::boostConfig;

QStringList BoostingParser::keywords()
{
    QStringList words;
	words.push_back("boosting.做多成功率");
	words.push_back("boosting.做多最大回撤");
	words.push_back("boosting.做多平均单笔收益");
	words.push_back("boosting.最小交易次数");
	words.push_back("boosting.做空成功率");
	words.push_back("boosting.做空最小回撤");
	words.push_back("boosting.做空平均单笔收益");
	words.push_back("boosting.单股分析");
	words.push_back("boosting.显示所有");
	words.push_back("@BOOSTING_VAR1");
	words.push_back("@BOOSTING_VAR2");
	words.push_back("@BOOSTING_VAR3");
	words.push_back("@BOOSTING_VAR4");
	words.push_back("@BOOSTING_VAR5");
	words.push_back("@BOOSTING_VAR6");
	words.push_back("@BOOSTING_VAR7");
	words.push_back("@BOOSTING_VAR8");
	words.push_back("@BOOSTING_VAR9");
	words.push_back("list");
	words.push_back("for");
	return words;
}

bool BoostingParser::show(int count,double success,double avg,double maxdown)
{
	if (boostConfig.showAll)
		return true;
	if(boostConfig.longProfit <= avg &&
		boostConfig.longRetreats <= success &&
		boostConfig.longSuccess <= maxdown &&
		boostConfig.minTradeCount <= count)
		return true;
	return false;
}

void BoostingParser::parse(const QString& name,const QString& value)
{
	if (name == "boosting.做多成功率")
		boostConfig.longSuccess = std::max(40.0, value.toDouble());
	else if (name == "boosting.做多最大回撤")
	{
		double d = value.toDouble();
		if (d > 0)
			d = -d;
		if (d > -5.0)
			d = -15.0;
		boostConfig.longRetreats = std::max(-36.0, d);
	}
	else if (name == "boosting.做多平均单笔收益")
		boostConfig.longProfit = std::max(-10.0, value.toDouble());
	else if (name == "boosting.最小交易次数")
		boostConfig.minTradeCount = std::max(5, value.toInt());
	else if (name == "boosting.单股分析")
		boostConfig.forEachStock = value == "yes" || value == "true";
	else if (name == "boosting.显示所有")
		boostConfig.showAll = value == "yes" || value == "true";
}

BoostingParser::BoostingParser()
{
}

bool BoostingParser::parse(const QString& buffer)
{
	QStringList list = buffer.split("\n");
	foreach(auto item, list)
	{
		bool add = true;
		QPair<QString, QString> node;
		if (item.startsWith("boosting."))
		{
			add = false;
			auto keyValue = item.split("=");
			if (keyValue.size() == 2)
				parse(keyValue[0], keyValue[1]);
		}
		else if (item.startsWith("config.source.pool="))
		{
			QStringList pair = item.split("=");
			if (pair.size() == 2)
				stockPool = pair[1];
		}
	}

	list = buffer.split("\n");
	foreach(auto item,list)
	{
		bool add = true;
		QPair<QString,QString> node;
		if(item.startsWith("boosting."))
		{
			add = false;
			auto keyValue = item.split("=");
			if(keyValue.size() == 2)
			    parse(keyValue[0],keyValue[1]);
		}
		else if (item.startsWith("@BOOSTING_VAR"))
		{
			VarMeta::Meta meta;
			bool ok = VarMeta::parseMeta(item,meta);
			if (!ok)
				return false;

			if (!varKeyMap.contains(meta.var))
				return false;

			if (meta.operation != "for" && meta.operation != "list")
				return false;

			auto var = BoostingVar::create(meta.operation);
			if (!var)
				return false;

			if (!var->evalute(meta.var,meta.arguments.join(";")))
				return false;

			add = false;
			varKeys.append(meta.var);
			varValues += var->values;
			delete var;
		}
		else if (!item.startsWith("items="))
		{
			if(boostConfig.forEachStock == true && item.startsWith("config.source.pool="))
			{
				stocksInStockPool = getStocksByInput(stockPool);
				add = false;
			}
			else
				node.second = item;
		}
		else
		{
			QString bf = item.mid(6);
			if (bf.startsWith("buy."))
			{
				node.first = "buy";
				node.second = bf.mid(4);
			}
			else if (bf.startsWith("sale."))
			{
				node.first = "sale";
				node.second = bf.mid(5);
			}
			else
			{
				//config.parse();
				continue;
			}
		}

		if(add)
			items.append(node);
	}

	if(!boostConfig.forEachStock)
	{
		if(!stockPool.isEmpty())
		{
			QPair<QString, QString> node;
			node.second = "config.source.pool=" + stockPool;
			items.append(node);
		}
	}
	else
	{
		if (stockPool.isEmpty())
		{
			POP_MSGBOX(QString("错误"),QString("boosting.单股分析 - 需要指定股票池"),nullptr)
			return false;
		}

		stocksInStockPool = getStocksByInput(stockPool);
		if (stocksInStockPool.size() == 0)
		{
			POP_MSGBOX(QString("错误"), QString("boosting.单股分析 - 指定股票池内无股票"),nullptr)
			return false;
		}
	}


	return buildScripts();
}

bool BoostingParser::buildScripts()
{	
	QVector<QStringList> scripts;
	foreach(auto node,items)
	{
		if(node.first.isEmpty())
			scripts.push_back(QStringList(node.second));
		else
		{
			QStringList tuple = node.second.split("^");
			QStringList statement;
			foreach(QString item,tuple)
			{
				statement.push_back(node.first + "." + item);
			}
			scripts.push_back(statement);
		}
	}

	ArrPool pool;
	foreach(auto script,scripts)
	{
		pool.update(script);
	}

	QVector<QStringList> allScripts = pool.get();
	allScripts = generateStringListAndReplaceKeyByValue(allScripts,varKeys,varValues);

	if(boostConfig.forEachStock)
	{
		auto temp = allScripts;
		allScripts.clear();
		foreach(auto unit,temp)
		{
			foreach(auto stock, stocksInStockPool)
			{
				auto items = unit;
				auto stockConfig = "config.source.pool=" + stock;
				QStringList current(unit);
				current.append(stockConfig);
				allScripts.push_back(current);
			}
		}
	}

	{
		int i = 0;
		foreach(auto script, allScripts)
		{
			auto item = script.join("\n").trimmed();
			if (!item.isEmpty())
				scriptList.push_back(item);
			i++;

			if (i > MAX_BOOSTING_COUNT)
				break;
		}
	}
 
	return true;
}

#endif