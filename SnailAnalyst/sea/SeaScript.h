#ifndef SEA_SCRIPT_H
#define SEA_SCRIPT_H
#include <QString>
#include <QStringList>
#include <QMap>
#include "config.h"
#include "../common/abstractFileReader.h"

#define STD_VAR_NAME "@VAR"

/*
command=放量
type=common
arguments=bit,offset
var=@VAR=express[vol(offset)/vol(offset-1)]
items=@VAR>value,bit

调用
sea.放量,1.12,0
*/

struct SeaScript
{
    QString file;
	QString command;
	QString type;
	QStringList arguments;
	QString var;
	QStringList items;

	QString replace(const QString& temp,const QStringList& parameters);
	QStringList build(const QStringList& parameters,bool buy = true);
};

class SeaScriptManager : public abstractFileReader
{
public:
	static SeaScriptManager* getInstance();
	int loadSeaScripts(const QString& dir);

	void reset(){ index = 0; }
	QString getCurrentVarTag();

	QStringList getSeaScriptKeys()const;
	SeaScript getSeaScript(const QString& name);

	QStringList buildScripts(const QString& expr);
private:
	void loadScript(const QString& file);
	void doLine(const QString& tag,const QString& value);
	void doAfter();

	SeaScriptManager(){}
	
	SeaScript script;
	QMap<QString,SeaScript> seaScripts;
	int index;

	static SeaScriptManager instance;
};

#endif