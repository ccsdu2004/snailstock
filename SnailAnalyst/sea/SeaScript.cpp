#include <QDir>
#include "SeaScript.h"
#include "../common/common.h"
#include "../common/FileUtils.h"
#include "../sea/SeaScript.h"
#include "../eval/cppeval.h"

QString SeaScript::replace(const QString& temp,const QStringList& parameters)
{
	QString str(temp);
	for (int i = 0;i<parameters.size();i++)
		str = str.replace(arguments[i],parameters[i]);
	return str;
}

QStringList SeaScript::build(const QStringList& parameters,bool buy)
{
    QStringList output;

	if (!var.isEmpty())
		output.append(replace(var,parameters));

	foreach(auto item,items)
	{
	    QString unit;
		if(buy)
			unit = "buy.";
		else
			unit = "sale.";

		if (!item.startsWith("@"))
			unit += item;
		else
			unit += item;

		unit = replace(unit,parameters);
		output.push_back(unit);
	}

	auto tag = SeaScriptManager::getInstance()->getCurrentVarTag();

	QStringList items;
	foreach(auto item,output)
	{
		item = item.replace(STD_VAR_NAME,tag);
		items += item;
	}
	return items;
}

SeaScriptManager SeaScriptManager::instance;

SeaScriptManager* SeaScriptManager::getInstance()
{
    return &instance;
}

int SeaScriptManager::loadSeaScripts(const QString& dirname)
{
    QDir dir(dirname);
	QFileInfoList files = dir.entryInfoList(QDir::Files);
 
    for(int i = 0;i != files.size();i++)
		loadScript(files.at(i).absoluteFilePath());
	return seaScripts.size();
}

void SeaScriptManager::loadScript(const QString& file)
{
	if(!file.endsWith(".sea"))
		return;

	script.file = file;
	QString buffer = readAllBufferFromFile(file);
	parse(buffer);
}

QStringList SeaScriptManager::getSeaScriptKeys()const
{
	return seaScripts.keys();
}

SeaScript SeaScriptManager::getSeaScript(const QString& name)
{
    return seaScripts[name];
}

void SeaScriptManager::doLine(const QString& tag,const QString& value)
{
	if (tag == "command")
		script.command = value.trimmed();
	else if (tag == "type")
		script.type = value.trimmed();
	else if (tag == "arguments")
		script.arguments.append(value.trimmed().split(","));
	else if (tag == "var")
		script.var = value.trimmed();
	else if (tag == "items")
	{
		script.items.append(value.trimmed());
	}
}

void SeaScriptManager::doAfter()
{
	seaScripts.insert(script.command,script);
	
	script.arguments.clear();
	script.command.clear();
	script.file.clear();
	script.items.clear();
	script.var.clear();
	script.type.clear();
}

QStringList SeaScriptManager::buildScripts(const QString& expr)
{
	QStringList output;
	if(expr.startsWith("#") || expr.startsWith("config.") || expr.startsWith("@"))
		output.append(expr);
	else if(expr.startsWith("buy."))
	{
		if(expr.startsWith("buy.sea."))
		{
			QStringList list = expr.mid(8).split(",");
			if(list.size() == 1)
				output.append(expr);
			else
			{
				auto keys = getSeaScriptKeys().toSet();
				bool has = keys.contains(list[0]);
				if(!has)
					output.append(expr);
				else
				{
				    SeaScript script = getSeaScript(list[0]);
					output.append(script.build(list.mid(1),true));
				}
			}
		}
		else
			output.append(expr);
	}
	else if(expr.startsWith("sale."))
	{
		if(expr.startsWith("sale.sea."))
		{
			QStringList list = expr.mid(9).split(",");
			if(list.size() == 1)
				output.append(expr);
			else
			{
			    auto keys = getSeaScriptKeys();
				int index = keys.indexOf(list[0]);
				if(index == -1)
					output.append(expr);
				else
				{
				    SeaScript script = getSeaScript(list[0]);
					output.append(script.build(list.mid(1),true));
				}
			}
		}
		else
			output.append(expr);
	}
	return output;
}

QString SeaScriptManager::getCurrentVarTag()
{
	return QString("@SEA_VAR_%1").arg(index++);
}