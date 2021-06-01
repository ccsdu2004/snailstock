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
    QString buffer = QString("蜗牛股票量化分析软件%1<br/>").arg(SNAIL_VERSION);
	buffer.append("配置信息:<br/>");
#ifndef SUPPORT_VIEW_K
	buffer.append("是否支持查看股票K线:N<br/>");
#else
	buffer.append("是否支持查看股票K线:Y<br/>");
#endif
#ifndef SUPPORT_OHLC
	buffer.append("是否支持OHLC:N<br/>");
#else
	buffer.append("是否支持OHLC:Y<br/>");
#endif 
#ifndef SUPPORT_BASIC
	buffer.append("是否支持高级涨幅参数:N<br/>");
#else
	buffer.append("是否支持高级涨幅参数:Y<br/>");
#endif 
#ifndef SUPPORT_MACD 
	buffer.append("是否支持MACD:N<br/>");
#else
	buffer.append("是否支持MACD:Y<br/>");
#endif  
	buffer.append("是否支持K线组合识别:Y<br/>");
#ifndef SUPPORT_BASIC
	buffer.append("是否支持变量参数:N<br/>");
	buffer.append("是否支持单目运算:N<br/>");
	buffer.append("是否支持OR:N<br/>");
	buffer.append("是否支持滑动买点:N<br/>");
	buffer.append("是否支持股票池:N<br/>");
#else
	buffer.append("是否支持变量参数:Y<br/>");
	buffer.append("是否支持单目运算:Y<br/>");
	buffer.append("是否支持OR:Y<br/>");
	buffer.append("是否支持滑动买点:Y<br/>");
	buffer.append("是否支持股票池:Y<br/>");
#endif
#ifndef SUPPORT_REF
	buffer.append("是否支持参考选股:N<br/>");
#else
	buffer.append("是否支持参考选股:Y<br/>");
#endif
#ifndef SUPPORT_PAIRS_TRADE
	buffer.append("是否支持配对量化:N<br/>");
#else
	buffer.append("是否支持配对量化:Y<br/>");
#endif
#ifndef SUPPORT_BATCH_SCRIPT
	buffer.append("是否支持批量策略:N<br/>");
#else
	buffer.append("是否支持批量策略:Y<br/>");
#endif
#ifndef SUPPORT_TICKET
	buffer.append("是否支持分笔数据分析:N<br/>");
#else
	buffer.append("是否支持分笔数据分析:Y<br/>");
#endif
#ifndef SUPPORT_LEVEL2
	buffer.append("是否支持LEVEL2数据分析:N<br/>");
#else
	buffer.append("是否支持LEVEL2数据分析:Y<br/>");
#endif 
#ifndef SUPPORT_BIG_SPAN
	buffer.append("是否支持大周期量化分析:N<br/>");
#else
	buffer.append("是否支持大周期量化分析:Y<br/>");
#endif
#ifndef SUPPORT_PER
	buffer.append("是否支持日线数据快照:N<br/>");
#else
	buffer.append("是否支持日线数据快照:Y<br/>");
#endif 
#ifdef SUPPORT_SCRIPT
	buffer.append("是否支持PYTHON脚本扩展:Y<br/>");
#else
	buffer.append("是否支持PYTHON脚本扩展:N<br/>");
#endif
#ifdef SUPPORT_BID
	buffer.append("是否支持竞价选股:Y<br/>");
#else
	buffer.append("是否支持竞价选股:N<br/>");
#endif
#ifdef SUPPORT_hyperbola_algo
	buffer.append("是否支持双曲线选股:Y<br/>");
#else
	buffer.append("是否支持双曲线选股:N<br/>");
#endif
#ifdef SUPPORT_SEA_SCRIPT
	buffer.append("是否支持SEA脚本扩展:Y<br/>");
#else
	buffer.append("是否支持SEA脚本扩展:N<br/>");
#endif
#ifdef SUPPORT_BOOSTING_ALGO
	buffer.append("是否支持Boosting算法:Y<br/>");
#else
	buffer.append("是否支持Boosting算法:N<br/>");
#endif
#ifdef SUPPORT_IMPLICIT_VAR
	buffer.append("是否支持隐含变量:Y<br/>");
#else
	buffer.append("是否支持隐含变量:N<br/>");
#endif
#ifdef SUPPORT_GPDX
	buffer.append("是否支持高抛低吸:Y<br/>");
#else
	buffer.append("是否支持高抛低吸:N<br/>");
#endif
#ifdef SUPPORT_UPDATE_STOCK
	buffer.append("是否支持更新股票日线数据:Y<br/>");
#else
	buffer.append("是否支持更新股票日线数据:N<br/>");
#endif
#ifdef SUPPORT_UPDATE_STOCK_TICK
	buffer.append("是否支持更新股票分时数据:Y<br/>");
#else
	buffer.append("是否支持更新股票分时数据:N<br/>");
#endif
#ifdef SUPPORT_OUTPUT_ARGUMENT
	buffer.append("是否支持导出参数:Y<br/>");
#else 
	buffer.append("是否支持导出参数:N<br/>");
#endif

	buffer.append(QString("单次最大允许分析股票数:%1<br/>").arg(SUPPORT_STOCK_SIZE));
	buffer.append("网址:<a href=\\\"www.snail007.com\\\">http://www.snail007.com/</a><br/>");
	buffer.append("淘宝店:<a href=\\\"https://sharesoft.taobao.com/\\\">http://sharesoft.taobao.com/</a><br/>");
	buffer.append("QQ群:297919841<br/>");
	//buffer.append("仅供13077776461使用");
	//buffer.append("仅供18669997369使用");
	//buffer.append("仅供13826558738(清风)使用");
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
	if (market == "中国大陆")
	{
		if (code.size() != 8)
			return false;
		if (code.contains("*"))
			return false;
	}
	else if (market == "香港")
	{
		if (code.size() != 7)
			return false;
	}

	if (market == "混合")
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