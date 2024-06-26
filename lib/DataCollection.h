/**
 * @file DataCollection.h
 * @brief 该文件定义了几个一般的仿真结果统计类Accumulate, Tally和Histogram
 * Accumulate用于收集依赖时间长度的数据
 * Tally用于收集独立于时间的数据
 * Histogram在Tally基础上生成成数据直方图
 * 这些对象类的设计参考了SIMULA的相关设计，
 * G.Birtwistle, "A System for Discrete Event Modelling on SIMULA".
 * Macmillan Computer Science Series, London 1979.
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-12
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#ifndef DATA_COLLECTION_H_
#define DATA_COLLECTION_H_

#include <string>
#include <cmath>
#include <map>
#include <limits> 
#include <iostream>
#include <iomanip>

namespace rubber_duck{

/**
 * @brief DataCollection类，Accumulate、Tally和Histogram的基类，提供基本的统计和打印接口。
 */
class DataCollection{
protected:
    /**
     * @brief 性能指标
     */
    std::string  title;
    /**
     * @brief 观测样本数量
     */
    int obs; 
    /**
     * @brief 统计开始时间
     */
    double resetAt;

public:
    /**
     * @brief 创建新的DataCollection对象
     * @param  title  性能指标名称，不能超过20个字符
     */
    DataCollection(const char * title) {
        this->title = title;
    }

    virtual ~DataCollection() {}
    /**
     * @brief 获取性能指标名称
     * @return const char* 性能指标名称
     */
    const char * getTitle() {
		return title.c_str();
	}
    /**
     * @brief 设置性能指标名称
     * @param  title  性能指标名称
     */
	void setTitle(const char * title) {
		this->title = title;
	}   
    /**
     * @brief 获取观测样本数量
     * @return int 观测样本数量
     */
    int getObs() {
        return obs;
    }
    /**
     * @brief 获取统计开始时间
     * @return double 统计开始时间
     */
    double getResetAt() {
        return resetAt;
    }
    /**
     * @brief 抽象方法，设置统计开始时间
     * @param  time  统计开始时间
     */
    virtual void reset(double time) = 0;
    /**
     * @brief 抽象方法，设置新样本
     * @param  v                样本值
     * @param  time             时间
     */
    virtual void update(double v,double time) = 0;
    /**
     * @brief 抽象方法，打印当前指标的统计结果报告
     */
    virtual void report();
    /**
     * @brief 抽象方法，获取样本最小值
     * @return double 样本最小值
     */
    virtual double min() = 0;
    /**
     * @brief 抽象方法，获取样本最大值
     * @return double 样本最大值
     */
    virtual double max() = 0;
    /**
     * @brief 抽象方法，获取样本均值
     * @return double 样本均值
     */
     virtual double mean() = 0;
    /**
     * @brief 抽象方法，获取样本方差
     * @return double 样本方差
     */
    virtual double variance() = 0;
    /**
     * @brief 获取样本均方差或标准差
     * @return double 样本均方差
     */
    double stdDev() {
        return sqrt(variance());
    }
    /**
     * @brief 获取置信区间半长
     * @param  level  置信水平
     * @return double 置信区间半长
     */
    double confidence(double level);
    /**
     * @brief 获取95%置信度的置信区间半长
     * @return double 置信区间半长
     */
    double confidence() {
        return confidence(0.95);
    }
    /**
     * @brief 返回T分布的上pth分位点或分位数，
     * Converted from Dirk Grunwald's C++ code:<br>
     * www.mit.edu/afs/athena/astaff/reference/4.3network2/lib/libg++/libg++/SmplStat.cc
     * @param  p     显著水平
     * @param  df    自由度
     * @return double pth分位点
     */
    double tValue(double p, int df);
    /**
     * @brief 打印统计结果标题
     */
    static void printHeading();
    /**
     * @brief 打印统计结果结束标志
     */
    static void printEnding();
};
/**
 * @brief Tally采用如下形式进行输出独立于时间的仿真结果统计
 * TITLE       (RE)SET   OBS.  MEAN   STDEV   MINIMUM  MAXIMUM   CONFIDENCE%95
 * Thru times    0.000   100   18.379 12.576    4.612   38.664          6.438
 * 仿真结束时，可以采用如下方式打印单个性能指标的统计结果：
 *       tally.printHeading();
 *       tally.report();
 *       tally.printEnding();
 * 最后一列表示95%的置信区间半长，上述例子中表示顾客在商店中的平均停留时间为18.379，
 * %95置信度下的置信区间半长为6.438
 */
class Tally :public DataCollection {
protected:
    /**
     * @brief resetAt开始后的样本值的和
     */
    double sum;
    /**
     * @brief resetAt开始后的样本值的和的平方
     */
    double sumsq; 
    /**
     * @brief resetAt开始后的最小样本值
     */
    double m_min;
    /**
     * @brief resetAt开始后的最大样本值
     */
    double m_max;
public:
    /**
     * @brief 创建一个新的Tally对象
     * @param  title  性能指标名称，不能超过20个字符
     */
    Tally(const char * title):DataCollection(title) {
    }

    virtual ~Tally() {}
    /**
     * @brief 设置统计开始时间
     * @param  time  统计开始时间
     */
    virtual void reset(double time);
    /**
     * @brief 设置新样本
     * @param  v                样本值
     * @param  time             时间
     */
    virtual void update(double v,double time);
    /**
     * @brief 获取样本方差
     * @return double 样本方差
     */
    virtual double variance();
    /**
     * @brief 获取样本最小值
     * @return double 样本最小值
     */
    virtual double min() { 
        return m_min; 
    }
    /**
     * @brief 获取样本最大值
     * @return double 样本最大值
     */
    virtual double max() { 
        return m_max; 
    }
    /**
     * @brief 获取样本均值
     * @return double 样本均值
     */
    virtual double mean();
};
/**
 * @brief Accumulate采用输出依赖于时间的仿真结果统计
 * 例如超市中的顾客平均数，可以采用以下方式进行统计
 * Accumulate no("No. in shop");
 * no.update(++n);或no.update(--n);
 * 典型的报告输出如下：
 * TITLE       (RE)SET   OBS.  MEAN   STDEV   MINIMUM  MAXIMUM   CONFIDENCE%95
 * No. in shop    0.000  1322  3.501  3.569    0.000   16.000          0.193
 * 仿真结束时，可以采用如下方式打印单个性能指标的统计结果：
 *       no.printHeading();
 *       no.report();
 *       no.printEnding();
 * 最后一列表示95%的置信区间半长，上述例子中表示顾客在商店中的平均数量为3.501，
 * %95置信度下的置信区间半长为0.193
 */
class Accumulate : public DataCollection {
private:
    /**
     * @brief resetAt开始后的样本值时间积分
     */
    double sum;
    /**
     * @brief resetAt开始后的样本值时间积分的平方
     */
    double sumsq;
    /**
     * @brief resetAt开始后的最小样本值
     */
    double m_min;
    /**
     * @brief resetAt开始后的最大样本值
     */
    double m_max;
    /**
     * @brief 上一次样本更新时间
     */
    double lastTime; 
    /**
     * @brief 上一个样本值
     */
    double lastV;
public:
    /**
     * @brief 创建一个新的Tally对象
     * @param  title  性能指标名称，不能超过20个字符
     */
     Accumulate(const char * title): DataCollection(title){
    }

    Accumulate(): DataCollection("Accumulate"){
    }

    virtual ~Accumulate() {}
    /**
     * @brief 设置统计开始时间
     * @param  time  统计开始时间
     */
    virtual void reset(double time);
    /**
     * @brief 设置新样本
     * @param  v                样本值
     * @param  time             时间
     */
    virtual void update(double v,double time);
    /**
     * @brief 获取样本最小值
     * @return double 样本最小值
     */
    virtual double min() { 
        return m_min;         
    }
    /**
     * @brief 获取样本最大值
     * @return double 样本最大值
     */
    virtual double max() { 
        return m_max; 
    }
    /**
     * @brief 获取样本均值
     * @return double 样本均值
     */
    virtual double mean();
    /**
     * @brief 获取样本方差
     * @return double 样本方差
     */
    virtual double variance();
};
/**
 * @brief Histogram用于收集独立于时间的仿真结果统计，并打印数据样本的统计直方图
 * Histogram根据设置的结果样本上限、下限和区间数量，按照等区间宽度输出频率直方图
 * 例如：
 * static Histogram perDamage("Perc.damage", 0, 100, 20);
 * perDamage.update(damage.state * 100 / material);
 * 典型的报告输出如下：
 * CELLs    LOWER COUNT FREQUENCY  CUM %
 *    0 -infinity     0    0.00    0.00  |
 *    1     0.000    16    0.12   11.59  |*****************
 *    2     5.000    38    0.28   39.13  |****************************************
 *    3    10.000    31    0.22   61.59  |*********************************
 *    4    15.000    17    0.12   73.91  |******************
 *    5    20.000    13    0.09   83.33  |**************
 *    6    25.000     4    0.03   86.23  |****
 *    7    30.000     4    0.03   89.13  |****
 *    8    35.000     6    0.04   93.48  |******
 *    9    40.000     2    0.01   94.93  |**
 *   10    45.000     4    0.03   97.83  |****
 *   11    50.000     1    0.01   98.55  |*
 *   12    55.000     0    0.00   98.55  |
 *   13    60.000     0    0.00   98.55  |
 *   14    65.000     0    0.00   98.55  |
 *   15    70.000     0    0.00   98.55  |
 *   16    75.000     2    0.01  100.00  |**
 *   17    80.000     0    0.00  100.00  |
 *   18    85.000     0    0.00  100.00  |
 *   19    90.000     0    0.00  100.00  |
 * 仿真结束时，可以采用如下方式打印单个性能指标的统计结果：
 *       perDamage.printHeading();
 *       perDamage.report();
 *       perDamage.printEnding();
 */
class Histogram : public Tally{
protected:
    /**
     * @brief 结果样本值的期望下限值
     */
    double lower;
    /**
     * @brief 结果样本值的期望上限值
     */
    double upper;
    /**
     * @brief 每个区间的宽度
     */
    double width;
    /**
     * @brief 区间数量
     */
    int nCells;
    /**
     * @brief 区间限制，为nCells + 1
     */
    int limit;
    /**
     * @brief 区间样本数量记录表
     */
    int * table;
public:
    /**
     * @brief 创建一个新的Histogram对象
     * @param  title    性能指标名称，不能超过20个字符
     * @param  lower    结果样本下限
     * @param  upper    结果样本上限
     * @param  nCells   区间数量
     */
    Histogram(const char * title, double lower, double upper, int nCells);
    virtual ~Histogram();
    /**
     * @brief 设置统计开始时间
     * @param  time  统计开始时间
     */
    virtual void reset(double time);
    /**
     * @brief 设置新样本
     * @param  v                样本值
     * @param  time             时间
     */
    virtual void update(double v,double time);
    /**
     * @brief 抽象方法，打印当前指标的统计结果报告
     */
    virtual void report();
};

}

#endif /* DATA_COLLECTION_H_ */