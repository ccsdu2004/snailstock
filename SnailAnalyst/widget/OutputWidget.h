#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H
#include <QDialog>

class OutputWidget : public QDialog
{
    Q_OBJECT
public:
	OutputWidget(const QString& content,QWidget* parent = nullptr);
};

#endif