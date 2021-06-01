#include <QBoxLayout>
#include <QFont>
#include <QPlainTextEdit>
#include "OutputWidget.h"

OutputWidget::OutputWidget(const QString& content,QWidget* parent) :
QDialog(parent)
{
	setFixedWidth(360);

	auto box = new QVBoxLayout(this);

	QFont font;
	font.setPointSize(12);
	auto edit = new QPlainTextEdit();
	edit->setFont(font);
	edit->setPlainText(content);
	box->addWidget(edit);
}