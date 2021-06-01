#include "StockShapeScanner.h"

#define MAX_OFFSET 24

StockHengPanTuPoScanner::StockHengPanTuPoScanner()
{
}

StockHengPanTuPoScanner::~StockHengPanTuPoScanner()
{
}

int StockHengPanTuPoScanner::check(const QString& stock,const QVector<Stock>& list,QStringList& desc)
{
    if(list.size() <= 72)
		return -1;

	int count = list.size();
	int cursor = count-1;
	Stock current = list[cursor];
	if(!(current.current > current.open && current.increase >= 6.0f))
		return -1;

	auto input = list.mid(list.size()-MAX_OFFSET-1,MAX_OFFSET);

	int toppos,lowpos;
	float low,high;
	low = 9999;
	high = 0;
	stock_findStockPolar(input,toppos,lowpos,high,low);

	if(low*1.12f > high)
		return -1;

	if(current.current < high || current.current*1.02f > high)
		return -1;

	desc.append("œ‰ÃÂ’µ¥Õª∆∆ø¥’«");
	return 0;
}
