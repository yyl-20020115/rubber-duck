/**
 * @file Philosopher_3P.h
 * @brief 哲学家就餐问题三段扫描仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#ifndef PHILOSPHER_3P_H_
#define PHILOSPHER_3P_H_

#include "EventNotice.h"
#include "Simulator.h"

using namespace rubber_duck;

typedef struct{
	int id;
	int state;
	double awaitStartTime;
	double thinkTime;
	double awaitTime;
}Philosopher;

//开始就餐事件
class StartEatEvent:public EventNotice{
public:
	StartEatEvent(Philosopher * pPhilosopher);
	virtual bool canTrigger(Simulator * pSimulator);
	virtual void trigger(Simulator * pSimulator);
};
//思考完成事件
class ThinkCompleteEvent:public EventNotice{
public:
	ThinkCompleteEvent(Philosopher * pPhilosopher,double time);
	virtual void trigger(Simulator * pSimulator);
};
//就餐结束事件
class EatCompleteEvent:public EventNotice{
public:
	EatCompleteEvent(Philosopher * pPhilosopher,double time);
	virtual void trigger(Simulator * pSimulator);
};

#endif /* PHILOSPHER_3P_H_ */
