#ifndef SNAIL_HOST_H
#define SNAIL_HOST_H
#include <tuple>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVector>
#include "common/DataWrapper.h"
#include "addon/SnailAddon.h"

#define SNAIL_ADDON_VERSION "1.0.0"
#define SNAIL_ADDON_VERSION_INT 1000

#if defined(WIN32) || defined(WIN64)
    #ifdef SNAIL_EXPORT
        #define SNAIL_API_EXPORT __declspec(dllexport)
    #else
        #define SNAIP_API_IMPORT __declspec(dllimport)
    #endif
#else
    #error 系统不支持
#endif

class StockImpl;

struct StockIndicator
{
	virtual void clear() = 0;
	virtual DataWrapper5<bool,float,float,float,float> update(const Stock& stock) = 0;

	virtual float getTrueRange(const Stock& prev,const Stock& current);
};

class StockBSChecker
{
public:
	StockBSChecker(){}
	virtual ~StockBSChecker(){}
public:
	virtual void clear() = 0;
	virtual bool update(const Stock& stock) = 0;
};

/*struct SnailAddon
{
	virtual ~SnailAddon(){}
	virtual void destroy() = 0;
    virtual const QString maker()const = 0;
	virtual const QString version()const = 0;
	virtual const QString description()const = 0;
	virtual const QString tips()const = 0;
    virtual void onRecevied(const QString& code,const QVector<Stock>& list) = 0;	
};

struct SnailAddonHost
{
	virtual const QString version()const = 0;
	virtual QMap<QString,Stock> getStockList()const = 0;

	static const int TAG_Indicator_Macd = 0;
	static const int TAG_Indicator_Ma   = 1;
	static const int TAG_Indicator_Boll = 2;
	static const int TAG_Indicator_Kdj  = 3;
	static const int TAG_Indicator_VolMa = 4;
	virtual StockIndicator* createIndicator(int id,int a,int b,int c) = 0;

	static const int TAG_XuRiDongSheng = 0;
	static const int TAG_HongSanBing = 1;
	static const int TAG_YangTunYin = 2;
	static const int TAG_DuoFangPao = 3;
	static const int TAG_QingChenZhiXing = 4;
	static const int TAG_ChuanCi = 5;
	static const int TAG_PingTouPingDi = 6;
	static const int TAG_ShangShengSanFa = 7;
	static const int TAG_SanYangKaiTai = 8;
	static const int TAG_ChuiZiZhiDie = 9;
	static const int TAG_DaoChuiTou = 10;
    static const int TAG_KanZhangMuZi = 11;

	static const int TAG_YinTunYang = 20;
	static const int TAG_QinPenDaYu = 21;
	static const int TAG_WuYunGaiDing = 22;
	static const int TAG_ZhangShiJinTou = 23;
	static const int TAG_KongFangPao = 24;
	static const int TAG_HuangHunZhiXing = 25;
	static const int TAG_XiaJiangSanFa = 26;
	static const int TAG_HeiSanBing = 27;
	static const int TAG_YangYunShiZi = 28;
	static const int TAG_ShuangFeiWuYa = 29;
	static const int TAG_DiaoJing = 30;
	static const int TAG_LiuXing = 31;
	static const int TAG_KanDieMuZi = 32;	
	virtual bool isThisK(int id,const QVector<Stock>& list) = 0;

	//static const int TAG_Algo_Exe_K_Daily = 0;
	virtual void append(const QString& code,const QString& name,const std::tuple<QString,float>& buy,const std::tuple<QString,float>& sale,int days,const QString& remark) = 0;
};*/

//QMap<QString,Stock> loadStocksFromPerBuffer(const QString& json);

#endif 