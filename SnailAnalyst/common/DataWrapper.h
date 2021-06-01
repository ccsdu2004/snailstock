#ifndef DATA_WRAPPER_H
#define DATA_WRAPPER_H
#include <QString>
#include <QVector>

template<class A,class B>
struct DataWrapper2
{
	A _1;
	B _2;

	DataWrapper2(){}
	DataWrapper2(A a,B b):_1(a),_2(b){}

	DataWrapper2<QString,QString> toString();
};

template<class A,class B>
DataWrapper2<QString,QString> DataWrapper2<A,B>::toString()
{
	DataWrapper2<QString,QString> wrapper;
	wrapper._1 = QString("%1").arg(_1);
	wrapper._2 = QString("%1").arg(_2);
	return wrapper;
}

template<class A,class B,class C>
struct DataWrapper3
{
	A _1;
	B _2;
	C _3;
	DataWrapper3(){}
	DataWrapper3(A a,B b,C c):
		_1(a),_2(b),_3(c)
	{
	}
};

template<class A,class B,class C,class D>
struct DataWrapper4
{
	A _1;
	B _2;
	C _3;
	D _4;

	DataWrapper4(){}
	DataWrapper4(A a,B b,C c,D d):
		_1(a),_2(b),_3(c),_4(d)
	{
	}
};

template<class A,class B,class C,class D,class E>
struct DataWrapper5
{
	DataWrapper5(){}
	DataWrapper5(A a,B b,C c,D d,E e):_1(a),_2(b),_3(c),_4(d),_5(e){}
	A _1;
	B _2;
	C _3;
	D _4;
	E _5;

	template<class T>
	T getValueByIndex(int offset)
	{
		if (offset < 1)
			offset = 1;
		else if (offset > 5)
			offset = 5;

		if (offset == 1)
			return _1;
		else if (offset == 2)
			return _2;
		else if (offset == 3)
			return _3;
		else if (offset == 4)
			return _4;
		return _5;
	}
};

template<class A,class B,class C,class D,class E,class F>
struct DataWrapper6
{
	A _1;
	B _2;
	C _3;
	D _4;
	E _5;
	F _6;
};

template<class A,class B,class C,class D,class E,class F,class G>
struct DataWrapper7
{
	A _1;
	B _2;
	C _3;
	D _4;
	E _5;
	F _6;
	G _7;
};

typedef QVector<DataWrapper5<bool,float,float,float,float>> DataWrapper5s;

#endif