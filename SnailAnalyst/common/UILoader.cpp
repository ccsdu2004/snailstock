#include <QDebug>
#include "UILoader.h"
#include "widget/MapTableWidget.h"

QWidget* UILoader::createWidget(const QString& className,QWidget* parent,const QString& name)
{
	if(className == "MapTableWidget")
		return new MapTableWidget(parent);

	return QUiLoader::createWidget(className,parent,name);
}