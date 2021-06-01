#ifndef STOCK_NATURE_ANALYZER_H
#define STOCK_NATURE_ANALYZER_H
#include <QMap>
#include <QDialog>
#include <QPushbutton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QMetaType>
#include <QThread>
#include "Stock.h"
#include "interface/StockParser.h"
#include "../widget/MapTableWidget.h"

class StockAnalezeThread : public QThread
{
    Q_OBJECT
public:
	StockAnalezeThread(QObject* parent);
	~StockAnalezeThread();

	void setData(const QStringList& stocks,const QVector<Stock>& ref,const QMap<QString,QMap<QString,QString>>& per);
	void run();

	void processInThread(const QString& code);
signals:
	int updateData(const QString& code);
private:
	QVector<Stock> ref;
	QStringList stocks;
	QMap<QString,QMap<QString,QString>> maper;
	QMap<QString,QMap<QString,QString>> per;
	QScopedPointer<StockParser> stockParser;
};

class StockNatrueAnalyzer : public QDialog
{
	Q_OBJECT
	static int getRangeIndex(double value);
public:
	StockNatrueAnalyzer(QWidget* parent = nullptr);
	~StockNatrueAnalyzer();
public slots:
	void onClear();
	void onAnalyze();
	void onExport();
	void insert(const QString& code,const QMap<QString,QString>& data);
	void writeData(const QString& code);
private:
	QLineEdit* stockPoolEdit;
	QPushButton* button;
	QPushButton* clearButton;
	MapTableWidget* tableWidget;

	StockAnalezeThread* thread;
	QVector<Stock> ref;
	QMap<QString,QMap<QString,QString>> data;
	QMap<QString,QMap<QString,QString>> per;
};



#endif

