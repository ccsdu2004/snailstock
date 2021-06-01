#ifndef EDIT_H
#define EDIT_H
#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStringList>
#include <QMap>
#include <QVector>

#pragma execution_character_set("utf-8")

struct StockListUnit
{
    QString name;
	QStringList items;
};

extern QMap<QString,StockListUnit> stocks;

class Edit : public QWidget
{
	Q_OBJECT
public:
	Edit(QWidget* parent = 0);
	~Edit();
private slots:
    void onClick();
	void onGenerate();
	void itemClicked(QListWidgetItem* item);
private:
	QListWidget* list;
	QLineEdit* name;
	QPlainTextEdit* edit;
	QPushButton* generate;
	QPushButton* button;
};

#endif // EDIT_H
