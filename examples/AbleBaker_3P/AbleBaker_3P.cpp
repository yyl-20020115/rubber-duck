/**
 * @file AbleBaker_3P.cpp
 * @brief Able、Baker呼叫中心技术服务系统三段扫描仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#include <vector>
#include <stdio.h>
#include "DataCollection.h"
#include "AbleBaker_3P.h"
#include "Queue.h"

using namespace std;
using namespace rubber_duck;

//输入
int		ArrivalInterval[]		= {1,2,3,4};
double	ArrivalIntervalProb[]	= {0.25,0.40,0.20,0.15};
int		AbleServiceTime[]		= {2,3,4,5};
double	AbleServiceTimeProb[]	= {0.30,0.28,0.25,0.17};
int		BakerServiceTime[]		= {3,4,5,6};
double	BakerServiceTimeProb[]	= {0.35,0.25,0.20,0.20};

CDFDiscreteTable * pArrivalIntervalCDF = makeDiscreteCDFTable(4,ArrivalInterval,ArrivalIntervalProb);
CDFDiscreteTable * pAbleServiceTimeCDF = makeDiscreteCDFTable(4,AbleServiceTime,AbleServiceTimeProb);
CDFDiscreteTable * pBakerServiceTimeCDF = makeDiscreteCDFTable(4,BakerServiceTime,BakerServiceTimeProb);

//统计量
int		TotalCallerEntered		= 0; //已经进入系统的Caller数
int		TotalCallerDeparted		= 0; //已经完成服务离开的Caller数
int		LongServiceCount		= 0; //等待时间超过4分钟的顾客数

Tally timeInSystemTally("TIME IN SYSTEM");
Accumulate queueLengthAccum("QUEUE LENGTH"),ableBusyAccum("ABLE UTILIZATION"),bakerBusyAccum("BAKER UTILIZATION");

//实体：Caller，仅考察到达时间属性
struct Caller{
	int callerID;//属性：Caller ID
	double arrivalTime;//属性：到达时间
};

//实体集合
Queue<Caller*> CallerQueue;//正在排队的Caller队列，按到达时间排序
//事件名称缓冲区
char nameBuffer[128];

//状态变量
Caller*	AbleCaller = NULL;	//Able是否正在服务: NULL表示空闲，即LS_A(t)==0，否则指向正在服务的Caller实体指针
Caller* BakerCaller = NULL;	//Baker是否正在服务:NULL表示空闲，即LS_B(t)==0，否则指向正在服务的Caller实体指针

//Able服务开始事件
AbleStartEvent::AbleStartEvent():EventNotice(-1) {
	setName("Able服务开始事件");
}

bool AbleStartEvent::canTrigger(Simulator * pSimulator){
	if(AbleCaller != NULL){
		return false;
	}
	//如果队列还有Caller，为其服务
	if(CallerQueue.getCount() == 0){
		return false;
	}
	return true;
}

void AbleStartEvent::trigger(Simulator * pSimulator){
	AbleCaller = CallerQueue.dequeue();
	queueLengthAccum.update(CallerQueue.getCount(),pSimulator->getClock());
	//计算其服务时间
	const int serviceTime = pSimulator->getRandom()->nextDiscrete(pAbleServiceTimeCDF);

	//调度其服务完成事件
	AbleCompleteEvent* pEvent = new AbleCompleteEvent(pSimulator->getClock() + serviceTime);
	sprintf(nameBuffer,"Able服务顾客%d完成事件",AbleCaller->callerID);
	pEvent->setName(nameBuffer);
	pSimulator->scheduleEvent(pEvent);
	pSimulator->print("CLOCK=%3d:\tAble开始为Caller %d 提供服务，结束时刻为%g\n",(int)pSimulator->getClock(),AbleCaller->callerID,pEvent->getTime());
	ableBusyAccum.update(1,pSimulator->getClock());
}

//Able服务完成事件
AbleCompleteEvent::AbleCompleteEvent(double time):EventNotice(time) {
}

void AbleCompleteEvent::trigger(Simulator * pSimulator){
	//离开时刻数据采集
	const double timeInSystem = pSimulator->getClock() - AbleCaller->arrivalTime;//Caller的停留时间
	if (timeInSystem > 4) LongServiceCount++; //长时顾客
	timeInSystemTally.update(timeInSystem,pSimulator->getClock());//更新总停留时间
	TotalCallerDeparted ++;//离开Caller数增加

	pSimulator->print("CLOCK=%3d:\tAbleComplete, Caller %d, 停留时间:%g\n",(int)pSimulator->getClock(),AbleCaller->callerID,timeInSystem);

	//顾客离开系统
	delete AbleCaller;

	AbleCaller = NULL;

	ableBusyAccum.update(0,pSimulator->getClock());

	pSimulator->scheduleConditionalEvent(new AbleStartEvent());

	if(TotalCallerDeparted > 100){
		pSimulator->stop();
	}
}
//Baker开始服务事件
BakerStartEvent::BakerStartEvent():EventNotice(-1) {
	setName("Baker服务开始事件");
}

bool BakerStartEvent::canTrigger(Simulator * pSimulator){
	if(AbleCaller == NULL){
		return false;
	}
	if(BakerCaller != NULL){
		return false;
	}
	//如果队列还有Caller，为其服务
	if(CallerQueue.getCount() == 0){
		return false;
	}
	return true;
}

void BakerStartEvent::trigger(Simulator * pSimulator){
	BakerCaller = CallerQueue.dequeue();
	queueLengthAccum.update(CallerQueue.getCount(),pSimulator->getClock());
	//计算其服务时间u
	const int serviceTime = pSimulator->getRandom()->nextDiscrete(pBakerServiceTimeCDF);

	//调度其服务完成事件
	BakerCompleteEvent* depart = new BakerCompleteEvent(pSimulator->getClock() + serviceTime);
	sprintf(nameBuffer,"Baker服务顾客%d完成事件",BakerCaller->callerID);
	depart->setName(nameBuffer);
	pSimulator->scheduleEvent(depart);
	bakerBusyAccum.update(1,pSimulator->getClock());
	pSimulator->print("CLOCK=%3d:\tBaker开始为Caller %3d 提供服务，结束时刻为%g\n",(int)pSimulator->getClock(),BakerCaller->callerID,depart->getTime());
}
//Baker服务完成事件
BakerCompleteEvent::BakerCompleteEvent(double time):EventNotice(time) {
}

void BakerCompleteEvent::trigger(Simulator * pSimulator){
	//离开时刻数据采集
	const double timeInSystem = pSimulator->getClock() - BakerCaller->arrivalTime;//Caller的停留时间
	if (timeInSystem > 4) LongServiceCount++; //长时顾客
	timeInSystemTally.update(timeInSystem,pSimulator->getClock());//更新总停留时间
	TotalCallerDeparted ++;//离开Caller数增加

	pSimulator->print("CLOCK=%3d:\tBakerComplete, Caller %d, 停留时间:%g\n",(int)pSimulator->getClock(),BakerCaller->callerID,timeInSystem);

	//顾客离开系统
	delete BakerCaller;
	BakerCaller = NULL;

	bakerBusyAccum.update(0,pSimulator->getClock());
	pSimulator->scheduleConditionalEvent(new BakerStartEvent());

	if(TotalCallerDeparted > 100){
		pSimulator->stop();
	}

}
//Caller到达事件
ArrivalEvent::ArrivalEvent(double time):EventNotice(time) {
}

void ArrivalEvent::trigger(Simulator * pSimulator){
	//首先创建Caller自己
	Caller* pCurrentCaller = new Caller();
	pCurrentCaller->callerID = ++TotalCallerEntered;
	pCurrentCaller->arrivalTime = pSimulator->getClock();
	pSimulator->print("CLOCK=%3d:\tCaller %3d 到达\n",(int)pSimulator->getClock(),pCurrentCaller->callerID);

	//其次调度下一Caller的到达事件
	const int nextArrivalInterval = pSimulator->getRandom()->nextDiscrete(pArrivalIntervalCDF);
	ArrivalEvent* pEvent = new ArrivalEvent(pSimulator->getClock() + nextArrivalInterval);
	sprintf(nameBuffer,"顾客%d到达事件",TotalCallerEntered + 1);
	pEvent->setName(nameBuffer);
	pSimulator->scheduleEvent(pEvent);

	pSimulator->print("CLOCK=%3d:\tCaller %3d 进入队列\n",(int)pSimulator->getClock(),pCurrentCaller->callerID);
	CallerQueue.enqueue(pCurrentCaller);

	queueLengthAccum.update(CallerQueue.getCount(),pSimulator->getClock());
}

//这里如果要算Able的利用率，有两个办法：
//办法1：每个顾客记一下开始服务时间
//办法2：记录Able的最近一次开始服务时间
void PrintReport(Simulator * pSimulator) {
	const double PC4   = ((double)LongServiceCount)/TotalCallerDeparted;

	pSimulator->print( "Able/Baker技术服务中心仿真 - 事件调度法\n");
	pSimulator->print( "\t仿真时长(分钟)            %f\n",pSimulator->getClock());
	pSimulator->print( "\t总进入系统Caller数        %d\n",TotalCallerEntered );
	pSimulator->print( "\t已完成Caller数	          %d\n",TotalCallerDeparted );
	pSimulator->print( "\t最大队长                 %f\n",queueLengthAccum.max());
	pSimulator->print( "\t平均停留时间(分钟)        %f\n",timeInSystemTally.mean());
	pSimulator->print( "\t停留超过4分钟顾客比例      %f\n",PC4 );

	DataCollection::printHeading();
	timeInSystemTally.report();
	queueLengthAccum.report();
	ableBusyAccum.report();
	bakerBusyAccum.report();
	DataCollection::printEnding();
}

void Initialization(Simulator * pSimulator){
	ableBusyAccum.update(0,pSimulator->getClock());
	bakerBusyAccum.update(0,pSimulator->getClock());
	queueLengthAccum.update(0,pSimulator->getClock());
	//第一个Caller到达事件
	ArrivalEvent* pEvent = new ArrivalEvent(pSimulator->getClock() + pSimulator->getRandom()->nextDiscrete(pArrivalIntervalCDF));
	sprintf(nameBuffer,"顾客%d到达事件",TotalCallerEntered + 1);
	pEvent->setName(nameBuffer);
	pSimulator->scheduleEvent(pEvent);
	pSimulator->scheduleConditionalEvent(new AbleStartEvent());
	pSimulator->scheduleConditionalEvent(new BakerStartEvent());
}

int main(int argc, char* argv[]){
	Simulator * pSimulator = new Simulator(12345678,"ablebaker_3p.txt");
	Initialization(pSimulator);
	pSimulator->setDebug();
	pSimulator->run();
	PrintReport(pSimulator);
	return 0;
}
