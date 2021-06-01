#include "dialog/SnailStockMsgBox.h"
#include "PythonScript.h"

PythonScript::PythonScript(QObject* parent):
	QObject(parent)
{
	Py_Initialize();
	if(!Py_IsInitialized())
	{
		POP_MSGBOX(QString("警告"),QString("初始化Python环境失败"),nullptr);
	}
	else
	{
		PyRun_SimpleString("import sys");
		PyRun_SimpleString("sys.path.append('python/')");
	}
}

PythonScript::~PythonScript()
{
	Py_Finalize();
}

bool PythonScript::init(const QString& module,QWidget* widget)
{
	pName = PyString_FromString(module.toLatin1().data());
	pModule = PyImport_Import(pName);
	if(!pModule)
	{
		POP_MSGBOX("警告","读取python文件夹下lv2.py出错",widget);
		return false;
	}

	pDict = PyModule_GetDict(pModule);
	if(!pDict)
	{
		POP_MSGBOX("警告","读取python文件夹下lv2.py模块字典出错",widget);
		return false;
	}

	{
		pLv2DataItem = PyDict_GetItemString(pDict,"lv2DataItem");
		if(!pLv2DataItem||!PyCallable_Check(pLv2DataItem))
		{
			POP_MSGBOX("警告","lv2DataItem函数不存在或不是函数",widget);
			return false;
		}
	}
	return true;
}