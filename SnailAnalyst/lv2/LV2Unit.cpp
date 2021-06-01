#include "common/common.h"
#include "LV2Unit.h"

QMap<QString,QString> StockLV2::toMap()
{
    QMap<QString,QString> output;
	output.insert("code",code);
	output.insert("date",date);
    output.insert("price",toString(price));
	output.insert("capital",toString(captical/10000));
	output.insert("volume",toString(volume/10000));
    output.insert("buyCount",toString(buyCount()));
	output.insert("saleCount",toString(saleCount()));

	output.insert("tinyBuy",toString(tinyBuy/10000));
	output.insert("tinyBuyCount",toString(tinyBuyCount));
	output.insert("tinySale",toString(tinySale/10000));
	output.insert("tinySaleCount",toString(tinySaleCount));

	output.insert("smallBuy",toString(smallBuy/10000));
	output.insert("smallBuyCount",toString(smallBuyCount));
	output.insert("smallSale",toString(smallSale/10000));
	output.insert("smallSaleCount",toString(smallSaleCount));

	output.insert("midBuy",toString(normalBuy/10000));
	output.insert("midBuyCount",toString(normalBuyCount));
	output.insert("midSale",toString(normalSale/10000));
	output.insert("midSaleCount",toString(normalSaleCount));

	output.insert("bigBuy",toString(bigBuy/10000));
	output.insert("bigBuyCount",toString(bigBuyCount));
	output.insert("bigSale",toString(bigSale/10000));
	output.insert("bigSaleCount",toString(bigSaleCount));

	output.insert("overBuy",toString(overBuy/10000));
	output.insert("overBuyCount",toString(overBuyCount));
	output.insert("overSale",toString(overSale/10000));
	output.insert("overSaleCount",toString(overSaleCount));
	return output;
}

bool StockLV2::netOut()
{
	if(buyCount()/double(saleCount())>1.5||
		tinyBuy/(double)captical>0.7||
		tinyBuy/double(tinySale)>1.8)
		return true;
	return false;
}

bool StockLV2::netIn()
{
	return saleCount()/(double)buyCount()>1.2 &&
		tinySale/(double)tinyBuy>1.2 &&
		tinySale/(double)captical>0.3;

	if(buyCount() < 100)
		return false;

	return true;
}