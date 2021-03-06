#include "StockShapeScanner.h"

StockGaoWeiHengPanScanner::StockGaoWeiHengPanScanner()
{
}

StockGaoWeiHengPanScanner::~StockGaoWeiHengPanScanner()
{
}

int StockGaoWeiHengPanScanner::check(const QString& stock,const QVector<Stock>& list,QStringList& desc)
{
    if(list.size() <= 60)
		return -1;

	int count = list.size();

	float high = 0.0f;
	int pos = stock_getHighPosition(list,count-7,count-1,high,false);
    
	float low = 0.0f;
	pos = stock_getLowPosition(list,count-7,count-1,low,false);

	high = (high-low)/low*100.0f;
	if(high >= 2.11f)
        return -1;

	float minlow = 0.0f;
	pos = stock_getLowPosition(list,count-60,count-1,minlow,false);

	float percent = high - minlow;
	percent /= low;
	percent *= 100.0f;
	if(percent < 20.0f)
		return -1;

	desc.append("高位横盘看跌");
	return 1;
}

StockDiWeiHengPanScanner::StockDiWeiHengPanScanner()
{
}

StockDiWeiHengPanScanner::~StockDiWeiHengPanScanner()
{
}

int StockDiWeiHengPanScanner::check(const QString& stock,const QVector<Stock>& list,QStringList& desc)
{
    if(list.size() <= 60)
		return -1;

	int count = list.size();

	float high = 0.0f;
	int pos = stock_getHighPosition(list,count-7,count-1,high,false);
    
	float low = 0.0f;
	pos = stock_getLowPosition(list,count-7,count-1,low,false);

	high = (high-low)/low*100.0f;
	if(high >= 2.11f)
        return -1;

	float minlow = 0.0f;
	pos = stock_getLowPosition(list,count-60,count-20,minlow,false);

	if(low > minlow*1.036f)
		return -1;

	desc.append("低位横盘看涨");
	return 0;
}