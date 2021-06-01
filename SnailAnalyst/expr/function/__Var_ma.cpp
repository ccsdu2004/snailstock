#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_ma : public VarMeta
{
	bool build(const Meta& input)
	{
		bool ok = VarMeta::build(input);
		if (!ok)
			return false;
		if (input.arguments.size() != 2)
			return false;

		ResultType ret;
		if (!evalutors[1]->isConstValue())
			return false;

		if (!evalutors[1]->evalute(0,ret))
			return false;

		if (!ret.isDoubleType())
			return false;

		n = ret.double_ + 0.01f;
		if (n < 3)
			return false;

		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		double ma = 0.0;

		ResultType res;

		for (int i = 0;i < n;i++)
		{
			bool ok = GLOBAL_GET_VAR_MANAGER->hasVariable(evalutors[0]->getOptimizeStringKey(),pos-n+i+1);
			if (!ok)
			{
				ok = evalutors[0]->evalute(pos - n + i + 1,res);
				GLOBAL_GET_VAR_MANAGER->addVariable(evalutors[0]->getOptimizeStringKey(),pos-n+i+1,res,ok);
			}
			else
			{
				auto node = GLOBAL_GET_VAR_MANAGER->variable(evalutors[0]->getOptimizeStringKey(),pos - n + i + 1);
				ok = node.second;
				res = node.first;
			}

			if (!res.isDoubleType())
				return false;
			if (!ok)
				return false;

			ma += res.double_;
		}

		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = ma / (double)n;
		return true;
	}

	double n;
	static ObjectRegister<VarMeta,VarMeta_ma,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_ma,QString> VarMeta_ma::dummy("ma");

#endif