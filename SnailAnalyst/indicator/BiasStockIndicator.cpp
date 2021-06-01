#include "indicator.h"

class BiasIndicator : public StockIndicator
{
public:
	BiasIndicator();
	~BiasIndicator();

	void setPara(int p1,int p2,int p3)
	{
		if (p1 < 300)
			p1 = 500;
		maIndicator->setPara(p1,p2,p3);
	}
	void clear();
	DataWrapper5<bool,float,float,float,float> update(const Stock& stock);
private:
	StockIndicator* maIndicator;
	bool init;
	static ObjectRegister<StockIndicator,BiasIndicator,QString> dummy;
};

ObjectRegister<StockIndicator,BiasIndicator,QString> BiasIndicator::dummy("bias");

BiasIndicator::BiasIndicator()
{
	maIndicator = ObjectFactory<StockIndicator,QString>::create("ma");
	init = false;
}

BiasIndicator::~BiasIndicator()
{
    delete maIndicator;
}

void BiasIndicator::clear()
{
    maIndicator->clear();
}

DataWrapper5<bool,float,float,float,float> BiasIndicator::update(const Stock& stock)
{
	DataWrapper5<bool,float,float,float,float> ret = maIndicator->update(stock);
	if(ret._1)// && init == true)
		ret._2 = (stock.current - ret._2)/ret._2*100.0;
	return ret;
}
