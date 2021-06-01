#ifndef SNAILSTOCKMSGBOX_H
#define SNAILSTOCKMSGBOX_H
#include <QDialog>

#pragma execution_character_set("utf-8")

class SnailStockMsgBox : public QDialog
{
    Q_OBJECT
public:
	SnailStockMsgBox(const QString& title,const QString& msg,QWidget* parent = 0);
	~SnailStockMsgBox();
};

#define POP_MSGBOX(title,msg,ptr)\
    {\
	    SnailStockMsgBox box(title,msg,ptr);\
		box.exec();\
    }

#endif 