#include "VarManager.h"

bool VarMetaManager::hasVariable(const QString& var,int index)
{
	QPair<QString,int> node(var,index);
	return data.find(node) != data.end();
}

QPair<ResultType,bool> VarMetaManager::variable(const QString& var,int index)
{
	QPair<QString,int> node(var,index);
	return data[node];
}

void VarMetaManager::addVariable(const QString& var,int index,const ResultType& result,bool flag)
{
	QPair<QString,int> node(var,index);
	data.insert(node,QPair<ResultType,bool>(result,flag));
}

void VarMetaManager::addRegisterVar(const QPair<double,QString>& value)
{
	registerVar.push_back(value);
}

bool VarMetaManager::getRegisterVar(const QString& name,QPair<double,QString>& value)
{
	auto itr = implicitVars.find(name);
	if (itr == implicitVars.end())
		return false;

	int pos = itr.value();

	if (pos >= 0 && pos < registerVar.size())
	{
		value = registerVar[pos];
		return true;
	}
	return false;
}

/*
bool VarMetaManager::get(VarMeta* meta,int index,ResultType& result)
{
	QString str = meta->str();
	if (hasVarValue(str,index))
	{
		auto node = varValue(str,index);
		result = node.first;
		return node.second;
	}

	bool ok = meta->execute(index,result);
	insertVarValue(str,index,result,ok);
	return ok;
}

bool VarMetaManager::get(MetaEvalutor* meta,int index,ResultType& result)
{
	QString str = meta->getOptimizeStringKey();
	if (str.isEmpty())
		return meta->evalute(index,result);

	if (hasVarValue(str,index))
	{
		QPair<ResultType,bool> node = varValue(str,index);
		result = node.first;
		return node.second;
	}

	bool ok = meta->evalute(index,result);
	insertVarValue(str,index,result,ok);
	return ok;
}*/