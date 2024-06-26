/**
 * @file AbleBaker_PI.cpp
 * @brief Able、Baker呼叫中心技术服务系统进程交互仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#include <vector>
#include <stdio.h>
#include "DataCollection.h"
#include "ProcessNotice.h"
#include "Simulator.h"
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

class Caller;

//实体集合
Queue<Caller*> CallerQueue;//正在排队的Caller队列，按到达时间排序

//状态变量
Caller*	AbleCaller = NULL;	//Able是否正在服务: NULL表示空闲，即LS_A(t)==0，否则指向正在服务的Caller实体指针
Caller* BakerCaller = NULL;	//Baker是否正在服务:NULL表示空闲，即LS_B(t)==0，否则指向正在服务的Caller实体指针
//事件名称缓冲区
char nameBuffer[128];

#define START			1	//服务开始复活点
#define COMPLETE		2	//服务完成复活点
#define ARRIVAL			3	//到达复活点

//呼叫进程
class Caller:public ProcessNotice{
public:
	int callerID;//属性：Caller ID
	float arrivalTime;//属性：到达时间
	//time复活时间
	Caller(double time):ProcessNotice(time,ARRIVAL) {
		arrivalTime = (float)time;
		callerID = ++TotalCallerEntered;
		sprintf(nameBuffer,"顾客%d",callerID);
		setName(nameBuffer);
	};
	//获得当前复活点名称
	virtual const char * getPhaseName(){
		switch(phase){
			case ARRIVAL: //到达复活点
				return "到达";
			case START://开始服务复活点
				return "开始服务";
			case COMPLETE://服务完成复活点
				return "服务完成";
		}
		return NULL;
	};
	//进程推进函数，进程复活后的推进函数，返回复活时间
	virtual double runToBlocked(Simulator * pSimulator){
		switch(phase){
			case ARRIVAL: //到达复活点
				return arrival(pSimulator);
			case START://开始服务复活点
				return start(pSimulator);
			case COMPLETE://服务完成复活点
				return departure(pSimulator);
		}
		return 0;
	};

	//[到达]复活点进程推进
	double arrival(Simulator * pSimulator){
		arrivalTime = (float)pSimulator->getClock();
		pSimulator->print("CLOCK=%3d:\tCaller %3d 到达\n",(int)pSimulator->getClock(),callerID);

		//创建下一个Caller，设定其激活时间为到达时间，该Caller进入BEL
		const int nextArrivalInterval = pSimulator->getRandom()->nextDiscrete(pArrivalIntervalCDF);
		Caller* nextCaller = new Caller(pSimulator->getClock() + nextArrivalInterval);
		pSimulator->activate(nextCaller);

		pSimulator->print("CLOCK=%3d:\tCaller %3d 进入队列\n",(int)pSimulator->getClock(),callerID);
		CallerQueue.enqueue(this);

		queueLengthAccum.update(CallerQueue.getCount(),pSimulator->getClock());
		//进入条件延迟，下一复活点为开始服务
		phase = START;
		return -1;
	}

	//[开始服务]复活点进程推进
	double start(Simulator * pSimulator){
		CallerQueue.dequeue();
		queueLengthAccum.update(CallerQueue.getCount(),pSimulator->getClock());
		//进入无条件延迟，复活点为服务结束
		phase = COMPLETE;
		int serviceTime = 0;
		double departTime = 0;
		if(AbleCaller == NULL){
			AbleCaller = this;
			//计算其服务时间
			serviceTime = pSimulator->getRandom()->nextDiscrete(pAbleServiceTimeCDF);
			departTime = pSimulator->getClock() + serviceTime;
			ableBusyAccum.update(1,pSimulator->getClock());
			pSimulator->print("CLOCK=%3d:\tAble开始为Caller %d 提供服务，结束时刻为%g\n",(int)pSimulator->getClock(),AbleCaller->callerID,departTime);
		}else{
			if(BakerCaller == NULL){
				BakerCaller = this;
				//计算其服务时间
				serviceTime = pSimulator->getRandom()->nextDiscrete(pBakerServiceTimeCDF);
				departTime = pSimulator->getClock() + serviceTime;				
				bakerBusyAccum.update(1,pSimulator->getClock());
				pSimulator->print("CLOCK=%3d:\tBaker开始为Caller %3d 提供服务，结束时刻为%g\n",(int)pSimulator->getClock(),BakerCaller->callerID,departTime);
			}
		}
		return departTime;
	}
	//[服务结束]复活点进程推进
	double departure(Simulator * pSimulator){
		//离开时刻数据采集
		const double timeInSystem = pSimulator->getClock() - arrivalTime;//Caller的停留时间
		if (timeInSystem > 4) LongServiceCount++; //长时顾客
		timeInSystemTally.update(timeInSystem,pSimulator->getClock());//更新总停留时间
		TotalCallerDeparted++;//离开Caller数增加

		if(BakerCaller == this){
			pSimulator->print("CLOCK=%3d:\tBakerComplete, Caller %d, 停留时间:%g\n",(int)pSimulator->getClock(),BakerCaller->callerID,timeInSystem);
			BakerCaller = NULL;
			bakerBusyAccum.update(0,pSimulator->getClock());
		}
		if(AbleCaller == this){
			pSimulator->print("CLOCK=%3d:\tAbleComplete, Caller %d, 停留时间:%g\n",(int)pSimulator->getClock(),AbleCaller->callerID,timeInSystem);
			AbleCaller = NULL;
			ableBusyAccum.update(0,pSimulator->getClock());
		}

		if(TotalCallerDeparted > 100){
			pSimulator->stop();
		}
		//进程结束
		terminated = true;
		return -1;
	}
	//如果进程处于条件延迟状态，确定当前进程延迟条件是否满足
	virtual bool isConditionalBlocking(Simulator * pSimulator){
		//判断是否处于开始服务条件延迟
		if(phase == START){
			if(AbleCaller != NULL && BakerCaller != NULL){
				return true;
			}
			return false;
		}
		return false;
	};
};

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
	Caller* caller = new Caller(pSimulator->getClock() + pSimulator->getRandom()->nextDiscrete(pArrivalIntervalCDF));
	pSimulator->activate(caller);
}

int main(int argc, char* argv[]){
	Simulator * pSimulator = new Simulator(12345678,"ablebaker_pi.txt");
	Initialization(pSimulator);
	pSimulator->setDebug();
	pSimulator->run();
	PrintReport(pSimulator);

	return 0;
}
