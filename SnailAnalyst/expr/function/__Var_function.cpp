#include "expr/VarMeta.h"
#include "eval/cppeval.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_function : public VarMeta
{
	bool build(const Meta& input)
	{
		meta = input;
		if (meta.arguments.size() != 1)
			return false;

		std::map<std::string,double> variables;
		std::string desc;
		auto value = cpp_eval::eval(meta.arguments[0].toStdString().c_str(),variables,SFuntionFactory::instance(),desc);
		if (!desc.empty())
			return false;
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		ResultType ret;
		ret.type = ResultType::TYPE_DOUBLE;
		bool has = StockScriptList::GetInstance().getVarMetaManager()->hasVariable(str(),pos);
		if (!has)
		{
			std::string desc;
			std::map<std::string,double> variables;
			auto value = cpp_eval::eval(meta.arguments[0].toStdString().c_str(),variables,SFuntionFactory::instance(),desc);
			if (desc.empty())
			{
				ret.double_ = value;
				StockScriptList::GetInstance().getVarMetaManager()->addVariable(str(),pos,ret,true);
			}
			else
				StockScriptList::GetInstance().getVarMetaManager()->addVariable(str(),pos,ret,false);
		}

		auto node = StockScriptList::GetInstance().getVarMetaManager()->variable(str(),pos);
		output = node.first;
		return node.second;
	}

	bool loadData(){ return true; }

	static ObjectRegister<VarMeta,VarMeta_function,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_function,QString> VarMeta_function::dummy("express");

#endif