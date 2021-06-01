#ifndef STOCK_SHAPE_SCANNER_H
#define STOCK_SHAPE_SCANNER_H
#include "Indicator.h"
#include "common/common.h"

class StockShapeScanner : public StockScanner
{
	Q_OBJECT
public:
	StockShapeScanner();
	~StockShapeScanner();
	virtual int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
	//date price data price 
	DataWrapper7<QString,float,QString,float,QString,float,QString> desc()const{return output;}
protected:
    DataWrapper7<QString,float,QString,float,QString,float,QString> output;
};

class StockMShapeScanner : public StockShapeScanner
{
	Q_OBJECT
public:
	StockMShapeScanner();
	~StockMShapeScanner();

	int type()const{return SCANNER_SHAPE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
    int checkTopPolar(float& price,int from,int to=-1);
    int checkLowPolar(float& price,int from,int to);
private:
	QString code;
	QVector<Stock> list;
	
	MaNIndicator ma5;
	MaNIndicator ma10;
	MaNIndicator ma20;
};

class StockDuanTouChaDaoScanner : public StockShapeScanner
{
	Q_OBJECT
public:
	StockDuanTouChaDaoScanner();
	~StockDuanTouChaDaoScanner();

	int type()const{return SCANNER_SHAPE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
	QString code;
	
	MaNIndicator ma5;
	MaNIndicator ma10;
	MaNIndicator ma20;
};
/*
class StockYiYangChuanSanXianScanner : public StockShapeScanner
{
	Q_OBJECT
public:
	StockYiYangChuanSanXianScanner();
	~StockYiYangChuanSanXianScanner();

	int type()const{return SCANNER_SHAPE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
	QString code;
	
	MaNIndicator ma5;
	MaNIndicator ma10;
	MaNIndicator ma30;
};*/

class StockGaoWeiHengPanScanner : public StockShapeScanner
{
	Q_OBJECT
public:
	StockGaoWeiHengPanScanner();
	~StockGaoWeiHengPanScanner();

	int type()const{return SCANNER_SHAPE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
};

class StockDiWeiHengPanScanner : public StockShapeScanner
{
	Q_OBJECT
public:
	StockDiWeiHengPanScanner();
	~StockDiWeiHengPanScanner();

	int type()const{return SCANNER_SHAPE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
};

class StockShangShengSanJiaoXingScanner : public StockShapeScanner
{
	Q_OBJECT
public:
	StockShangShengSanJiaoXingScanner();
	~StockShangShengSanJiaoXingScanner();
	int type()const{return SCANNER_SHAPE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
	QString code;
	QVector<Stock> list;
};

class StockWShapeScanner : public StockShapeScanner
{
	Q_OBJECT
public:
	StockWShapeScanner();
	~StockWShapeScanner();

	int type()const{return SCANNER_SHAPE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
    int checkTopPolar(float& price,int from,int to=-1);
    int checkLowPolar(float& price,int from,int to);
private:
	QString code;
	QVector<Stock> list;
	
	MaNIndicator ma5;
	MaNIndicator ma10;
	MaNIndicator ma20;
};
/*
class StockUpwardTrendScanner : public StockShapeScanner
{
	Q_OBJECT
	static const int SHORT_TERM_MA = 5;
	static const int LONG_TERM_MA  = 20;//21;
public:
	StockUpwardTrendScanner();
	~StockUpwardTrendScanner();

	int type()const{return SCANNER_SHAPE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
    int checkTopPolar(float& price,int from,int to=-1);
    int checkLowPolar(float& price,int from,int to);
private:
	QString code;
	QVector<Stock> list;
	
	MaNIndicator ma5;
	MaNIndicator ma20;
	int minDistance;
	float maxIncrease;
	float maxDecrease;
	float currentMaxIncrease;
	int maxLap;
};*/

class StockHengPanTuPoScanner : public StockShapeScanner
{
	Q_OBJECT
public:
	StockHengPanTuPoScanner();
	~StockHengPanTuPoScanner();

	int type()const{return SCANNER_SHAPE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
};

#endif 
