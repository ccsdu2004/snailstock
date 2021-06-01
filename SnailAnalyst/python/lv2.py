
#return 0-8
def lv2DataItem(code,date,price,capital,volume,buyCount,saleCount,tinyBuy,tinyBuyCount,tinySale,tinySaleCount,smallBuy,smallBuyCount,smallSale,smallSaleCount,midBuy,midBuyCount,midSale,midSaleCount,bigBuy,bigBuyCount,bigSale,bigSaleCount,overBuy,overBuyCount,overSale,overSaleCount,increase):
    #...

    color = 0

    if buyCount > saleCount*1.5 or tinyBuy > capital*0.7 or tinyBuy > tinySale*1.8:
        color = 1
    elif increase < -1.2 and saleCount > buyCount*1.2 and tinySale > tinyBuy*1.2 and tinySale > capital*0.3:
        color = 3
    elif saleCount > buyCount*1.2 and tinySale > tinyBuy*1.2 and tinySale > capital*0.3:
        color = 2
    return color

