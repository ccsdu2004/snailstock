#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>
#include "config.h"
#include "SnailStockMsgBox.h"

SnailStockMsgBox::SnailStockMsgBox(const QString& title,const QString& msg,QWidget* parent):
QDialog(parent)
{
	setWindowIcon(QIcon(":logo/snail.png"));
	setWindowTitle(title);

    QGridLayout* gridLayout = new QGridLayout(this);
    QLabel* label = new QLabel(msg);
	label->setOpenExternalLinks(true);
    label->setAlignment(Qt::AlignCenter);
	label->setWordWrap(true);
	label->adjustSize();
    gridLayout->addWidget(label,0,0,1,3);

    QFrame* line = new QFrame();
	line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    gridLayout->addWidget(line,1,0,2,3);

    QPushButton* button = new QPushButton("È·¶¨");
    gridLayout->addWidget(button,2,1,2,1);
	connect(button,SIGNAL(clicked()),this,SLOT(close()));

    QSpacerItem* horizontalSpacer = new QSpacerItem(66,20,QSizePolicy::Expanding,QSizePolicy::Minimum);
	gridLayout->addItem(horizontalSpacer,3,0,1,1);
    horizontalSpacer = new QSpacerItem(66,20,QSizePolicy::Expanding,QSizePolicy::Minimum);
    gridLayout->addItem(horizontalSpacer,3,2,1,1);    
}

SnailStockMsgBox::~SnailStockMsgBox()
{
}