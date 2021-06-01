#ifndef INDICATOR_H
#define INDICATOR_H
#include <QVector>
#include <QPair>
#include <QStringList>
#include "Stock.h"
#include "SnailAddonHost.h"
#include "common/common.h"
#include "common/DataWrapper.h"

typedef QVector<DataWrapper5<bool,float,float,float,float>> IndicList;
#define BUILD_INDICATOR_LIST(indicator,stocks)\
	IndicList list;\
	foreach(Stock stock,stocks)\
		list.push_back(indicator.update(stock));

#define DEFAULT_INDICATOR_RET DataWrapper5<bool,float,float,float,float>(false,0,0,0,0)

class NumericIndicator : public QObject
{
	Q_OBJECT
public:
	enum Type
	{
	    ma,
		ema
	};
    static NumericIndicator* createIndicator(Type type,int n,QObject* parent = nullptr);
	NumericIndicator(QObject* parent = nullptr);
	virtual ~NumericIndicator(){}
	virtual QPair<bool,double> update(double value) = 0;
	virtual void clear() = 0;
};

class EMAIndicator : public StockIndicator
{
public:
	EMAIndicator(int n = 6);
	~EMAIndicator();
	void clear();
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
    QVector<Stock> dataN;
	int n;
	float ema;
};

class BollIndicator : public StockIndicator
{
public:
	BollIndicator(int n = 20,double m = 2.0);
	~BollIndicator(){}
	void clear(){datas.clear();}
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	int n;
	double m;
    QVector<Stock> datas;
};

class MacdIndicator : public StockIndicator
{
public:
	MacdIndicator(int ia = 12,int ib = 26,int ic = 9);
	~MacdIndicator();
	void clear()
	{
	    m12 = 0.0f;
		m26 = 0.0f;
		diff = 0.0f;
		dea = 0.0f;
		flag = -1;
	}
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	int a,b,c;
    float m12,m26,diff,dea,bar;
	int flag;
};

class KDJIndicator : public StockIndicator
{
public:
	KDJIndicator();
	~KDJIndicator();
	void clear()
	{
	    datas.clear();
		k = d = j = 0;
	}
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	void findMinAndMax(float& min,float& max);
private:
    QVector<Stock> datas;
	float k,d,j;
};

class MaNIndicator : public StockIndicator
{
public:
	MaNIndicator(int n = 5);
	~MaNIndicator();
	void clear(){datas.clear();}
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
    QVector<Stock> datas;
	int n;
};

class BiasIndicator : public StockIndicator
{
public:
	BiasIndicator(int n = 12);
	~BiasIndicator();
	void clear();
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
    MaNIndicator* maIndicator; 
	bool init;
};

class TRIndicator : public StockIndicator
{
public:
	TRIndicator();
	~TRIndicator();
	void clear();
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	bool init;
	Stock prev;
};

class WRIndicator : public StockIndicator
{
public:
	WRIndicator(int n = 6);
	~WRIndicator();
	void clear(){datas.clear();}
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
    QVector<Stock> datas;
	int n;
};

class ADStockIndicator : public StockIndicator
{
public:
	ADStockIndicator();
	~ADStockIndicator();
	void clear(){ sum = 0.0f; }
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	float sum;
};

class OBVStockIndicator : public StockIndicator
{
public:
	OBVStockIndicator();
	~OBVStockIndicator();
	void clear(){ obv = 0.0f; }
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	float obv;
};

class BOPStockIndicator : public StockIndicator
{
public:
	BOPStockIndicator();
	~BOPStockIndicator();
	void clear(){}
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
};

class PVIStockIndicator : public StockIndicator
{
public:
	PVIStockIndicator();
	~PVIStockIndicator();
	void clear(){ prev.tradeVolume = 0.0f; pvi = 1000.0f; init = false; }
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	bool init;
	Stock prev;
	float pvi;
};

class StockWadIndicator : public StockIndicator
{
public:
	StockWadIndicator();
	~StockWadIndicator();
	void clear(){ init = false; sum = 0; }
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	bool init;
	float sum;
};

/*
class MDStockIndicator : public StockIndicator
{
public:
	MDStockIndicator(int period = 12);
	~MDStockIndicator();
	void clear(){ datas.clear();}
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	QVector<Stock> datas;
	int n;
	float sum;
};*/

class PSYIndicator : public StockIndicator
{
public:
	PSYIndicator(int n = 10);
	~PSYIndicator();
	void clear();
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
    QVector<Stock> datas;
	int n;
};

class VolMaNIndicator : public StockIndicator
{
public:
	VolMaNIndicator(int n = 5);
	~VolMaNIndicator();
	void clear(){datas.clear();}
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
    QVector<Stock> datas;
	int n;
};

class DMIStockIndicator : public StockIndicator
{
public:
	DMIStockIndicator(int n = 14,int m = 6);
	~DMIStockIndicator();
	void clear();
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	int spanN,spanM;
	QVector<Stock> dataN;
	QVector<double> dxM;
	QVector<double> adxM;
	float prevMAdx;
	float prevDpi;
	float prevMdi;
	float prevAdx;
	float prevAdxr;
};

class RSIIndicator : public StockIndicator
{
public:
	RSIIndicator(int in=6);
	~RSIIndicator();
	void clear()
	{
		up = down = 0.0f;
		data.clear();
	}
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	int n;
	float up,down;
    QVector<Stock> data;
};

class StockScanner : public QObject
{
	Q_OBJECT
public:
	StockScanner();
	virtual ~StockScanner();
public:
	static const int SCANNER_NONE = -1;
	static const int SCANNER_PRICE = 0;
	static const int SCANNER_VOL = 1;
	static const int SCANNER_K = 2;
	static const int SCANNER_SHAPE = 3;
	static const int SCANNER_BREACH = 4;
	static const int SCANNER_KDJ = 5;
	static const int SCANNER_PRICEPOS = 6;
   
	static const int SCANNER_QUANT = 95;
	static const int SCANNER_HOT = 96;
	static const int SCANNER_TREND = 97;
	static const int SCANNER_NEWSTOCK = 98;
	static const int SCANNER_TAIL = 99;

	virtual int type()const {return SCANNER_NONE;}
    //0 for s,1 for w
	virtual int check(const QString& code,const QVector<Stock>& list,QStringList& desc) {return -1;}
};

/*
class StockMACDScanner : public StockScanner
{
	Q_OBJECT
public:
	StockMACDScanner();
	~StockMACDScanner();

	int type()const{return SCANNER_PRICE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
	MacdIndicator indicator;
};

class StockKDJScanner : public StockScanner
{
	Q_OBJECT
public:
	StockKDJScanner();
	~StockKDJScanner();
	int type()const{return SCANNER_KDJ;}

	static const int KD_OVERSOLD = 1;
	static const int KD_OVERBUY = 2;
	static const int KD_JC = 3;
	static const int KD_SC = 4;
	static const int KD_OVERSOLD_JC = 5;
	static const int KD_OVERBUY_SC = 6;
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
	KDJIndicator indicator;
};

class StockVolScanner : public StockScanner
{
	Q_OBJECT
public:
	StockVolScanner();
	~StockVolScanner();

	int type()const{return SCANNER_VOL;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
	VolMaNIndicator vol5,vol10,vol60;
};

class StockMaNScanner : public StockScanner
{
public:
	StockMaNScanner();
	~StockMaNScanner();

	int type()const{return SCANNER_PRICE;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
    MaNIndicator ma5,ma10;
};*/

class PricePositionScanner : public StockScanner
{
	Q_OBJECT
	static const int DAYS = 20;
public:
	int type()const{return SCANNER_PRICEPOS;}
    //0 for s,1 for w,-1 for none
	int check(const QString& code,const QVector<Stock>& list,QStringList& desc);
};

class LianDieNDaysScanner : public StockScanner
{
	Q_OBJECT
public:
	LianDieNDaysScanner(int day = 5);
	~LianDieNDaysScanner();

	int type()const{return -1;}
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc = QStringList());
private:
	int days;
};
/*
class StockCommentFactory : public StockScanner
{
	Q_OBJECT
    DECLARE_SINGLETON(StockCommentFactory)
public:
	void addStockScanner(StockScanner* scanner){scanners.append(scanner);}

	int type()const{return -1;}
	int check(const QString& code,const QVector<Stock>& list,QStringList& desc);
	QMap<int,QStringList> data()const{return output;}
private:
	void fillString(int id);
private:
	QMap<int,QStringList> output;
	QVector<StockScanner*> scanners;
};*/

class StockTailScanner : public StockScanner
{
	Q_OBJECT
public:
	StockTailScanner();
	~StockTailScanner();
	int type()const{return SCANNER_TAIL;}

	bool isStockinRange(const Stock& stock);
	int check(const QString& stock,const QVector<Stock>& list,QStringList& desc);
private:
};

int computeStockScore(const QString& file);
bool isNDaysTop(const QVector<Stock>& stocks,int n,int pos);
bool checkNDaysOverBollTop(const QVector<std::tuple<bool,float,float,float,float>>& datas,float value,int n);
bool isMacdJX(const QVector<Stock>& stocks);

bool isDiKaiDiZou(const QVector<Stock>& stocks);
int checkStockMA5Vol(const QString& code,const QVector<Stock>& list,QString& tag);

#endif 