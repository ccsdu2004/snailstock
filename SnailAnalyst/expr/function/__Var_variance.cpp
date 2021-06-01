#include "expr/VarMeta.h"
#include "math/math.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_variance : public VarMeta
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
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		ResultType res;
		QVector<double> d;

		for (int i = pos;i > pos - n;i--)
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
		output.double_ = gsl_variance(d.data(),d.size());
		return true;
	}

	int n;
	static ObjectRegister<VarMeta,VarMeta_variance,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_variance,QString> VarMeta_variance::dummy("variance");

#endif