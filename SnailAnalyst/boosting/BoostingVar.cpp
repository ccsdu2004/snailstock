#include "BoostingParser.h"

#ifdef SUPPORT_BOOSTING_ALGO

BoostingVar::~BoostingVar()
{
}

struct BoostingVar_list : public BoostingVar
{
	bool evalute(const QString& key,const QString& value)
	{
		values = value.split(";");
		name = key;
		return !values.empty();
	}
};

struct BoostingVar_for : public BoostingVar
{
	bool evalute(const QString& key,const QString& value)
	{
		values.clear();
		auto list = value.split(";");
		if (list.size() != 3)
			return false;

		double start = list.at(0).toDouble();
		double end = list.at(1).toDouble();
		double steps = list.at(2).toDouble();

		if (steps < 0.001 || start >= end)
			return false;

		for (;start <= end;start += steps)
			values += QString("%1").arg(start);

		if (values.size() > 100)
			return false;

		name = key;
		return !values.empty();
	}
};

BoostingVar* BoostingVar::create(const QString& tag)
{
	if (tag == "list")
		return new BoostingVar_list();
	else if (tag == "for")
		return new BoostingVar_for();
	return nullptr;
}

#endif