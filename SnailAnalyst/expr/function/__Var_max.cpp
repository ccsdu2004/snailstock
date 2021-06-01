#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_max : public VarMeta
{
	bool build(const Meta& input)
	{
		bool ok = VarMeta::build(input);
		if (!ok)
			return false;
		if (input.arguments.size() < 2)
			return false;
		return true;
	}

	bool execute(int pos,ResultType& output)
	{
		ResultType ret;
		double max_ = std::numeric_limits<double>::min();
		foreach(auto item,evalutors)
		{
			bool ok = GLOBAL_GET_VAR_MANAGER->hasVariable(item->getOptimizeStringKey(),pos);
			if (!ok)
			{
				ok = item->evalute(pos,ret);
				GLOBAL_GET_VAR_MANAGER->addVariable(item->getOptimizeStringKey(),pos,ret,ok);
			}
			else
			{
				auto node = GLOBAL_GET_VAR_MANAGER->variable(item->getOptimizeStringKey(),pos);
				ok = node.second;
				ret = node.first;
			}

			if (!ok)
				return false;
			if (!ret.isDoubleType())
				return false;
			max_ = std::max(max_,ret.double_);
		}
		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = max_;
		return true;
	}
	static ObjectRegister<VarMeta,VarMeta_max,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_max,QString> VarMeta_max::dummy("max");

#endif