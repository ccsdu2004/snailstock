#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_if : public VarMeta
{
	StatementObject* is;

	VarMeta_if()
	{
		is = new StatementObject();
	}

	~VarMeta_if()
	{
		DELETE_PTR(is)
	}

	bool build(const Meta& input)
	{
		if (input.arguments.size() != 3)
			return false;

		if (!is->build(input.arguments[0]))
			return false;

		Meta meta;
		meta.var = input.var;
		meta.arguments = input.arguments;
		meta.operation = input.operation;
		meta.arguments.removeAt(0);

		bool ok = VarMeta::build(meta);
		if (!ok)
			return false;
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		ResultType ret1,ret2;
		bool ok1 = GLOBAL_GET_VAR_MANAGER->hasVariable(evalutors[0]->getOptimizeStringKey(),pos);
		bool ok2 = GLOBAL_GET_VAR_MANAGER->hasVariable(evalutors[1]->getOptimizeStringKey(),pos);

		if (!ok1)
		{
			ok1 = evalutors[0]->evalute(pos,ret1);
			GLOBAL_GET_VAR_MANAGER->addVariable(evalutors[0]->getOptimizeStringKey(),pos,ret1,ok1);
		}
		else
		{
			auto node = GLOBAL_GET_VAR_MANAGER->variable(evalutors[0]->getOptimizeStringKey(),pos);
			ok1 = node.second;
			ret1 = node.first;
		}

		if (!ok2)
		{
			ok2 = evalutors[0]->evalute(pos,ret2);
			GLOBAL_GET_VAR_MANAGER->addVariable(evalutors[1]->getOptimizeStringKey(),pos,ret2,ok2);
		}
		else
		{
			auto node = GLOBAL_GET_VAR_MANAGER->variable(evalutors[1]->getOptimizeStringKey(),pos);
			ok2 = node.second;
			ret2 = node.first;
		}

		output.type = ResultType::TYPE_DOUBLE;

		QVector<float> args;
		bool ok = is->check(pos,args);
		if (ok)
		{
			if (ok1)
			{

				output = ret1;
				return true;
			}
		}
		else
		{
			if (ok2)
			{
				output = ret2;
				return true;
			}
		}

		return false;
	}
	static ObjectRegister<VarMeta,VarMeta_if,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_if,QString> VarMeta_if::dummy("if");

#endif