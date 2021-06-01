#include <QGridLayout>
#include "SeaScriptDialog.h"
#include "SeaScript.h"
#include "../common/common.h"
#include "common/FileUtils.h"
#include "style/WidgetStyle.h"

SeaScriptDialog::SeaScriptDialog(QWidget* parent)
{
	setWindowTitle("Sea插件清单");
	setWindowIcon(QIcon(":logo/snail.png"));
    resize(560,300);
    
	auto grid = new QGridLayout(this);
    list = new QListWidget();
	list->setFixedWidth(92);
	grid->addWidget(list,0,0,1,1);

    edit = new QTextEdit();
	setTextEditStyle(edit);
    grid->addWidget(edit,0,1,1,2);

    auto horizontalSpacer = new QSpacerItem(268,20,QSizePolicy::Expanding,QSizePolicy::Minimum);
    grid->addItem(horizontalSpacer,1,0,1,2);

    button = new QPushButton();
	button->setText("确定");
	button->setFixedWidth(72);
    grid->addWidget(button,1,2,1,1);

	connect(list,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClicked(QListWidgetItem*)));

	QStringList keys = SeaScriptManager::getInstance()->getSeaScriptKeys();
	list->addItems(keys);

	edit->setReadOnly(true);
	QFont font;
	font.setPointSize(10);
	edit->setFont(font);
}

SeaScriptDialog::~SeaScriptDialog()
{
}

void SeaScriptDialog::itemClicked(QListWidgetItem* item)
{
	QString name = item->text();
	SeaScript script = SeaScriptManager::getInstance()->getSeaScript(name);

	auto buffer = readAllBufferFromFile(script.file);
	edit->setText(buffer);
}