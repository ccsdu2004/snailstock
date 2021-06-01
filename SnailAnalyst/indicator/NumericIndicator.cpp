#include <QVector>
#include "Indicator.h"

class NumericIndicator_MA : public NumericIndicator
{
public:
	NumericIndicator_MA(int in,QObject* parent):
	  NumericIndicator(parent),
	  n(in)
	{
	}

	QPair<bool,double> update(double value)
	{
	    data.append(value);
        if(data.size() < n)
	        return QPair<bool,double>(false,0.0);
		if(data.size() > n)
		    data.remove(0);

	    double sum = 0.0;
		std::for_each(data.begin(),data.end(),[&](double v){sum += v;});
        return QPair<bool,double>(true,sum/(double)n);
	}

    void clear()
	{
	    data.clear();
	}
private:
	int n;
	QVector<double> data;
};

class NumericIndicator_EMA : public NumericIndicator
{
public:
	NumericIndicator_EMA(int in,QObject* parent):
	  NumericIndicator(parent),
	  n(in),
	  ema(0.0)
	{
	}

	QPair<bool,double> update(double value)
	{
	    data.append(value);
        if(data.size() < n)
		{
			ema = value;
	        return QPair<bool,double>(false,0.0);
		}
		if(data.size() > n)
		    data.remove(0);

		double alpha = 2.0/(1.0+n);
	    ema = alpha*value + (1.0f-alpha)*ema;
        return QPair<bool,double>(true,ema);
	}

    void clear()
	{
	    data.clear();
	}
private:
	int n;
	double ema;
	QVector<double> data;
};

NumericIndicator* NumericIndicator::createIndicator(Type type,int n,QObject* parent)
{
	if(type == ma)
		return new NumericIndicator_MA(n,parent);
	else if(type == ema)
		return new NumericIndicator_EMA(n,parent);
	return nullptr;
}

NumericIndicator::NumericIndicator(QObject* parent):
QObject(parent)
{
}