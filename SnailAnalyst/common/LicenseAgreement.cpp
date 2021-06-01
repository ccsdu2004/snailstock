#include <QPixmap>
#include <QHBoxLayout>
#include "LicenseAgreement.h"
#include "common.h"

LicenseAgreement::LicenseAgreement(const QString& desc,QWidget* parent):
QDialog(parent),
showSnail(false)
{
	createWidget(desc);
	connnectSlots();
}

LicenseAgreement::~LicenseAgreement()
{
}

bool LicenseAgreement::showMainWindow()
{
    return showSnail;
}

void LicenseAgreement::onYes()
{
	close();

	//ConfigFile config("config.ini");
	//config.setVariable("config","init","true");
	showSnail = true;
}

void LicenseAgreement::onNo()
{
    close();
}

void LicenseAgreement::connnectSlots()
{
	connect(mYesButton,SIGNAL(clicked()),this,SLOT(onYes()));
    connect(mNoButton,SIGNAL(clicked()),this,SLOT(onNo()));
}

void LicenseAgreement::createWidget(const QString& desc)
{
    resize(400,320);	
    setWindowIcon(QIcon(":logo/snail.png"));
    setWindowTitle(QString("蜗牛股票量化分析软件%1").arg(SNAIL_VERSION));

	auto vbox = new QVBoxLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    
	QLabel* label = new QLabel(this);
	QPixmap pixmap(":logo/snail.png");
	label->setPixmap(pixmap);
    //label->setScaledContents(true);
    horizontalLayout->addWidget(label);

    QLabel* label_2 = new QLabel(this);
	label_2->setText("请阅读下面的许可协议");
    horizontalLayout->addWidget(label_2);

    QSpacerItem* horizontalSpacer = new QSpacerItem(40,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    vbox->addLayout(horizontalLayout);

    mDesc = new QPlainTextEdit(this);
	mDesc->appendHtml(desc);
	mDesc->setReadOnly(true);
	QTextCursor cursor = mDesc->textCursor();
	cursor.movePosition(QTextCursor::Start);
    vbox->addWidget(mDesc);

    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout();
    QSpacerItem* horizontalSpacer_2 = new QSpacerItem(40,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_2->addItem(horizontalSpacer_2);

    QLabel* label_3 = new QLabel(this);
	label_3->setText("您是否接受以上协议");
    horizontalLayout_2->addWidget(label_3);

    QSpacerItem* horizontalSpacer_3 = new QSpacerItem(40,20,QSizePolicy::Expanding,QSizePolicy::Minimum);
    horizontalLayout_2->addItem(horizontalSpacer_3);

    vbox->addLayout(horizontalLayout_2);

    QHBoxLayout* horizontalLayout_3 = new QHBoxLayout();
    QSpacerItem* horizontalSpacer_4 = new QSpacerItem(40,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_3->addItem(horizontalSpacer_4);

    mYesButton = new QPushButton(this);
	mYesButton->setText("是");
    horizontalLayout_3->addWidget(mYesButton);

    mNoButton = new QPushButton(this);
	mNoButton->setText("否");
    horizontalLayout_3->addWidget(mNoButton);

    QSpacerItem* horizontalSpacer_5 = new QSpacerItem(40,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_3->addItem(horizontalSpacer_5);

    vbox->addLayout(horizontalLayout_3);
	vbox->setMargin(4);
}


 