#include <cassert>
#include "math.h"
#include "ArraySolver.h"

template<class T>
QVector<T> copyListWithCountN(const T& input, int n)
{
	QVector<T> output;
	output += input;

	n--;
	if (n > 1)
	{
		for (int i = 0; i<n; i++)
			output += input;
	}

	return output;
}

QString replaceStringKeyByalue(const QString& input, const QVector<int>& ref, const QStringList& keys, const QVector<QStringList>& values)
{
	QString output(input);
	for (int i = 0; i < ref.size(); i++)
		output = output.replace(keys[i], values[i][ref[i]]);
	return output;
}

QStringList replaceStringListWithKeyByValue(const QStringList& input, const QVector<int>& ref, const QStringList& keys, const QVector<QStringList>& values)
{
	QStringList output;
	foreach(auto item, input)
		output += replaceStringKeyByalue(item, ref, keys, values);

	return output;
}

QVector<QVector<int>> getArrayRef(const QStringList& keys, const QVector<QStringList>& values)
{
	assert(keys.size() == values.size());

	ArraySolver solver;
	foreach(auto item, values)
	    solver.update(item.size());
	
	return solver.get();
}

QVector<QStringList> fissionStringList(const QStringList& input, const QStringList& keys, const QVector<QStringList>& values, const QVector<QVector<int>>& ref)
{
	QVector<QStringList> output;
	if (keys.isEmpty())
	{
		output += input;
		return output;
	}

	output = copyListWithCountN<QStringList>(input, ref.size());
	assert(output.size() == ref.size());

	for (int i = 0; i<ref.size(); i++)
		output[i] = replaceStringListWithKeyByValue(output[i], ref[i], keys, values);

	return output;
}

QVector<QStringList> generateStringListAndReplaceKeyByValue(const QVector<QStringList>& list, const QStringList& keys, const QVector<QStringList>& values)
{
    assert(keys.size() == values.size());
	auto ref = getArrayRef(keys,values);

	QVector<QStringList> output;
	foreach(auto item, list)
	{
		output += fissionStringList(item, keys, values, ref);
	}
	return output;
}