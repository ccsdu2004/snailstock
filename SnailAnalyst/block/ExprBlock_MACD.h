#ifndef EXPR_BLOCK_MACD_H
#define EXPR_BLOCK_MACD_H
#include "../ExprBlock.h"
#include "../Indicator.h"
#include "../DataProvider.h"

class ExprBlock_MACD : public ExprBlock
{
	QString MACD_DIFF;
	QString MACD_DEA;
	QString MACD_BAR;
public:	
    ExprBlock_MACD(ExprBlock::SpanMode spanmode)
    {
		MACD_DIFF = QString("macd.diff");
	    MACD_DEA  = QString("macd.dea");
	    MACD_BAR  = QString("macd.bar");

    	mode = spanmode;
    	a = 12;
    	b = 26;
    	c = 9;
		offset = 0;
    	type = MACD_DIFF;
    }

    ~ExprBlock_MACD()
    {
    }
	
	const QStringList tag()const
	{
        return QString("macd.diff|macd.dea|macd.bar").split("|");
	}

	const QString getLabel()const{return label;}
	
	bool returnBool()const{return false;}

	//input:macd.bar,12,26,9
	bool prepare(const QString& expr) 
	{
	    QStringList items = expr.split(",");
	    if(items.size() != 5 || items.size() != 2)
	    	return false;
	    if(items.size() == 5)
	    {
	    	a = items[1].toInt();
	    	b = items[2].toInt();
	    	c = items[3].toInt();
	    	if(a < 5 || a > b - 2 || c < 4)
	    		return false;

			offset = evalute_express(items[4]);
			if(offset > 0)
				return false;
	    }
		else if(items.size() == 2)
		{
			offset = evalute_express(items[1]);
			if(offset > 0)
				return false;
		}

	    if(!tag().contains(items[0]))
	    	return false;

	    label = QString("macd,%1,%2,%3").arg(a).arg(b).arg(c);
	    return true;
	}

	int  evalate(int pos,int offset,QVariant& result,const QString& tag = QString())
	{
		int current = pos+offset;
		if(current <= 0 || current >= data.size())
			return -1;
		auto item = data[current];
		if(!item._1)
			return -1;

        QString input = tag;
		if(input.isEmpty())
			input = MACD_DIFF;
        
        if(input == MACD_DIFF)
        	result.setValue<double>(item._2);
        else if(input == MACD_DEA)
        	result.setValue<double>(item._3);
        else if(input == MACD_BAR)
        	result.setValue<double>(item._4);
        else
        	return -1;
		return 1;
	}

	bool loadData()
	{
    	data.clear();
		MacdIndicator indicator(a,b,c);
		if(mode == ExprBlock::normal)
		{
		    foreach(auto stock,dataProvider.stocks)
				data.push_back(indicator.update(stock));
	    }
	}
	void clear(){data.clear();}
private:
	SpanMode mode;
	int a,b,c,offset;
	QString type;
	QString label;
    QVector<DataWrapper5<bool,float,float,float,float>> data;	
};

#endif