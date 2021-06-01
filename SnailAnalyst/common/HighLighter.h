#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H
#include <QStringList>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

//! [0]
class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(const QStringList& list,QTextDocument* parent = 0);
protected:
    void highlightBlock(const QString& text);
private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat keywordFormat;
    //QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    //QTextCharFormat quotationFormat;
    //QTextCharFormat functionFormat;
};

#endif
