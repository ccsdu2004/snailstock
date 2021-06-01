#include "StockBidData.h"
#include "common/common.h"

StockBidData StockBidData::parse(const QString& buffer)
{
	StockBidData data;
	auto items = buffer.split(",");
	if (items.size() == 16)
	{
		data.code = buildStockCodeByDigitStockCode(items[0]);
		data.increase = items[1].toDouble();
		data.privClose = items[2].toDouble();
		data.vol = items[3].toInt();
		data.capital = items[4].toInt();
		data.turnover = items[5].toDouble();

		if (isEqual(data.increase,-100.0))
		{
			data.increase = 0.0;
			data.price = data.privClose;
		}

		int buy = 0;
		int sale = 0;

		for (int i = 0;i < 5;i++)
		{
			data.buy[i] = items[i + 6].toInt();
			buy += data.buy[i];
		}

		for (int i = 0;i < 5;i++)
		{
			data.sale[i] = items[i + 11].toInt();
			sale += data.sale[i];
		}

		if (buy == sale && buy == 0)
			data.ok = false;
		else
			data.ok = true;
	}
	else if (items.size() == 17)
	{
		data.code = items[0];
		if(data.code.size() == 6)
			data.code = buildStockCodeByDigitStockCode(data.code);

		data.increase = items[1].toDouble();
		data.privClose = items[2].toDouble();
		data.vol = items[3].toInt();
		data.capital = items[4].toInt();
		data.turnover = items[5].toDouble();

		if (isEqual(data.increase,-100.0))
		{
			data.increase = 0.0;
			data.price = data.privClose;
		}

		int buy = 0;
		int sale = 0;

		for (int i = 0;i < 5;i++)
		{
			data.buy[i] = items[i + 6].toInt();
			buy += data.buy[i];
		}

		for (int i = 0;i < 5;i++)
		{
			data.sale[i] = items[i + 11].toInt();
			sale += data.sale[i];
		}

		if (buy == sale && buy == 0)
			data.ok = false;
		else
			data.ok = true;
	}

	return data;
}