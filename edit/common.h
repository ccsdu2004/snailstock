#ifndef COMMON_H
#define COMMON_H
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QMap>
#include <QScopedPointer>
#include <QVector>
#include <QDate>

#pragma execution_character_set("utf-8")

//#define SNAIL_VERSION_FREE
#ifdef SNAIL_VERSION_FREE
#define SNAIL_STOCK_COUNT 180
#endif 

#ifdef SNAIL_VERSION_480
#ifndef SNAIL_STOCK_COUNT
#define SNAIL_STOCK_COUNT 480
#endif 
#endif 

#ifdef SNAIL_VERSION_960
#ifndef SNAIL_STOCK_COUNT
#define SNAIL_STOCK_COUNT 960
#endif 
#endif 

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
#define GET_INCREASE(current,prev)\
	100.0*(current-prev)/prev

class QListWidget;
class QTableWidget;

QString readAllBufferFromFile(const QString& file);
void saveToFile(const QString& file,const QString& content);

QStringList splitBuffer(const QString& buffer);

int senderMail(const QString& title,const QString& text,bool all = true);
int senderMail_zaoPan(const QString& title,const QString& text);

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

#define nonTradeStockFile "data/none"

//[{name:value,...}]
QVector<QMap<QString,QString> > parserJson(const QString& content);

inline bool isSameValue(double a,double b,double delta = 0.0001){return abs(a - b) < delta;}

bool isHoliday(const QDate& date);
QDate getLastWorkDay(const QDate& date);
QDate getNextWorkDay(const QDate& date);

class ItrLooper
{
public:
	virtual ~ItrLooper(){}
	virtual int loop(const QString& tag,int i,int count) = 0;
};

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

inline double getStockHead(double open,double top,double low,double close)
{
    return top - std::max(open,close);
}

inline double getStockBody(double open,double top,double low,double close)
{
    return std::max(open,close) - std::min(close,open);
}

inline double getStockFooter(double open,double top,double low,double close)
{
    return std::min(open,close) - low;
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

bool exportToFile(const QString& file,QListWidget* list);
bool exportToCSV(const QString& file,QTableWidget* table);

QStringList pickupStrings(const QString& data,const QString& express);

QFileInfoList GetFileList(const QString& path);
QFileInfoList GetDirList(const QString& path);

QString getSoftwareConfigInformation();

#endif 