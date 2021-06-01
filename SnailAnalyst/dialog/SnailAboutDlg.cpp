#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit.h>
#include <QTabWidget>
#include "common/common.h"
#include "style/WidgetStyle.h"
#include "StockScriptEngine.h"
#include "SnailAddonHost.h"
#include "SnailAboutDlg.h"

QTextEdit* createTextEdit(const QString& text,QWidget* parent,bool html)
{
	QTextEdit* edit = new QTextEdit(parent);
	setTextEditStyle(edit);
	edit->setReadOnly(true);
	if(html)
		edit->setHtml(text);
	else
		edit->setText(text);
	return edit;
}

QWidget* createWeiXin(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);

	auto box = new QHBoxLayout(widget);
	auto label = new QLabel("蜗牛股票量化分析软件官方公众号，提供专业的软件使用，策略展示和操作记实等");
	auto pen = label->palette();
	pen.setColor(QPalette::Base,QColor(255,255,232));
	label->setPalette(pen);

	label->setWordWrap(true);
	box->addWidget(label);

	label = new QLabel();
	auto bitmap = QPixmap(":/logo/weixin.png");
	bitmap = bitmap.scaled(QSize(128,128),Qt::KeepAspectRatio);
	label->setPixmap(bitmap);
	label->setPixmap(bitmap);
	label->setTextFormat(Qt::PlainText);
	label->setAlignment(Qt::AlignCenter);
	label->setWordWrap(true);
	label->setIndent(0);
	box->addWidget(label);

	return widget;
}

SnailAboutDlg::SnailAboutDlg(QWidget* parent):
QDialog(parent)
{
    setFixedSize(420,248);
	QGridLayout* grid = new QGridLayout(this);
    
	QLabel* label = new QLabel();
    QFont font;
    font.setPointSize(12);
    label->setFont(font);
    label->setAlignment(Qt::AlignCenter);

	setWindowTitle(QString("蜗牛股票量化分析软件%1").arg(SNAIL_VERSION));
	setWindowIcon(QIcon(":logo/snail.png"));

	QString app = QString("蜗牛股票量化分析软件%1").arg(SNAIL_VERSION);
	label->setText(app);
    grid->addWidget(label,0,0,1,2);

	QTabWidget* tab = new QTabWidget(this);
	grid->addWidget(tab,2,0,1,2);

	tab->addTab(createTextEdit(getSoftwareConfigInformation(),this,true),"软件配置");

	StockScriptEngine engine;
	auto edit = createTextEdit(QString(),this,true);
	QSet<QString> keys = engine.keywords().toSet();
	QStringList list = keys.toList();
	list.sort();
	foreach(auto item,list)
		edit->append(item);
	tab->addTab(edit,"命令参数");

	QWidget* weixin = createWeiXin(this);
	tab->addTab(weixin,"微信公众号");

    QSpacerItem* horizontalSpacer = new QSpacerItem(308,20,QSizePolicy::Expanding,QSizePolicy::Minimum);
    grid->addItem(horizontalSpacer,5,0,1,1);

    QPushButton* button = new QPushButton("确定");
    grid->addWidget(button,5,1,1,1);
	connect(button,SIGNAL(clicked()),this,SLOT(close()));

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    grid->addWidget(line,1,0,1,2);
}

SnailAboutDlg::~SnailAboutDlg()
{
}