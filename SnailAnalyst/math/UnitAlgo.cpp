#include "UnitAlgo.h"
#include "../SnailContext.h"

int getPreJC(int current,const DataWrapper5s& input,int offset1,int offset2,bool& upJC)
{
    for(int i=current;i>0;i--)
    {
        auto node = input[i];
        auto prev = input[i-1];
		if(!node.getValueByIndex<bool>(1))
			return -1;
		if(node.getValueByIndex<float>(offset1) < node.getValueByIndex<float>(offset2))
			return -1;
        if(node.getValueByIndex<float>(offset1) > node.getValueByIndex<float>(offset2) &&
           prev.getValueByIndex<float>(offset1) < prev.getValueByIndex<float>(offset2))
        {
            if(node.getValueByIndex<float>(offset1) > 0)
                upJC = true;
            else
                upJC = false;
            return i;
        }
    }
    return -1;
}

//计算上升趋势最终索引,不是上升趋势则返回负数
int isUpward(int current,const QVector<Stock>& stocks,int offset,const DataWrapper5s& arguments)
{
    DataWrapper5s input = arguments;
    auto prev = input[current].getValueByIndex<float>(offset);
    for(int i = current-1;i>0;i--)
    {
        if(!input[i].getValueByIndex<bool>(1))
            return -1;
        float value = input[i].getValueByIndex<float>(offset);
        if(value >= prev)
            return i;
        prev = value;
    }
    return -1;
}

//计算下降趋势最终索引,不是下降趋势则返回负数
int isDownward(int current,const QVector<Stock>& stocks,int offset,const DataWrapper5s& arguments)
{
    DataWrapper5s input = arguments;
    auto prev = input[current].getValueByIndex<float>(offset);
    for(int i = current-1;i>0;i--)
    {
        if(!input[i].getValueByIndex<bool>(1))
            return -1;      
        float value = input[i].getValueByIndex<float>(offset);
        if(value <= prev)
            return i;
        prev = value;
    }
    return -1;
}

bool isFitness_MACD_FSXS(int pos,const QVector<Stock>& stocks,const DataWrapper5s& arguments)
{
    if(stocks.size() != arguments.size())
        return false;
    if(pos < 30)
        return false;

    int current = pos-1;

    bool jxOver0 = false;
    QVariant var;
    bool ok = SnailContext::open()->getConfigFactory()->getVar(QString("config.unit.macd.佛手向上.线上金叉"),var);
	if(ok)
	    jxOver0 = var.toBool();

    int upMinCount = 3;
	int upMaxCount = 36;
    ok = SnailContext::open()->getConfigFactory()->getVar(QString("config.unit.macd.佛手向上.上行最小间隔"),var);
	if(ok)
		upMinCount = var.toInt();

    ok = SnailContext::open()->getConfigFactory()->getVar(QString("config.unit.macd.佛手向上.上行最大间隔"),var);
    if(ok)
		upMaxCount = var.toInt();

    int downMinCount = 3;
	int downMaxCount = 36;
    ok = SnailContext::open()->getConfigFactory()->getVar(QString("config.unit.macd.佛手向上.下行最小间隔"),var);
    if(ok)
		downMinCount = var.toInt();

    ok = SnailContext::open()->getConfigFactory()->getVar(QString("config.unit.macd.佛手向上.下行最大间隔"),var);
    if(ok)
		downMaxCount = var.toInt(); 

    float ratio = 0.8f;
    ok = SnailContext::open()->getConfigFactory()->getVar(QString("config.unit.macd.佛手向上.回调最小倍率"),var);
    if(ok)
		ratio = var.toFloat();

    bool diffContinueUp = true;
    ok = SnailContext::open()->getConfigFactory()->getVar(QString("config.unit.macd.佛手向上.慢线持续上行"),var);
	if(ok)
		diffContinueUp = var.toBool();

    bool stdShape = true;
    ok = SnailContext::open()->getConfigFactory()->getVar(QString("config.unit.macd.佛手向上.标准形态"),var);
    if(ok)
		stdShape = var.toBool();

    DataWrapper5s inputArugments = arguments;
    ok = inputArugments[current-1].getValueByIndex<bool>(1);
    if(!ok)
        return false;

    float bar = inputArugments[current].getValueByIndex<float>(2);
    float minBar = inputArugments[current-1].getValueByIndex<float>(2);
    if(bar <= minBar || minBar < 0.0f)
        return false;

    int maxBarPos = isDownward(current-1,stocks,2,arguments);
    if(maxBarPos < 0)
        return false;
    if(current-1-maxBarPos < downMinCount)
        return false;
	if(current-1-maxBarPos > downMaxCount)
        return false;

    float maxBar = inputArugments[maxBarPos].getValueByIndex<float>(2);
    if(minBar/maxBar > ratio)
        return false;

    int minBarPos = isUpward(maxBarPos,stocks,2,arguments);
    if(minBarPos < 0)
        return false;
    if(maxBarPos-minBarPos < upMinCount)
        return false;
    if(maxBarPos-minBarPos > upMaxCount)
        return false;

    if(diffContinueUp)
    {
        int deaUpMinCount = isUpward(current-1,stocks,4,arguments);
        if(deaUpMinCount < minBarPos)
            return false;
    }

    auto minBarNode = arguments[minBarPos];
    if(minBarNode.getValueByIndex<float>(2) < 0.0f)
        return false;

    auto minBarNodePrev = arguments[minBarPos-1];
    if(!minBarNodePrev.getValueByIndex<bool>(1))
        return false;

    bool up = false;
    int jcpos = getPreJC(current,arguments,3,4,up);
	
	Stock stock= stocks[jcpos];
    if(jcpos > 0 && jxOver0 && up)
        return true;
	else if(jxOver0)
		return false;

    return true;
}