#include "WidgetStyle.h"

void setTextEditStyle(QTextEdit* edit)
{
	auto pen = edit->palette();
	pen.setColor(QPalette::Base,QColor(255,255,232));
	edit->setPalette(pen);
	QFont font;
	font.setPointSize(11);
	edit->setFont(font);
}