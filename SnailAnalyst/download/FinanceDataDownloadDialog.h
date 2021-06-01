#ifndef FINANCE_DATA_DOWNLOAD_DLG_H
#define FINANCE_DATA_DOWNLOAD_DLG_H
#include <QDialog>
#include <QProgressbar>
#include <QPushButton>
#include <QLineEdit>
#include <QStringList>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <QtConcurrent>

class FinanceDataDownloadDialog : public QDialog
{
	Q_OBJECT
public:
	FinanceDataDownloadDialog(QWidget* parent = nullptr);
	~FinanceDataDownloadDialog();
private slots:
	void onClick();
private:
	void download(const QStringList& codec);
	int  downloadInThread(const QString& code);
private:
	QProgressBar* progress;
	QPushButton* button;
	QLineEdit* input;

	QJsonArray stockJsonArray;
	QFutureWatcher<int>* downloadWatcher;
};

#endif