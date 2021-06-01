#ifndef MATH_ARRAY_SOLVER_H
#define MATH_ARRAY_SOLVER_H
#include <qvector>
 
class ArraySolver
{
public:
	void update(int n);
	auto get()const->QVector<QVector<int>>
	{
		return list;
	}
private:
	QVector<QVector<int>> list;
};

#endif

