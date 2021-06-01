#include "StatementObject.h"
#include "MetaEvalutor.h"
#include "script/ScriptManager.h"
#include "StockScriptEngine.h"

struct Comparator_equal : public Comparator
{
	bool compare(const QVector<MetaEvalutor*>& input,int pos,QVector<float>& arguments)
	{
		if (input.size() != 2)
			return false;

		auto d1 = input[0];
		auto d2 = input[1];

		ResultType t1,t2;
		if (d1->evalute(pos,t1) && d2->evalute(pos,t2))
		{
			arguments << t1.double_ << t2.double_;
			if (std::fabs(t1.double_ - t2.double_) < 0.001)
				return true;
			else
				return false;
		}

		arguments << t1.double_ << t2.double_;
		return false;
	}
};

struct Comparator_Greater2 : public Comparator
{
	bool compare(const QVector<MetaEvalutor*>& input,int pos,QVector<float>& arguments)
	{
		if (input.size() != 2)
			return false;

		auto d1 = input[0];
		auto d2 = input[1];

		ResultType t1,t2;
		if (d1->evalute(pos,t1) && d2->evalute(pos,t2))
		{
			arguments << t1.double_ << t2.double_;
			if (t1.double_ > t2.double_)
				return true;
			else
				return false;
		}
		arguments << t1.double_ << t2.double_;
		return false;
	}
};

struct Comparator_GreaterOrEqual2 : public Comparator
{
	bool compare(const QVector<MetaEvalutor*>& input,int pos,QVector<float>& arguments)
	{
		if (input.size() != 2)
			return false;

		auto d1 = input[0];
		auto d2 = input[1];

		ResultType t1,t2;
		if (d1->evalute(pos,t1) && d2->evalute(pos,t2))
		{
			arguments << t1.double_ << t2.double_;
			if (t1.double_ >= t2.double_)
				return true;
			return false;
		}

		arguments << t1.double_ << t2.double_;
		return false;
	}
};

struct Comparator_Less2 : public Comparator
{
	bool compare(const QVector<MetaEvalutor*>& input,int pos,QVector<float>& arguments)
	{
		if (input.size() != 2)
			return false;

		auto d1 = input[0];
		auto d2 = input[1];

		ResultType t1,t2;
		if (d1->evalute(pos,t1) && d2->evalute(pos,t2))
		{
			arguments << t1.double_ << t2.double_;
			if (t1.double_ < t2.double_)
				return true;
			return false;
		}

		arguments << t1.double_ << t2.double_;
		return false;
	}
};

struct Comparator_LessOrEqual2 : public Comparator
{
	bool compare(const QVector<MetaEvalutor*>& input,int pos,QVector<float>& arguments)
	{
		if (input.size() != 2)
			return false;

		auto d1 = input[0];
		auto d2 = input[1];

		ResultType t1,t2;
		if (d1->evalute(pos,t1) && d2->evalute(pos,t2))
		{
			arguments << t1.double_ << t2.double_;
			if (t1.double_ <= t2.double_)
				return true;
			return false;
		}

		arguments << t1.double_ << t2.double_;
		return false;
	}
};

struct Comparator_Or2 : public Comparator
{
	bool compare(const QVector<MetaEvalutor*>& input,int pos,QVector<float>& arguments)
	{
		foreach(auto meta,input)
		{
			ResultType ret;
			if (meta->evalute(pos,ret))
			{
				arguments << 1.0;
				return true;
		    }
			return false;
 	    }
        
		arguments << 0.0;
		return false;
	}
};

struct Comparator_Boolean : public Comparator
{
	bool compare(const QVector<MetaEvalutor*>& input,int pos,QVector<float>& arguments)
	{
		if (input.size() != 1)
			return false;

		ResultType ret;
		if (input[0]->evalute(pos,ret))
		{
			if (ret.type == ResultType::TYPE_BOOL && ret.bool_)
			{
				arguments << 1;
				return true;
			}
			else
			{
				arguments << -1;
				return false;
			}
	    }
		arguments << -999999.99;
		return false;
	}
};

StatementObjectNode::~StatementObjectNode()
{
    foreach(auto itr,list)
		delete itr;
	list.clear();
}

bool StatementObjectNode::build(const QString& statement)
{
	QStringList items = statement.split("|");
	foreach(QString item,items)
	{
	    StatementObject* ptr = new StatementObject();
		if(ptr->build(item))
			list.push_back(ptr);
		else
		{
			delete ptr;
			return false;
		}
	}
	return true;
}

void StatementObjectNode::clear()
{
    foreach(auto itr,list)
		itr->clear();
}

void StatementObjectNode::loadData(const QVector<Stock>& stocks)
{
    foreach(auto itr,list)
		itr->loadData(stocks);
}

bool StatementObjectNode::check(int pos,QVector<float>& arguments)
{
	arguments.clear();
	foreach(auto itr,list)
		if (itr->check(pos,arguments))
			return true;
	return false;
}

StatementObject::StatementObject()
{
    comparator = nullptr;
}

StatementObject::~StatementObject()
{
    if(comparator)
		delete comparator;
	comparator = nullptr;

	foreach(auto ptr,objects)
		delete ptr;
	objects.clear();
}

bool StatementObject::build(const QString& statement)
{
	QStringList list;

#define BUILD_OBJECT(str,flag)\
	list = MetaEvalutor::splitStatement(statement,str);\
	if (list.size() == 2)\
	{\
        auto node1 = MetaEvalutor::splitArgument(list[0]);\
	    auto node2 = MetaEvalutor::splitArgument(list[1]);\
        StockScriptList::GetInstance().getScriptEngine()->appendArgument(list[0]);\
        StockScriptList::GetInstance().getScriptEngine()->appendArgument(list[1]);\
		MetaEvalutor* ptr1 = MetaEvalutor::create(node1._2,node1._1);\
		MetaEvalutor* ptr2 = MetaEvalutor::create(node2._2,node2._1);\
		if (ptr1 == nullptr || ptr2 == nullptr)\
		{\
			DELETE_PTR(ptr1)\
			DELETE_PTR(ptr2)\
			return false;\
		}\
		if (!ptr1->build(node1._2,node1._3))\
		{\
			DELETE_PTR(ptr1)\
			DELETE_PTR(ptr2)\
			return false;\
		}\
		if (!ptr2->build(node2._2,node2._3))\
		{\
			DELETE_PTR(ptr1)\
			DELETE_PTR(ptr2)\
			return false;\
		}\
        ptr1->setOptimizeStringKey(list[0]);\
        ptr2->setOptimizeStringKey(list[1]);\
        objects.push_back(ptr1);\
        objects.push_back(ptr2);\
        comparator = new flag();\
        return true;\
    }

#ifdef SUPPORT_BASIC
	BUILD_OBJECT("(.*)>=(.*)",Comparator_GreaterOrEqual2)
	BUILD_OBJECT("(.*)<=(.*)",Comparator_LessOrEqual2)
	BUILD_OBJECT("(.*)=(.*)",Comparator_equal)
#endif

	BUILD_OBJECT("(.*)>(.*)",Comparator_Greater2)
	BUILD_OBJECT("(.*)<(.*)",Comparator_Less2)

	auto node = MetaEvalutor::splitArgument(statement);
	StockScriptList::GetInstance().getScriptEngine()->appendArgument(statement);

#ifndef SUPPORT_BIG_SPAN
	if (node._1 == MetaEvalutor::day5 ||
		node._1 == MetaEvalutor::day10 ||
		node._1 == MetaEvalutor::day20)
		return false;
#else

#endif

#ifndef SUPPORT_TICKET
	if (node._1 == MetaEvalutor::minute5 ||
		node._1 == MetaEvalutor::minute10 ||
		node._1 == MetaEvalutor::minute15 ||
		node._1 == MetaEvalutor::minute30 ||
		node._1 == MetaEvalutor::minute60)
		return false;
#else

#endif

	MetaEvalutor* ptr = MetaEvalutor::create(node._2,node._1);
	if (ptr == nullptr)
		return false;

	if (!ptr->build(node._2,node._3))
	{
		DELETE_PTR(ptr);
		return false;
	}

	ptr->setOptimizeStringKey(statement);\
	objects.push_back(ptr);
	comparator = new Comparator_Boolean();
	return true;
}

void StatementObject::clear()
{
	foreach(auto ptr,objects)
		ptr->clear();
}
	
void StatementObject::loadData(const QVector<Stock>& stocks)
{
	foreach(auto ptr,objects)
		ptr->loadData();
}

bool StatementObject::check(int pos,QVector<float>& arguments)
{
	arguments.clear();
	return comparator->compare(objects,pos,arguments);
}