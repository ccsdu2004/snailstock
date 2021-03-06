#include <QDebug>
#include <QRegExp>
#include <QTextStream> 
#include <QSet>
#include <QDate>
#include <cstdlib>
#include "Interface.h"
#include "StockNewQQHistoryInterface.h"
#include "update/DataLoader.h"
#include "SnailApplication.h"
#include "common/common.h"
#include "common/FileUtils.h"

//http://vip.stock.finance.sina.com.cn/quotes_service/api/json_v2.php/CN_Transactions.getALLPageTime?data=2017-04-11&symbol=sz000009

QStringList pickupStrings(const QString& data,const QString& express)
{
	QStringList output;

	QRegExp regex(express);
	regex.setCaseSensitivity(Qt::CaseInsensitive);
	regex.setMinimal(true); 
    int pos(0);
	while((pos = regex.indexIn(data,pos)) != -1)
	{
		output.append(regex.capturedTexts().at(0));
	    pos += regex.matchedLength();  
	}
	return output;
}

void StringListInterface::onError(const QString& error)
{
    qDebug() << error <<"\n";
}

void StringMapInterface::onError(const QString& err)
{
    qDebug() << err <<"\n";
}

void NoticeInterface::onError(const QString& err)
{
    qDebug() << err <<"\n";
}

QStringList EastmoneyInterface::buildLinks(const QStringList& list)
{
    const QString url = "http://data.eastmoney.com/notices/hsa/%1.html";
	QStringList output;
	foreach(QString item,list)
	{
	    QString link(url);
		link = link.arg(item);
		output.push_back(link);
	}
	return output;
}

QString EastmoneyInterface::rawParse(const QString& content)
{
	QString output;
	QRegExp regex("defjson:(.*)charset");
	regex.setCaseSensitivity(Qt::CaseInsensitive);
	regex.setMinimal(true); 
    int pos(0);
	if((pos = regex.indexIn(content,pos)) != -1)
    {
        output = regex.capturedTexts().at(0);
		pos += regex.matchedLength();
    }
	return output;
}

QString EastmoneyInterface::pickupString(const QString& data,const QString& expr)
{
	QRegExp regex(expr);
	regex.setCaseSensitivity(Qt::CaseInsensitive);
	regex.setMinimal(true); 
    int pos(0);
	if((pos = regex.indexIn(data,pos)) != -1)
    {
        return regex.capturedTexts().at(0);
    }
	return QString();
}

QVector<Data> EastmoneyInterface::parseContent(const QString& content,const QString& url)
{
    QString data = rawParse(content);
	QVector<Data> output;
	if(data.isEmpty())
		return output;

	QString buffer(content);

	QStringList list = buffer.split("\"NOTICEDATE\":");
	
	foreach(QString item,list)
	{
		item = item.simplified();

		if(item.isEmpty())
			continue;

		QString code = pickupString(item,"\"SECURITYCODE\":\"(.*)\"");
		code = code.mid(code.size()-7,6);
		
		QString name = pickupString(item,"\"SECURITYSHORTNAME\":\"(.*)\"");
		name = name.mid(21,name.size()-22);

		QString title = pickupString(item,"\"NOTICETITLE\":\"(.*)\"");
		title = title.mid(15,title.size()-16);

		if(title.startsWith(code))
			title = title.mid(7);

		QString date = pickupString(item,"\"EUTIME\":\"(.*)\"");
		date = date.mid(10,date.size()-26);

		QString category = pickupString(item,"\"COLUMNNAME\":\"(.*)\"");
		category = category.mid(14,category.size()-15);

		Data data;
		data.code = code;
		data.name = name;
		data.date = date;
		data.plus["title"] = title;
		data.plus["type"] = category;
		output.append(data);
	}
	return output;
}

//http://data.eastmoney.com/tfpxx/
QStringList EastmoneyRecoveryTransactionInterface::buildLinks(const QStringList& list)
{
    QStringList output("http://data.eastmoney.com/tfpxx/");
	return output;
}

QString EastmoneyRecoveryTransactionInterface::rawParse(const QString& content)
{
	QString output;
	QRegExp regex("defjson:(.*)maketr:");
	regex.setCaseSensitivity(Qt::CaseInsensitive);
	regex.setMinimal(true); 
    int pos(0);
	if((pos = regex.indexIn(content,pos)) != -1)
    {
        output = regex.capturedTexts().at(0).mid(8);
		pos += regex.matchedLength();
    }
	output = output.mid(output.indexOf("\"")+1);
	return output;
}

QStringList EastmoneyRecoveryTransactionInterface::pickupString(const QString& data,const QString& expr)
{
	QStringList output;

	QRegExp regex(expr);
	regex.setCaseSensitivity(Qt::CaseInsensitive);
	regex.setMinimal(true); 
    int pos(0);
	while((pos = regex.indexIn(data,pos)) != -1)
    {
		output.append(regex.capturedTexts().at(0));
		pos += regex.matchedLength();  
    }
	return output;
}

QVector<Data> EastmoneyRecoveryTransactionInterface::parseContent(const QString& content,const QString& url)
{
    QString data = rawParse(content);
	QVector<Data> output;
	if(data.isEmpty())
		return output;

	QStringList list = data.split("\",\"");
	foreach(QString item,list)
	{
		if(!item.contains("type"))
		{
			item = item.simplified();
			QStringList items = item.split(",");
			Data data;
			data.code = items[0];
			data.name = items[1];
			data.plus["stopDate"] = items[7];
			data.date = items[8];
			data.plus["span"] = items[4].mid(0,10);
			data.plus["desc"] = items[5];
   			output.push_back(data);
		}
	}

	return output;
}

//http://data.eastmoney.com/stock/lhb.html
/*QStringList EastmoneyLHBListInterface::buildLinks(const QStringList& list)
{
	QStringList output;
	if(!list.isEmpty())
	{
	    QString date = list[0];
		const QString str = "http://data.eastmoney.com/DataCenter_V3/stock2016/TradeDetail/pagesize=200,page=1,sortRule=-1,sortType=,startDate=%1,endDate=%1,gpfw=0,js=vardata_tab_1.html";
		output.append(QString(str).arg(date));
	}
	return output;
}
	
QVector<QMap<QString,QString> > EastmoneyLHBListInterface::parseContent(const QString& content,const QString& url)
{
	return parserJson(content);
}*/

EastmoneyLHBDataInterface::EastmoneyLHBDataInterface()
{
}

EastmoneyLHBDataInterface::~EastmoneyLHBDataInterface()
{
}

QStringList EastmoneyLHBDataInterface::buildLinks(const QStringList& list)
{
	QStringList output;
	const QString link = "http://data.eastmoney.com/stock/lhb,%1,%2.html";
	foreach(QString item,list)
	{
		QStringList blocks = item.split(",");
		item = QString(link).arg(blocks[1].trimmed()).arg(blocks[0].trimmed());;
		output.append(item);
	}

	return output;
}
	
LHBUnit EastmoneyLHBDataInterface::parseNetValue(const QString& content,bool& ok)
{
	LHBUnit unit;
	// class="total-tr">
	QStringList blocks = pickupStrings(content,"class=\"total-tr\">(.*)</tr>");
	if(blocks.size() != 1)
		return unit;

	unit.flag = 3;
    
	QString buffer = blocks.at(0);
    blocks = pickupStrings(buffer,"span(.*)</span>");

	if(blocks.size() != 6)
		return unit;

	buffer = blocks[1];
	buffer = buffer.mid(buffer.indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('<'));
	unit.buy = buffer.trimmed();

	buffer = blocks[2];
	buffer = buffer.mid(buffer.indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('%')+1);
	unit.tradeBuyPercent = buffer.trimmed();

	buffer = blocks[3];
	buffer = buffer.mid(buffer.indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('<'));
	unit.sale = buffer.trimmed();

	buffer = blocks[4];
	buffer = buffer.mid(buffer.indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('%')+1);
	unit.tradeSalePercent = buffer.trimmed();

	buffer = blocks[5];
	buffer = buffer.mid(buffer.indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('<'));
	unit.netValue = buffer.trimmed();

	ok = true;
	return unit;
}

QVector<LHBUnit> EastmoneyLHBDataInterface::parseContent(const QString& content,const QString& url)
{
    QVector<LHBUnit> output;
	QStringList blocks = pickupStrings(content,"class=\"content-sepe\">(.*)</html>");
	if(blocks.isEmpty())
		return output;

    blocks = pickupStrings(blocks[0],"<tbody>(.*)</tbody>");
	int size = blocks.size() >= 2 ? 2 : blocks.size();

	for(int i=0;i<size;i++)
		output += parseContentUnitImpl(blocks[i],i == 0);

	bool ok = false;
	LHBUnit unit = parseNetValue(content,ok);
	if(ok)
		output.append(unit);

	return output;
}

LHBUnit EastmoneyLHBDataInterface::parseUnit(const QString& content,bool& ok)
{
	LHBUnit unit;
    QStringList items = pickupStrings(content,".html\">(.*)</a></a>");
	if(items.isEmpty())
		return unit;

	QString buffer = items[0].mid(7);
	buffer = buffer.mid(0,buffer.size()-8);
    
	unit.department = buffer;

	items = pickupStrings(content,"=\"times\">(.*)</span>");
	if(items.isEmpty())
		return unit;

	buffer = items[0];
    buffer = buffer.mid(buffer.indexOf('>') + 1);
	buffer = buffer.mid(0,buffer.indexOf('<')-2).trimmed();

	unit.count = buffer;

	items = pickupStrings(content,"<span class=\"percent\"(.*)</span");
	if(items.isEmpty())
		return unit;

	buffer = items[0];
	buffer = buffer.mid(buffer.indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('<')).trimmed();
    unit.success = buffer;

	items = pickupStrings(content,"<td(.*)</td>");
	if(items.size() != 7)
		return unit;

	buffer = items[0].mid(4,items[0].size()-9);
	unit.index = buffer;

	buffer = items[2].mid(items[2].indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('<'));
	
	unit.buy = buffer;

	buffer = items[3].mid(items[3].indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('%')+1);
	unit.tradeBuyPercent = buffer;
    
	buffer = items[4];
	buffer = buffer.mid(buffer.indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('<'));
	unit.sale = buffer;

	buffer = items[5];
	buffer = buffer.mid(buffer.indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('%')+1);
	unit.tradeSalePercent = buffer;

	buffer = items[6];
	buffer = buffer.mid(buffer.indexOf('>')+1);
	buffer = buffer.mid(0,buffer.indexOf('<'));
	unit.netValue = buffer;
	ok = true;
	return unit;
}

QVector<LHBUnit> EastmoneyLHBDataInterface::parseContentUnitImpl(const QString& content,bool buy)
{
    QStringList items = pickupStrings(content,"<tr>(.*)</tr>");
	QVector<LHBUnit> output;

	foreach(QString item,items)
	{
		bool ok = false;
		LHBUnit unit = parseUnit(item,ok);
		unit.flag = buy ? 1 : 2;
		if(ok)
			output.append(unit);
	}

	return output;
}

//quotes.money.163.com/service/chddata.html?code=1300127&start=20140501&end=20170330
/*QStringList Stock163Interface::buildLinks(const QStringList& list)
{
	const int span = 360*3;
	QDate from = QDate::currentDate();
	from = from.addDays(-span);
	//const QMap<QString,Stock> stocks = snail->getStocks();
	static const QString LINK = "http://quotes.money.163.com/service/chddata.html?code=%1&start=%2";
	QStringList output;
	foreach(QString item,list)
	{
		QString code;
		if(item.startsWith("6"))
			code = "0" + item;
		else
			code = "1" + item;
		//if(stocks.find(item) != stocks.end())
		{
			if(stocks[item].timeToMarket > from.toString("yyyyMMdd"))
				item = LINK.arg(code).arg(stocks[item].timeToMarket);
			else
				item = LINK.arg(code).arg(from.toString("yyyyMMdd"));
			output.append(item);
		}
	}
	return output;
}

QVector<Stock> Stock163Interface::parseContent(const QString& content,const QString& code)
{
	QVector<Stock> output;
	QStringList items = content.split("\n");
	if(items.size() < 27)
		return output;

	Stock temp;
	//QMap<QString,Stock> stocks = snail->getStocks();

	items.removeAt(0);
	for(int i=items.size()-1;i>=0;i--)
	{
		QString item = items[i];
		QStringList ss = item.split(',');
		if(ss.size() != 16)
			continue;
	    
		Stock stock;
		stock.date = ss[0];
		stock.code = ss[1].mid(1);
		if(temp.code.isEmpty() && stocks.find(stock.code) != stocks.end())
			temp = stocks[stock.code];
	
		stock.industry = temp.industry;
		stock.area = temp.area;
		stock.name = ss[2];
	
		stock.current = ss[3].trimmed().toFloat();
		if(stock.current < 0.01)
			continue;
		stock.top = ss[4].toFloat();
		stock.low = ss[5].toFloat();
		stock.open = ss[6].toFloat();
		stock.offset = ss[8].toFloat();
		stock.increase = ss[9].toFloat();
		stock.turnover = ss[10].toFloat();
		stock.tradeCapital = ss[11].toFloat();
		stock.tradeVolume = ss[12].toFloat();
		stock.totals = ss[13].toFloat();
		stock.outstanding = ss[14].toFloat();
		output.append(stock);
	}

	if(!temp.code.isEmpty())
		saveToFile(targetDir + temp.code,content);

	return output;
}

void Stock163Interface::setParam(int flag,const QString& value)
{
	if(StockInterface::FLAG_TARGET_DIR == flag)
		targetDir = value;
}*/



/*
StockListInterface::StockListInterface()
{
}

StockListInterface::~StockListInterface()
{
}

QStringList StockListInterface::buildLinks(const QStringList& list)
{
    return QStringList("http://218.244.146.57/static/all.csv");
}

QVector<Stock> StockListInterface::parseContent(const QString& content,const QString& flag)
{
    const int code = 0;
	const int name = 1;
	const int industry = 2;
	const int area = 3;
	const int pe = 4;
	const int outstanding = 5;
	const int totals = 6;
	const int totalAssets = 7;
	const int liquidAssets = 8;
	const int fixedAssets = 9;
	const int reserved = 10;
	const int reservedPerShare = 11;
	const int esp = 12;
	const int bvps = 13;
	const int pb = 14;
	const int timeToMarket = 15;
	const int undp = 16;
	const int perundp = 17;
	const int rev = 18;
	const int profit = 19;
	const int gpr = 20;
	const int npr = 21;
	const int holders = 22;

    QVector<Stock> stocks;

	QStringList list = splitBuffer(content);
	if(list.isEmpty())
		return stocks;

	for(int i=1;i<list.size();i++)
	{
		QStringList blocks = list[i].split(",");
		if(blocks.size() == 23)
		{
			Stock stock;
			stock.code = blocks[code];
			stock.name = blocks[name];
			stock.industry = blocks[industry];
			stock.area = blocks[area];
			stock.pe = blocks[pe].toFloat();
			stock.outstanding = blocks[outstanding].toFloat()*10000;
			stock.totals = blocks[totals].toFloat()*10000;
			stock.totalAssets = blocks[totalAssets].toFloat();
			stock.liquidAssets = blocks[liquidAssets].toFloat();
			stock.fixedAssets = blocks[fixedAssets].toFloat();
			stock.reserved = blocks[reserved].toFloat();
			stock.reservedPerShare = blocks[reservedPerShare].toFloat();
			stock.esp = blocks[esp].toFloat();
			stock.bvps = blocks[bvps].toFloat();
			stock.pb = blocks[pb].toFloat();
			stock.timeToMarket = blocks[timeToMarket];
			stock.undp = blocks[undp].toFloat();
			stock.perundp = blocks[perundp].toFloat();
			stock.rev = blocks[rev].toFloat();
			stock.profit = blocks[profit].toFloat();
			stock.gpr = blocks[gpr].toFloat();
			stock.npr = blocks[npr].toFloat();
			stock.holders = blocks[holders].toInt();
			if(stock.totals > 0 && stock.outstanding > 0 && (!stock.name.contains("ST")))
				stocks.append(stock);
		}
	}
#ifndef SNAIL_VERSION_FREE
	saveToFile("data/all.csv",content);
#endif 
	return stocks;
}*/

/*
QQLHBDataInterface::QQLHBDataInterface()
{
}

QQLHBDataInterface::~QQLHBDataInterface()
{
}

QStringList QQLHBDataInterface::buildLinks(const QStringList& list)
{
    static const QString var = "http://stock.finance.qq.com/cgi-bin/sstock/q_lhb_xx_js?c=%1&b=%2&l=070001";
	QStringList output;
	foreach(QString item,list)
	{
		QStringList blocks = item.split(",");
		if(blocks.size() == 2)
		{
			QString date = blocks.at(1);
			date = date.remove("-");
			QString link = QString(var).arg(blocks.at(0)).arg(date);
			output.push_back(link);
		}
	}
	return output;
}

QVector<LHBUnit> QQLHBDataInterface::parseContent(const QString& content,const QString& url)
{
	QVector<LHBUnit> output;
	if(!content.contains("[["))
		return output;
	if(!content.contains("]]"))
		return output;

	QString buffer = content.mid(content.indexOf("[[")+2);
	buffer = buffer.mid(0,buffer.lastIndexOf("]]"));
	buffer = buffer.replace('"',"");
	QStringList segment = buffer.split("],[");
	foreach(QString seg,segment)
	{
		QStringList items = seg.split(",");
		if(items.size() == 8)
		{
			LHBUnit unit;
			//Data data;
			//data.code = items[0];
			//data.name = items[1];
			if(items[2] == "B")
			{
				unit.flag = 1;
				//data.plus["flag"] = "1";
			}
			else
			{
				unit.flag = 2;
				//data.plus["flag"] = "2";
			}
			unit.index = items[3];
			//data.date = items[4];
			unit.department = items[5];
			unit.buy = QString("%1").arg(items[6].toFloat()*0.0001);
			unit.sale = QString("%1").arg(items[7].toFloat()*0.0001);
			//data.var.setValue(unit);
			output.append(unit);
		}
	}

	return output;
}

SinaLHBStockListInterface::SinaLHBStockListInterface(){}
SinaLHBStockListInterface::~SinaLHBStockListInterface(){}

QStringList SinaLHBStockListInterface::buildLinks(const QStringList& list)
{
    QStringList output;
	if(list.isEmpty())
		output.append("http://vip.stock.finance.sina.com.cn/q/go.php/vInvestConsult/kind/lhb/index.phtml");
	else
	{
		const QString link("http://vip.stock.finance.sina.com.cn/q/go.php/vInvestConsult/kind/lhb/index.phtml?tradedate=%1");
	    foreach(QString item,list)
			output.append(QString(link).arg(item));
	}
	return output;
}

QVector<Stock> SinaLHBStockListInterface::parseContent(const QString& content,const QString& code)
{
    QVector<Stock> output; 
	QStringList blocks = pickupStrings(content,"<table class=\"list_table(.*)<div class=\"pages\">");
	if(blocks.isEmpty())
		return output;

	blocks = pickupStrings(blocks[0],"<tr(.*)</tr>");
	foreach(QString item,blocks)
	{
		Stock stock = doTr(item);
		if(stock.flag)
			output.push_back(stock);
	}
	return output;
}

Stock SinaLHBStockListInterface::doTr(const QString& content)
{
    Stock stock;
	stock.type = desc;

	int offset = content.indexOf("</span>");
	if(offset != -1)
	{
		stock.type = content.mid(0,offset);
		stock.type = stock.type.mid(stock.type.lastIndexOf(">")+1).trimmed();
		desc = stock.type;
	}
	else
	{
		offset = content.indexOf("_blank");
		if(offset != -1)
		{
		    QStringList blocks = pickupStrings(content,"<td(.*)</td>");
			if(blocks.size() == 8)
			{
				for(int i=0;i<blocks.size();i++)
				{
					QString buffer = blocks[i].mid(0,blocks[i].size()-5).trimmed();
					if(i==1)
					{
						stock.code = buffer.mid(buffer.indexOf("q=")+2);
						stock.code = stock.code.mid(0,6);
					}
					else if(i==2)
					{
						stock.name = buffer.mid(0,buffer.size()-4);
						stock.name = stock.name.mid(stock.name.lastIndexOf(">")+1).trimmed();
					}
					else if(i==3)
					{
						buffer = buffer.mid(buffer.lastIndexOf(">")+1);
						stock.current = buffer.toFloat();
					}
					else if(i==5)
					{
						buffer = buffer.mid(buffer.lastIndexOf(">")+1);
						stock.tradeVolume = buffer.toFloat();
					}
					else if(i==6)
					{
						buffer = buffer.mid(buffer.lastIndexOf(">")+1);
						stock.tradeCapital = buffer.toFloat();
					}
					else if(i==7)
					{
						buffer = buffer.mid(0,buffer.indexOf("',this"));
						buffer = buffer.mid(buffer.size()-10);
						stock.date = buffer;
					}
				}
			    stock.flag = true;
			}
		}
	}

	return stock;
}


QStringList QQDcotorStringListInterface::buildLinks(const QStringList& list)
{
	const QString url = "http://stockapp.finance.qq.com/doctor/%1.html";
	QStringList output;
	foreach(QString item,list)
	{
		QString link = QString(url).arg(item);
		output.append(link);
	}

	return output;
}

QStringList QQDcotorStringListInterface::parseSeg(const QString& content,const QString& regex,QString& out)
{
    QStringList output;
	QStringList blocks = pickupStrings(content,"<li title=\"(.*)\"");
	foreach(QString block,blocks)
        output.append(block.mid(12,block.size()-13));	

	blocks = pickupStrings(content,"<span>(.*)</span>");
	if(!blocks.isEmpty())
		out = blocks[0].mid(7,blocks[0].size()-13).trimmed();

	return output;
}

QVector<QStringList> QQDcotorStringListInterface::parseContent(const QString& content)
{
	QVector<QStringList> output;
	QStringList blocks = pickupStrings(content,"<div class=\"ranks(.*)\">");
	if(blocks.size() == 0)
		return output;

	QString score = blocks[0];
	score = score.mid(score.indexOf('"')+7,2);
	
    blocks = pickupStrings(content,"<div class=\"text-head\">(.*)</div>");
	if(blocks.size() == 0)
		return output;

	blocks[0] = blocks[0].mid(23).trimmed();
	blocks[0] = blocks[0].mid(0,blocks[0].size()-6).trimmed();

	QString tip = blocks[0];

	blocks = pickupStrings(content,"<div class=\"text-content\"(.*)</div>");
	if(blocks.size() == 0)
		return output;

	blocks[0] = blocks[0].mid(0,blocks[0].size()-6);
	blocks[0] = blocks[0].mid(blocks[0].indexOf(">")+1);
	blocks[0] = blocks[0].trimmed();
	QString tipDesc = blocks[0];

	QString trendDesc,qualityDesc;
	QString trend = parseSeg(content,"<div class=\"icon01\">(.*)</div>",trendDesc).join("\n");
	QString quality = parseSeg(content,"<div class=\"icon02\">(.*)</div>",qualityDesc).join("\n");

	blocks = pickupStrings(content,"<span class=\"dqzs\">(.*)</span>");

	return output;
}

SinaAchievementInterface::SinaAchievementInterface()
{
}

SinaAchievementInterface::~SinaAchievementInterface()
{
}

QStringList SinaAchievementInterface::buildLinks(const QStringList& list)
{
	QString year = list[0];
	QString quarter = list[1];
	const static QString temp = "http://vip.stock.finance.sina.com.cn/q/go.php/vFinanceAnalyze/kbind/preformance/index.phtml?s_i=&s_a=&s_c=&reportdate=%1&quarter=%2&p=%3&page=30";
	QStringList output;
	for(int i=0;i<20;i++)
	{
		QString link = QString(temp).arg(year).arg(quarter).arg(i+1);
		output.append(link);
	}

	return output;
}

QVector<QMap<QString,QString> > SinaAchievementInterface::parseContent(const QString& content,const QString& url)
{
	QVector<QMap<QString,QString> > output;

	QStringList blocks = pickupStrings(content,"</thead>(.*)<div class=\"clear\"></div>");
	if(blocks.isEmpty())
		return output;

	QString buffer = blocks[0];
	blocks = pickupStrings(buffer,"<td style=\"width(.*)</a></td>");
	foreach(QString item,blocks)
	{
	    QMap<QString,QString> stock;
		QStringList items = pickupStrings(item,"<td(.*)</td>");
		if(items.size() == 12)
		{
		    QString code = items[0].mid(items[0].size()-22,6);
			QString name = items[1].mid(items[1].size()-20);
			if(name.startsWith(">"))
				name = name.mid(1);
			name = name.mid(0,name.indexOf("<"));
			QString eps = items[2].mid(4,items[2].size()-9);
			QString eps_r = items[3].mid(4,items[3].size()-9);
			QString navps = items[4].mid(4,items[4].size()-9);
			QString roe = items[5].mid(4,items[5].size()-9);
			QString cps = items[6].mid(4,items[6].size()-9);
			QString netprofit = items[7].mid(4,items[7].size()-9);
			QString netprofit_percent = items[8].mid(4,items[8].size()-9);
			QString date = items[10].mid(4,items[10].size()-9);

#define STR(tag) #tag
#define LINK(tag) stock[STR(tag)] = tag;
			LINK(code)
			LINK(name)
			LINK(eps)
			LINK(eps_r)
			LINK(navps)
			LINK(roe)
			LINK(cps)
			LINK(netprofit)
			LINK(netprofit_percent)
			LINK(date)
			output.push_back(stock);
#undef LINK
		}
	}

	return output;
}

StockNewsInterface::StockNewsInterface()
{
}

StockNewsInterface::~StockNewsInterface()
{
}

QStringList StockNewsInterface::buildLinks(const QStringList& list)
{
    QStringList output;
	const QString temp = "http://ifzq.gtimg.cn/appstock/news/info/search?type=3&page=%1&n=6&symbol=%2&_appName=android&_net=WIFI&__random_suffix=%3";
    foreach(QString item,list)
	{
		QString url = QString(temp).arg(1).arg(item).arg(StockInterface::rand()*INT_MAX);
		output.push_back(url);
	}
	return output;
}

QVector<QMap<QString,QString> > StockNewsInterface::parseContent(const QString& content,const QString& url)
{
    return parserJson(content);
}*/

//http://ifzq.gtimg.cn/appstock/app/HsDealinfo/getMingxi?code=sz000850&index=0&_appName=android&_dev=ZTE+G720C&_devId=6c718af3c71ab19144b2b21d08ff7cfb1318d4c0&_mid=6c718af3c71ab19144b2b21d08ff7cfb1318d4c0&_md5mid=0931642B3253D5385ABFEF3A441CE5F5&_omgid=fbca81e6093ca14a586be751d728e4e213bc001021190d&_omgbizid=83b7888a8cb37c47aa09e40b5927e4728d67014021190d&_appver=5.3.5&_ifChId=76&_screenW=1080&_screenH=1920&_osVer=4.4.4&_uin=283317457&_wxuin=283317457&_net=WIFI&__random_suffix=31684
//http://ifzq.gtimg.cn/appstock/app/HsDealinfo/getDadan?code=sz000850&_appName=android&_dev=ZTE+G720C&_devId=6c718af3c71ab19144b2b21d08ff7cfb1318d4c0&_mid=6c718af3c71ab19144b2b21d08ff7cfb1318d4c0&_md5mid=0931642B3253D5385ABFEF3A441CE5F5&_omgid=fbca81e6093ca14a586be751d728e4e213bc001021190d&_omgbizid=83b7888a8cb37c47aa09e40b5927e4728d67014021190d&_appver=5.3.5&_ifChId=76&_screenW=1080&_screenH=1920&_osVer=4.4.4&_uin=283317457&_wxuin=283317457&_net=WIFI&__random_suffix=56208
//http://ifzq.gtimg.cn/appstock/app/minute/query?code=sz000850&_appName=android&_net=WIFI&__random_suffix=91966
//http://ifzq.gtimg.cn/stock/corp/cwbb/search?symbol=sz000850&type=sum&jianjie=1&_appName=android&_net=WIFI&__random_suffix=39530
//http://ifzq.gtimg.cn/appstock/smart/Smart2/update?more=1&norules=1&version=20170316043027&_appName=android&_appver=5.3.5&_ifChId=76&_screenW=1080&_screenH=1920&_osVer=4.4.4&_net=WIFI&__random_suffix=71413
//http://ifzq.gtimg.cn/appstock/app/invest/get3?_appName=android&_osVer=4.4.4&_net=WIFI&__random_suffix=93322
//http://ifzq.gtimg.cn/appstock/news/info/search?type=3&page=1&n=6&symbol=sz002571&_appName=android&_net=WIFI&__random_suffix=76607
//http://appqt.gtimg.cn/utf8/q=bkqt012003,bkqt021208,bkqt021225,bkqt021011,bkqt021116,bkqt021219,bkqt021088,bkqt021104,?_appName=android&_net=WIFI&__random_suffix=97930
//http://ifzq.gtimg.cn/appstock/app/day/query?p=1&code=sz000048&_appName=android&_net=WIFI&__random_suffix=1366