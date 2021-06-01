#ifndef LICENSE_AGREEMENT_H
#define LICENSE_AGREEMENT_H
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QDialog>

class LicenseAgreement : public QDialog
{
    Q_OBJECT
public:
	LicenseAgreement(const QString& desc,QWidget* parent = 0);
	~LicenseAgreement();

	bool showMainWindow();
public slots:
    void onYes();
	void onNo();
private:
	void createWidget(const QString& desc);
	void connnectSlots();
private:
	bool showSnail;
	QPlainTextEdit* mDesc;
	QPushButton* mYesButton;
    QPushButton* mNoButton;
};

#endif