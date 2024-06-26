/**
 * @file Error.cpp
 * @brief 
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-23
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#include <iostream>
#include "Error.h"
using namespace std;
namespace rubber_duck{

void err (int n){
	static const char *errmsg[] ={
		"均匀分布的上限必须大于下限",			//"must have upper limit > lower limit in uniform distribution",
		"正态分布的均值不能为负值",				//"negative mean in normal distribution",
		"正态分布的方差不能为负值",				//"negative standard deviation in normal distribution",
		"正态对数分布的均值不能为负值",			//"negative mean in normal log-distribution",
		"正态对数分布的方差不能为负值", 		//"negative standard deviation in log-normal distribution", 
		"参数值不能小于0",					   //"value of arguments less than zero",
		"贝塔分布的参数不能等于0",				//"argument equal to zero in beta function",
		"概率值必须为0到1之间的值",				//"probability values must be in range 0 to 1.0",
		"三角分布的众数取值超出范围",			//"mode out of range in triangular distribution",
		"离散分布的离散值数量不能超过100或小于1",//"discrete count is too small or too large, count size is in 100",
		"直方图下限超过上限",					//"new Histogram: lower >= upper",
		"直方图区间数量小于1",					//"new Histogram: nCells < 1",
		"离散分布累积概率不等于1",				//"CDF is not equal 1",
		"该进程未被调度执行, await或suspend函数错误",					//"the process has not been scheduled,activate or suspend",
		"伽马分布的参数不能小于0",				//"alpha or beta in gamma distribution less than zero",
		"表函数中Y值必须是增大的",				//"y values must be increasing in tabulated function",
		"表函数中X值必须是增大的",			    //"x values must be increasing in tabulated function",
    	"表函数中Y值必须在0到1之间",			//"y values must be in the range 0 to 1.0 in tabulated function",
	    "累计经验分布表为空指针",				//"NULL table",
	};
	cerr << endl << "运行错误 : " << errmsg[n] << endl;
	exit(0);
}

void warn (int n){
	static const char *warnmsg[] = {
		"事件列表为空"//"scheduled events list empty"
	};
	cerr << endl << "运行警告 : " << warnmsg[n] << endl;
}

}