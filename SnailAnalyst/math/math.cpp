#include <cassert>
#include "common/common.h"
#include "common/FileUtils.h"
#include "ConfigurationParser.h"
#include "math.h"

double gsl_mean(const double data[],const size_t size)
{
	/* Compute the arithmetic mean of a dataset using the recurrence relation
	mean_(n) = mean(n-1) + (data[n] - mean(n-1))/(n+1)   */

	long double mean = 0;
	size_t i;

	for (i = 0; i < size; i++)
	{
		mean += (data[i] - mean) / (i + 1);
	}

	return mean;
}

double stock_mean(QVector<Stock>& stocks,int flag)
{
	double mean = 0;
	for (int i = 0; i < stocks.size(); i++)
	{
		mean += (stocks[i].getValueByIndex(flag) - mean) / (double)(i + 1);
	}

	return mean;
}

double absdev_m(const double data[],const size_t n,const double mean)
{
	/* takes a dataset and finds the absolute deviation */

	double sum = 0,absdev;
	size_t i;

	/* find the sum of the absolute deviations */
	for (i = 0; i < n; i++)
	{
		const double delta = fabs(data[i] - mean);
		sum += delta;
	}

	absdev = sum / n;
	return absdev;
}

double gsl_absdev(const double data[],const size_t n)
{
	const double mean = gsl_mean(data,n);
	return absdev_m(data,n,mean);
}

double stdev_m(const double data[],const size_t n,const double mean)
{
	/* takes a dataset and finds the absolute deviation */

	double sum = 0,stdev;
	size_t i;

	/* find the sum of the absolute deviations */
	for (i = 0; i < n; i++)
	{
		const double delta = fabs(data[i] - mean);
		sum += delta*delta;
	}

	return sqrt(sum / (n-1));
}

double gsl_stdev(const double data[],const size_t n)
{
	const double mean = gsl_mean(data,n);
	return stdev_m(data,n,mean);
}

double gsl_covariance_m(const double data1[],const double data2[],const size_t n,const double mean1,const double mean2)
{
	/* takes a dataset and finds the covariance */

	long double covariance = 0;

	size_t i;

	/* find the sum of the squares */
	for (i = 0; i < n; i++)
	{
		const long double delta1 = (data1[i] - mean1);
		const long double delta2 = (data2[i] - mean2);
		covariance += (delta1 * delta2 - covariance) / (i + 1);
	}

	return covariance;
}

double covariance_m(const double data1[],
	const double data2[],
	const size_t n,
	const double mean1,const double mean2)
{
	const double covariance = gsl_covariance_m(data1,data2,n,mean1,mean2);
	return covariance * ((double)n / (double)(n - 1));
}

double gsl_covariance(const double data1[],
	const double data2[],
	const size_t n)
{
	const double mean1 = gsl_mean(data1,n);
	const double mean2 = gsl_mean(data2,n);

	return covariance_m(data1,data2,n,mean1,mean2);
}

/*
gsl_stats_correlation()
Calculate Pearson correlation = cov(X, Y) / (sigma_X * sigma_Y)
This routine efficiently computes the correlation in one pass of the
data and makes use of the algorithm described in:

B. P. Welford, "Note on a Method for Calculating Corrected Sums of
Squares and Products", Technometrics, Vol 4, No 3, 1962.

This paper derives a numerically stable recurrence to compute a sum
of products

S = sum_{i=1..N} [ (x_i - mu_x) * (y_i - mu_y) ]

with the relation

S_n = S_{n-1} + ((n-1)/n) * (x_n - mu_x_{n-1}) * (y_n - mu_y_{n-1})
*/

double gsl_correlation(const double data1[],const double data2[],const size_t n)
{
	size_t i;
	long double sum_xsq = 0.0;
	long double sum_ysq = 0.0;
	long double sum_cross = 0.0;
	long double ratio;
	long double delta_x,delta_y;
	long double mean_x,mean_y;
	long double r;

	/*
	* Compute:
	* sum_xsq = Sum [ (x_i - mu_x)^2 ],
	* sum_ysq = Sum [ (y_i - mu_y)^2 ] and
	* sum_cross = Sum [ (x_i - mu_x) * (y_i - mu_y) ]
	* using the above relation from Welford's paper
	*/

	mean_x = data1[0];
	mean_y = data2[0];

	for (i = 1; i < n; ++i)
	{
		ratio = i / (i + 1.0);
		delta_x = data1[i] - mean_x;
		delta_y = data2[i] - mean_y;
		sum_xsq += delta_x * delta_x * ratio;
		sum_ysq += delta_y * delta_y * ratio;
		sum_cross += delta_x * delta_y * ratio;
		mean_x += delta_x / (i + 1.0);
		mean_y += delta_y / (i + 1.0);
	}

	r = sum_cross / (sqrt(sum_xsq) * sqrt(sum_ysq));

	return r;
}

double gsl_variance(const double data[],const size_t n,const double mean)
{
	/* takes a dataset and finds the variance */

	long double variance = 0;

	size_t i;

	/* find the sum of the squares */
	for (i = 0; i < n; i++)
	{
		const long double delta = (data[i] - mean);
		variance += (delta * delta - variance) / (i + 1);
	}

	return variance;
}

double gsl_tts(const double data[],const size_t n,const double mean)
{
	/* takes a dataset and finds the sum of squares about the mean */

	long double tss = 0;

	size_t i;

	/* find the sum of the squares */
	for (i = 0; i < n; i++)
	{
		const long double delta = (data[i] - mean);
		tss += delta * delta;
	}

	return tss;
}

double gsl_variance_with_fixed_mean(const double data[],const size_t n,const double mean)
{
	const double variance = gsl_variance(data,n,mean);
	return variance;
}

double gsl_sd_with_fixed_mean(const double data[],const size_t n,const double mean)
{
	const double variance = gsl_variance(data,n,mean);
	const double sd = sqrt(variance);

	return sd;
}

double gsl_variance_m(const double data[],const size_t n,const double mean)
{
	const double variance = gsl_variance(data,n,mean);
	return variance * ((double)n / (double)(n - 1));
}

double gsl_sd_m(const double data[],const size_t n,const double mean)
{
	const double variance = gsl_variance(data,n,mean);
	const double sd = sqrt(variance * ((double)n / (double)(n - 1)));
	return sd;
}

double gsl_variance(const double data[],const size_t n)
{
	const double mean = gsl_mean(data,n);
	return gsl_variance_m(data,n,mean);
}

double gsl_sd(const double data[],const size_t n)
{
	const double mean = gsl_mean(data,n);
	return gsl_sd_m(data,n,mean);
}

double maxDownValue(const QVector<double>& input)
{
    double maxsum, maxhere;
    maxsum = maxhere = -input[0];
    for (int i = 1; i<input.size(); i++) {
        if (maxhere <= 0)
            maxhere = -input[i];  //如果前面位置最大连续子序列和小于等于0，则以当前位置i结尾的最大连续子序列和为a[i] 
        else
            maxhere += -input[i]; //如果前面位置最大连续子序列和大于0，则以当前位置i结尾的最大连续子序列和为它们两者之和 
        if (maxhere > maxsum) {
            maxsum = maxhere;  //更新最大连续子序列和 
        }
    }
    return -maxsum;
}

float getStockActivteValue(const QVector<Stock>& stocks)
{
	if (stocks.isEmpty())
		return 0.0f;

	float value = 0.0f;

	foreach(auto item,stocks)
	{
		float increase = GET_INCREASE(item.top,item.low);
		increase *= increase;
		value += increase;
	}
	
	value /= (float)stocks.size();
	return std::min<float>(value*1.5/**3.09f*/,100.0f);
}

double pearson(double* l,double* r,int size,double* delta,double* slope)
{
	double sumlr = 0.0;
	double suml = 0.0;
	double sumr = 0.0;
	double suml2 = 0.0;
	double sumr2 = 0.0;

	for (int i = 0;i<size;i++)
	{
		sumlr += l[i] * r[i];
		suml += l[i];
		sumr += r[i];
		suml2 += l[i] * l[i];
		sumr2 += r[i] * r[i];
	}

	*delta = (size*sumlr - suml * sumr) / (size * suml2 - suml * suml);
	*slope = (sumr - *delta * suml) / size;
	return (size*sumlr - suml * sumr) / (sqrt((size * suml2 - suml * suml) * (size*sumr2 - sumr * sumr)));
};

QVector<QPair<int,int>> getCombinationVector(int n)
{
	QVector<QPair<int,int>> output;
	if (n < 2)
		return output;

	for(int i=0;i<n-1;i++)
		for (int j = i + 1;j < n;j++)
		{
			output += QPair<int,int>(i,j);
		}

	return output;
}

QString getPairTradeStockData(const QString& market,const QString& path,const QString& format,const QString& code1,const QString& code2,QVector<Stock>& stock1,QVector<Stock>& stock2)
{
	auto file1 = getStockFileByPathAndFormatAndCode(market,path,format,code1);
	auto file2 = getStockFileByPathAndFormatAndCode(market,path,format,code2);

	if (!(file1.isFile() && file1.exists()))
	{
		return QString("股票%1不存在或缺少文件").arg(code1);
	}

	if (!(file2.isFile() && file2.exists()))
	{
		return QString("股票%1不存在或缺少文件").arg(code2);
	}

	stock1 = globalConfig.readStocksFromFile(file1.absoluteFilePath());
	stock2 = globalConfig.readStocksFromFile(file2.absoluteFilePath());

	if (stock1.isEmpty() || stock2.isEmpty())
		return QString("股票无数据");
	unionStocks(stock1,stock2);
	//if (stock1.size() < 60)
	//	return QString("配对股票数据量过少");
	return QString();
}

double pearsonStock(const QVector<Stock>& s1,const QVector<Stock>& s2)
{
	if(s1.size() != s2.size())
		return 0.0;

	double* d1 = new double[s1.size()];
	double* d2 = new double[s2.size()];

	int i = 0;
	foreach(auto item,s1)
		d1[i++] = item.current;
	i = 0;
	foreach(auto item,s2)
		d2[i++] = item.current;

	double a,b;
	double cor = pearson(d1,d2,s1.size(),&a,&b);

	delete[]d1;
	delete[]d2;
	return cor;
}