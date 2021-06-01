#include "SnailContext.h"
#include "config.h"

QMutex SnailContext::mutex;
QMap<QString,SnailContext*> SnailContext::contexts;

SnailContext::SnailContext(const QString& tag):
name(tag)
{
	configFactory = new ConfigFactory();
}

SnailContext* SnailContext::open(const QString& tag)
{
	/*if(contexts.find(tag) == contexts.end())
	{
		mutex.lock();
		contexts.insert(tag,new SnailContext(tag));
		mutex.unlock();
	}
	return contexts[tag];*/
	static SnailContext context(tag);
	return &context;
}

void SnailContext::shutdown()
{
	delete configFactory;
    mutex.lock();
	contexts.remove(name);
	mutex.unlock();
}

QStringList SnailContext::getIntelligentPrompts()
{
	QStringList items;
	items << "config.candlestick.theme";
	items << "config.candlestick.theme=ChartThemeLight";
	items << "config.candlestick.theme=ChartThemeBlueCerulean";
	items << "config.candlestick.theme=ChartThemeDark";
	items << "config.candlestick.theme=ChartThemeBrownSand";
	items << "config.candlestick.theme=ChartThemeBlueNcs";
	items << "config.candlestick.theme=ChartThemeHighContrast";
	items << "config.candlestick.theme=ChartThemeBlueIcy";
	items << "config.candlestick.theme=ChartThemeQt";

	items << "config.source.market";
	items << "config.source.market=中国大陆";
	items << "config.source.market=香港";
	items << "config.source.market=美国";
	items << "config.source.market=混合";

	items << "config.source.k.dir";
	items << "config.source.k.format";
	items << "conifg.source.k.format=default";
	items << "config.source.l.format=auto";
	items << "config.source.l.format=tushare";

	items << "config.source.ref.code";
	items << "config.trade.max.hold.day";
	items << "config.trade.min.exclude";
	items << "config.trade.max.profit";
	items << "config.trade.max.lost";
	items << "config.trade.max.count";
	items << "config.trade.sale.point";
	items << "conifg.trade.sale.point=open";
	items << "config.trade.sale.point=close";
	items << "config.trade.max.allow.increase";
	items << "config.trade.max.allow.decrease";
	items << "config.trade.buy.point";
	items << "config.trade.buy.point=open";
	items << "config.trade.buy.point=auto";
	items << "config.trade.buy.order=voldesc";
	items << "config.trade.buy.order=volasc";
	items << "config.trade.buy.order=increasedesc";
	items << "config.trade.buy.order=increaseasc";
	items << "config.trade.sale.rapid";
	items << "config.trade.sale.rapid=no";
	items << "config.trade.sale.rapid=yes";
	items << "config.trade.启用每日累加收益";
	items << "config.trade.启用每日累加收益=yes";
	items << "config.trade.启用每日累加收益=no";
	items << "config.source.ticket.dir";
	items << "config.mode";
	items << "config.source.pool";
	items << "config.source.from";
	items << "config.source.to";
	items << "config.trade.tradecost";
	items << "config.trade.buy.movepoint";
	items << "config.trade.max.subnewstock";
	items << "source.ticket.maxtickloaded";
	items << "config.source.lv2.dir";
	items << "config.source.lv2.v1";
	items << "config.source.lv2.v2";
	items << "config.source.lv2.v3";
	items << "config.source.bid.dir";
	items << "config.source.per.dir";
	items << "config.source.ref.index";
	items << "config.source.minactive";
	items << "config.source.maxactive";

#ifdef SUPPORT_REF
	items << "config.source.ref.code";
#endif

#ifdef SUPPORT_PAIRS_TRADE
	items << "config.unit.pairtrade.mincor";
	items << "config.unit.pairtrade.maxcor";
	items << "config.unit.pairtrade.stock1";
	items << "config.unit.pairtrade.stock2";
	items << "config.unit.pairtrade.k";
	items << "config.unit.pairtrade.relative";
	items << "config.unit.pairtrade.relative=yes";
	items << "config.unit.pairtrade.relative=no";
	items << "config.unit.pairtrade.自动中轴";
	items << "config.unit.pairtrade.自动中轴=yes";
    items << "config.unit.pairtrade.自动中轴=no";
	items << "config.unit.pairtrade.中轴偏差";
	items << "config.unit.pairtrade.自动带宽";
	items << "config.unit.pairtrade.自动带宽=yes";
	items << "config.unit.pairtrade.自动带宽=no";
	items << "config.unit.pairtrade.带宽";
	items << "config.unit.pairtrade.最少股票数据量";
	items << "config.unit.pairtrade.移除极点数";
	items << "config.unit.pairtrade.长久持股";
	items << "config.unit.pairtrade.长久持股=yes";
	items << "config.unit.pairtrade.长久持股=no";
#endif

#ifdef SUPPORT_BOOSTING_ALGO
	items << "boosting.做多平均单笔收益";
	items << "boosting.做多成功率";
	items << "boosting.做多最大回撤";
#endif

#ifdef SUPPORT_GPDX
	items << "config.unit.调整买入.前低前高最小间距";
	items << "config.unit.调整买入.前低前高最大间距";
	items << "config.unit.调整买入.前高反弹最小间距";
	items << "config.unit.调整买入.前高反弹最大间距";
	items << "config.unit.调整买入.是否采用最低价作为前低=yes";
	items << "config.unit.调整买入.是否采用最高价作为前高=yes";
	items << "config.unit.调整买入.是否采用最低价作为前低=no";
	items << "config.unit.调整买入.是否采用最高价作为前高=no";
	items << "config.unit.调整买入.前高前低最小涨幅";
	items << "config.unit.调整买入.斐波那契最大误差";
	items << "config.unit.调整买入.使用扩展斐波那契数列=yes";
	items << "config.unit.调整买入.使用扩展斐波那契数列=no";

#endif 

	return items;
}