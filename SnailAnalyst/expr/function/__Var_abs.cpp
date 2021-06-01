#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_abs : public VarMeta
{
	bool build(const Meta& input)
	{
		bool ok = VarMeta::build(input);
		if (!ok)
			return false;
		if (input.arguments.size() != 1)
			return false;
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		auto ptr1 = evalutors[0];
		ResultType ret1;

		bool ok1 = GLOBAL_GET_VAR_MANAGER->hasVariable(ptr1->getOptimizeStringKey(),pos);

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

		if (!ok1)
			return false;

		if (ret1.type != ResultType::TYPE_DOUBLE)
			return false;

		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = std::abs(ret1.double_);
		return true;
	}
	static ObjectRegister<VarMeta,VarMeta_abs,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_abs,QString> VarMeta_abs::dummy("abs");

#endif