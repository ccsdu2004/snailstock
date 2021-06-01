#ifndef SNAIL_FACTORY_H
#define SNAIL_FACTORY_H
#include <QString>
#include <QList>

template<class Product>
class Factory
{
public:
	virtual ~Factory(){}
	virtual Product* create(const QString tag) = 0;
};

template<class Product,class Factory>
class SnailFactory
{
public:
	~SnailFactory()
	{
		foreach(auto item, factories)
			delete item;
		factories.clear();
	}

	void registerFactory(Factory* factory)
	{
		factories.push_back(factory);
	}

	Product* create(const QString name)
	{
		Product* ptr = nullptr;
		foreach(auto item, factories)
		{
			ptr = item->create(name);
			if (ptr)
				return ptr;
	    }
		return ptr;
	}
private:
	QVector<Factory*> factories;
};


#endif