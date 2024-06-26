/**
 * @file Simulator.h
 * @brief 仿真引擎类Simulator
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-01
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <stdio.h>
#include "EventList.h"
#include "ProcessNotice.h"
#include "Random.h"

namespace rubber_duck{
/**
 * @brief 仿真引擎对象类，负责事件调度、随机变量生成和输出打印等
 */
class Simulator{
private:
	/**
	 * @brief 仿真时钟
	 */
	double clock = 0;
	/**
	 * @brief 未来事件表
	 */
	EventList futureEventList;
	/**
	 * @brief 条件事件表
	 */
	EventList conditionalEventList;
	/**
	 * @brief 仿真是否终止
	 */
	bool terminated = false;
	/**
	 * @brief 打印输出文件句柄指针
	 */
	FILE* printFile = NULL;
	/**
	 * @brief 是否跟踪打印事件调度过程
	 */
	bool debug = false;
	/**
	 * @brief 缺省的随机变量生成器
	 */
	Random * random = NULL;

	double duration = -1;
	/**
	 * @brief 扫描调度条件事件
	 */
	void scanConditionalEvents();
	void scanFutureEvents(bool CEL);
	void triggerEvent(EventNotice* pEvent);
	bool isEnd();
public:
	/**
	 * @brief 创建Simulator对象
	 * @param  seed             缺省随机变量生成器的种子值
	 * @param  printFileName    为输出打印文件名称，NULL表示不进行文件打印
	 */
	Simulator(unsigned long int seed,const char * printFileName);
	/**
	 * @brief 删除Simulator对象
	 */
	~Simulator();
	/**
	 * @brief 重新初始化Simulator，支持批量仿真运行
	 * @param  seed             缺省随机变量生成器的种子值
	 * @param  printFileName    为输出打印文件名称，NULL表示不进行文件打印
	 */
	void reset(unsigned long int seed,const char * printFileName);
	/**
	 * @brief 调度未来事件，将pEvent插入到FEL中
	 * @param  pEvent  插入的未来事件指针
	 */
	void scheduleEvent(EventNotice * pEvent);
	/**
	 * @brief 调度条件事件，将pEvent插入到CEL中
	 * @param  pEvent   条件事件指针
	 */
	void scheduleConditionalEvent(EventNotice * pEvent);
	/**
	 * @brief 取消FEL和CEL事件调度
	 * @param  pEvent  取消的事件指针
	 */
	void cancelEvent(EventNotice * pEvent);
	/**
	 * @brief 指定事件是否参与仿真运行
	 * @param  pEvent  指定事件
	 */
	bool hasEvent(EventNotice * pEvent);
	/**
	 * @brief 仿真运行，进行事件调度
	 * @param  bCEL   表示是否按照解结规则执行相同时间的仿真事件
	 */
	void run(double duration = -1,bool bCEL = false);
	/**
	 * @brief 终止仿真运行
	 */
	void stop(){	terminated = true;	};
	/**
	 * @brief 跟踪打印事件调度过程
	 */
	void setDebug(){	debug = true;	};
	/**
	 * @brief 判断是否跟踪打印事件调度过程
	 * @return true 跟踪打印事件调度
	 * @return false 不跟踪打印事件调度
	 */
	bool isDebug(){	return debug;	};
	/**
	 * @brief 获取当前仿真时间
	 * @return double 当前仿真时间
	 */
	double getClock(){	return clock;	};
	/**
	 * @brief 获取缺省的随机变量生成器
	 * @return Random* 随机变量生成器指针
	 */
	Random * getRandom(){	return random;	};
	/**
	 * @brief 同时打印到控制台和文件的函数，语法格式同printf函数
	 * 如果设置了printFileName，则将输出同时打印到该文本文件
	 * @param  strFormat        printf函数的打印格式描述符
	 * @param  ...              打印参数
	 */
	void print(const char* strFormat,...);
	/**
	 * @brief 激活无条件延迟进程参与仿真调度
	 * @param  pProcess         激活的进程
	 * @param  time             进程激活时间，如果小于0，则按照pProcess时间调度该进程
	 */
	void activate(ProcessNotice * pProcess,double time = -1);
	/**
	 * @brief 进程挂起，不参与仿真运行
	 * @param  pProcess       被挂起的进程
	 */
	void suspend(ProcessNotice * pProcess);
	/**
	 * @brief 被挂起的进程继续执行
	 * @param  pProcess    继续执行的进程
	 */
	void resume(ProcessNotice * pProcess);
	/**
	 * @brief 调度条件延迟进程参与仿真运行
	 * @param  pProcess    进入条件延迟的进程
	 */
	void await(ProcessNotice * pProcess);
};

}

#endif /* SIMULATOR_H_ */
