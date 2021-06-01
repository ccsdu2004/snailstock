#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_sma : public VarMeta
{
	bool build(const Meta& input)
	{
		bool ok = VarMeta::build(input);
		if (!ok)
			return false;
		if (input.arguments.size() != 3)
			return false;

		if (!evalutors[1]->isConstValue())
			return false;

		if (!evalutors[2]->isConstValue())
			return false;

		ResultType ret;
		if (!evalutors[1]->evalute(0,ret))
			return false;

		if (!ret.isDoubleType())
			return false;

		n = ret.double_;
		if (n < 2)
			return false;

		if (!evalutors[2]->evalute(0,ret))
			return false;

		if (!ret.isDoubleType())
			return false;

		w = ret.double_;
		if (w < 0 || n <= w)
			return false;

		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		double sma = 0.0;

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

			sma = (w * res.double_ + (n - w)*sma) / n;
		}

		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = sma;
		return true;
	}

	double n,w;
	static ObjectRegister<VarMeta,VarMeta_sma,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_sma,QString> VarMeta_sma::dummy("sma");

#endif