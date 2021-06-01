#include <QRegexp>
#include "SnailContext.h"
#include "QuantStatement.h"
#include <iostream>

struct QuantComparator
{
	virtual ~QuantComparator(){}
	virtual bool compare(const QVector<QuantStatement*>& input,int pos) = 0;
};

struct QuantComparator_equal : public QuantComparator
{
	bool compare(const QVector<QuantStatement*>& input,int pos)
	{
		if (input.size() != 2)
			return false;

		auto d1 = input[0];
		auto d2 = input[1];

		QVariant t1,t2;
		if (d1->evalute(pos,t1) && d2->evalute(pos,t2))
		{
			if(t1.type() == QVariant::Double && t2.type() == QVariant::Double)
			if (std::fabs(t1.toDouble() - t2.toDouble()) < 0.001)
				return true;
		}

		return false;
	}
};

struct QuantComparator_greater : public QuantComparator
{
	bool compare(const QVector<QuantStatement*>& input,int pos)
	{
		if (input.size() != 2)
			return false;

		auto d1 = input[0];
		auto d2 = input[1];

		QVariant t1,t2;
		if (d1->evalute(pos,t1) && d2->evalute(pos,t2))
		{
			if (t1.type() == QVariant::Double && t2.type() == QVariant::Double 
				&& t1.toDouble() > t2.toDouble())
			{
				std::cout << t1.toDouble() << ">" << t2.toDouble() << std::endl;
				return true;
			}
		}

		return false;
	}
};

struct QuantComparator_less : public QuantComparator
{
	bool compare(const QVector<QuantStatement*>& input,int pos)
	{
		if (input.size() != 2)
			return false;

		auto d1 = input[0];
		auto d2 = input[1];

		QVariant t1,t2;
		if (d1->evalute(pos,t1) && d2->evalute(pos,t2))
		{
			if (t1.type() == QVariant::Double && t2.type() == QVariant::Double)
				if (t1.toDouble() < t2.toDouble())
					return true;
		}

		return false;
	}
};

struct QuantComparator_lessOrEqual : public QuantComparator
{
	bool compare(const QVector<QuantStatement*>& input,int pos)
	{
		if (input.size() != 2)
			return false;

		auto d1 = input[0];
		auto d2 = input[1];

		QVariant t1,t2;
		if (d1->evalute(pos,t1) && d2->evalute(pos,t2))
		{
			if (t1.type() == QVariant::Double && t2.type() == QVariant::Double)
				if (t1.toDouble() <= t2.toDouble())
					return true;
		}

		return false;
	}
};

struct QuantComparator_greaterOrEqual : public QuantComparator
{
	bool compare(const QVector<QuantStatement*>& input,int pos)
	{
		if (input.size() != 2)
			return false;

		auto d1 = input[0];
		auto d2 = input[1];

		QVariant t1,t2;
		if (d1->evalute(pos,t1) && d2->evalute(pos,t2))
		{
			if (t1.type() == QVariant::Double && t2.type() == QVariant::Double)
				if (t1.toDouble() >= t2.toDouble())
					return true;
		}

		return false;
	}
};

struct QuantComparator_or : public QuantComparator
{
	bool compare(const QVector<QuantStatement*>& input,int pos)
	{
		foreach(auto meta,input)
		{
			QVariant ret;
			if (meta->evalute(pos,ret))
			{
				if (ret.type() == QVariant::Bool && ret.toBool())
					return true;
			}
		}
		return false;
	}
};

struct  QuantComparator_Boolean : public  QuantComparator
{
	bool compare(const QVector<QuantStatement*>& input,int pos)
	{
		if (input.size() != 1)
			return false;

		QVariant ret;
		if (input[0]->evalute(pos,ret))
			return ret.type() == QVariant::Bool && ret.toBool();
		return false;
	}
};

QStringList QuantStatementObject::splitStatement(const QString& statement,const QString& regex)
{
	QStringList output;
	QRegExp rx(regex);
	int index = rx.indexIn(statement);
	if (index >= 0)
	{
		int count = rx.captureCount();
		for (int i = 1;i < count+1;i++)
			output.append(rx.cap(i));
	}
	else
		output.append(statement);
	return output;
}

QuantStatementObject::QuantStatementObject()
{
	comparator = nullptr;
}

QuantStatementObject::~QuantStatementObject()
{
	if (comparator)
		delete comparator;
	comparator = nullptr;

	foreach(auto ptr,objects)
		delete ptr;
	objects.clear();
}

bool QuantStatementObject::build(const QString& statement)
{
	auto list = splitStatement(statement,"(.*)>=(.*)");
	if (list.size() == 2)
	{
		auto ptr1 = SnailContext::open()->getQuantFactory()->create(list[0]);
		auto ptr2 = SnailContext::open()->getQuantFactory()->create(list[1]);
		if (ptr1 == nullptr || ptr2 == nullptr)
		{
		    DELETE_PTR(ptr1)
			DELETE_PTR(ptr2)
			return false;
	    }
		comparator = new QuantComparator_greaterOrEqual();
		objects.push_back(ptr1);
		objects.push_back(ptr2);
		return true;
	}

	list = splitStatement(statement,"(.*)<=(.*)");
	if (list.size() == 2)
	{
		auto ptr1 = SnailContext::open()->getQuantFactory()->create(list[0]);
		auto ptr2 = SnailContext::open()->getQuantFactory()->create(list[1]);
		if (ptr1 == nullptr || ptr2 == nullptr)
		{
			DELETE_PTR(ptr1)
			DELETE_PTR(ptr2)
			return false;
		}
		comparator = new QuantComparator_lessOrEqual();
		objects.push_back(ptr1);
		objects.push_back(ptr2);
		return true;
	}

	list = splitStatement(statement,"(.*)=(.*)");
	if (list.size() == 2)
	{
		auto ptr1 = SnailContext::open()->getQuantFactory()->create(list[0]);
		auto ptr2 = SnailContext::open()->getQuantFactory()->create(list[1]);
		if (ptr1 == nullptr || ptr2 == nullptr)
		{
			DELETE_PTR(ptr1)
			DELETE_PTR(ptr2)
			return false;
		}
		comparator = new QuantComparator_equal();
		objects.push_back(ptr1);
		objects.push_back(ptr2);
		return true;
	}

	list = splitStatement(statement,"(.*)>(.*)");
	if (list.size() == 2)
	{
		auto ptr1 = SnailContext::open()->getQuantFactory()->create(list[0]);
		auto ptr2 = SnailContext::open()->getQuantFactory()->create(list[1]);
		
		if (ptr1 == nullptr || ptr2 == nullptr)
		{
			DELETE_PTR(ptr1)
			DELETE_PTR(ptr2)
			return false;
		}
		comparator = new QuantComparator_greater();
		objects.push_back(ptr1);
		objects.push_back(ptr2);
		return true;
	}

	list = splitStatement(statement,"(.*)<(.*)");
	if (list.size() == 2)
	{
		auto ptr1 = SnailContext::open()->getQuantFactory()->create(list[0]);
		auto ptr2 = SnailContext::open()->getQuantFactory()->create(list[1]);
		if (ptr1 == nullptr || ptr2 == nullptr)
		{
			DELETE_PTR(ptr1)
			DELETE_PTR(ptr2)
			return false;
		}
		comparator = new QuantComparator_less();
		objects.push_back(ptr1);
		objects.push_back(ptr2);
		return true;
	}

	std::cout << statement.toLocal8Bit().data()<< std::endl;
	auto ptr = SnailContext::open()->getQuantFactory()->create(statement);
	if (ptr == nullptr)
		return false;

	objects.push_back(ptr);
	comparator = new QuantComparator_Boolean();
	return true;
}

void QuantStatementObject::clear()
{
	foreach(auto ptr,objects)
		ptr->clear();
}

bool QuantStatementObject::evalute(int pos)
{
	return comparator->compare(objects,pos);
	//int ret = comparator->compare(objects,pos);
	//return 0;//ret;
}

QuantStatementObject_OR::~QuantStatementObject_OR()
{
	foreach(auto itr,list)
		delete itr;
	list.clear();
}

bool QuantStatementObject_OR::build(const QString& statement)
{
	QStringList items = statement.split("|");
	foreach(QString item,items)
	{
		auto* ptr = new QuantStatementObject();
		if (ptr->build(item))
			list.push_back(ptr);
		else
		{
			delete ptr;
			return false;
		}
	}
	return true;
}

void QuantStatementObject_OR::clear()
{
	foreach(auto itr,list)
		itr->clear();
}

bool QuantStatementObject_OR::evalute(int pos)
{
	foreach(auto itr,list)
		if (itr->evalute(pos))
			return true;
	return false;
}