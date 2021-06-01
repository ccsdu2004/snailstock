#include "expr/VarMeta.h"
#include "StockScriptEngine.h"
#include "ObjectFactory.h"

#ifdef SUPPORT_BASIC

struct VarMeta_multi : public VarMeta
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
		double value = 1.0;
		foreach(auto item,evalutors)
		{
			auto str = item->getOptimizeStringKey();
			bool ok = GLOBAL_GET_VAR_MANAGER->hasVariable(str,pos);
			if (!ok)
			{
				ok = item->evalute(pos,output);
				if (!ok)
				{
					GLOBAL_GET_VAR_MANAGER->addVariable(str,pos,output,ok);
					return false;
				}
			}
			else
			{
				auto node = GLOBAL_GET_VAR_MANAGER->variable(str,pos);
				if (!node.second)
					return false;
				output = node.first;
			}

			if (output.type != ResultType::TYPE_DOUBLE)
				return false;
			value *= output.double_;
		}
		output.type = ResultType::TYPE_DOUBLE;
		output.double_ = value;
		return true;
	}
	static ObjectRegister<VarMeta,VarMeta_multi,QString> dummy;
};

ObjectRegister<VarMeta,VarMeta_multi,QString> VarMeta_multi::dummy("multi");

#endif