#ifndef META_EVALUTOR_H 
#define META_EVALUTOR_H
#include <QStringList>
#include <QVector>
#include <QObject>
#include "Stock.h"
#include "Indicator.h"
#include "common/common.h"

#pragma execution_character_set("utf-8")

struct MetaEvalutor
{
	enum Span
	{
		normal = 0,
		day5,day10,day20,
		minute5,minute10,minute15,minute30,minute60
	};

	static MetaEvalutor* create(const QString& tag,Span span);
	static QStringList   keywords();

	static QStringList   splitStatement(const QString& statement,const QString& regex);
	static DataWrapper3<Span,QString,QVector<double>> splitArgument(const QString& command);
	static DataWrapper5s getIndicatorValue(const QVector<Stock>& stocks,StockIndicator* indicator);

	static QVector<Stock> getTickData(const QString& code,int current,int k);

	MetaEvalutor():span(normal){}
	MetaEvalutor(Span spn):span(spn){}
	virtual ~MetaEvalutor();

	Span getSpan()const{ return span; }
	virtual const QStringList tag()const = 0;
	virtual bool returnBool()const = 0;
	virtual bool build(const QString& label,const QVector<double>& arguments) = 0;
	
	virtual bool evalute(int pos,ResultType& output) = 0;
	virtual bool loadData() = 0;
	virtual bool isConstValue()const { return false; }
	virtual void clear() = 0;

	const QString& getOptimizeStringKey()const{return optimizeStringKey;}
	void setOptimizeStringKey(const QString& key){ optimizeStringKey = key; }
protected:
	Span span;
	QString optimizeStringKey;
};

struct Comparator
{
	virtual ~Comparator(){}
	//1 ture -1 false 0 
	virtual bool compare(const QVector<MetaEvalutor*>& input,int pos,QVector<float>& arguments) = 0;
	static Comparator* create(const QString& tag);
	static QStringList tags();
};

#endif 