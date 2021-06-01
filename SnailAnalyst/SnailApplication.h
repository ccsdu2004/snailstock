#ifndef SNAILCENTER_H
#define SNAILCENTER_H
#include <QApplication>
#include <QString>
#include <QObject>
#include <QStringList>
#include <Python.h>
#include "Stock.h"
#include "common/common.h"
#include "ConfigurationParser.h"
#include "grid/GridTradeWidget.h"
#include "python/PythonScript.h"
#include "lv2/LV2Config.h"

class SnailApplication : public QApplication
{
    Q_OBJECT
public:
	static SnailApplication* getInstance();

	SnailApplication(int argc,char** argv);
	~SnailApplication();
public:
	GridTradeConfig gridTradeConfig;
	LV2Config lv2Config;
	PythonScript* pythonScript;
};

#endif 