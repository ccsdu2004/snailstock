#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_div : public VarMeta
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
		auto ptr1 = evalutors[0];
		auto ptr2 = evalutors[1];
		ResultType ret1,ret2;

		bool ok1 = GLOBAL_GET_VAR_MANAGER->hasVariable(ptr1->getOptimizeStringKey(),pos);
		bool ok2 = GLOBAL_GET_VAR_MANAGER->hasVariable(ptr2->getOptimizeStringKey(),pos);

		if (!ok1)
		{
			ok1 = ptr1->evalute(pos,ret1);
			GLOBAL_GET_VAR_MANAGER->addVariable(ptr1->getOptimizeStringKey(),pos,ret1,ok1);
		}
		else
		{
			auto node = GLOBAL_GET_VAR_MANAGER->variable(ptr1->getOptimizeStringKey(),pos);
			ok1 = node.second;
			ret1 = node.first;
		}

		if (!ok2)
		{
			ok2 = ptr2->evalute(pos,ret2);
			GLOBAL_GET_VAR_MANAGER->addVariable(ptr2->getOptimizeStringKey(),pos,ret2,ok2);
		}
		else
		{
			auto node = GLOBAL_GET_VAR_MANAGER->variable(ptr2->getOptimizeStringKey(),pos);
			ok2 = node.second;
			ret2 = node.first;
		}

		if (ok1 == false || ok2 == false)
			return false;

		if (ret1.type != ResultType::TYPE_DOUBLE || ret2.type != ResultType::TYPE_DOUBLE)
			return false;

		if (isEqual(ret2.double_,0.0))
			return false;

		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = ret1.double_ / ret2.double_;
		return true;
	}
	static ObjectRegister<VarMeta,VarMeta_div,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_div,QString> VarMeta_div::dummy("div");

#endif