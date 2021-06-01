#include "ScriptManager.h"

#ifdef SUPPORT_SCRIPT

bool callPyObject(PyObject* object,QString& ret)
{
    PyObject* retvalue = PyObject_CallObject(object,0);
    if(PyString_Check(retvalue))
    {
        ret = PyString_AsString(retvalue);
        Py_DECREF(retvalue);
        return true;
    }
    else
        Py_DECREF(retvalue);
    return false;
}

ScriptAddon::ScriptAddon()
{
    pName = nullptr;
    pModule = nullptr;
    pDict = nullptr;
	function = nullptr;
}

ScriptAddon::~ScriptAddon()
{
    if(function)
		Py_DECREF(function);
	if(pDict)
        Py_DECREF(pDict);
    if (pModule)
        Py_DECREF(pModule);
    if (pName)
        Py_DECREF(pName);
}

bool ScriptAddon::loadFromFile(const QString& file)
{
	pName = PyString_FromString(file.toLatin1().data());
    if (!pName)
        return false;

    pModule = PyImport_Import(pName);
    if (!pModule)
        return false;

    pDict = PyModule_GetDict(pModule);
    if(!pDict)
        return false;

	auto ptr = loadFunction(ADDON_NAME);
	QString ret;
	bool ok = callPyObject(ptr,ret);
	if(ok)
		name = ret;
	else
		return false;

	ptr = loadFunction(ADDON_VERSION);
	ok = callPyObject(ptr,ret);
	if(ok)
		version = ret;
	else
		return false;

	ptr = loadFunction(ADDON_RETTYPE);
	ok = callPyObject(ptr,ret);
	if(ok)
		retType = ret;
	else
		return false;

	ptr = loadFunction(ADDON_DESCRIPTION);
	ok = callPyObject(ptr,ret);
	if(ok)
		description = ret;
	else
		return false;

	function = loadFunction(ADDON_EVALUTE_CANDLESTICK);

	filename = file; 
    return true;
}

PyObject* ScriptAddon::loadFunction(const QString& function)
{
	auto pFunc = PyDict_GetItemString(pDict,function.toLatin1().data());
    if(!pFunc)
        return nullptr;

    if(!PyCallable_Check(pFunc))
    {
        Py_DECREF(pFunc);
        return nullptr;
    }
    
	return pFunc;
}

#endif