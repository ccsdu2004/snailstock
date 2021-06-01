#include <qgridlayout.h>
#include <qpushbutton.h>
#include <QSpacerItem>
#include "widget/LineDelegate.h"
#include "ScriptManager.h"

#define CELL(i,value) (i = new QTableWidgetItem(QString("%1").arg(value)))

ScriptAddonView::ScriptAddonView(QWidget* parent):
QDialog(parent)
{
	setWindowTitle("脚本插件列表");
    auto grid = new QGridLayout(this);
	setFixedSize(480,240);
	table = new QTableWidget(this);
    grid->addWidget(table,0,0,1,2);

    auto spacer = new QSpacerItem(298,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
    grid->addItem(spacer,1,0,1,1);

    button = new QPushButton(this);
	button->setText("关闭");
    grid->addWidget(button,1,1,1,1);
	connect(button,SIGNAL(clicked()),this,SLOT(close()));

	QStringList header;
    table->setColumnCount(5);     
    header<<"名称"<<"描述"<<"返回值类型"<<"版本"<<"文件";
	table->setHorizontalHeaderLabels(header);

#ifdef SUPPORT_SCRIPT

	auto addons = ScriptManager::instance().getScriptInfos();
	foreach(auto addon,addons)
	{
	    int row = table->rowCount();
        table->insertRow(row);

	    QTableWidgetItem* i = 0;
		table->setItem(row,0,CELL(i,addon->name));
		table->setItem(row,1,CELL(i,addon->description));
		table->setItem(row,2,CELL(i,addon->retType));
		table->setItem(row,3,CELL(i,addon->version));
		table->setItem(row,4,CELL(i,addon->filename));
	}

#endif

	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
    
	table->verticalHeader()->setVisible(false);
	table->horizontalHeader()->setVisible(true);

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->setEditTriggers(QTableView::NoEditTriggers);
	table->setAlternatingRowColors(true);

    table->setShowGrid(false);
	table->setItemDelegate(new LineDelegate(table));
	table->setSortingEnabled(true);
}

ScriptAddonView::~ScriptAddonView()
{
}
