//#include <QCleanlooksStyle>
#include <QTextCodec>
#include <QTranslator>
#include <QApplication>
#include "edit.h"

int main(int argc,char* argv[])
{
	QTextCodec* codec = QTextCodec::codecForName("utf-8");
	QTextCodec::setCodecForLocale(codec);

	QApplication app(argc,argv);
    QTranslator* translator = new QTranslator();
	if(translator->load(QString(":data/qt_zh_CN.qm")))
        app.installTranslator(translator);
    else
        delete translator;

	//app.setStyle(new QCleanlooksStyle());

	Edit w;
	w.show();
	return app.exec();
}
