#ifndef LV2CONFIGWIDGET_H
#define LV2CONFIGWIDGET_H
#include <QDialog>
#include "ui_lv2config.h"

struct LV2Config
{
	static QString ConfigFile;

	QString dataPath;
	int _1 = 8;
	int _2 = 50;
	int _3 = 120;
	int _4 = 500;
	int count = 3;
	double minLimitR = 1.8;
	bool usePython = false;
	bool selectMode = false;

	bool load();
	bool save();
};

class LV2ConfigWidget : public QDialog
{
    Q_OBJECT
public:
	LV2ConfigWidget(QWidget* parent = nullptr);
	~LV2ConfigWidget();

private:
	Ui::Form ui;
};

#endif
