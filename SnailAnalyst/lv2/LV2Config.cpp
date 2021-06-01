#include <QBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "math/math.h"
#include "common/FileUtils.h"
#include "SnailApplication.h"
#include "LV2Config.h"

QString LV2Config::ConfigFile = "config/lv2Config.json";

bool LV2Config::load()
{
	auto buffer = readAllBufferFromFile(ConfigFile);
	auto doc = QJsonDocument::fromJson(buffer.toLatin1());
	if(doc.isNull())
		return false;

	auto object = doc.object();

	dataPath = object["path"].toString();
	if(dataPath.isEmpty())
		dataPath = "F://LV2//";

	_1 = object["_1"].toInt();
	_1 = 10000*std::clamp(_1,4,10);
	_2 = object["_2"].toInt();
	_2 = 10000*std::clamp(_2,20,50);
	_3 = object["_3"].toInt();
	_3 = 10000*std::clamp(_3,80,120);
	_4 = object["_4"].toInt();
	_4 = 10000*std::clamp(_4,400,800);
	count = object["count"].toInt();
	minLimitR = object["minLimitR"].toDouble();
	selectMode = object["selectMode"].toBool();
	usePython = object["usePython"].toBool();
	return true;
}

bool LV2Config::save()
{
	QJsonObject object;
	object.insert("path",dataPath);
	object.insert("_1",_1/10000);
	object.insert("_2",_2/10000);
	object.insert("_3",_3/10000);
	object.insert("_4",_4/10000);
	object.insert("minLimitR",minLimitR);
	object.insert("usePython",usePython);
	object.insert("selectMode",selectMode);
	object.insert("count",count);

	QJsonDocument doc;
	doc.setObject(object);
	auto json = doc.toJson(QJsonDocument::JsonFormat::Indented);
	saveToFile(ConfigFile,json);
	return true;
}

LV2ConfigWidget::LV2ConfigWidget(QWidget* parent):
QDialog(parent)
{
	ui.setupUi(this);
	setWindowTitle("LV2分析参数设置");
	setFixedSize(240,198);

	ui.filePath->setText("F://LV2");

	connect(ui.save,&QPushButton::clicked,[&]()
	   {
		   LV2Config* config = &SnailApplication::getInstance()->lv2Config;
		   config->dataPath = ui.filePath->text();
		   config->_1 = 10000*ui.spinBox1->value();
		   config->_2 = 10000*ui.spinBox2->value();
		   config->_3 = 10000*ui.spinBox3->value();
		   config->_4 = 10000*ui.spinBox4->value();
		   config->count = ui.spinBox->value();
		   config->usePython = ui.checkBox->isChecked();
		   config->selectMode = ui.selectStock->isChecked();
		   config->save();
		   close();
	   }
	);

	LV2Config config = SnailApplication::getInstance()->lv2Config;

	ui.filePath->setText(config.dataPath);
	ui.spinBox->setValue(config.count);
	ui.spinBox1->setValue(config._1/10000);
	ui.spinBox2->setValue(config._2/10000);
	ui.spinBox3->setValue(config._3/10000);
	ui.spinBox4->setValue(config._4/10000);
	ui.selectStock->setChecked(config.selectMode);
	ui.checkBox->setChecked(config.usePython);
}

LV2ConfigWidget::~LV2ConfigWidget()
{
}