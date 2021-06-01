#include "dialog/SnailStockMsgBox.h"
#include "PythonScript.h"

PythonScript::PythonScript(QObject* parent):
	QObject(parent)
{
	Py_Initialize();
	if(!Py_IsInitialized())
	{
		POP_MSGBOX(QString("����"),QString("��ʼ��Python����ʧ��"),nullptr);
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
		POP_MSGBOX("����","��ȡpython�ļ�����lv2.py����",widget);
		return false;
	}

	pDict = PyModule_GetDict(pModule);
	if(!pDict)
	{
		POP_MSGBOX("����","��ȡpython�ļ�����lv2.pyģ���ֵ����",widget);
		return false;
	}

	{
		pLv2DataItem = PyDict_GetItemString(pDict,"lv2DataItem");
		if(!pLv2DataItem||!PyCallable_Check(pLv2DataItem))
		{
			POP_MSGBOX("����","lv2DataItem���������ڻ��Ǻ���",widget);
			return false;
		}
	}
	return true;
}