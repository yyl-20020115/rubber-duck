/**
 * @file AbleBaker_3P.h
 * @brief Able、Baker呼叫中心技术服务系统三段扫描仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#ifndef ABLEBAKER_3P_H_
#define ABLEBAKER_3P_H_

#include "EventNotice.h"
#include "Simulator.h"

using namespace rubber_duck;

//Able服务开始事件
class AbleStartEvent:public EventNotice{
public:
	AbleStartEvent();
	virtual bool canTrigger(Simulator * pSimulator);
	virtual void trigger(Simulator * pSimulator);
};
//Able服务完成事件
class AbleCompleteEvent:public EventNotice{
public:
	AbleCompleteEvent(double time);
	virtual void trigger(Simulator * pSimulator);
};
//Baker服务开始事件
class BakerStartEvent:public EventNotice{
public:
	BakerStartEvent();
	virtual bool canTrigger(Simulator * pSimulator);
	virtual void trigger(Simulator * pSimulator);
};
//Baker服务完成事件
class BakerCompleteEvent:public EventNotice{
public:
	BakerCompleteEvent(double time);
	virtual void trigger(Simulator * pSimulator);
};
//Caller到达事件
class ArrivalEvent:public EventNotice{
public:
	ArrivalEvent(double time);
	virtual void trigger(Simulator * pSimulator);
};

#endif /* ABLEBAKER_3P_H_ */
