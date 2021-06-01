#ifndef PYTHON_SCRIPT_H
#define PYTHON_SCRIPT_H
#include <Python.h>
#include <QObject>

class PythonScript : public QObject
{
    Q_OBJECT
public:
	PythonScript(QObject* parent = nullptr);
	~PythonScript();
public:
	bool init(const QString& module,QWidget* widget = nullptr);

	PyObject* pName = nullptr;
	PyObject* pModule = nullptr;
	PyObject* pDict = nullptr;
	PyObject* pLv2DataItem = nullptr;
};

#endif