#ifndef STOCK_COR_DIALOG
#define STOCK_COR_DIALOG
#include <qdialog.h>

#pragma execution_character_set("utf-8")

class QLineEdit;

class StockCorDialog : public QDialog
{
    Q_OBJECT
public:
	StockCorDialog(QWidget* parent = nullptr);
private slots:
	void setValue();
private:
	QLineEdit* stock1;
	QLineEdit* stock2;
	QLineEdit* value;
};

#endif