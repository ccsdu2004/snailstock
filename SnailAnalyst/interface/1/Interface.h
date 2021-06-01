#ifndef INTERFACE_H
#define INTERFACE_H
#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <cmath>
#include "Stock.h"
#include "common/common.h"



class StringListInterface
{
public:
	StringListInterface(){}
	virtual ~StringListInterface(){}
public:
	virtual void onError(const QString& error);
    virtual QStringList buildLinks(const QStringList& list) = 0;
	virtual QVector<QStringList> parseContent(const QString& content) = 0;
};

class StringMapInterface
{
public:
	StringMapInterface(){}
	virtual ~StringMapInterface(){}
public:
	virtual void onError(const QString& err);
    virtual QStringList buildLinks(const QStringList& list) = 0;
	virtual QVector<QMap<QString,QString> > parseContent(const QString& content,const QString& url = QString()) = 0;
};

struct Data
{
	QString date;
	QString code;
	QString name;
    QMap<QString,QString> plus;  
	QVariant var;
};

Q_DECLARE_METATYPE(Data)

class NoticeInterface
{
public:
	NoticeInterface(){}
	virtual ~NoticeInterface(){}
public:
	virtual void onError(const QString& err);
    virtual QStringList buildLinks(const QStringList& list) = 0;
	virtual QVector<Data> parseContent(const QString& content,const QString& url = QString()) = 0;
};

struct LHBUnit
{
    QString department;
	QString index;
	QString count;
	QString success;
	QString buy;
	QString tradeBuyPercent;
	QString sale;
	QString tradeSalePercent;
	QString netValue;
	// buy = 1,sale = 2,net = 3
	int flag;

	LHBUnit()
	{
		flag = 0;
	}
};

Q_DECLARE_METATYPE(LHBUnit)

class LHBInterface
{
public:
	LHBInterface(){}
	virtual ~LHBInterface(){}
public:
	virtual QStringList buildLinks(const QStringList& list) = 0;
	virtual QVector<LHBUnit> parseContent(const QString& content,const QString& code = QString()) = 0;
};

QStringList pickupStrings(const QString& data,const QString& regex);

class EastmoneyInterface : public NoticeInterface
{
public:
	EastmoneyInterface(){}
	~EastmoneyInterface(){}
public:
	QStringList buildLinks(const QStringList& list);
	QVector<Data> parseContent(const QString& content,const QString& url = QString());
private:
	QString rawParse(const QString& content);
	QString pickupString(const QString& data,const QString& regex);
};

class EastmoneyRecoveryTransactionInterface: public NoticeInterface
{
public:
	EastmoneyRecoveryTransactionInterface(){}
	virtual ~EastmoneyRecoveryTransactionInterface(){}
public:
    QStringList buildLinks(const QStringList& list);
	QVector<Data> parseContent(const QString& content,const QString& url = QString());
private:
	QString rawParse(const QString& content);
	QStringList pickupString(const QString& data,const QString& regex);
};

/*class EastmoneyLHBListInterface : public StringMapInterface
{
public:
	EastmoneyLHBListInterface(){}
	~EastmoneyLHBListInterface(){}
public:
	QStringList buildLinks(const QStringList& list);
	QVector<QMap<QString,QString> > parseContent(const QString& content,const QString& url = QString());
private:
};*/

class QQLHBDataInterface : public LHBInterface
{
public:
	QQLHBDataInterface();
	~QQLHBDataInterface();
public:
	QStringList buildLinks(const QStringList& list);
	QVector<LHBUnit> parseContent(const QString& content,const QString& url = QString());
};

class EastmoneyLHBDataInterface : public LHBInterface
{
public:
	EastmoneyLHBDataInterface();
	~EastmoneyLHBDataInterface();
public:
	QStringList buildLinks(const QStringList& list);
	QVector<LHBUnit> parseContent(const QString& content,const QString& url = QString());
private:
	QVector<LHBUnit> parseContentUnitImpl(const QString& content,bool buy);
	LHBUnit parseUnit(const QString& content,bool& ok);
	LHBUnit parseNetValue(const QString& content,bool& ok);
};
/*
class Stock163Interface : public StockInterface
{
public:
    QStringList buildLinks(const QStringList& list);
	QVector<Stock> parseContent(const QString& content,const QString& code = QString());
	void setParam(int flag,const QString& value);
private:
	QString targetDir;
};*/

/*
class SinaLHBStockListInterface : public StockInterface
{
public:
	SinaLHBStockListInterface();
	~SinaLHBStockListInterface();
public:
    QStringList buildLinks(const QStringList& list);
	QVector<Stock> parseContent(const QString& content,const QString& code = QString());
private:
	Stock doTr(const QString& content);

	QString desc;
};

class QQDcotorStringListInterface : public StringListInterface
{
public:
    QStringList buildLinks(const QStringList& list);
	QVector<QStringList> parseContent(const QString& content);
private:
	QStringList parseSeg(const QString& content,const QString& regex,QString& out);
};

class SinaAchievementInterface : public StringMapInterface
{
public:
	SinaAchievementInterface();
	~SinaAchievementInterface();
public:
	QStringList buildLinks(const QStringList& list);
	QVector<QMap<QString,QString> > parseContent(const QString& content,const QString& url = QString());
private:
};  

struct StockBonus
{
    QString nd;
	QString sg;
	QString zz;
	QString fh;
	QString djr;
	QString cqr;
};
/*
struct StockFinance
{
    QString flag;
	QString eps;
	QString income;
	QString mainIncome;
	QString incomeNet;
	QString otherNet;
	QString investIncome;
	QString netProfit;
	QString netProfitRate;

	QString bvps;
	QString roe;
	QString nonliquidAsset;
	QString liquidAsset;
	QString asset;
	QString cashLiability;
	QString fixedLiability;
    QString liability;
	QString holderQquity;

    QString cfps;
	QString businessCfps;
	QString investCfps;
	QString financeCfps;
};

struct StockHolder
{
    QString symbol;
	QString id;
	QString company_code;
	QString rank;
	QString shareholder_name;
	QString shareholder_type;
	QString shareholder_num;
	QString shareholder_percent;
    QString report_date;
	QString shareholder_change;

	static QString toHtmlTable(const QVector<StockHolder>& list);
};

struct StockFundHolder
{
    QString jjdm;
	QString zqdm;
	QString zqmc;
	QString zqsclx;
    QString sz;
	QString ccsz;
	QString jzb;
	QString bcbgrq;
	QString bcccgs;
	QString scbgrq;
	QString scccgs;
	QString ccbd;
	QString updatetime;
	QString jjmc;

	static QString toHtmlTable(const QVector<StockFundHolder>& list);
};

struct StockIncome
{
    QString sector;
	QString income;
	QString unit;
};

class StockInfo
{
public:
    StockInfo();
	~StockInfo();
public:
	QString buildLink(const QString& code);
	bool parseContent(const QString& code,const QString& content,const QString& dir = QString());

	QString company()const;
	QString listDate()const;
	QString listPrice()const;
	QString listVolume()const;
	QString area()const;
	QString business()const;
	QString plate()const;
	QStringList concept()const;
	QVector<StockIncome> income()const;
	QVector<StockBonus> bonus()const;
    QVector<QPair<QString,QString> > finance()const;
    QVector<StockHolder> holder()const;
	QVector<StockHolder> fund()const;

	QString totalStocks()const;
	QString outstandsStocks()const;
	QString holderNumber()const;
	QString pch()const;
private:
	bool parseBonus(const QString& content);
	void doyysr(const QString& content);
	void docwbb(const QString& contnt);
	void doHolder(const QString& content);
private:
	QString stockCompany;
	QString stockListDate;
	QString stockListPrice;
	QString stockListVolume;
	QString stockArea;
	QString stockBusiness;
	QString stockPlate;
	QStringList stockConcept;
	QVector<StockIncome> sotckIncome;
	QVector<StockBonus> stockBonus;
    QVector<QPair<QString,QString> > stockFinance;
    QVector<StockHolder> stockHolder;
	QVector<StockHolder> stockFund;

	QString stockTotalStocks;
	QString stockOutstandsStocks;
	QString stockHolderNumber;
	QString stockHolderNumberAvg;
	QString stockHolderNumberPrev;
};

class StockNewsInterface : public StringMapInterface
{
public:
	StockNewsInterface();
	~StockNewsInterface();
public:
	QStringList buildLinks(const QStringList& list);
	QVector<QMap<QString,QString> > parseContent(const QString& content,const QString& url = QString());
private:
};*/

#endif