#ifndef SNAIL_MATH_H
#define SNAIL_MATH_H
#include <QVector>
#include <qpair.h>
#include <qstringlist.h>
#include "Stock.h"

namespace std
{
    
template<class T>
T clamp(T value,T low,T high)
{
	return std::min<T>(std::max<T>(value,low),high);
}

}

//y=ax+b
void lineFitting(const QVector<DataWrapper2<int,float> >& list,float& a,float& b);
void lineFitting(QVector<DataWrapper2<int,float> >& list);

double gsl_mean(const double data[],const size_t size);
double gsl_absdev(const double data[],const size_t n);
double gsl_stdev(const double data[],const size_t n);
double gsl_covariance(const double data1[],const double data2[],const size_t n);
double gsl_correlation(const double data1[],const double data2[],const size_t n);
double gsl_tts(const double data[],const size_t n,const double mean);
double gsl_variance_with_fixed_mean(const double data[],const size_t n,const double mean);
double gsl_sd_with_fixed_mean(const double data[],const size_t n,const double mean);
double gsl_variance(const double data[],const size_t n);
double gsl_sd(const double data[],const size_t n);

double stock_mean(QVector<Stock>& stocks,int flag);

QVector<QPair<int,int>> getCombinationVector(int n);

double pearson(double* l,double* r,int size,double* delta,double* slope);
double pearsonStock(const QVector<Stock>& s1,const QVector<Stock>& s2);

float getStockActivteValue(const QVector<Stock>& stocks);

void unionStocks(QVector<Stock>& stock1,QVector<Stock>& stock2);
QString getPairTradeStockData(const QString& market,const QString& path,const QString& format,const QString& code1,const QString& code2,QVector<Stock>& stock1,QVector<Stock>& stock2);

QVector<QStringList> generateStringListAndReplaceKeyByValue(const QVector<QStringList>& list,const QStringList& keys,const QVector<QStringList>& values);

#endif