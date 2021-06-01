#include <QDir>
#include "ScriptManager.h"

#ifdef SUPPORT_SCRIPT

PyObject* stockToPyDict(const Stock& stock)
{
    PyObject* dict = PyDict_New();
	PyDict_SetItemString(dict,"date",Py_BuildValue("s",stock.date.toLatin1().data()));
	PyDict_SetItemString(dict,"open",Py_BuildValue("f",stock.open));
	PyDict_SetItemString(dict,"high",Py_BuildValue("f",stock.top));
	PyDict_SetItemString(dict,"low",Py_BuildValue("f",stock.low));
	PyDict_SetItemString(dict,"close",Py_BuildValue("f",stock.current));
	PyDict_SetItemString(dict,"volume",Py_BuildValue("f",stock.tradeVolume));
	PyDict_SetItemString(dict,"increase",Py_BuildValue("f",stock.increase));
	return dict;
}

ScriptManager::ScriptManager()
{
    Py_Initialize();  
}

ScriptManager::~ScriptManager()
{
    Py_Finalize();     
}

ScriptManager& ScriptManager::instance()
{
    static ScriptManager mgr;
    return mgr;
}

bool ScriptManager::loadScripts(const QString& dirname)
{
	if(!Py_IsInitialized())   
		return false;

	PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");
    QString str = QString("sys.path.append('./%1')").arg(dirname);

	PyRun_SimpleString(str.toLatin1().data());

    QDir dir(dirname);
	QFileInfoList files = dir.entryInfoList(QDir::Files);
 
    for(int i = 0;i != files.size();i++)
		loadScript(files.at(i).absoluteFilePath(),files.at(i).baseName());
 
	return !scripts.isEmpty();  
}

void ScriptManager::cleanup()
{
	scripts.clear();
}

QStringList ScriptManager::keywords()
{
    QStringList list;
	auto itr = scripts.begin();
	while(itr != scripts.end())
	{
		list.push_back(itr.value()->name);
		itr ++;
	}

	return list;
}

QVector<QSharedPointer<ScriptAddon>> ScriptManager::getScriptInfos()
{
    QVector<QSharedPointer<ScriptAddon>> list;
	auto itr = scripts.begin();
	while(itr != scripts.end())
	{
		list.push_back(itr.value());
		itr ++;
	}

	return list;
}

QSharedPointer<ScriptAddon> ScriptManager::getScript(const QString& script)
{
	QSharedPointer<ScriptAddon> var;
	auto itr = scripts.find(script);
	if(itr == scripts.end())
		return var;
	return itr.value();
}

bool ScriptManager::hasScript(const QString& script)
{
	return scripts.find(script) != scripts.end();
}

QString ScriptManager::evalute(const QString& script,const QString& code,const QVector<Stock>& stocks)
{
	/*if(scripts.find(script) == scripts.end())
		return QString("null");

	auto var = scripts[script];

	PyObject* stockList = PyTuple_New(stocks.size());
	for(int i=0;i<stocks.size();i++)
	{
		auto stock = stocks.at(i);
		auto dict = stockToPyDict(stock);
		PyTuple_SetItem(stockList,i,dict);
	}

    PyObject* args = PyTuple_New(2);
	PyTuple_SetItem(args,0,Py_BuildValue("s",code.toLatin1().data()));
	PyTuple_SetItem(args,1,stockList);
	
	PyObject* pRet = PyObject_CallObject(var->,args);
    char* buffer = PyString_AsString(pRet);
	QString ret(buffer);

	Py_DECREF(args);*/
	return QString();
}

bool ScriptManager::loadScript(const QString& file,const QString& base)
{
	if(!file.endsWith(".py"))
		return false;

	QString pyname = file.mid(0,file.size()-3); 
	QSharedPointer<ScriptAddon> addon(new ScriptAddon());

	bool ok = addon->loadFromFile(file);
	if(!ok)
		return false;

	QString name = addon->name;
	if(name.isEmpty())
		return false;
	scripts.insert(name,addon);
	return true;
}

#endif 