#ifndef CPP_EVAL_H
#define CPP_EVAL_H
#include <cmath>
#include <map>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <functional>
#include <QString>
#include <QMap>

namespace cpp_eval
{

template< typename number, typename functions >
number eval(const char* expression,const ::std::map<QString,number >& variables, functions& funcs, std::string& desc);

template< typename number,typename functions>
class evaler
{
	const ::std::map<::std::string,number>& mVariables;
	functions& mFuncs;
	const char* mCurrent;
	enum Type
	{
		ADD_OR_POSITIVE = '+',
		SUBTRACT_OR_NEGATIVE = '-',
		MULTIPLY = '*',
		DIVIDE = '/',
		POWER = '~',
		LEFT_BRACKET = '(',
		RIGHT_BRACKET = ')',
		PARAMETER_SEPERATOR = ',',
		IDENTIFIER = 257,
		NUMBER = 258,
		FINISHED = 259
	};
	
	Type mType;
	std::string mIdentifier;
	number mValue;
	
	void look_ahead(std::string& desc)
	{
		for(;;)
		{
			if (isspace(*mCurrent))
			{
				++mCurrent;
				continue;
			}
			else if (*mCurrent == '*' && *(mCurrent + 1) == '*')
				mType = POWER, mCurrent += 2;
			else if (*mCurrent == ADD_OR_POSITIVE || *mCurrent == SUBTRACT_OR_NEGATIVE ||
				*mCurrent == MULTIPLY || *mCurrent == DIVIDE ||
				*mCurrent == LEFT_BRACKET || *mCurrent == RIGHT_BRACKET ||
				*mCurrent == PARAMETER_SEPERATOR)
				mType = (Type)*mCurrent, ++mCurrent;
			else if (isalpha(*mCurrent))
			{
				mType = IDENTIFIER;
				mIdentifier.clear();
				mIdentifier += *mCurrent;
				++mCurrent;
				while (isalpha(*mCurrent) || isdigit(*mCurrent))
					mIdentifier += *mCurrent, ++mCurrent;
			}
			else if (*mCurrent == 0)
				mType = FINISHED;
			else
			{
				mType = NUMBER;
				std::istringstream iss(mCurrent);
				iss >> mValue;
				if (!iss)
				{
					desc = "数字格式有误";
					return;
				}
				mCurrent += iss.rdbuf()->pubseekoff(0,std::ios::cur,std::ios::in);
			}
			break;
		}
	}
	
	void match(Type type,std::string& desc)
	{
		if (mType == type)
			look_ahead(desc);
		else
		{
			desc = "数字格式有误";
			return;
		}
	}
	
	number expression(std::string& desc)
	{ 
		//expression -> higher_expression expression_R
		auto value = higher_expression(desc);
		if (!desc.empty())
			return number();
		number result = expression_R(value,desc);
		return result;
	}
	
	number expression_R(const number& left,std::string& desc)
	{
		//expression_R -> + higher_expression expression_R | - higher_expression expression_R | /e/
		number result = left;
		if (mType == ADD_OR_POSITIVE)
			match(ADD_OR_POSITIVE,desc), result = expression_R(left + higher_expression(desc),desc);
		else if (mType == SUBTRACT_OR_NEGATIVE)
			match(SUBTRACT_OR_NEGATIVE,desc), result = expression_R(left - higher_expression(desc),desc);
		return result;
	}

	number higher_expression(std::string& desc)
	{
		//higher_expression -> sign_expression higher_expression_R
		return higher_expression_R(sign_expression(desc),desc);
	}

	number higher_expression_R(const number& left,std::string& desc)
	{
		//higher_expression_R -> * sign_expression higher_expression_R | / sign_expression higher_expression_R | /e/
		number result = left;
		if (mType == MULTIPLY)
			match(MULTIPLY,desc), result = higher_expression_R(left * sign_expression(desc),desc);
		else if (mType == DIVIDE)
			match(DIVIDE,desc), result = higher_expression_R(left / sign_expression(desc),desc);
		return result;
	}

	number sign_expression(std::string& desc)
	{
		//sign_expression -> + sign_expression | - sign_expression | power_expression
		number result;
		if (mType == ADD_OR_POSITIVE)
			match(ADD_OR_POSITIVE,desc), result = sign_expression(desc);
		else if (mType == SUBTRACT_OR_NEGATIVE)
			match(SUBTRACT_OR_NEGATIVE,desc), result = -sign_expression(desc);
		else
			result = power_expression(desc);
		return result;
	}

	number power_expression(std::string& desc)
	{
		//power_expression -> factor ** power_expression | factor
		number result = factor(desc);
		if (!desc.empty())
			return result;
		if (mType == POWER)
			match(POWER,desc), result = std::pow(result, static_cast<int>(power_expression(desc)));
		return result;
	}
	
	number factor(std::string& desc)
	{
		//factor -> number | ( expression ) | lang_structure
		number result;
		if (mType == NUMBER)
			result = mValue, match(NUMBER,desc);
		else if (mType == LEFT_BRACKET)
			match(LEFT_BRACKET,desc), result = expression(desc), match(RIGHT_BRACKET,desc);
		else
			result = lang_structure(desc);
		return result;
	}
	
	number lang_structure(std::string& desc)
	{
		//lang_structure -> identifier lang_tail
		std::string id = mIdentifier;
		match(IDENTIFIER,desc);
		return lang_tail(id,desc);
	}
	
	number lang_tail(const std::string& id, std::string& desc)
	{
		//lang_tail -> ( parameter_list | /e/
		number result = 0;
		if (mType == LEFT_BRACKET)
		{
			match(LEFT_BRACKET,desc);
			std::vector<number> param = parameter_list(desc);
			result = mFuncs(id.c_str(),param,desc);
		}
		else
		{
			if (mVariables.find(id) == mVariables.end())
			{
				desc = "数字格式有误";
				return result;
			}
			result = mVariables.find(id)->second;
		}
		return result;
	}

	std::vector<number> parameter_list(std::string& desc)
	{
		//parameter_list -> ) | expression parameter_tail
		std::vector<number> result;
		for (;;)
		{
			if (mType == RIGHT_BRACKET)
			{
				match(RIGHT_BRACKET,desc);
				break;
			}
			result.push_back(expression(desc));
		    parameter_tail(result,desc);
		}
		return result;
	}
	
	void parameter_tail(std::vector<number>& param, std::string& desc)
	{
		//parameter_tail -> , expression parameter_tail | /e/
		if(mType == PARAMETER_SEPERATOR)
			match(PARAMETER_SEPERATOR,desc), param.push_back(expression(desc)), parameter_tail(param,desc);
	}

public:
	evaler(const ::std::map<::std::string,number>& variables,functions& funcs)
		: mVariables(variables), mFuncs(funcs)
	{
	    //mVariables.insert("PI",3.14159265);
		//mVariables.insert("E",2.718281828); 
	}

	number operator()(const char* expr,std::string& desc)
	{
		desc.clear();
		mCurrent = expr;
		look_ahead(desc);
		if(!desc.empty())
			return number();
		number result = expression(desc);
		if(mType != FINISHED)
		    desc = "表达式格式有误";
		return result;
	}
};

template<typename number,typename functions>
number eval(const char* expression,const ::std::map<::std::string,number>& variables,functions& func,std::string& desc)
{
	return evaler<number,functions>(variables,func)(expression,desc);
}

};

typedef std::function<double(const std::vector<double>&,std::string&)> SFunction;

class SFuntionFactory
{
public:
	static SFuntionFactory& instance()
	{
		static SFuntionFactory factory;
		return factory;
	}

	double operator()(const char* name,const ::std::vector<double>& params,std::string& desc)
	{
		if (mapper.find(name) == mapper.end())
		{
			desc = "函数不存在"; 
			return 0.0;
		}
		return mapper[name](params,desc);
	}

	void registerFunction(const QString& name,const SFunction& fn)
	{
		mapper[name] = fn;
	}

private:
	SFuntionFactory();
	QMap<QString,SFunction> mapper;
};

inline double evalute_express(const QString& expr)
{
	std::map<std::string,double> variables;
	std::string desc;
	return cpp_eval::eval(expr.toStdString().c_str(),variables,SFuntionFactory::instance(),desc);
}

#endif