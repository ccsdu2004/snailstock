#ifndef COMMON_H
#define COMMON_H
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QMap>
#include <QScopedPointer>
#include <QVector>
#include <QDate>
#include <QtableWidget>
#include "SnailAddonHost.h"

#define SNAIL_VERSION "4.8.5.0"

#pragma execution_character_set("utf-8")

#undef SNAIL_STOCK_USE_163
#undef SNAIL_STOCK_USE_IFENG
#undef SNAIL_STOCK_USE_QQ

#define SNAIL_STOCK_USE_QQ

#ifdef SNAIL_STOCK_USE_163
	#ifdef SNAIL_STOCK_USE_IFENG
        #ifndef SNAIL_STOCK_USE_QQ
            #error error occur!
        #endif 
	#endif 
#endif 

#undef  SNAIL_USE_EMAIL
#undef  SNAIL_USE_LHB_ADV 

#define STR_CEIL(v) QString::number(v,'f',2) 

class QListWidget;
class QTableWidget;

QStringList splitBuffer(const QString& buffer);

#define StockFocus_index 0
#define StockFocus_code  1
#define StockFocus_name  2
#define StockFocus_value 3
#define StockFocus_valueDelta 4
#define StockFocus_valueDeltaPrecent 5
#define StockFocus_trend 6
#define StockFocus_price 7

#define StockAHWidget_name    0
#define StockAHWidget_hkcode  1
#define StockAHWidget_hkPrice 2
#define StockAHWidget_hkRise  3
#define StockAHWidget_code    4
#define StockAHWidget_price   5
#define StockAHWidget_rise    6
#define StockAHWidget_r       7
#define StockAHWidget_premium 8

#define STOCK_DOCTOR_score   0
#define STOCK_DOCTOR_tip     1
#define STOCK_DOCTOR_tipDesc 2
#define STOCK_DOCTOR_trend   3
#define STOCK_DOCTOR_trendDesc  4
#define STOCK_DOCTOR_quality 5
#define STOCK_DOCTOR_qualityDesc 6
#define STOCK_DOCTOR_shortTrend 7
#define STOCK_DOCTOR_longTrend 8

#define DELETE_PTR(ptr)\
	if(ptr)\
	    delete ptr;\
	ptr = nullptr;

template<class T>
inline QVector<T> mapToVec(const QMap<QString,T>& data)
{
    QVector<T> output;
	QMap<QString,T>::iterator itr = data.begin();
	while(itr != data.end())
	{
		output.append(itr.value());
	    itr ++;
	}
	return output;
}

//[{name:value,...}]
QVector<QMap<QString,QString> > parserJson(const QString& content);

inline bool isSameValue(double a,double b,double delta = 0.0001){return abs(a - b) < delta;}

bool isHoliday(const QDate& date);
QDate getLastWorkDay(const QDate& date);
QDate getNextWorkDay(const QDate& date);

class OrderTableWidgetItem : public QTableWidgetItem 
{
public:
    OrderTableWidgetItem(const QString& text):
        QTableWidgetItem(text)
    {
    }
public:
    inline bool operator <(const QTableWidgetItem &other) const
    {
        return text().toFloat() < other.text().toFloat();
    }
};

struct ResultType
{
	static const int TYPE_BOOL = 0;
	static const int TYPE_INT = 1;
	static const int TYPE_DOUBLE = 2;
	static const int TYPE_FLOAT = 3;
	static const int TYPE_STRING = 4;

	ResultType():type(TYPE_BOOL),bool_(false){}
	ResultType(const ResultType& var)
	{
        type = var.type;
        bool_ = var.bool_;
        int_ = var.int_;
        double_ = var.double_;
        float_ = var.float_;
        string_ = var.string_;    
    }

	bool isDoubleType(){return type == TYPE_DOUBLE;}
	bool isBoolType(){ return type == TYPE_BOOL; }

    int type;
	bool bool_;
	int int_;
	double double_;
	float float_;
	QString string_;
};

class ItrLooper
{
public:
	virtual ~ItrLooper(){}
	virtual int loop(const QString& tag,int i,int count) = 0;
};

template<class T>
inline QString toString(T value)
{
	return QString("%1").arg(value);
}

void loopDir(const QString& dir,ItrLooper* looper);

struct Line
{
	// y = ax+b
    float a,b;
	float d(float x,float y);
	bool isOK(float x,float y,float delta);
	static Line build(float x1,float y1,float x2,float y2);
};

#define DECLARE_SINGLETON(Class) \
public:\
	Class();\
	~Class();\
public: \
    static Class* instance() \
    { \
        static QScopedPointer<Class> inst; \
        if (Q_UNLIKELY(!inst)) { \
            if (!inst) inst.reset(new Class); \
        } \
        return inst.data(); \
    }


inline double isEqual(double a,double b,double delta = 0.0001f)
{
    return fabs(a-b) < delta; 
}

inline double getStockBody(double open,double top,double low,double close)
{
    return std::max<double>(open,close) - std::min<double>(close,open);
}

inline double getStockFooter(double open,double top,double low,double close)
{
    return std::min<double>(open,close) - low;
}

inline double isIncreaseTop(double prev,double value)
{
    return (prev+0.015)*1.1 >= value && value >= (prev-0.015)*1.1;
}

inline double isDecreaseLow(double prev,double value)
{
    return (prev+0.015)*0.9 >= value && value >= (prev-0.015)*0.9;
}

void test();

float getDistance(const QLineF& line,float x,float y);
double maxDownValue(const QVector<double>& input);

QStringList pickupStrings(const QString& data,const QString& express);

bool isStockCode(const QString& market,const QString& code);

QString getSoftwareConfigInformation();

inline QColor IntToQColor(const int& intColor)
{
	//将Color 从int 转换成 QColor  
	int red = intColor & 255;
	int green = intColor >> 8 & 255;
	int blue = intColor >> 16 & 255;
	return QColor(red, green, blue);
}

QString buildStockCodeByDigitStockCode(const QString& code);
QString unicodeToUtf8(const QString& unicode);

//QMap<QString,QMap<QString,QString>> loadPerDataFromJsonContent(const QString& json);

QStringList getStocksByInput(const QString& input);
QStringList trimStringList(const QStringList& list);

QWidget* loadWidgetFromUI(const QString& uifile);

#endif per