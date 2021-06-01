#include <QHeaderView>
#include "widget/LineDelegate.h"
#include "MapTableWidget.h"
#include "common/common.h"

#include <QDebug>

#define CELL(i,value) (i = new QTableWidgetItem(QString("%1").arg(value)))
#define CELL_ORDER(i,value) (i = new OrderTableWidgetItem(QString("%1").arg(STR_CEIL(value))))

MapTableWidget::MapTableWidget(QWidget* parent):
QTableWidget(parent)
{
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	verticalHeader()->setVisible(false);
	horizontalHeader()->setVisible(true);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setEditTriggers(QTableView::NoEditTriggers);
	setAlternatingRowColors(true);

	setShowGrid(false);
	setItemDelegate(new LineDelegate(this));
	setSortingEnabled(true);
}

MapTableWidget::~MapTableWidget()
{
}

void MapTableWidget::initTable(const QList<QPair<QString,QString>>& head,bool checkBox,bool number)
{
	header = head;
	withCheckBox = checkBox;
	if(withCheckBox)
	{
		QPair<QString,QString> check;
		header.append(check);
	}

	setColumnCount(header.size());

	QStringList items;
	auto itr = header.begin();
	while(itr != header.end())
	{
		items << (*itr).second;
		itr++;
	}

	setHorizontalHeaderLabels(items);
	withNumber = number;
}

void MapTableWidget::setBackgroundCallback(std::function<long(const QStringList&,const QMap<QString,Stock>&)> fn,
	const QMap<QString,Stock>& data)
{
    backgroundFn = fn;
	stockData = data;
}

void MapTableWidget::clearTable()
{
	clearContents();
	setRowCount(0);
}

void MapTableWidget::addData(const QMap<QString,QString>& data)
{
	if(!data.contains(header.at(0).first))
		return;
	
	int row = rowCount();
	insertRow(row);

	QStringList items;

	for(int j = 0;j<header.size();j++)
		items += data[header[j].first];

	long color = 0;
	bool withColor = false;
	if(backgroundFn)
		color = backgroundFn(items,stockData);
	if(color != 0)
		withColor = true;

	if(color == 1)
		color = qRgb(240,0,0);
	else if(color == 2)
		color = qRgb(0,0,240);
	else if(color == 3)
		color = qRgb(0,240,0);
	else if(color == 4)
		color = qRgb(0,240,240);
	else if(color == 5)
		color = qRgb(240,240,0);
	else if(color == 6)
		color = qRgb(240,0,240);
	else if(color == 7)
		color = qRgb(128,255,255);
	else if(color == 8)
		color = qRgb(255,255,128);

	QTableWidgetItem* i = 0;
	for(int j=0;j<header.size();j++)
	{
		auto node = header[j];
		if(withCheckBox && j == header.size()-1)
		{
			QTableWidgetItem* checkItem = new QTableWidgetItem;
			checkItem->setCheckState(Qt::CheckState::Unchecked);
			setItem(row,j,checkItem);
		}
		else
		{
			auto value = data[node.first];
			if(node.first=="name")
			{
				value = unicodeToUtf8(value);
			}
			
			if(j<=1)
			{
				setItem(row,j,CELL(i,value));
				if(withColor)
				    i->setBackgroundColor(QColor(color));
			}
			else
			{
				if(withNumber)
					setItem(row,j,CELL_ORDER(i,value.toDouble()));
				else
					setItem(row,j,CELL(i,value));

				if(withColor)
					i->setBackgroundColor(QColor(color));
			}
		}
	}
}

