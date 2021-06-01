#include <qfile.h>
#include <qtextstream.h>
#include <QDataStream>
#include <qfileinfo.h>
#include <qdir.h>
#include <qdebug.h>
#include <algorithm>
#include <qtablewidget.h>
#include <qlistwidget.h>
#include <qheaderview.h>
#include "FileUtils.h"
#include "common.h"
#include <Windows.h>

//0.236 0.382 0.5 0.618 0.809 1
QString readAllBufferFromFile(const QString& filename)
{
	QFileInfo info(filename);
	if (!info.exists())
	{
		qDebug() << filename << " not exists";
		return QString();
	}

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "open file failed:" << filename;
		return QString();
	}

	QTextStream stream(&file);
	stream.setCodec("gb2312");
	QString buffer = stream.readAll();
	file.close();
	return buffer;
}

QByteArray AnsiToUtf8(const QByteArray &ansi)
{
#ifdef _CUR_SYS_LINUX_
	QByteArray utf8;
	char *buf = new char[ansi.size() * 2];
	char utf8Buf[] = { "utf-8" };
	char gb2312Buf[] = { "gb2312" };
	//ansi.resize(ansi.size()*2);
	int code = code_convert(gb2312Buf, utf8Buf, ansi.data(), ansi.size(), buf, 2 * ansi.size());
	if (code == -1) {
		delete[]buf;
		utf8 = "";
		return utf8;
	}
	utf8 = buf;
	delete[]buf;
	return utf8;
#else
	int len;
	QByteArray result;
	//ANSI转UNICODE
	len = MultiByteToWideChar(CP_ACP, NULL, ansi.data(), -1, NULL, 0);
	WCHAR * unicode = new WCHAR[len + 1];
	memset(unicode, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, NULL, ansi.data(), -1, unicode, len);
	//UNICODE转utf8
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	char *utf8 = new char[len + 1];
	memset(utf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, len, NULL, NULL);
	//赋值
	result = utf8;
	delete[] unicode;
	delete[] utf8;
	return result;
#endif
}

void saveToFile(const QString& filename,const QByteArray& content,const QString& codec)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream stream(&file);
	if(codec == "utf-8")
	{
		auto data = AnsiToUtf8(content);
		stream << data;
		file.close();
	}
	else if (codec.isEmpty())
		stream.setCodec("gb2312");

	stream << content;
	file.close();
}

bool exportToCSV(const QString& filename,QTableWidget* table)
{
	QFile file(filename);
	bool ret = file.open(QIODevice::WriteOnly);
	if (!ret)
		return false;

	QTextStream stream(&file);
	QString contents;
	QHeaderView* header = table->horizontalHeader();
	if(header)
	{
		QStringList items;
		for(int i = 0;i<header->count();i++)
		{
			QTableWidgetItem* item = table->horizontalHeaderItem(i);
			if(!item)
				continue;

			items<<item->text().trimmed();
		}

		stream << items.join(",");
		stream << "\n";
	}


	for (int i = 0;i < table->rowCount();i++)
	{
		QStringList items;
		for (int j = 0;j < table->columnCount();j++)
		{
			QTableWidgetItem* item = table->item(i,j);
			if (!item)
				continue;
			
			QString str = item->text();
			str.replace(","," ");

			items << str;
		}
		stream<<items.join(",");
		stream<<"\n";
	}
	file.close();
	return true;
}

bool exportToFile(const QString& filename,QListWidget* list)
{
	QFile file(filename);
	bool ret = file.open(QIODevice::WriteOnly);
	if (!ret)
		return false;

	QTextStream stream(&file);
	for (int i = 0;i<list->count();i++)
	{
		QString code = list->item(i)->text();
		stream << code << ",";
	}

	file.close();
	return true;
}

QString getStockCodeByFileName(const QString& market,const QFileInfo& info,const QString& format)
{
	if (format == "tdx")
		return info.baseName();
	return info.baseName();
}

QFileInfoList getFilesInPath(const QString& market,const QString& path)
{
	QDir dir(path);
	auto all = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
	QFileInfoList output;

#ifdef SUPPORT_HK_MARKET
	if (market == "混合")
		return all;
	if (market == "香港")
	{
		foreach(auto item,all)
			if (item.baseName().startsWith("hk"))
				output += item;
	}
#endif
	if (market == "中国大陆")
	{
		foreach(auto item,all)
			if (item.baseName().startsWith("sh") || item.baseName().startsWith("sz"))
				output += item;
	}
	else
		output = all;
	return output;
}

QFileInfoList getTDXStockFileList(const QString& market,const QString& tdx)
{
	QFileInfoList files;
	files += getFilesInPath(market,tdx + "//vipdoc//sh//lday");
	files += getFilesInPath(market,tdx + "//vipdoc//sz//lday");
	return files;
}

QString getStockFileByStockCode(const QString& code,const QString& path,const QString& format)
{
	QString ret;
	if (format == "tdx")
	{
		if(code.startsWith("sh"))
			ret = path + "//vipdoc//sh//lday//" + code + ".day";
		else
			ret = path + "//vipdoc//sz//lday//" + code + ".day";
	}
	else
		ret = path + "//" + code;
	return ret;
}

QFileInfoList getRandomFileInList(const QFileInfoList& files,int count)
{
	if (files.size() <= count)
		return files;
	QVector<QFileInfo> infos = files.toVector();
	std::random_shuffle(infos.begin(),infos.end());
	return infos.mid(0,count).toList();
}

QFileInfoList getStockFilesByPathAndFormat(const QString& market,const QString& path,const QString& format)
{
	QFileInfoList files;
	if (format == "tdx")
		files = getTDXStockFileList(market,path);
	else
		files = getFilesInPath(market,path);
	return files;
}

bool isRegexCode__(const QString& code,const QString& regex)
{	
	QString input(regex);
	int count = std::min<int>(code.size(),regex.size());
	for (int i = 0;i<count;i++)
	{
		if (regex[i] == '*')
			input[i] = code[i];
	}
	return input == code;
}

QFileInfoList getStockFilesByRegex(const QString& market,const QFileInfoList& files,const QString& regex,const QString& format)
{
	QFileInfoList list;
	foreach(auto item,files)
	{
		QString code = getStockCodeByFileName(market,item,format);
		if (isStockCode(market,code) && isRegexCode__(code,regex))
			list.append(item);
	}
	return list;
}

QFileInfoList getStockFilesByStockCode(const QString& market,const QFileInfoList& files,const QString& code,const QString& format)
{
	QFileInfoList list;
	foreach(auto item,files)
	{
		if(getStockCodeByFileName(market,item,format) == code)
			list += item;   
	}

	return list;
}

QFileInfo getStockFileByPathAndFormatAndCode(const QString& market,const QString& path,const QString& format,const QString& code)
{
	auto list = getStockFilesByPathAndFormat(market,path,format);
	foreach(auto item,list)
	{
		if (getStockCodeByFileName(market,item,format) == code)
			return item;
	}
	return QFileInfo();
}