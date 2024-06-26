/**
 * @file Random.h
 * @brief 随机变量生成对象类Random
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-01
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#ifndef __RANDOM_H
#define __RANDOM_H

#include <random>

namespace rubber_duck{

/**
 * @brief 累计概率分布表
 */
typedef struct{
  double  * x_axis;
  double  * y_axis;
  int size;
} CDFTable;

typedef struct{
  int  * x_axis;
  double  * y_axis;
  int size;
} CDFDiscreteTable;

/**
 * @brief 生成支持经验数据产生随机变量样本的表函数对象
 * @param  number            经验数据数量
 * @param  x_axis            经验数据随机变量取值列表数组
 * @param  y_axis            经验数据随机变量取值累计概率列表数组
 * @return 累计分布表对象指针
 */
CDFTable * makeCDFTable (int number, double  *x_axis, double  *y_axis);
/**
 * @brief 生成支持离散变量经验数据产生随机变量样本的表函数对象
 * @param  valuelist        离散变量取值
 * @param  problist         每个离散变量取值的发生概率
 * @param  number           离散变量取值数量
 * @return 累计分布表对象指针
 */
CDFDiscreteTable * makeDiscreteCDFTable(int number, int valuelist[], double problist[]);

/**
 * @brief 随机变量生成对象类
 */
class Random{
private:
	/**
	 * @brief C++标准的线性同余法伪随机数生成引擎
	 */
	std::mt19937_64 * e;
	/**
	 * @brief C++标准的均匀分布随机变量生成对象
	 */
	std::uniform_real_distribution<double> u{0.0,1.0};
public:
	/**
	 * @brief 创建Random对象
	 * @param  seed  随机变量生成器的种子值
	 */
	Random(unsigned long int seed);
	/**
	 * @brief 删除Random对象
	 */
	~Random();
	/**
	 * @brief 产生下一个服从[0,1]均匀分布的随机变量样本
	 * @return double [0,1]均匀分布的随机变量样本
	 */
	double nextDouble();
	/**
	 * @brief 产生下一个服从指定区间均匀分布的整数随机变量样本
	 * @param  lower            区间下限
	 * @param  upper            区间上限
	 * @return double 服从指定区间均匀分布的随机变量样本
	 */
	int  nextInteger(int  lower, int  upper );
	/**
	 * @brief 产生下一个服从指定区间均匀分布的浮点随机变量样本
	 * @param  lower            区间下限
	 * @param  upper            区间上限
	 * @return double 服从指定区间均匀分布的随机变量样本
	 */
	double  nextDouble(double  lower, double  upper );
	/**
	 * @brief 产生下一个服从指数分布的随机变量样本
	 *  * @param mean mean value
	 * @return double 服从指数分布的随机变量样本
	 */
	double  nextExponential(double p = 1.0);
	/**
	 * @brief 产生下一个服从正态分布的、为非负的随机变量样本
	 * @param  mean             正态分布均值
	 * @param  std_dev          正态分布方差
	 * @return double 下一个服从正态分布的随机变量样本
	 */
	double nextTruncNormal(double mean, double std_dev);
	/**
	 * @brief 采用Polar拒绝法产生下一个服从正态分布的随机变量样本
	 * @param  mean             正态分布均值
	 * @param  std_dev          正态分布方差
	 * @return double 下一个服从正态分布的随机变量样本
	 */
	double  nextNormal (double  mean, double  std_dev );
	/**
	 * @brief 采用Box–Muller产生下一个服从正态分布的随机变量样本
	 * @param  mean             正态分布均值
	 * @param  std_dev          正态分布方差
	 * @return double 下一个服从正态分布的随机变量样本
	 */
	double  nextNormalBM(double  mean, double  std_dev );
	/**
	 * @brief 采用Box–Muller产生下一个服从标准正态分布的随机变量样本
	 * @return double 下一个服从正态分布的随机变量样本
	 */
	double  nextUnitNormalBM();
	/**
	 * @brief 产生下一个服从开方分布的随机变量样本
	 * @param  n    自由度
	 * @return double 下一个服从开方分布的随机变量样本
	 */
	double  nextChiSquare (unsigned int n );
	/**
	 * @brief 产生下一个服从T分布的随机变量样本
	 * @param  n    自由度
	 * @return double 下一个服从T分布的随机变量样本
	 */
	double  nextStudentT (unsigned int n );
	/**
	 * @brief 产生下一个服从对数正态分布的随机变量样本
	 * @param  mean             对数正态分布均值
	 * @param  stdDev           对数正态分布方差
	 * @return double 下一个服从对数正态分布的随机变量样本
	 */
	double  nextLogNormal (double  mean, double  stdDev );
	/**
	 * @brief 产生下一个服从爱尔朗分布的随机变量样本
	 * @param  k                阶数
	 * @return double 下一个服从爱尔朗分布的随机变量样本
	 */
	double nextErlang (unsigned int k, double m);
	double gamma_Marsaglia2000(double alpha);
	double gamma_MarsagliaTransf(double alpha);
	/**
	 * @brief 产生下一个服从伽马分布的随机变量样本
 	 * @param alpha >0  the "shape" parameter
 	 * @param theta >0  the "scale" parameter
	 * @return double 下一个服从伽马分布的随机变量样本
	 */
	double  nextGamma (double  alpha, double  beta );
	/**
	 * @brief 产生下一个服从贝塔分布的随机变量样本
	 * @param  alpha            alpha
	 * @param  beta             beta
	 * @return double 
	 */
	double  nextBeta (double  alpha, double  beta );
	/**
	 * @brief 产生下一个服从F分布的随机变量样本
	 * @param  n1              n1
	 * @param  n2              n2
	 * @return double 下一个服从F分布的随机变量样本
	 */
	double  nextF (unsigned int n1, unsigned int n2 );
	/**
	 * @brief 产生下一个服从泊松分布的随机变量样本
	 * @param  lambda   泊松分布均值
	 * @return double 下一个服从泊松分布的随机变量样本
	 */
	double  nextPoisson (double  lambda );
	/**
	 * @brief 产生下一个服从几何分布的随机变量样本
	 * @param  p      发生概率
	 * @return double 下一个服从几何分布的随机变量样本
	 */
	double  nextGeometric (double p );
	/**
	 * @brief 产生下一个服从韦伯尔分布的随机变量样本
	 * @param  alpha               形状参数 
	 * @param  beta                尺度参数
	 * @return double 下一个服从韦伯尔分布的随机变量样本
	 */
	double  nextWeibull (double  alpha, double  beta);
	/**
	 * @brief 产生下一个服从二项分布的随机变量样本
	 * @param  p                发生概率
	 * @param  n                试验次数
	 * @return double 下一个服从二项分布的随机变量样本
	 */
	double  nextBinomial (double  p, unsigned int n );
	/**
	 * @brief 产生下一个服从负r二项p分布的随机变量样本
	 * @param  p                发生概率
	 * @param  r                失败次数
	 * @return double 下一个服从负二项分布的随机变量样本
	 */
	double  nextNegBinomial (double  p, unsigned int r);
	/**
	 * @brief 产生下一个服从三角分布的随机变量样本
	 * @param  a                最小值
	 * @param  b                众数
	 * @param  c                最大值
	 * @return double 下一个服从三角分布的随机变量样本
	 */
	double  nextTriang (double a, double b, double c);
	/**
	 * @brief 产生一个[0,1]均匀分布随机变量样本，判断其是否发生在指定发生概率之内，相当于伯努利试验
	 * @param  p                指定发生概率
	 * @return true 	发生
	 * @return false 	未发生
	 */
	bool   probability (double  p);
	/**
	 * @brief 产生下一个服从经验分布的连续随机变量样本
	 * @param  pTable        	累计分布表对象指针
	 * @return int 下一个服从经验分布的连续随机变量样本
	 */
	double nextContinuous(CDFTable * pTable,double p = -1);
	/**
	 * @brief 产生下一个服从离散分布的离散随机变量样本
	 * @param  pTable        	离散分布表对象指针
	 * @return int 下一个服从离散分布的离散随机变量样本
	 */
	int nextDiscrete(CDFDiscreteTable * pTable,double p = -1);
};

}
#endif
