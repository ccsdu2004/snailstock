#ifndef QUANT_STATEMENT_H
#define QUANT_STATEMENT_H
#include <QStringList>
#include <QVariant>
#include "SnailFactory.h"
#include "../Stock.h"

#pragma execution_character_set("utf-8")

class QuantStatement
{
public:
	enum Span
	{
		normal = 0,
		day5,day10,day20,
		minute60,minute30,minute15,minute10,minute5
	};

	virtual ~QuantStatement(){}

	virtual QStringList getKeywords()const = 0;
	virtual QString getMeta()const = 0;
	virtual Span getSpan()const = 0;
	virtual bool returnBool()const = 0;
	virtual bool prepare(const QString& meta,const QVector<double>& argument) = 0;
	virtual bool evalute(int offset,QVariant& result,const QString& tag = QString()) = 0;
	virtual void clear() = 0;

	static DataWrapper3<Span,QString,QVector<double>> splitArgument(const QString& command);
	static QVector<Stock> fetchStockData(const QString& stock,int pos,Span span);
	static DataWrapper5s clacData(const QVector<Stock>& stocks,StockIndicator* indicator);
};

class QuantComparator;

class QuantStatementObject
{
public:
	QuantStatementObject();
	virtual ~QuantStatementObject();

	static QStringList splitStatement(const QString& statement,const QString& regex);
public:
	virtual bool build(const QString& statement);
	virtual void clear();
	virtual bool evalute(int pos);
private:
	QVector<QuantStatement*> objects;
	QuantComparator* comparator;
};

class QuantStatementObject_OR : public QuantStatementObject
{
public:
	~QuantStatementObject_OR();
public:
	bool build(const QString& statement);
	void clear();
	bool evalute(int pos);
private:
	QVector<QuantStatementObject*> list;
};

class QuantStatementFactory
{
public:
	QuantStatement* create(const QString& command);
	QStringList keywords()const;
};

#endif