#include "StockThreadTarget.h"
#include "config.h"

StockThreadTarget::StockThreadTarget(StockTargetCallback cbp,QObject* parent):
QThread(parent),
targetCallback(cbp)
{
}

StockThreadTarget::~StockThreadTarget()
{
}

void StockThreadTarget::setData(const QVector<DataWrapper2<QString,QString>>& scriptlist)
{
    if(isRunning())
		return;

	scripts = scriptlist;
}

void StockThreadTarget::run()
{
	QString remark;
	foreach(auto item,scripts)
	{
		bool ok = targetCallback(item._1,item._2,remark);
		if(!ok)
			break;

#ifndef SUPPORT_BATCH_SCRIPT
		break;
#endif
	}

	flagFinishedScript.submit(remark);
}
