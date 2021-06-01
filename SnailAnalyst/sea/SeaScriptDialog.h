#ifndef SEA_SCRIPT_DIALOG
#define SEA_SCRIPT_DIALOG
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qtextedit.h>

class SeaScriptDialog : public QDialog
{
    Q_OBJECT
public:
	SeaScriptDialog(QWidget* parent = nullptr);
	~SeaScriptDialog();
private slots:
    void itemClicked(QListWidgetItem* item);
private:
	QListWidget* list;
	QTextEdit*   edit;
	QPushButton* button;
};

#endif