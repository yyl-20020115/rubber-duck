/**
 * @file Queue_3P.h
 * @brief 单通道排队系统三段扫描仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#ifndef QUEUE_3P_H_
#define QUEUE_3P_H_

#include "EventNotice.h"

using namespace rubber_duck;

//开始服务事件
class StartEvent:public EventNotice{
public:
	StartEvent();
	virtual bool canTrigger(Simulator * pSimulator);
	virtual void trigger(Simulator * pSimulator);
};
//到达事件
class ArrivalEvent:public EventNotice{
public:
	ArrivalEvent(double time);
	virtual void trigger(Simulator * pSimulator);
};
//离开事件
class DepartureEvent:public EventNotice{
public:
	DepartureEvent(double time);
	virtual void trigger(Simulator * pSimulator);
};

#endif /* QUEUE_3P_H_ */
