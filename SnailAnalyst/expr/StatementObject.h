#ifndef STATEMENT_OBJECT_H
#define STATEMENT_OBJECT_H
#include <QVector>
#include "MetaEvalutor.h"

class StatementObject
{
public:
	StatementObject();
	virtual ~StatementObject();
public:
	virtual bool build(const QString& statement);
	virtual void clear();
	virtual void loadData(const QVector<Stock>& stocks);
	virtual bool check(int pos,QVector<float>& arguments);
private:
	QVector<MetaEvalutor*> objects;
	Comparator* comparator;
};

class StatementObjectNode : public StatementObject
{
public:
	~StatementObjectNode();
public:
	bool build(const QString& statement);
	void clear();
	void loadData(const QVector<Stock>& stocks);
	bool check(int pos,QVector<float>& arguments);
private:
	QVector<StatementObject*> list;
};

#endif 