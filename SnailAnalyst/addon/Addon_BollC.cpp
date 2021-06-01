#include "Addon_Boll_C.h"
#include <qDebug>

bool isFit(const QVector<Stock>& list,const QVector<DataWrapper5<bool,float,float,float,float>>& bolls,float input)
{
	if(list.size() != bolls.size())
		return false;

	int last = bolls.size() - 1;
	auto stock = list.at(last);

	auto boll = bolls.at(last);

	if (!boll._1)
		return false;

	float mid = boll._2;
	float low = boll._2 - boll._3;
	if (stock.top >= mid)
		return false;

	float prevValue = std::numeric_limits<float>::min();
	QString prevDate = stock.date;

	for(int i = last; i >= 0; i--)
	{
		Stock current = list[i];
		auto boll = bolls.at(i);
		if (!boll._1)
			return false;

		low = boll._2 - boll._3;
		mid = boll._2;

		if (current.current < low)
			prevValue = std::max<float>(prevValue,current.current);

		if (current.top >= mid)
		{
			float increase = GET_INCREASE(stock.current,prevValue);
			if (stock.current > prevValue)
				return false;

			if (increase <= -input)
			{
				return true;
			}
			return false;
		}
	}

	return false;
}