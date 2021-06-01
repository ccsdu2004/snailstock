#ifndef SCRIPT_MANAGER_H
#define SCRIPT_MANAGER_H
#include <QString>
#include <QStringList>
#include <QVector>
#include <qdialog.h>
#include <qsharedpointer.h>
#include <qtablewidget.h>
#include "config.h"
#ifdef SUPPORT_SCRIPT
#include <include/Python.h>
#endif
#include "SnailAddonHost.h"

class ScriptAddonView : public QDialog
{
    Q_OBJECT
public:
	ScriptAddonView(QWidget* parent = nullptr);
	~ScriptAddonView();
private:
	QTableWidget* table;
	QPushButton* button;
};

#ifdef SUPPORT_SCRIPT

#define ADDON_VERSION "get_version"
#define ADDON_NAME    "get_name"
#define ADDON_DESCRIPTION "get_description"
#define ADDON_RETTYPE  "get_return_type"
#define ADDON_EVALUTE_CANDLESTICK "evalute_candlestick"

struct ScriptAddon
{
    PyObject* pName;
    PyObject* pModule;
    PyObject* pDict;
	QString filename;

	QString version;
	QString name;
	QString description;
	QString retType;
	PyObject* function;

    ScriptAddon();
    ~ScriptAddon();
    bool loadFromFile(const QString& file);
private:
    PyObject* loadFunction(const QString& function);
};
/*
inline QString callAddonFunction(QSharedPointer<ScriptAddon> addon,const QString& function)
{
	QString value;
	if(addon->call(function,value))
		return value;
	value.clear();
	return value;
}*/

class ScriptManager
{
public:
	static ScriptManager& instance();

	bool loadScripts(const QString& dir);
	void cleanup();

	QStringList keywords();
	QVector<QSharedPointer<ScriptAddon>> getScriptInfos();
	QSharedPointer<ScriptAddon> getScript(const QString& script);
	QString evalute(const QString& script,const QString& code,const QVector<Stock>& stocks);
	bool hasScript(const QString& script);
private:
	ScriptManager();
	~ScriptManager();

	bool loadScript(const QString& script,const QString& basename);

	QMap<QString,QSharedPointer<ScriptAddon>> scripts;
};

#endif
#endif