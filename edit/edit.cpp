#include <QIcon>
#include <QSet>
#include <qgridlayout.h>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonvalue.h>
#include <qjsonobject.h>
#include "common.h"
#include "edit.h"

QMap<QString,double> getStocks(const QString& fileName)
{
	QMap<QString,double> output;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
	{
	    return output;
    }

	QStringList items;
	QTextStream stream(&file);
	while(!stream.atEnd())
	{
		items = stream.readLine().split(",");
		if(items.size() == 6)
			output.insert(items[0],items[5].toDouble()*0.0001);
	}
	return output;
}

bool hasTextInListWidget(QListWidget* list,const QString& text)
{
	auto items = list->findItems(text,Qt::MatchExactly);
	if(items.isEmpty())
		return false;
	return true;
}

QMap<QString,StockListUnit> stocks;

Edit::Edit(QWidget* parent):
QWidget(parent)
{
	setWindowTitle("π…∆±≥ÿ±‡º≠∆˜");
	setWindowIcon(QIcon(":data/edit.png"));
    setFixedWidth(360);

	QFont font;
	font.setPointSize(11);
	setFont(font);
 
    auto grid = new QGridLayout(this);
    
	list = new QListWidget(this);
	list->setFixedWidth(120);
    
    grid->addWidget(list,0,0,4,1);

    name = new QLineEdit(this);
    grid->addWidget(name,0,1,1,1);

    edit = new QPlainTextEdit(this);
    grid->addWidget(edit,1,1,1,1);

	generate = new QPushButton(this);
	generate->setText("÷ÿ÷√");
	grid->addWidget(generate,2,1,1,1);
	connect(generate,SIGNAL(clicked()),this,SLOT(onGenerate()));

    button = new QPushButton(this);
	button->setText("–¬‘ˆ");
    grid->addWidget(button,3,1,1,1);

	connect(button,SIGNAL(clicked()),this,SLOT(onClick()));
	connect(list,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClicked(QListWidgetItem*)));

	QFile file("config/stockpool.json");
	if(file.open(QIODevice::ReadOnly))
	{
		QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
		if (doc.isArray())
		{
			auto lists = doc.array();
			foreach(auto item,lists)
			{
				auto object = item.toObject();
				QString name = object.value("name").toString();
				auto data = object.value("data").toArray();
				QStringList units;
				foreach(auto i,data)
					units += i.toString();
				StockListUnit slu;
				slu.items = units;
				slu.name = name;
				stocks.insert(name,slu);

				if (!hasTextInListWidget(list,name))
					list->addItem(name);
			}
		}
	}
}

Edit::~Edit()
{
	QJsonDocument doc;
	QJsonArray array;
	foreach(auto item,stocks)
	{
		QJsonArray unit;
		foreach(auto i,item.items)
			unit.append(QJsonValue(i));
		QJsonObject pool;
		pool.insert("name",item.name);
		pool.insert("data",unit);
		array.append(pool);
	}

	doc.setArray(array);
	QString buffer = doc.toJson(QJsonDocument::Indented);
	saveToFile("config/stockpool.json",buffer);
}

void Edit::onGenerate()
{
	QMap<QString,double> stockss = getStocks("data/stocks");
	if(stockss.isEmpty())
		return;

	StockListUnit u11,u20,u50,u90,u200,u500,u10000;
	u11.name = "Œ¢≈Ã";
	u20.name = "Œ¢–°≈Ã";
	u50.name = "–°≈Ã";
	u90.name = "÷–≈Ã";
	u200.name = "÷–¥Û≈Ã";
	u500.name = "¥Û≈Ã";
	u10000.name = "æﬁ≈Ã";

	auto itr = stockss.begin();
	while(itr != stockss.end())
	{
		if(itr.value() < 11.0)
			u11.items.append(itr.key());
		else if(itr.value() < 20.0)
			u20.items.append(itr.key());
		else if(itr.value() < 50.0)
			u50.items.append(itr.key());
		else if(itr.value() < 90.0)
			u90.items.append(itr.key());
		else if(itr.value() < 200.0)
			u200.items.append(itr.key());
		else if(itr.value() < 500.0)
			u500.items.append(itr.key());
		else
			u10000.items.append(itr.key());
	    itr ++;
	}

	if(!hasTextInListWidget(list,"Œ¢≈Ã"))
	    list->addItem(u11.name);
	
	if(!hasTextInListWidget(list,"Œ¢–°≈Ã"))
		list->addItem(u20.name);
	
	if(!hasTextInListWidget(list,"–°≈Ã"))
		list->addItem(u50.name);
	
	if(!hasTextInListWidget(list,"÷–≈Ã"))
		list->addItem(u90.name);
	
	if(!hasTextInListWidget(list,"÷–¥Û≈Ã"))
		list->addItem(u200.name);
	
	if(!hasTextInListWidget(list,"¥Û≈Ã"))
		list->addItem(u500.name);
	
	if(!hasTextInListWidget(list,"æﬁ≈Ã"))
		list->addItem(u10000.name);

	stocks.insert(u11.name,u11);
	stocks.insert(u20.name,u20);
	stocks.insert(u50.name,u50);
	stocks.insert(u90.name,u90);
	stocks.insert(u200.name,u200);
	stocks.insert(u500.name,u500);
	stocks.insert(u10000.name,u10000);

	QFile file(":data/pool.json");
	if (file.open(QIODevice::ReadOnly))
	{
		QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
		if (doc.isArray())
		{
			auto lists = doc.array();
			foreach(auto item,lists)
			{
				auto object = item.toObject();
				QString name = object.value("name").toString();
				auto data = object.value("data").toArray();
				QStringList units;
				foreach(auto i,data)
					units += i.toString();
				StockListUnit slu;
				slu.items = units;
				slu.name = name;
				stocks.insert(name,slu);

				if (!hasTextInListWidget(list,name))
					list->addItem(name);
			}
		}
	}
}

void Edit::onClick()
{
	QSet<QString> set;
	QString dn = name->text();
	if(dn.isEmpty())
		return;

	stocks.remove(dn);
	StockListUnit unit;
	unit.name = dn;
	QStringList items = edit->toPlainText().split("\n");
	set = items.toSet();
	foreach(QString item,set)
		unit.items.append(item);

	stocks.insert(dn,unit);

	name->clear();
	edit->clear();

	auto size = list->findItems(dn,Qt::MatchExactly);
	if(size.isEmpty())
		list->addItem(dn);
}

void Edit::itemClicked(QListWidgetItem* item)
{
	StockListUnit unit = stocks[item->text()];
	name->setText(unit.name);
	edit->setPlainText(unit.items.join("\n"));
}