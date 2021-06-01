#ifndef STOCK_H
#define STOCK_H
#include <QString>
#include <QVector>
#include <QMap>
#include <QtCharts/QCandlestickSeries>
#include "SnailAddonHost.h"

using namespace QtCharts;

//http://qt.gtimg.cn/r=0.8409869808238q=s_sh000001,s_sz399001,s_sz399006

QVector<Stock> stock_toLogStock(const QVector<Stock>& stocks);
int stock_getLowPosition(const QVector<Stock>& stocks,int from,int to,float& price,bool all = true);
int stock_getHighPosition(const QVector<Stock>& stocks,int from,int to,float& price,bool all = true);
bool stock_getMinMax(const QVector<Stock>& stocks, int from, int to,double& minv,double& maxv,bool all = true,bool* seq = nullptr);

double stock_getAvg(const QVector<Stock>& stocks,int from,int to,bool vol = true);
void stock_findStockPolar(const QVector<Stock>& list,int& toppos,int& lowpos,float& top,float& low);
QVector<Stock> getStockSpanK(const QVector<Stock>& stocks,int count = 5);

void unionStocks(QVector<Stock>& stock1,QVector<Stock>& stock2);

//QVector<Stock>::iterator stock_findSpanHigh(const QVector<Stock>::iterator begin,const QVector<Stock>::iterator end,bool withHigh);
//QVector<Stock>::iterator stock_findSpanLow(const QVector<Stock>::iterator begin,const QVector<Stock>::iterator end,bool withLow);

void bindStockDataToCandlestickSeries(QCandlestickSeries* series,const QVector<Stock>& stocks);

QMap<QString,Stock> vecToMapStock(const QVector<Stock>& stocks);

#endif 