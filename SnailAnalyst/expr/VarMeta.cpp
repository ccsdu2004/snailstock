#include <limits>
#include "VarMeta.h"
#include "DataProvider.h"
#include "StatementObject.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"
#include "eval/cppeval.h"
#include "math/math.h"

#ifdef SUPPORT_BASIC

QStringList VarMeta::keywords()
{
	QStringList items;
	items.append("abs");
	items.append("floor");
	items.append("ceil");
	items.append("plus");
	items.append("minus");
	items.append("multi");
	items.append("div");
	items.append("min");
	items.append("max");
	items.append("express");
	items.append("equal");
	items.append("distance");
	items.append("minVol");
	items.append("maxVol");
	items.append("minClose");
	items.append("minOpen");
	items.append("minHigh");
	items.append("minLow");
	items.append("maxHigh");
	items.append("maxLow");
	items.append("maxClose");
	items.append("maxOpen");
	items.append("minVar");
	items.append("maxVar");
	items.append("plusVar");
	items.append("pow");
	items.append("log");
	items.append("if");
	items.append("ma");
	items.append("ema");
	items.append("sma");
	items.append("variance");
	items.append("absdev");
	items.append("stdev");
	items.append("prev");
	return items;
}

bool VarMeta::parseMeta(const QString& input,Meta& meta)
{
	if(!input.startsWith("@"))
		return false;

	if(!input.endsWith("]"))
		return false;

	int pos = input.indexOf("=");
	if(pos == -1)
		return false;

	meta.var = input.mid(0,pos);
	QString segment = input.mid(pos+1);
	segment = segment.mid(0,segment.size()-1);

	pos = segment.indexOf("[");
	if(pos == -1)
		return false;

	meta.operation = segment.mid(0,pos);
	meta.arguments = segment.mid(pos+1).split(";");
	return true;
}

bool VarMeta::parseExpress(const QString& input,QString& express)
{
	QString str(input);
	if(str.startsWith("[") && str.endsWith("]"))
	{
	    std::string desc;
		std::map<std::string,double> variables;
		express = input.mid(1,input.size()-2);
		auto node = cpp_eval::eval(express.toLatin1().data(),variables,SFuntionFactory::instance(),desc);
		if(!desc.empty())
		{
			express = QString::fromStdString(desc);
		    return false;
		}

		return true;
	}
	return false;
}

bool VarMeta::build(const Meta& input)
{	
	if(input.arguments.isEmpty())
		return false;
	if(input.operation.isEmpty())
		return false;

	meta = input;
	
	foreach(QString item,meta.arguments)
	{
		auto node = MetaEvalutor::splitArgument(item);
		MetaEvalutor* evalutor = MetaEvalutor::create(node._2,node._1);
		if(!evalutor)
			return false;
		if(!evalutor->build(node._2,node._3))
			return false;
		evalutor->setOptimizeStringKey(item);
		evalutors.push_back(evalutor);
	}
	return true;
}

bool VarMeta::loadData()
{
	foreach(auto item,evalutors)
		item->loadData();
	return true;
}
	
void VarMeta::clear()
{
	foreach(auto item,evalutors)
		item->clear();
}
	
VarMeta::~VarMeta()
{
	foreach(auto ptr,evalutors)
		delete ptr;
	evalutors.clear();
}

bool VarMeta::fetch(int pos,ResultType& output)
{
	if (GLOBAL_GET_VAR_MANAGER->hasVariable(str(),pos) && !needReCacluate())
	{
		auto node = GLOBAL_GET_VAR_MANAGER->variable(str(),pos);
		output = node.first;
		return node.second;
	}
	
	bool ok = execute(pos,output);
	GLOBAL_GET_VAR_MANAGER->addVariable(str(),pos,output,ok);
	return ok;
}


/*

struct VarMeta_not : public VarMeta
{
	StatementObject* is;

	VarMeta_not()
	{
		is = new StatementObject();
	}

	~VarMeta_not()
	{
		DELETE_PTR(is)
	}

	bool build(const Meta& input)
	{
		if (input.arguments.size() != 1)
			return false;

		if (!is->build(input.arguments[0]))
			return false;

		Meta meta;
		meta.var = input.var;
		meta.arguments = input.arguments;
		meta.operation = input.operation;
		/*meta.arguments.removeAt(0);

		bool ok = VarMeta::build(meta);
		if (!ok)
			return false;
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		ResultType ok;
		output.type = ResultType::TYPE_BOOL;

		QVector<float> args;
		return is->check(pos,args);
	}
};

struct VarMeta_sd : public VarMeta
{
	bool build(const Meta& input)
	{
		bool ok = VarMeta::build(input);
		if (!ok)
			return false;
		if (input.arguments.size() != 2)
			return false;
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		ResultType ret;
		if (1 != evalutors[1]->evalute(pos,ret))
			return false;

		int n = ret.double_;

		ResultType res;
		QVector<double> data;

		for (int i = 0;i < n;i++)
		{
			if (1 != evalutors[0]->evalute(pos - n + i + 1,res))
				return false;
			data += res.double_;
		}

		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = gsl_sd(data.data(),data.size());
		return true;
	}
};


*/

#endif 