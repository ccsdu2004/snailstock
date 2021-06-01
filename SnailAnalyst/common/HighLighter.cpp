#include <QtGui>
#include "highlighter.h"

Highlighter::Highlighter(const QStringList& list,QTextDocument* parent):
QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    foreach(const QString& pattern,list) 
	{
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
	}

    /*classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);*/
//! [2]

//! [3]
    singleLineCommentFormat.setForeground(Qt::gray);
	rule.pattern = QRegExp("#.*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

//! [3]

//! [4]
    /*quotationFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegExp("#(.*)\n");
    rule.format = quotationFormat;
    highlightingRules.append(rule);*/
//! [4]
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}
//! [11]
