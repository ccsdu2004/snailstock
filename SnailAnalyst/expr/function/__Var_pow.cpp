#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_pow : public VarMeta
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

		n = ret.double_;
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		ResultType ret;
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

		if (ok1 == false)
			return false;

		if (ret1.type != ResultType::TYPE_DOUBLE)
			return false;

		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = std::pow(ret1.double_,n);
		return true;
	}

	double n;
	static ObjectRegister<VarMeta,VarMeta_pow,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_pow,QString> VarMeta_pow::dummy("pow");

#endif