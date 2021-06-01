#ifndef STOCKTHREADTARGET_H
#define STOCKTHREADTARGET_H
#include <QThread>
#include <QFileInfo>
#include "Stock.h"
#include "common/Sigslot.h"

typedef bool (*StockTargetCallback)(const QString& desc,const QString& script,QString& error);

class StockThreadTarget : public QThread
{
	Q_OBJECT
public:
    StockThreadTarget(StockTargetCallback cbp,QObject* parent = 0);
    ~StockThreadTarget();

	void setData(const QVector<DataWrapper2<QString,QString>>& scripts);

	Signal1<QString> flagFinishedScript;
protected: 
    void run();
private:
	StockTargetCallback targetCallback;
	QVector<DataWrapper2<QString,QString>> scripts;
};

#endif 