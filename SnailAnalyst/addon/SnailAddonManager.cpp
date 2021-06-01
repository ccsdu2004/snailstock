#include <QDir>
#include <QDebug>
#include "SnailAddonManager.h"

SnailAddonManager::SnailAddonManager()
{
}

SnailAddonManager::~SnailAddonManager()
{
}

void SnailAddonManager::addAddonInDir(const QString& dirName)
{
	QDir dir(dirName);
	dir.setFilter(QDir::Files | QDir::NoSymLinks);

	QFileInfoList list = dir.entryInfoList();
	int count = 0;
	foreach(QFileInfo info,list)
	{
		QString file = info.absoluteFilePath();
		addAddon(file);
	}
}

void SnailAddonManager::addAddon(const QString& addon)
{
	loader.setFileName(addon);
	if(!loader.load())
	{
		qDebug() << "bad addon file:"<<addon;
		return;
	}

	auto object = loader.instance();
	if (object)
	{
		SnailAddon* snail = qobject_cast<SnailAddon*>(object);
		if (!snail)
			qDebug() << "bad addon snail object";
		else
		{
			addons += snail->tag();
		}
	}
	else
		qDebug() << "bad snail addon instance:" << addon;
}
	
void SnailAddonManager::release()
{
}

int SnailAddonManager::getAddonCount()
{
	return addons.size();
}

SnailAddonInfo SnailAddonManager::addonInfo()const
{
	static SnailAddonInfo info;
	return info;
}

QString SnailAddonManager::tag()const
{
	return tags().join(",");
}

QStringList SnailAddonManager::tags()const
{
	return addons;
}

bool SnailAddonManager::build(const QString& label,const QVector<double>& arguments)
{
	auto cmds = tags();
	if(!cmds.contains(label))
		return false;




	return true;
}
	
bool SnailAddonManager::load(const QVector<Stock>& stocks)
{
	return false;
}

bool SnailAddonManager::evalute(int pos, bool& fitness)
{
	return false;
}

void SnailAddonManager::clear()
{
}