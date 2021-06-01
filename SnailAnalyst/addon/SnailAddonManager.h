#ifndef SnailAddonManager_HPP
#define SnailAddonManager_HPP
#include <qplugin.h>
#include <QPluginLoader>
#include <QMap>
#include "SnailAddon.h"

class SnailAddonManager : SnailAddon
{
	Q_OBJECT
public:
	SnailAddonManager();
	~SnailAddonManager();
public:
	void addAddonInDir(const QString& dir);
	void addAddon(const QString& addon);
	void release();

	int  getAddonCount();
public:
	SnailAddonInfo addonInfo()const;
	QString tag()const;
	QStringList tags()const;
    bool build(const QString& label,const QVector<double>& arguments);
    bool load(const QVector<Stock>& stocks);
    bool evalute(int pos, bool& fitness);
    void clear();
private:
	QPluginLoader loader;
	QStringList addons;
	SnailAddon* snailAddon = nullptr;
	QString current;
};

#endif