#ifndef SNAIL_CONTEXT_H
#define SNAIL_CONTEXT_H
#include <QString>
#include <QMap>
#include <QMutex>
#include "config/ConfigItem.h"

class SnailContext
{
public:
	static QStringList getIntelligentPrompts();
	static SnailContext* open(const QString& tag = QString());
	void shutdown();
public:
	ConfigFactory* getConfigFactory(){return configFactory;}
private:
	SnailContext(const QString& tag);
public:
	QString name;
	ConfigFactory* configFactory;

	static QMutex  mutex;
	static QMap<QString,SnailContext*> contexts;
};

#endif