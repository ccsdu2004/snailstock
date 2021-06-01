#ifndef MAP_TABLE_WIDGET_H
#define MAP_TABLE_WIDGET_H
#include <QMap>
#include <QList>
#include <QPair>
#include <QTableWidget>
#include "Stock.h"

class MapTableWidget : public QTableWidget
{
    Q_OBJECT
public:
	MapTableWidget(QWidget* parent = nullptr);
	~MapTableWidget();
public:
	void initTable(const QList<QPair<QString,QString>>& head,bool withCheckBox,bool number = true);
	void setBackgroundCallback(std::function<long(const QStringList&,const QMap<QString,Stock>&)> fn,const QMap<QString,Stock>& data);
	void addData(const QMap<QString,QString>& data);
	void clearTable();
private:
	QList<QPair<QString,QString>> header;
	bool withCheckBox;
	bool withNumber;

	QMap<QString,Stock> stockData;
	std::function<long(const QStringList&,const QMap<QString,Stock>&)> backgroundFn;
};

#endif
