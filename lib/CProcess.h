/**
 * @file CProcess.h
 * @brief 面向C语言进程例程的进程对象类CProcess，
 * 采用了OMNET++的协程实现方法
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2022-10-02
 * 
 * @copyright Copyright(C) 2022 liqun
 */

#ifndef CPROCESS_H_
#define CPROCESS_H_

#include "EventNotice.h"
#include "Simulator.h"
#include "Queue.h"
#include "DataCollection.h"
#include "Coroutine.h"

namespace rubber_duck{

void process(void * param);

class Resource;

/**
 * @brief 面向C语言进程例程的进程对象类
 */
class CProcess:public EventNotice{
private:
	/**
	 * @brief 协作例程指针
	 */
	Coroutine * coroutine;
	/**
	 * @brief 仿真引擎对象指针
	 */
	static Simulator * simulator;
public:
	/**
	 * @brief 创建CProcess对象，进程缺省堆栈为12800
	 * @param  ftime  为进程复活时间
	 */
	CProcess(double ftime) :EventNotice(ftime){
		coroutine = new Coroutine();
		if(!coroutine->setup(process,this,12800)){
			printf("进程初始化错误！\n");
		}
		reserved = true;
	}

	/**
	 * @brief 删除CProcess对象
	 */
	virtual ~CProcess() {
		delete coroutine;
	}
	/**
	 * @brief 进程结束，由进程函数自动调用
	 */
	void processFinished(){
		coroutine->taskFinished();
	}
	/**
	 * @brief 初始化进程仿真引擎对象和Linux进程堆栈
	 * @param  pSimulator    进程仿真引擎对象指针
	 * @param  totalStack    总堆栈大小
	 * @param  mainStack     主堆栈大小，注意主堆栈不能大于等于总堆栈
	 */
	static void init(Simulator * pSimulator,unsigned totalStack, unsigned mainStack){
		CProcess::simulator = pSimulator;
		Coroutine::init(totalStack,mainStack);
	}
	/**
	 * @brief 获得进程仿真引擎对象
	 * @return Simulator * 进程仿真引擎对象指针
	 */
	static Simulator * getSimulator(){
		return CProcess::simulator;
	}
	/**
	 * @brief 获得当前仿真时间
	 * @return double 当前仿真时间
	 */
	double Time(){
		return CProcess::simulator->getClock();
	}
	/**
	 * @brief 确定是否在事件调度后保留进程对象，由进程函数自动调用
	 * @param  r  是否在事件调度后保存该进程对象
	 */
	void setReserved(bool r){
		reserved = r;
	}
	/**
	 * @brief 该进程类的进程推进函数，由进程函数自动调用，进程对象子类必须实现进程推进函数
	 */
	virtual void actions() = 0;

	/**
	 * @brief 重载EventNotice事件处理函数，执行进程推进函数
	 * @param  pSimulator    仿真引擎对象指针
	 */
	virtual void trigger(Simulator * pSimulator){
		Coroutine::switchTo(coroutine);
	}
	/**
	 * @brief 进程进入无条件延迟
	 * @param  delayTime  无条件延迟时间
	 */
	void wait(double delayTime){
		setTime(CProcess::simulator->getClock() + delayTime);
		CProcess::simulator->scheduleEvent(this);
		Coroutine::switchToMain();
	}
	/**
	 * @brief 进程进入无条件延迟
	 * @param  delayTime  无条件延迟时间
	 */
	void hold(double delayTime){
		wait(delayTime);
	}	
	/**
	 * @brief 进程进入无条件延迟
	 * @param  delayTime  无条件延迟时间
	 */
	void work(double delayTime){
		wait(delayTime);
	}	
	/**
	 * @brief 进程进入无条件延迟
	 * @param  delayTime  无条件延迟时间
	 */
	void delay(double delayTime){
		wait(delayTime);
	}	
	/**
	 * @brief 在指定时刻激活进程参与仿真调度
	 * @param  pProcess  激活的进程
	 * @param  time      进程激活时间，如果小于0，则按照pProcess时间调度该进程
	 */
	static void activate(CProcess * pProcess,double time = -1){
		if(time < 0){
			CProcess::simulator->scheduleEvent(pProcess);
		}else{
			pProcess->setTime(time);
			CProcess::simulator->scheduleEvent(pProcess);
		}
	}
	/**
	 * @brief 在当前时刻激活进程参与仿真调度
	 * @param  pProcess  激活的进程
	 */
	static void activateNow(CProcess * pProcess){
		pProcess->setTime(CProcess::simulator->getClock());
		CProcess::simulator->scheduleEvent(pProcess);
	}
	/**
	 * @brief 挂起指定进程，不再参与仿真运行
	 * @param  pProcess  被挂起的进程
	 */
	static void suspend(CProcess * pProcess){
		if(CProcess::simulator->hasEvent(pProcess)){
			CProcess::simulator->cancelEvent(pProcess);
		}else{
			err(PROCUSCH);
		}
	}
	/**
	 * @brief 中断指定进程，不再参与仿真运行
	 * @param  pProcess  被中断的进程
	 */
	static void interrupt(CProcess * pProcess){
		suspend(pProcess);
	}
	/**
	 * @brief 在当前时刻继续执行被挂起的进程
	 * @param  pProcess    继续执行的进程
	 */
	static void resume(CProcess * pProcess){
		if(CProcess::simulator->hasEvent(pProcess)){
			err(PROCSCH);
			return;
		}
		pProcess->setTime(CProcess::simulator->getClock());
		CProcess::simulator->scheduleEvent(pProcess);
	}
	/**
	 * @brief 当前进程请求资源
	 * @param  r    	资源对象指针
	 * @param  units    请求的资源数量
	 */
	void request(Resource * r,int units);
	/**
	 * @brief 当前进程释放资源
	 * @param  r    	资源对象指针
	 * @param  units    释放的资源数量
	 */
	void relinquish(Resource * r,int units);
};

/**
 * @brief 资源对象中等待资源的进程对象信息类
 */
class WaitProcess{
public:
	/**
	 * @brief 等待资源的进程对象指针
	 */
	CProcess * p;
	/**
	 * @brief 请求的资源数量
	 */
	int requestUnits;
	/**
	 * @brief 创建WaitProcess对象
	 * @param  p  				等待资源的进程对象指针
	 * @param  requestUnits  	请求的资源数量
	 */
	WaitProcess(CProcess * p,int requestUnits) {
		this->p = p;
		this->requestUnits = requestUnits;
	}
};
/**
 * @brief 资源管理对象类
 */
class Resource{
	/**
	 * @brief 资源名称
	 */
	std::string name;
	/**
	 * @brief 资源容量
	 */
	int capacity;
	/**
	 * @brief 当前可用资源数量
	 */
	int currentUnits;
	/**
 	 * @brief 等待资源的进程队列
 	 */
	Queue<WaitProcess*> waitingLine; 
	/**
 	 * @brief 正在使用资源的进程队列
 	 */	
	std::list<CProcess *>servedLine;
	/**
 	 * @brief 等待队列统计对象
 	 */	
	Accumulate * waitingAccumulate = NULL ;
	/**
 	 * @brief 资源服务统计对象
 	 */	
	Accumulate * workingAccumulate = NULL;
public:
	/**
	 * @brief 创建Resource对象
	 * @param  name  	资源名称
	 * @param  capacity 资源容量
	 */
	Resource(const char * name,int capacity){
		this->name = name;
		this->capacity = capacity;
		this->currentUnits = capacity;
		std::string reportName = this->name + "队列";
		waitingAccumulate = new Accumulate(reportName.c_str());
		reportName = this->name + "利用率";
		workingAccumulate = new Accumulate(reportName.c_str());
	}
	/**
	 * @brief 获得等待资源的进程数量
	 * @return int 等待资源的进程数量
	 */
	int waitSize(){
		return (int)waitingLine.size();
	}
	/**
	 * @brief 请求资源
	 * @param  p  	请求资源的进程对象指针
	 * @param  n  	请求的资源数量
	 * @return bool 第一次请求是否成功
	 */
	bool request(CProcess * p, int n){
		if (currentUnits >= n) {
			currentUnits -= n;
			servedLine.push_back(p);
			workingAccumulate->update((double)(capacity - currentUnits)
						/ capacity, CProcess::getSimulator()->getClock());
			return true;
		}else {
			WaitProcess * w = new WaitProcess(p,n);
			waitingLine.enqueue(w);
			waitingAccumulate->update((double)waitingLine.size(), CProcess::getSimulator()->getClock());
			return false;
		}
	}
	/**
	 * @brief 释放资源
	 * @param  p  	释放资源的进程对象指针
	 * @param  n  	释放的资源数量
	 */
	void relinquish(CProcess * p, int n){
		currentUnits += n;
		servedLine.remove(p);
		workingAccumulate->update((double)(capacity - currentUnits) / capacity,
					CProcess::getSimulator()->getClock());
		if (currentUnits > capacity) {
//			console.log("Resource Unit Number Mismatch!");
		}
		if (waitingLine.size() > 0) {
			WaitProcess * w = waitingLine.front();
			while (true) {
				if (w->requestUnits <= currentUnits) {
					currentUnits -= w->requestUnits;
					waitingLine.dequeue();
					servedLine.push_back(w->p);
					CProcess::activateNow(w->p);
					delete w;
					waitingAccumulate->update((double)waitingLine.size(),
								CProcess::getSimulator()->getClock());
					workingAccumulate->update((double)(capacity - currentUnits) / capacity,
								CProcess::getSimulator()->getClock());
				} else {
					return;
				}
				if (currentUnits == 0) {
					return;
				}
				if (waitingLine.size() == 0) {
					return;
				}
				w = waitingLine.front();
			}
		}
	}

	int getCapacity(){	return capacity; };
	Accumulate * getWaitingAccumulate(){	return waitingAccumulate;	};
	Accumulate * getWorkingAccumulate(){	return workingAccumulate;	};
};

}
#endif /* CPROCESS_H_ */
