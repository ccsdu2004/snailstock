#include "ArraySolver.h"

void ArraySolver::update(int n)
{
	if (list.empty())
	{
		for (int i = 0; i<n; i++)
		{
			QVector<int> item;
			item += i;
			list.push_back(item);
		}
	}
	else
	{
		decltype(list) tmp;
		foreach(auto item, list)
		{
			for (int i = 0; i < n; i++)
				tmp += item;
		}

		list = std::move(tmp);

		int i = 0;
		auto itr = list.begin();
		while (itr != list.end())
		{
			itr->push_back(i++%n);
			itr++;
		}
	}
}