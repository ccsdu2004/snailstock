#ifndef VAR_EVALUTOR_H
#define VAR_EVALUTOR_H
#include <QVector>
#include <QStringList>
#include <QObject>
#include "config.h"
#include "expr/MetaEvalutor.h"

#ifdef SUPPORT_BASIC

struct VarMeta
{
	struct Meta
	{
		QString var;
		QString operation;
		QStringList arguments;
	};

	Meta meta;

	static bool parseMeta(const QString& input,Meta& meta);
	static bool parseExpress(const QString& input,QString& express);

	static VarMeta* create(const QString& function);
	static QStringList keywords();
	
	virtual ~VarMeta();
	const QString name()const{return meta.var;}
	const QString str()const{ return QString("%1@%2").arg(meta.operation).arg(meta.arguments.join(",")); }
	const QString functor()const{return meta.operation;}
	virtual bool build(const Meta& meta);

	virtual bool execute(int pos,ResultType& output) = 0;
	bool fetch(int pos,ResultType& output);
	virtual bool loadData();
	virtual bool needReCacluate(){ return false; }
	void clear();
protected:
	QVector<MetaEvalutor*> evalutors;
};

//@@VAR=ma,5,0
struct StoreMeta
{
public:
    StoreMeta()
    {
        hasData = false;
        value = 0.0;
        meta= nullptr;       
    }
   
    ~StoreMeta()
    {
        if(meta)
            delete meta;
        meta = nullptr;
    }

    QString name;
    bool hasData;
    double value;
    bool build(const QString& var,const QString& function)
    {  
        name = var;
        QString key = function.split(",")[0];
        meta = MetaEvalutor::create(key,MetaEvalutor::normal);
		QVector<double> arguments;
        if(!meta)
            return false;
        if(!meta->build(function,arguments))
            return false;
        return true;    
    }

    bool execute(int pos)
    {
        hasData = false;
        ResultType ret;
        if(!meta->evalute(pos,ret))
            return false;
        if(ret.type != ResultType::TYPE_DOUBLE)
            return false;
        value = ret.double_;
        hasData = true;
        return true;
    }
    void loadData()
    {
        if(meta)
            meta->loadData();
    }
	void clear()
	{
	    if(meta)
			meta->clear();
	}
private:
    MetaEvalutor* meta;
};

#endif 
#endif 