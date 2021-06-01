#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_prev : public VarMeta
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
		if(!evalutors[1]->evalute(0,ret))
			return false;

		if (!ret.isDoubleType())
			return false;

		if (ret.double_ < 1)
			return false;

		n = ret.double_ + 0.01;
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		auto ptr1 = evalutors[0];
		ResultType ret1;

		int newpos = pos - n;

		bool ok1 = GLOBAL_GET_VAR_MANAGER->hasVariable(ptr1->getOptimizeStringKey(),newpos);
		if (!ok1)
		{
			ok1 = ptr1->evalute(newpos,ret1);
			GLOBAL_GET_VAR_MANAGER->addVariable(ptr1->getOptimizeStringKey(),newpos,ret1,ok1);
		}
		else
		{
			auto node = GLOBAL_GET_VAR_MANAGER->variable(ptr1->getOptimizeStringKey(),pos);
			ok1 = node.second;
			ret1 = node.first;
		}

		if (!ok1)
			return false;

		if (!ret1.isDoubleType())
			return false;
 
		output = ret1;
		return true;
	}

	int n;
	static ObjectRegister<VarMeta,VarMeta_prev,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_prev,QString> VarMeta_prev::dummy("prev");

#endif