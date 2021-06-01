#include <QApplication>
#include <QTranslator>
#include <QDate>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QMetaType>  
#include <QStyleFactory>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <ctime>
#include "SnailAnalyst.h"
#include "SnailApplication.h"
#include "common/FileUtils.h"
#include "common/LicenseAgreement.h"
#include "script/ScriptManager.h"
#include "sea/SeaScript.h"
#include "eval/cppeval.h"
#include "common/FileMapper.h"
#include "per/StockPerData.h"

static void customMessageHandler(QtMsgType type,const QMessageLogContext& context,const QString& msg)
{
    QString txt;
    switch(type)
    {
    case QtDebugMsg:  
         txt = QString("Debug: %1").arg(msg);
         break;
    case QtWarningMsg:
         txt = QString("Warning: %1").arg(msg);
         break;
    case QtCriticalMsg:
         txt = QString("Critical: %1").arg(msg);
         break;
    case QtFatalMsg:
         txt = QString("Fatal: %1").arg(msg);
         abort();
    default:
         break;
    }

	QString date = QDate::currentDate().toString("yyyy-MM-dd");
	QString time = QTime::currentTime().toString();
	txt = date + " " + time + " " + txt; 
            
    QFile outFile("snail.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char* argv[])
{
	QTextCodec* codec = QTextCodec::codecForName("utf-8");
	QTextCodec::setCodecForLocale(codec);

	qInstallMessageHandler(customMessageHandler);

	QCoreApplication::setAttribute(Qt::ApplicationAttribute::AA_ShareOpenGLContexts);
	SnailApplication app(argc,argv);

	qRegisterMetaType<QVector<int>>("QVector<int>");

	srand((unsigned)time(0));

	QString scriptFile;
	if (app.arguments().size() >= 2)
		scriptFile = app.arguments().at(1);

	QTranslator* translator = new QTranslator();
	if (translator->load(QString(":qrc/widgets.qm")))
	{
		bool loaded = app.installTranslator(translator);
	}
	else
		delete translator;

	translator = new QTranslator();
	if (translator->load(QString(":qrc/qt_zh_CN")))
	{
		bool loaded = app.installTranslator(translator);
    }
	else
		delete translator;

#ifdef SUPPORT_SCRIPT
	ScriptManager::instance().loadScripts("addon");
#endif

	SFuntionFactory::instance();

#ifdef SUPPORT_SEA_SCRIPT
	SeaScriptManager::getInstance()->loadSeaScripts("sea");
#endif

    SnailAnalyst widget(scriptFile);
	app.pythonScript->init("pyscript",&widget);
	widget.show();

	return app.exec();
}
