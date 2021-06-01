#include <QHeaderView>
#include <QTableWidget>
#include <QTextStream>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QFileInfo>
#include <QDir>
#include <QDate>
#include <QDebug>
#include <cmath>
#include <fstream>
#include "SnailApplication.h"
#include <QLineF>
#include "common.h"
#include "config.h"
#include "UILoader.h"

QString unicodeToUtf8(const QString& unicode)
{
	QString result;
	int index = unicode.indexOf("\\u");
	if(index == -1)
		return unicode;
	while (index != -1)
	{
		QString s1 = unicode.mid(index + 2, 4);
		result.append(s1.toUShort(0, 16));
		index = unicode.indexOf("\\u", index + 5);
	}
	return result.toUtf8().constData();
}

QString getSoftwareConfigInformation()
{
    QString buffer = QString("��ţ��Ʊ�����������%1<br/>").arg(SNAIL_VERSION);
	buffer.append("������Ϣ:<br/>");
#ifndef SUPPORT_VIEW_K
	buffer.append("�Ƿ�֧�ֲ鿴��ƱK��:N<br/>");
#else
	buffer.append("�Ƿ�֧�ֲ鿴��ƱK��:Y<br/>");
#endif
#ifndef SUPPORT_OHLC
	buffer.append("�Ƿ�֧��OHLC:N<br/>");
#else
	buffer.append("�Ƿ�֧��OHLC:Y<br/>");
#endif 
#ifndef SUPPORT_BASIC
	buffer.append("�Ƿ�֧�ָ߼��Ƿ�����:N<br/>");
#else
	buffer.append("�Ƿ�֧�ָ߼��Ƿ�����:Y<br/>");
#endif 
#ifndef SUPPORT_MACD 
	buffer.append("�Ƿ�֧��MACD:N<br/>");
#else
	buffer.append("�Ƿ�֧��MACD:Y<br/>");
#endif  
	buffer.append("�Ƿ�֧��K�����ʶ��:Y<br/>");
#ifndef SUPPORT_BASIC
	buffer.append("�Ƿ�֧�ֱ�������:N<br/>");
	buffer.append("�Ƿ�֧�ֵ�Ŀ����:N<br/>");
	buffer.append("�Ƿ�֧��OR:N<br/>");
	buffer.append("�Ƿ�֧�ֻ������:N<br/>");
	buffer.append("�Ƿ�֧�ֹ�Ʊ��:N<br/>");
#else
	buffer.append("�Ƿ�֧�ֱ�������:Y<br/>");
	buffer.append("�Ƿ�֧�ֵ�Ŀ����:Y<br/>");
	buffer.append("�Ƿ�֧��OR:Y<br/>");
	buffer.append("�Ƿ�֧�ֻ������:Y<br/>");
	buffer.append("�Ƿ�֧�ֹ�Ʊ��:Y<br/>");
#endif
#ifndef SUPPORT_REF
	buffer.append("�Ƿ�֧�ֲο�ѡ��:N<br/>");
#else
	buffer.append("�Ƿ�֧�ֲο�ѡ��:Y<br/>");
#endif
#ifndef SUPPORT_PAIRS_TRADE
	buffer.append("�Ƿ�֧���������:N<br/>");
#else
	buffer.append("�Ƿ�֧���������:Y<br/>");
#endif
#ifndef SUPPORT_BATCH_SCRIPT
	buffer.append("�Ƿ�֧����������:N<br/>");
#else
	buffer.append("�Ƿ�֧����������:Y<br/>");
#endif
#ifndef SUPPORT_TICKET
	buffer.append("�Ƿ�֧�ֱַ����ݷ���:N<br/>");
#else
	buffer.append("�Ƿ�֧�ֱַ����ݷ���:Y<br/>");
#endif
#ifndef SUPPORT_LEVEL2
	buffer.append("�Ƿ�֧��LEVEL2���ݷ���:N<br/>");
#else
	buffer.append("�Ƿ�֧��LEVEL2���ݷ���:Y<br/>");
#endif 
#ifndef SUPPORT_BIG_SPAN
	buffer.append("�Ƿ�֧�ִ�������������:N<br/>");
#else
	buffer.append("�Ƿ�֧�ִ�������������:Y<br/>");
#endif
#ifndef SUPPORT_PER
	buffer.append("�Ƿ�֧���������ݿ���:N<br/>");
#else
	buffer.append("�Ƿ�֧���������ݿ���:Y<br/>");
#endif 
#ifdef SUPPORT_SCRIPT
	buffer.append("�Ƿ�֧��PYTHON�ű���չ:Y<br/>");
#else
	buffer.append("�Ƿ�֧��PYTHON�ű���չ:N<br/>");
#endif
#ifdef SUPPORT_BID
	buffer.append("�Ƿ�֧�־���ѡ��:Y<br/>");
#else
	buffer.append("�Ƿ�֧�־���ѡ��:N<br/>");
#endif
#ifdef SUPPORT_hyperbola_algo
	buffer.append("�Ƿ�֧��˫����ѡ��:Y<br/>");
#else
	buffer.append("�Ƿ�֧��˫����ѡ��:N<br/>");
#endif
#ifdef SUPPORT_SEA_SCRIPT
	buffer.append("�Ƿ�֧��SEA�ű���չ:Y<br/>");
#else
	buffer.append("�Ƿ�֧��SEA�ű���չ:N<br/>");
#endif
#ifdef SUPPORT_BOOSTING_ALGO
	buffer.append("�Ƿ�֧��Boosting�㷨:Y<br/>");
#else
	buffer.append("�Ƿ�֧��Boosting�㷨:N<br/>");
#endif
#ifdef SUPPORT_IMPLICIT_VAR
	buffer.append("�Ƿ�֧����������:Y<br/>");
#else
	buffer.append("�Ƿ�֧����������:N<br/>");
#endif
#ifdef SUPPORT_GPDX
	buffer.append("�Ƿ�֧�ָ��׵���:Y<br/>");
#else
	buffer.append("�Ƿ�֧�ָ��׵���:N<br/>");
#endif
#ifdef SUPPORT_UPDATE_STOCK
	buffer.append("�Ƿ�֧�ָ��¹�Ʊ��������:Y<br/>");
#else
	buffer.append("�Ƿ�֧�ָ��¹�Ʊ��������:N<br/>");
#endif
#ifdef SUPPORT_UPDATE_STOCK_TICK
	buffer.append("�Ƿ�֧�ָ��¹�Ʊ��ʱ����:Y<br/>");
#else
	buffer.append("�Ƿ�֧�ָ��¹�Ʊ��ʱ����:N<br/>");
#endif
#ifdef SUPPORT_OUTPUT_ARGUMENT
	buffer.append("�Ƿ�֧�ֵ�������:Y<br/>");
#else 
	buffer.append("�Ƿ�֧�ֵ�������:N<br/>");
#endif

	buffer.append(QString("����������������Ʊ��:%1<br/>").arg(SUPPORT_STOCK_SIZE));
	buffer.append("��ַ:<a href=\\\"www.snail007.com\\\">http://www.snail007.com/</a><br/>");
	buffer.append("�Ա���:<a href=\\\"https://sharesoft.taobao.com/\\\">http://sharesoft.taobao.com/</a><br/>");
	buffer.append("QQȺ:297919841<br/>");
	//buffer.append("����13077776461ʹ��");
	//buffer.append("����18669997369ʹ��");
	//buffer.append("����13826558738(���)ʹ��");
	return buffer;
}

QStringList splitBuffer(const QString& buffer)
{
    QStringList splits = buffer.split("\n");
	QStringList output;
	foreach(QString item,splits)
		output.append(item.trimmed());
	return output;
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

QStringList pickupStrings(const QString& data,const QString& express)
{
	QStringList output;

	QRegExp regex(express);
	regex.setCaseSensitivity(Qt::CaseInsensitive);
	regex.setMinimal(true);
	int pos(0);
	while ((pos = regex.indexIn(data,pos)) != -1)
	{
		output.append(regex.capturedTexts().at(0));
		pos += regex.matchedLength();
	}
	return output;
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

QStringList trimStringList(const QStringList& list)
{
	QStringList output;
	foreach(auto item,list)
		if (item.isEmpty())
			output.append(item);
	return output;
}

bool isStockCode(const QString& market,const QString& code)
{
	if (market == "�й���½")
	{
		if (code.size() != 8)
			return false;
		if (code.contains("*"))
			return false;
	}
	else if (market == "���")
	{
		if (code.size() != 7)
			return false;
	}

	if (market == "���")
	{
		if (code.contains(","))
			return false;
		return true;
	}
	return true;
}

QString buildStockCodeByDigitStockCode(const QString& code)
{
	if (code.size() == 8)
		return code;

	if (code.startsWith("6"))
		return "sh" + code;
	return "sz" + code;
}

QStringList getStocksByInput(const QString& input)
{
	auto items = input.split(",");
	if (items.size() >= 2)
		return items;

	if (config.stockPool.contains(input))
		return config.stockPool[input].items;

	return QStringList(input);
}

QWidget* loadWidgetFromUI(const QString& uifile)
{
	UILoader loader;
	QFile file(uifile);
	if (!file.open(QFile::ReadOnly))
		return nullptr;
	return loader.load(&file);
}