#ifndef UI_LOADER_H
#define UI_LOADER_H
#include <QtUiTools/QuiLoader.h>

class UILoader : public QUiLoader
{
    Q_OBJECT 
public:
	QWidget* createWidget(const QString& className,QWidget* parent = nullptr,const QString &name = QString());
};

#endif