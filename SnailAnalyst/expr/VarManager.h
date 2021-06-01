#ifndef VAR_MANAGER_H
#define VAR_MANAGER_H
#include <qobject.h>
#include "expr/VarMeta.h"

class VarMetaManager : public QObject
{
	Q_OBJECT
public:
	VarMetaManager(QObject* parent = nullptr):
		QObject(parent)
	{
		const QString templatevar("@IMPLICIT_VAR%1");
		for (int i = 0;i < 20;i++)
		{
			QString str = QString(templatevar).arg(i+1);
			implicitVars.insert(str,i);
		}
	}

	void clear()
	{
		data.clear(); 
		registerVar.clear();
	}

	bool hasVariable(const QString& var,int index);
	QPair<ResultType,bool> variable(const QString& var,int index);

	void addVariable(const QString& var,int index,const ResultType& result,bool flag);

	void clearImplicitVaraible(){ registerVar.clear(); }

	void addRegisterVar(const QPair<double,QString>& value);
	bool getRegisterVar(const QString& name,QPair<double,QString>& value);
private:
	QMap<QString,int> implicitVars;

	QMap<QPair<QString,int>,QPair<ResultType,bool>> data;
	QVector<QPair<double,QString>> registerVar;
};


#endif