#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include "AutoTextEdit.h"

AutoTextEdit::AutoTextEdit(QWidget* parent): 
QTextEdit(parent),
completer(0)
{
}

AutoTextEdit::~AutoTextEdit()
{
}

void AutoTextEdit::setCompleter(QCompleter* icompleter)
{
    if(completer)
        QObject::disconnect(completer,0,this,0);

    completer = icompleter;

    if(!completer)
        return;
	
    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);

	//completer->popup()->setStyleSheet("background-color:#294662;color:#ffffff;border:1px solid #375C80;border-radius:5px;padding:0px 0px 0px 0px;min-width:17px;");
	completer->popup()->setStyleSheet("background-color:#dddd99;border:1px solid #dedede;border-radius:5px;min-width:17px;font-size: 14px;selection-background-color:#aaaaaa;color:#d0a0a0");
    QObject::connect(completer,SIGNAL(activated(QString)),this,SLOT(insertCompletion(QString)));
}

QCompleter* AutoTextEdit::getCompleter() const
{
    return completer;
}

void AutoTextEdit::insertCompletion(const QString& completion)
{
    if(completer->widget() != this)
        return;
    
    QTextCursor tc = textCursor();
    int extra = completion.length() - completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString AutoTextEdit::textUnderCursor() const
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    return cursor.selectedText();
}

void AutoTextEdit::focusInEvent(QFocusEvent *e)
{
    if(completer)
        completer->setWidget(this);
    QTextEdit::focusInEvent(e);
}

void AutoTextEdit::keyPressEvent(QKeyEvent *e)
{
    if(completer && completer->popup()->isVisible()) 
    {
       switch(e->key()) 
       {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; 
       default:
           break;
       }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E);
    if(!completer || !isShortcut)
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if(!completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if(!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 1
                      || eow.contains(e->text().right(1)))) 
    {
		completer->popup()->hide();
        return;
    }

    if(completionPrefix != completer->completionPrefix()) 
    {
		completer->setCompletionPrefix(completionPrefix);
		completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
    }
    
    QRect cr = cursorRect();
    cr.setWidth(completer->popup()->sizeHintForColumn(0)
                + completer->popup()->verticalScrollBar()->sizeHint().width());
	completer->complete(cr);
}
