# snailstock
蜗牛股票量化分析软件开源了
使用帮助请参考 - 蜗牛股票量化分析软件使用帮助

感兴趣的朋友可以加我微信rawman2013

软件基于Qt5.10版本和msvc2017开发，同时需要先安装好Python2.7.6

开源版本移除掉了内置数据下载器、分时分析、LV2相关分析等内容

开源版本请使用通达信股票数据，使用方式请参考 https://zhuanlan.zhihu.com/p/42425762


量化工作机制 
蜗牛量化在执行过程中，针对一个股票，量化游标会从股票日线数据起始位置开始向后逐一搜索，在执行的某一节点，称当期游标位置对应日期为量化日。每个量化指令的最后一个参数即为量化计算日相对于量化日的日期偏移。该值必须小于或者等于0。例如：

buy.close,0>ma,5,0 
含义:量化日收盘价格超过当日5日均线则构成一个买点 
buy.close,-1>ma,5,-1 
含义:量化前一日满足收盘价超过当日5日均线则构成一个买点
软件量化配置项 
标准软件指令分为3个类型，分别是配置项，买点以及卖点。配置项用于软件和量化参数的配置；买点和卖点用于设定股票买入以及卖出条件。对多个买点，同时满足则为策略买入条件；对于多个卖点，满足一个则为卖出条件

注释 
策略脚本行以#标记为注释，例子如下:

#蜗牛股票量化分析软件是一款股民专用的策略分析软件 
#本软件主要用户炒股策略的量化测定
配置

config.source.k.dir=data
设置软件目录下的data文件夹为股票日线文件夹

config.source.ticket.dir=ticket
设置软件目录下的ticket文件夹为股票分时数据文件夹

config.source.lv2.dir=lv2
设置软件LEVEL2数据文件夹

config.source.ticket.maxtickloaded=8
设置软件分时数据单次最大加载文件数，默认为5

config.source.k.format=default
设置软件日线数据格式为默认模式

config.source.ref.index=sh000300
指定沪深300为参考指数

config.source.market=China
指定量化股票市场（可指定为US，HK，furtures）

config.candlestick.theme=
指定盈亏图，蜡烛图风格，当前支持八种风格，分别是ChartThemeLight，ChartThemeBlueCerulean，ChartThemeDark，ChartThemeBrownSand，ChartThemeBlueNcs，ChartThemeHighContrast，ChartThemeBlueIcy，ChartThemeQt

config.source.ref.code=sz000985
设定sz000985为参考量化股票(仅支持参考量化功能支持)

config.trade.buy.order=volasc
如果单日有多个股票同时满足买入条件，则以量化前一日成交量降序排列为买入次序，可选模式：voldesc,volasc,inreasedesc,increaseasc，含义分别为成交量降序，成交量升序，涨幅降序，涨幅升序

config.trade.sale.point=close
设定股票卖出时机为尾盘，可选时机为open(早盘)

config.trade.sale.rapid=yes
是否使用立即止盈模式，达到指定盈利则立刻卖出

config.trade.max.hold.day=3
设定最大股票持有日期为3日，昨日买入，今日卖出为2日

config.trade.min.exclude=6
设定股票卖出后禁止买入最小日期间隔

config.trade.max.profit=64.0
设定持股最大盈利卖出点数，如果持股盈利达到该点数则以config.trade.sale.point设定时间卖出股票

config.trade.max.lost=-9.0
设定单股止损点，达到则以config.trade.sale.point设定时间卖出

config.trade.max.count=3
设定单日最大可买入股票数目

config.trade.max.allow.increase=3.2
config.trade.max.allow.decrease=-3.2
如果股票满足买入条件，第二日开盘股票开盘涨跌在config.trade.max.allow.increase和config.trade.max.allow.decrease之间则为实买入股票，否则不作为买入股票计入统计

config.trade.tradecost=0.8
设定股票买入卖出交易成本，单位为每千(单项),在卖出日一次计入

config.source.lv2.v1=5.0
设置LEVEL2资金流散户资金单笔上限为5万元

config.source.lv2.v2=50.0
设置LEVEL2资金流中单单笔上限为50万

config.source.lv2.v3=100.0
设置LEVEL2资金流大单上限为100万元，之上则为超大买卖单

config.source.from=1980-01-01
config.source.to=2018-12-31
设定股票回测日期区间

config.trade.max.subnewstock=60
针对单股仅测试前60日内数据（即次新股模式）

config.trade.启用每日累加收益=?
指定盈亏统计算法，可选值为yes，no；默认为no，如为yes，统计盈亏时则把单日持股涨幅累加计算；如为no，则把单日持股涨幅平均作为单日统计涨幅；可以理解为设置为yes，会放大盈亏，视为带有杠杆性质。

config.source.minactive=0
config.source.maxactive=100
设置股价可接受活跃度区间[0,100]，一个股票只有一个活跃度，值越大说明股票越活跃。注意，此指令带有未来函数

config.runningMode=回测
设定股票运行模式，可选回测，选股基于配对交易，默认为回测

config.source.pool=6000** 
config.source.pool=000*** 
config.source.pool=00**01 
config.source.pool=0009** 
config.source.pool=300506,000056 
config.source.pool=随机50 
config.source.pool=我的股票池
设定回测股票池，如果参数值为随机50，随机100，随机300之一，则实际股票池为随机选出的50，100或者300个股票；如果为我的股票池，则之前需要在菜单配置-股票池中自定义股票池清单；如果使用类似000***等包含通配符的模式，则股票池为参考股票代码以000开头的股票列表；如果股票池为类似300506,000056模式则以给定股票列表为准 
NOTE：免费版本不支持通配符类型的股票池

config.trade.max.subnewstock=60
定义次新股，当前软件无法区分次新股，可以借助本参数模式次新股选股，参数值60含义为股票日线数据前60个日为可能次新股

config.trade.buy.movepoint=1.0
设定股票买入滑动涨跌幅，含义为以第二日开盘涨跌上浮给定涨幅为买入点，如果该涨幅无法买入，则指定股票不纳入统计；可选范围为[-2.0 - 2.0]，默认为0.0

买卖点 
软件买点以buy.开头，卖点以sale.开头

指令元 
本软件量化指令元可分为2类，一类是布尔指令元，一类是数值指令元，前者可作为独立的策略语句，后者则需要使用判断语句组合为独立的策略语句 
一个简单的例子如下:

buy.close,0>ma,5,0
buy.sea.大阳线,0
在这个例子中

close,0 
ma,5,0 
sea.大阳线,0 
即为指令元
下面逐一介绍指令元

OHLC指令元

OHLC指令元系列用于获取股票的open/high/low/close以及vol等基础数据

ohlc.open,offset
获取股票开盘价，offset为量化偏移(offset含义下同不再赘述)

ohlc.high,offset
获取股票最高价

ohlc.low,offset
获取股票最低价

ohlc.close,offset
获取股票收盘价

ohlc.increase,offset
获取股票涨幅

ohlc.increase,flag,offset
获取股票给定类型的涨幅 
flag=1，结果为开盘价相对于昨日涨幅 
flag=2，结果为最高价相对于昨日涨幅 
flag=3，结果为最低价相对于昨日涨幅

ohlc.swing,offset
获取股票振幅，计算公式为swing=(high-low)/low*100.0

ohlc.swing,flag,offset
flag=1，计算最高价相对于开盘价，收盘价平均的涨幅 
flag=2，计算股票开盘价，收盘价平均相对于最低价的涨幅 
flag=3，计算股票收盘价相对于开盘价涨幅 
flag=4，计算股票开盘价相对于最低价的涨幅 
flag=5，计算股票最高价相对于收盘价涨幅

ohlc.vol,flag
获取股票当日成交量

ohlc.总市值,flag
获取股票总市值（单位：万元）

ohlc.换手率,flag
获取股票换手率数据，注意事项，此处换手率是根据股票最新流通市值和量化日成交量数据换算出的，如果中间有股票解禁则可能会有偏差，如果需要精确的换手率数据请使用tick.turnover指令

布林轨道 
布林轨道指令元用于计算布林轨道参数

boll.upper,offset
boll.mid,offset
boll.lower,offset
boll.width,offset
以上指令分别用于获取布林通道(20,2)的上轨，中轨，下轨和轨道宽度(boll.width=boll.upper-boll.mid)

对于收费版本，可以使用以下格式的带参数布林指标，如下：

boll.upper,n,m,offset 
boll.mid,n,m.offset 
boll.width,n,m,offset 
其中n为布林指标中的取点数，m为布林带宽度系数，boll.width计算公式为*(boll.upper-boll.mid)/boll.mid*100.0*
MA/EMA/VOLMA指令元系列

ma指令用于计算股票简单移动均线值

ma,n,offset
获取股票n日均线值

ema指令用于计算股票指数移动均线值

ema,n,offset
获取股票n日加权平均值

volma指令用于计算股票简单移动均线值

volma,n,offset
获取股票n日成交量平均值

MACD指令元

macd.diff,offset
macd.bar,offset
macd.dea,offset
获取股票MACD的diff,bar和dea值，默认参数为12,26,9 
对于收费版本，可以自定义MACD参数，格式如下

macd.diff,a,b,c,offset
macd.bar,a,b,c,offset
macd.dea,a,b,c,offset
KDJ指令元

kdj.k,offset
kdj.d,offset
kdj.j,offset
获取股票日线KDJ的k，d，j值，默认参数为9,3,3

DMI指令元

dmi.pdi,n,m,offset
dmi.mdi,n,m,offset
dmi.adx,n,m,offset
dmi.adxr,n,m,offset
获取过DMI指标的pdi，mdi，adx和adxr值，指定参数为n,m,默认为14,6

其他指令元

psy,n,offset
获取给定周期n的心理线psy指标值

bias,n,offset
获取给定周期n的乖离率bias指标值

wr,n,offset
获取给定周期n的威廉wr指标值

ad,offset
获取给定的积累派发线指标ad值

pvi,offset
获取正成交量指标pvi值

bop,offset
获取力量平衡指标bop值

obv,offset
获取能量潮指标obv值

wad,offset
获取威廉多空力度线wad值

tr,offset
获取股价真实波动值,如果需要atr值，请参考如下代码 
@tr=tr,0 
@atr=ma[@tr,value,14.0]

value,data
构造一个值为data的常量，例如：

buy.increase,0>value,4.0
含义是若量化日股票涨幅超过4.0%则为买入条件

若需要完整版本软件或需要进一步开发  
请联系作者 wx rawman2013 收费1万起步！
