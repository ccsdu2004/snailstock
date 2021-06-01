#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QDir>
#include <QDate>
#include <QDebug>
#include <cmath>
#include <fstream>  
#include <QLineF>
#include "common.h"

//0.236 0.382 0.5 0.618 0.809 1
QString readAllBufferFromFile(const QString& filename)
{
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug()<<"open file failed:"<<file.errorString();
		return QString();
	}

	QString buffer = file.readAll();
	file.close();
	return buffer;
}

void saveToFile(const QString& filename,const QString& content)
{
    QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
		return;

	file.write(content.toLocal8Bit().data());
	file.close();
}

QStringList splitBuffer(const QString& buffer)
{
    QStringList splits = buffer.split("\n");
	QStringList output;
	foreach(QString item,splits)
		output.append(item.trimmed());
	return output;
}

QFileInfoList GetDirList(const QString& path)
{
    QDir dir(path);
    //QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
 
    /*for(int i = 0; i != folder_list.size(); i++)
    {
         QString name = folder_list.at(i).absoluteFilePath();
         QFileInfoList child_file_list = GetFileList(name);
         //file_list.append(child_file_list);
    }*/
 
    return folder_list;
}

QFileInfoList GetFileList(const QString& path)
{
    QDir dir(path);
    return dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
}

QString pickupString(const QString& data,const QString& expr)
{
	QRegExp regex(expr);
	regex.setCaseSensitivity(Qt::CaseInsensitive);
	regex.setMinimal(true); 
    int pos(0);
	if((pos = regex.indexIn(data,pos)) != -1)
    {
        return regex.capturedTexts().at(0);
    }
	return QString();
}

QVector<QMap<QString,QString> > parserJson(const QString& content)
{
    QVector<QMap<QString,QString> > output;
    QString buffer(content);
	buffer = buffer.remove("\"");
	buffer = buffer.mid(buffer.indexOf("[{")+2).trimmed();
	buffer = buffer.mid(0,buffer.lastIndexOf("}]"));
	QStringList list = buffer.split("},{");
	foreach(QString item,list)
	{
		QStringList pairs = item.split(",");
		QMap<QString,QString> node;
		foreach(QString pair,pairs)
		{
			int pos = pair.indexOf(":");
			if(pos != -1)
			{
				QString name = pair.mid(0,pos);
				QString value = pair.mid(pos+1);
				node.insert(pair.mid(0,pos),pair.mid(pos+1));
		    }
		}
        if(!node.isEmpty())
			output.append(node);
	}
	return output;
}

bool isHoliday(const QDate& date)
{
    QString str = date.toString("yyyy-MM-dd");
    const QStringList holiday = QString("2017-05-01,2017-01-27,2017-01-30,2017-01-31,2017-02-01,2017-04-04,2016-04-03,2016-03-31").split(",");
    foreach(QString item,holiday)
        if(str == item)
            return true;
	if(date.dayOfWeek() >= 6)
		return true;
    return false;
};

QDate getLastWorkDay(const QDate& date)
{
    QDate current(date);
    current = current.addDays(-1);
    while(true)
    {
        if(isHoliday(current))
            current = current.addDays(-1);
        if(current.dayOfWeek() > 5)
            current = current.addDays(-1);

        if(isHoliday(current) == false && current.dayOfWeek() < 6)
            break;
    }
    return current;
}

QDate getNextWorkDay(const QDate& date)
{
    QDate current(date);
    current = current.addDays(1);
    while(true)
    {
        if(isHoliday(current))
            current = current.addDays(1);
        if(current.dayOfWeek() > 5)
            current = current.addDays(1);

        if(isHoliday(current) == false && current.dayOfWeek() < 6)
            break;
    }
    return current;
}

void loopDir(const QString& path,ItrLooper* looper)
{
    QDir dir(path);
    if(!dir.exists())
        return;
 
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    
	QFileInfoList list = dir.entryInfoList();
	int count = 0;
    foreach(QFileInfo info,list)
    {
		QString file = info.absoluteFilePath();
		looper->loop(file,count ++,list.size());
    }
}

// = - under line + up line
float getDistance(const QLineF& line,float x,float y)
{
	float k = (line.p1().y() - line.p2().y())/(line.p1().x() - line.p2().x());
	float b =  line.p1().y() - k * line.p1().x();
	
	float v = sqrtf(k*k+1);
	return (y - x*k - b)/v;
}

bool exportToCSV(const QString& filename,QTableWidget* table)
{
    QFile file(filename);  
    bool ret = file.open(QIODevice::WriteOnly);  
    if(!ret)  
        return false;  
  
    QTextStream stream(&file);  
    QString contents;                  
    QHeaderView* header = table->horizontalHeader();  
    if(header)  
    {  
        for(int i = 0;i < header->count();i++)  
        {  
            QTableWidgetItem* item = table->horizontalHeaderItem(i);  
            if(!item)    
                continue;    
            contents += item->text() + ",";    
        }  

		contents += "\n";  
  
        for(int i = 0;i < table->rowCount();i++)  
        {  
            for(int j = 0;j < table->columnCount();j++)  
            {  
                QTableWidgetItem* item = table->item(i,j);  
                if(!item)  
                    continue;  
                QString str = item->text();  
                str.replace(","," ");  
                contents += str + ",";  
            }  
            contents += "\n";  
        }  
        stream << contents;  
        file.close();  
    }  
	return true;
}

bool exportToFile(const QString& filename,QListWidget* list)
{
    QFile file(filename);  
    bool ret = file.open(QIODevice::WriteOnly);  
    if(!ret)  
        return false;  
  
    QTextStream stream(&file);
	for(int i=0;i<list->count();i++)
	{
		QString code = list->item(i)->text();
		stream << code <<",";
	}

    file.close();    
	return true;
}