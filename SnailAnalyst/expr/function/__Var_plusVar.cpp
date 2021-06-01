#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_plusVar : public VarMeta
{
	bool build(const Meta& input)
	{
		bool ok = VarMeta::build(input);
		if (!ok)
			return false;
		if (input.arguments.size() != 2)
			return false;

		if (!evalutors[1]->isConstValue())
			return false;

		ResultType ret;
		if (!evalutors[1]->evalute(0,ret))
			return false;

		if (!ret.isDoubleType())
			return false;

		n = ret.double_ + 0.01;
		if (n < 2)
			return false;
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		ResultType res;
		double d = 0.0;

		for (int i = pos;i > pos-n;i--)
		{
			bool ok = GLOBAL_GET_VAR_MANAGER->hasVariable(evalutors[0]->getOptimizeStringKey(),i);
			if (!ok)
			{
				ok = evalutors[0]->evalute(i,res);
				GLOBAL_GET_VAR_MANAGER->addVariable(evalutors[0]->getOptimizeStringKey(),i,res,ok);
			}
			else
			{
				auto node = GLOBAL_GET_VAR_MANAGER->variable(evalutors[0]->getOptimizeStringKey(),i);
				ok = node.second;
				res = node.first;
			}
			if (!ok)
				return false;
			if (!res.isDoubleType())
				return false;
			d += res.double_;
		}

		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = d;
		return true;
	}

	int n;
	static ObjectRegister<VarMeta,VarMeta_plusVar,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_plusVar,QString> VarMeta_plusVar::dummy("plusVar");

#endif