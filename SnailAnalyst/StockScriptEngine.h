#ifndef STOCK_SCRIPT_PARSER_H
#define STOCK_SCRIPT_PARSER_H
#include <QVector>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QMap>
#include "config.h"
#include "Stock.h"
#include "DataProvider.h"
#include "expr/StatementObject.h"
#include "tradewriter/StockTradeWriter.h"
#include "interface/StockRealTimeInfo.h"
#include "StockThreadTarget.h"
#include "ConfigurationParser.h"
#include "common/abstractFileReader.h"
#include "common/Sigslot.h"
#include "expr/VarMeta.h"
#include "expr/VarManager.h"

class Indicator;

class StockScriptEngine 
{
public:
	StockScriptEngine();
	~StockScriptEngine();
public:
	bool prepare(const QString& script,QString& err);

	void checkRefStock(const QString& stock);
	void checkStock(const QString& stock);
	void checkPairTradeStock(const QString& stock);

	QStringList keywords();
#ifdef SUPPORT_BASIC
	VarMeta* getVarMetaByVar(const QString& var);
	StoreMeta* getStoreMetaByVar(const QString& var);
#endif
	QStringList getStockTicketFileList(const QString& stock);

	QStringList getArguments()const{ return arguments; }
	void appendArgument(const QString& argument);
private:
	bool afterPrepare(QString& error);
	void clearEvalutor();
	int  parseLine(const QString& statement);
	void reloadStockData(const QString& stock);

	bool isRefMode();

	bool isBuyPoint(int pos,QVector<float>& arguments);
	bool isRefBuyPoint(int pos,QVector<float>& arguments);
	bool isSalePoint(int pos);
	bool isRefSalePoint(int pos);
	void reclacStoreMapper(int pos);
	bool containInRefSet(const QString& date);

#ifdef SUPPORT_BASIC
	QMap<QString,VarMeta*> varMapper;  
	QMap<QString,StoreMeta*> storeMapper;
#endif
	
	QVector<StatementObject*> buyEvalutor;
	QVector<StatementObject*> buyRefEvalutor;
	QVector<StatementObject*> saleEvalutor;
	QVector<StatementObject*> saleRefEvalutor;

	QStringList pairTradeStocks;
	QVector<QPair<QString,QString>> refBSSet;

	QStringList arguments;
};

class StockScriptList: public QObject,public abstractFileReader,public SlotHolder
{
	Q_OBJECT
public:
	static StockScriptList& GetInstance();
	
	void setTargetObject(QObject* target);
	bool loadAndRunScripts(const QString& buffer,const QString& scriptMode = "script");

	StockScriptEngine* getScriptEngine(){return engine;}
	void callbackScript(const QString& desc,const QString& script,QString& error);
	QVector<TradeReport> getStockTradeItems()const{return stockTradeItems;}
	void clearStockTradeItems(){ stockTradeItems.clear();}
	QString getScriptMode()const{return scriptMode;}
	QString getBaseScript()const{ return baseScriptContent; }

#ifdef SUPPORT_BASIC
	VarMetaManager* getVarMetaManager(){ return varMetaManager; }
#endif
signals:
    void onUpdate(int,int,int,const QString& code);
	void onFinish(const QString& error);
public slots:
	void execute();
private:
	void receivedFinishSignal(QString error);
	void checkRefOneStock(const QString& stockcode);
	void checkOneStock(const QString& stockFile);
private:
	StockScriptList();
	~StockScriptList();

    void doLine(const QString& tag,const QString& value);
    void doAfter();

	QString path;
	
	QVector<DataWrapper2<QString,QString>> scripts;
	int scriptIndex;
	
	bool showInOne;
	QString scriptMode;

    QVector<TradeReport> stockTradeItems;

	StockScriptEngine* engine;
	StockThreadTarget* thread;
	QObject* target;
	QString currentScriptName;
	QString currentScriptContent;
	QString baseScriptContent;
#ifdef SUPPORT_BASIC
	VarMetaManager* varMetaManager;
#endif
};

bool StockTargetCallbackImpl(const QString& desc,const QString& script,QString& error);

#ifdef SUPPORT_BASIC
#define GLOBAL_GET_VAR_MANAGER StockScriptList::GetInstance().getVarMetaManager()
#endif

#endif 