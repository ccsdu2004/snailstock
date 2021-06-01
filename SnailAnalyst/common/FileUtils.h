#ifndef FILE_UTILS_H
#define FILE_UTILS_H
#include <QString>
#include <qfileinfo.h>

#pragma execution_character_set("utf-8")

class QListWidget;
class QTableWidget;

QString readAllBufferFromFile(const QString& file);
void saveToFile(const QString& file,const QByteArray& content,const QString& codec = QString());

bool exportToFile(const QString& file,QListWidget* list);
bool exportToCSV(const QString& file,QTableWidget* table);

QString getStockCodeByFileName(const QString& market,const QFileInfo& info,const QString& format);
QString getStockFileByStockCode(const QString& code,const QString& path,const QString& format);

QFileInfoList getFilesInPath(const QString& market,const QString& path);
QFileInfoList getTDXStockFileList(const QString& market,const QString& tdx);

QFileInfoList getRandomFileInList(const QFileInfoList& files,int count);

QFileInfoList getStockFilesByPathAndFormat(const QString& market,const QString& path,const QString& format);
QFileInfoList getStockFilesByRegex(const QString& market,const QFileInfoList& files,const QString& regex,const QString& format);
QFileInfoList getStockFilesByStockCode(const QString& market,const QFileInfoList& files,const QString& code,const QString& format);
QFileInfo     getStockFileByPathAndFormatAndCode(const QString& market,const QString& path,const QString& format,const QString& code);

#endif