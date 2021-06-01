#include "StockScriptEngine.h"
#include "ConfigurationParser.h"

bool StockTargetCallbackImpl(const QString& desc,const QString& script,QString& error)
{
	StockScriptList::GetInstance().callbackScript(desc,script,error);
	return true;
}