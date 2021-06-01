#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_ema : public VarMeta
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
		if (n < 5.0)
			return false;

		n = 2.0/(double) (n + 1);
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		double ema = 0.0;
		ResultType res;

		for (int i = 0;i < pos;i++)
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

			if (!res.isDoubleType())
				return false;

			if (!ok)
				res.double_ = 0.0;

			ema = n * res.double_ + (1.0 - n)*ema;
		}

		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = ema;
		return true;
	}

	double n;
	static ObjectRegister<VarMeta,VarMeta_ema,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_ema,QString> VarMeta_ema::dummy("ema");

#endif