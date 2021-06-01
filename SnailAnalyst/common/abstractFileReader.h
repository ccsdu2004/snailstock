#ifndef ABSTRACT_FILE_READER_H
#define ABSTRACT_FILE_READER_H
#include <QString>
#include <QIODevice>

class abstractFileReader
{
public:
    abstractFileReader();
    virtual ~abstractFileReader();
public:
    bool parse(const QString& buffer);
	bool parseBuffer(const QString& buffer);
    virtual void doLine(const QString& tag,const QString& value) = 0;
    virtual void doAfter();
private:
};

#endif