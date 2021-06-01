#include <numeric>
#include "StockRealTimeInfo.h"
#include "common/common.h"

StockRealTimeInfo::StockRealTimeInfo()
{
	flag = false;
	capitcalFlow._1 = 0;
	capitcalFlow._2 = 0;
	capitcalFlow._3 = 0;
}

StockTimeDataInterface::StockTimeDataInterface()
{
}

QString StockTimeDataInterface::buildLink(const QString& code)
{
	/*QString arg;
	if(code.startsWith("6"))
		arg = "sh" + code;
	else
		arg = "sz" + code;*/
	return QString("http://ifzq.gtimg.cn/appstock/app/minute/query?code=%1&_appName=android&_net=WIFI&__random_suffix=%2").arg(code).arg(qrand());
}

StockRealTimeInfo StockTimeDataInterface::parseContent(const QString& content)
{
	StockRealTimeInfo info;
	info.low.first  = std::numeric_limits<float>::max();
	info.high.first = std::numeric_limits<float>::min();

	QString buffer(content);
	//buffer = buffer.remove("\n");

	int pos = buffer.indexOf("]");
	buffer = buffer.mid(0,pos);
	buffer = buffer.mid(buffer.indexOf("[")+1).remove("\"");
	QStringList list = buffer.split(",");    

	float sum = 0.0f;
	float vol = 0.0f;

	int p1c = 0;
	int p2c = 0;
	float oldma = 0.0f;

	QVector<StockTimeLine> timeData;
	foreach(QString item,list)
	{
		QStringList ds = item.split(" ");
	    StockTimeLine data;
		if(ds.size() == 3)
		{
			data.time = QTime::fromString(ds[0],"hhmm");
			if(data.time.hour() > 12)
				data.time = data.time.addSecs(-5400+60);
			data.price = ds[1].toFloat();
			data.volume = ds[2].toInt() - vol;
			vol = ds[2].toInt();
			sum += data.price*data.volume;
			data.ma = sum/(float)vol;

			if(data.price < info.low.first)
			{
				info.low.first = data.price;
				info.low.second = data.time;
			}
			else if(data.price > info.high.first)
			{
				info.high.first = data.price;
				info.high.second = data.time;
			}

		    timeData.push_back(data);

			if(data.price >= data.ma)
				p1c ++;
			if(data.ma >= oldma)
				p2c ++;
			oldma = data.ma;
		}
	}

	float p1 = p1c/(float)timeData.size()*100.0f;
	float p2 = p2c/(float)timeData.size()*100.0f;
	info.stock.data.insert(Stock::TAG_P1,STR_CEIL(p1));
	info.stock.data.insert(Stock::TAG_P2,STR_CEIL(p2));
	info.list = timeData;

	int flag = content.indexOf("data");
	if(flag != -1)
	{
		info.code = content.mid(flag + 8);//10);
		info.code = info.code.mid(0,6);
	}

	QString remain = content.mid(pos+1);
	list = pickupStrings(remain,"\"date\":\"(.*)\"");
	if(!list.isEmpty())
	{
		info.flag = true;
		info.date = list[0].mid(8);
		info.date = info.date.mid(0,8);
		
		remain = remain.mid(20);
		remain = remain.mid(2+remain.indexOf("]"));
		QString code = remain.mid(0,pos = remain.indexOf("[")-1);
		code = code.mid(3,6);

        QString name = remain.mid(pos+8);
		name = name.mid(0,name.indexOf("\""));
		info.name = name;

		remain = remain.mid(remain.indexOf(code,10)+9);

		QString seg = remain.mid(0,remain.indexOf(":")).remove("\"");
		list = seg.split(",");
		if(list.size() == 27)
		{
			info.stock.current = list[0].toFloat();
			info.stock.privClose = list[1].toFloat();
			info.stock.open = list[2].toFloat();
			info.stock.tradeVolume = list[3].toFloat();

			info.buy[0].first = list[6].toFloat();
			info.buy[0].second = list[7].toInt();

			info.buy[1].first = list[8].toFloat();
			info.buy[1].second = list[9].toInt();

			info.buy[2].first = list[10].toFloat();
			info.buy[2].second = list[11].toInt();

			info.buy[3].first = list[12].toFloat();
			info.buy[3].second = list[13].toInt();

			info.buy[4].first = list[14].toFloat();
			info.buy[4].second = list[15].toInt();

			info.sale[0].first = list[16].toFloat();
			info.sale[0].second = list[17].toInt();

		    info.sale[1].first = list[18].toFloat();
			info.sale[1].second = list[19].toInt();

		    info.sale[2].first = list[20].toFloat();
			info.sale[2].second = list[21].toInt();

		    info.sale[3].first = list[22].toFloat();
			info.sale[3].second = list[23].toInt();

	 	    info.sale[4].first = list[24].toFloat();
			info.sale[4].second = list[25].toInt();
		}

		remain = remain.mid(remain.indexOf(":")-2);
		list = remain.mid(0,pos = remain.indexOf("\"")).split("|");
		foreach(QString item,list)
		{
			QStringList ss = item.replace("\\/",",").split(",");
			if(ss.size() == 6)
			{
			    DataWrapper6<QString,float,int,QString,int,int> wrap6;
				wrap6._1 = ss[0];
				wrap6._2 = ss[1].toFloat();
				wrap6._3 = ss[2].toInt();
				wrap6._4 = ss[3];
				wrap6._5 = ss[4].toInt();
				wrap6._6 = ss[5].toInt();
				info.details.push_back(wrap6);
			}
		}

		remain = remain.mid(pos+1);
		list = remain.mid(0,pos = remain.indexOf("]")).remove("\"").split(",");

		remain = remain.mid(pos+2);

		QString ending = remain.mid(pos = remain.indexOf("["));
		ending = ending.mid(ending.lastIndexOf("["));
		ending = ending.mid(ending.indexOf("\"")+1);
		ending = ending.mid(0,ending.indexOf("\""));
		list = ending.split("^");
		foreach(QString item,list)
		{
			QStringList segs = item.split("~");
			if(segs.size() == 4)
			{
			    DataWrapper4<float,int,int,float> wp;
				wp._1 = segs[0].toFloat();
				wp._2 = segs[1].toInt();
				wp._3 = segs[2].toInt();
				wp._4 = segs[3].toFloat();
				info.priceBars.push_back(wp);
			}
		}

		//remain = remain.mid(0,remain.lastIndexOf("["));
		int index = content.indexOf("\"market\"");//zjlx\":");
		if(index > 0)
		{
		    remain = content.mid(index+10);
			//remain = remain.mid(0,remain.indexOf("]"));
			QStringList items = remain.split("\",\"");

			info.stock.code = items[2];
			info.stock.current = items[3].toFloat();
			info.stock.privClose = items[4].toFloat();
			info.stock.open = items[5].toFloat();
			info.stock.tradeVolume = items[6].toFloat();
			info.stock.data.insert(Stock::TAG_WAIPAN,items[7]);
			info.stock.data.insert(Stock::TAG_NEIPAN,items[8]);
			info.stock.tradeCapital = items[37].toFloat();
			info.stock.turnover = items[38].toFloat();
			info.stock.top = items[41].toFloat();
			info.stock.low = items[42].toFloat();
			info.stock.swing = items[43].toFloat();
            
			info.stock.data.insert(Stock::TAG_LIUTONGSHIZHI,items[44]);
			info.stock.data.insert(Stock::TAG_ZONGSHIZHI,items[45]);
			info.stock.data.insert(Stock::TAG_PE_D,items[39]);
			info.stock.data.insert(Stock::TAG_PB,items[46]);

            QString bi = items[49];
			bi = bi.mid(0,bi.indexOf("\""));
			info.stock.data.insert(Stock::TAG_LIANGBI,bi);	
        }

		index = content.indexOf("zjlx\":[");
		if(index>0)
		{
			QStringList items = content.mid(index+7).remove('"').split(",");
			if(items.size() > 4)
			{
				info.capitcalFlow._1 = items[1].toDouble();
		        info.capitcalFlow._2 = items[2].toDouble();
				info.capitcalFlow._3 = items[3].toDouble();
		    }
		}
	}
	
	return info;
}

QString StockTuShareTimeDataInterface::buildLink(const QString& code)
{
    return QString();
}

StockRealTimeInfo StockTuShareTimeDataInterface::parseContent(const QString& content)
{
	StockRealTimeInfo info;
	info.low.first  = std::numeric_limits<float>::max();
	info.high.first = std::numeric_limits<float>::min();

	QStringList items = content.split("\n");
	std::reverse(items.begin(),items.end());

	double sum = 0.0;
	double vol = 0.0;

	int p1c = 0;
	int p2c = 0;
	float oldma = 0.0f;

	int posi = 0;

	StockTimeLine tick;
	tick.capital = 0;
	tick.ma = 0;
	tick.price = 0;
	tick.volume = 0;
	tick.time.setHMS(0,0,0,0);

	bool init = false;

	foreach(QString item,items)
	{
		QStringList ds = item.split(",");
		if(ds.size() == 7)
		{
			QString time = ds[1].mid(0,5);
			QTime curTime = QTime::fromString(time,"hh:mm");
			if(curTime.hour() > 12)
				curTime = curTime.addSecs(-5400+60);

			sum += ds[5].toInt();
			vol += ds[4].toInt();

			tick.price = ds[2].toFloat();

			if(tick.price < info.low.first)
			{
				info.low.first = tick.price;
				info.low.second = curTime;
			}
			else if(tick.price > info.high.first)
			{
				info.high.first = tick.price;
				info.high.second = curTime;
			}

			if(tick.time == curTime)
			{
			    tick.volume += ds[4].toInt();
				tick.price = ds[2].toFloat();
				tick.capital += ds[5].toInt();
			}
			else
			{
				if(init)
				{
					double m = sum/(double)vol;
					tick.ma = m*0.01;
					if(tick.price >= tick.ma)
						p1c ++;
					if(tick.ma >= oldma)
						p2c ++;
					info.stock.open = tick.price;
					info.list.push_back(tick);
				}

				init = true;
				oldma = tick.ma;
			    tick.time = curTime;
				tick.volume = ds[4].toInt();
				tick.price = ds[2].toFloat();
				tick.capital = ds[5].toInt();
				tick.ma = tick.price;
			}
		}
	}

	if(!tick.time.isNull())
	{
		tick.ma = sum/(double)vol*0.01;
		if(tick.price >= tick.ma)
			p1c ++;
		if(tick.ma >= oldma)
			p2c ++;
		info.list.push_back(tick);
	}

	float p1 = p1c/(float)info.list.size()*100.0f;
	float p2 = p2c/(float)info.list.size()*100.0f;
    info.stock.data.insert(Stock::TAG_P1,STR_CEIL(p1));
	info.stock.data.insert(Stock::TAG_P2,STR_CEIL(p2));
	info.stock.current = tick.price;
	info.stock.low = info.low.first;
	info.stock.top = info.high.first;
	info.flag = true;
	return info;
}

QVector<Stock> convertToTimeLine(const StockRealTimeInfo& info,const QString& date,int minute)
{
	auto list = info.list;
	QVector<Stock> output;
	if(list.isEmpty())
		return output;

	Stock current;
	//StockTimeLine prev;
#define INIT_CURRENT(value)\
	current.open = current.low = current.top = value;\
	current.tradeVolume = 0;\
    current.date = date;

	INIT_CURRENT(info.list[0].price)

	QTime flag(9,30);
	flag = flag.addSecs(minute*60);
	QTime am(11,30);
	current.time = flag.toString("HH:mm");
	bool timeflag = false;
	
	float prevClose = current.open;
	int pos = 0;
	foreach(auto item,info.list)
	{
		if(item.time < flag)
		{
			current.tradeVolume += item.volume;
			current.current = item.price;
			current.low = std::min<float>(current.low,current.current);
			current.top = std::max<float>(current.top,current.current);
		}
		else
		{
			current.tradeVolume += item.volume;
			current.current = item.price;
			current.low = std::min<float>(current.low,current.current);
			current.top = std::max<float>(current.top,current.current);
			current.privClose = prevClose;
			output.push_back(current);
			INIT_CURRENT(item.price)

			if(item.time >= am && !timeflag)
			{
				flag = flag.addSecs(150*60);
				timeflag = true;
				break;
			}
			flag = flag.addSecs(minute*60);
			current.time = flag.toString("HH:mm");
		}

		prevClose = item.price;
		pos ++;
	}

	flag.setHMS(13,minute,0);

	for(pos=pos+1;pos < info.list.size();pos++)
	{
		auto item = info.list.at(pos);
		item.time = item.time.addSecs(89*60);
	    if(item.time < flag)
		{
			current.tradeVolume += item.volume;
			current.current = item.price;
			current.low = std::min<float>(current.low,current.current);
			current.top = std::max<float>(current.top,current.current);
		}
		else
		{
			current.tradeVolume += item.volume;
			current.time = flag.toString("HH:mm");
			current.current = item.price;
			current.low = std::min<float>(current.low,current.current);
			current.top = std::max<float>(current.top,current.current);
			current.privClose = prevClose;
			output.push_back(current);
			INIT_CURRENT(item.price)

			flag = flag.addSecs(minute*60);
		}

		prevClose = item.price;
	}

	return output;
}