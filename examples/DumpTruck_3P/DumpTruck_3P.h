/**
 * @file DumpTruck_3P.h
 * @brief 卡车卸货转运系统三段扫描仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#ifndef DUMPTRUCK_3P_H_
#define DUMPTRUCK_3P_H_

#include "EventNotice.h"
#include "Simulator.h"

using namespace rubber_duck;

//实体：Truck
struct Truck {
	int truckID;
	double beginLoadTime;
	double beginWeighTime;
	Truck(int ID)
	{
		truckID = ID;
		beginWeighTime = 0.;
		beginLoadTime = 0.;
	}
};

//开始装载事件
class BeginLoadEvent:public EventNotice{
public:
	BeginLoadEvent();
	virtual bool canTrigger(Simulator * pSimulator);
	virtual void trigger(Simulator * pSimulator);
};
//装载完成事件
class EndLoadEvent:public EventNotice{
public:
	EndLoadEvent(double time,Truck * truck);
	virtual void trigger(Simulator * pSimulator);
};
//开始称重事件
class BeginWeighEvent:public EventNotice{
public:
	BeginWeighEvent();
	virtual bool canTrigger(Simulator * pSimulator);
	virtual void trigger(Simulator * pSimulator);
};
//称重完成事件
class EndWeighEvent:public EventNotice{
public:
	EndWeighEvent(double time,Truck * truck);
	virtual void trigger(Simulator * pSimulator);
};
//转运完成事件
class EndTravelEvent:public EventNotice{
public:
	EndTravelEvent(double time,Truck * truck);
	virtual void trigger(Simulator * pSimulator);
};

#endif /* DUMPTRUCK_3P_H_ */
