#include <QStringList>
#include "common/abstractFileReader.h"

abstractFileReader::abstractFileReader()
{
}
abstractFileReader::~abstractFileReader()
{
}

bool abstractFileReader::parse(const QString& buffer)
{
    QString tag,value;
	QStringList items = buffer.split("\n");
	foreach(QString item,items)
    {
        value = item;
		auto list = value.split("=");
		if (list.size() < 2)
		{
			return false;
		}
		else if (list.size() >= 2)
		{
			auto pos = value.indexOf('=');
			list[0] = value.mid(0,pos);
			list[1] = value.mid(pos+1);;
		}
		doLine(list[0],list[1]);
    }

    doAfter();
    return true;
}

void abstractFileReader::doAfter()
{
}
 