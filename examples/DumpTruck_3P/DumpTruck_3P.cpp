/**
 * @file DumpTruck_3P.cpp
 * @brief 卡车拉煤转运系统三段扫描仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-23
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#include <stdarg.h>
#include "vector"
#include "EventNotice.h"
#include "Queue.h"
#include "DumpTruck_3P.h"

using namespace std;
using namespace rubber_duck;

//输入
int		LoadingTime[3]		= {5,10,15};
double	LoadingTimeProb[3]	= {0.3f,0.5f,0.2f};
int		WeighingTime[2]		= {12,16};
double	WeighingTimeProb[2]	= {0.7f,0.3f};
int		TravelTime[4]		= {40,60,80,100};
double	TravelTimeProb[4]	= {0.4f,0.3f,0.2f,0.1f};
const double	StopSimulationTime	= 100;

CDFDiscreteTable * pLoadingTimeCDF = makeDiscreteCDFTable(3,LoadingTime,LoadingTimeProb);
CDFDiscreteTable * pWeighingTimeCDF = makeDiscreteCDFTable(2,WeighingTime,WeighingTimeProb);
CDFDiscreteTable * pTravelTimeCDF = makeDiscreteCDFTable(4,TravelTime,TravelTimeProb);

//统计量
double	BL		= 0; //装载车总工作时间
double	BS		= 0; //称重台总工作时间
int     MLQ		= 0; //最大装载队长
int     MWQ		= 0; //最大称重队长

//实体集合
Queue<Truck*> LoaderQueue;	//正在排队装载的卡车，按到达时间排序
Queue<Truck*> ScaleQueue;	//正在排队称重的卡车，按到达时间排序

//状态变量
int Lt;	//正在装载的卡车数量 0,1,2
int Wt; //正在称重的卡车数量 0,1

//事件名称缓冲区
char nameBuffer[128];

//开始装载(条件事件)
BeginLoadEvent::BeginLoadEvent():EventNotice(-1) {
	setName("开始装载事件");
	reserved = true;
}

bool BeginLoadEvent::canTrigger(Simulator * pSimulator) {
	if (LoaderQueue.getCount() > 0 && Lt < 2){
		return true;
	}
	return false;
}

void BeginLoadEvent::trigger(Simulator * pSimulator) {
	//队首Truck
	Truck* loaderTruck = LoaderQueue.dequeue();
	loaderTruck->beginLoadTime = pSimulator->getClock();

	//占用一个Loader
	Lt++;

	//调度其结束装载事件
	const double LoadTime = pSimulator->getRandom()->nextDiscrete(pLoadingTimeCDF);
	EndLoadEvent* pEvent = new EndLoadEvent(pSimulator->getClock() + LoadTime,loaderTruck);
	sprintf(nameBuffer,"卡车%d装载结束事件",loaderTruck->truckID);
	pEvent->setName(nameBuffer);
	pSimulator->scheduleEvent(pEvent);

	pSimulator->print("CLOCK=%3d:\tTruck %d 开始装载，结束时间为%d\n",(int)pSimulator->getClock(),loaderTruck->truckID,(int)pEvent->getTime());
	pSimulator->scheduleConditionalEvent(this);
}
//结束装载(确定事件)
EndLoadEvent::EndLoadEvent(double time,Truck * truck):EventNotice(time) {
	owner = truck;
}

void EndLoadEvent::trigger(Simulator * pSimulator) {
	Truck * pTruck = (Truck *)owner;
	pSimulator->print("CLOCK=%3d:\t完成了对Truck %3d 的装载\n",(int)pSimulator->getClock(),pTruck->truckID);

	//进入称重队列
	ScaleQueue.enqueue(pTruck);
	MWQ = max(MWQ,ScaleQueue.getCount());
	//释放一个装载车
	Lt--;

	//计算装载时间
	BL += pSimulator->getClock() - pTruck->beginLoadTime;
}

//开始称重(条件事件)
BeginWeighEvent::BeginWeighEvent():EventNotice(-1) {
	setName("开始称重事件");
	reserved = true;
}

bool BeginWeighEvent::canTrigger(Simulator * pSimulator) {
	if (ScaleQueue.getCount()>0 && Wt == 0){
		return true;
	}
	return false;
}

void BeginWeighEvent::trigger(Simulator * pSimulator) {
	//占用Scale
	Wt = 1;

	//队首卡车
	Truck* weighingTruck = ScaleQueue.dequeue();
	weighingTruck->beginWeighTime = pSimulator->getClock();

	//调度其结束称重事件
	const double weighTime = pSimulator->getRandom()->nextDiscrete(pWeighingTimeCDF);
	EndWeighEvent* pEvent = new EndWeighEvent(pSimulator->getClock() + weighTime,weighingTruck);
	sprintf(nameBuffer,"卡车%d称重结束事件",weighingTruck->truckID);
	pEvent->setName(nameBuffer);
	pSimulator->scheduleEvent(pEvent);
	pSimulator->print("CLOCK=%3d:\tTruck %d 开始称重，结束时间为%d\n",(int)pSimulator->getClock(),weighingTruck->truckID,(int)(pEvent->getTime()));
	pSimulator->scheduleConditionalEvent(this);
}

//结束称重(确定事件)=开始转运
EndWeighEvent::EndWeighEvent(double time,Truck * truck):EventNotice(time) {
	owner = truck;
}

void EndWeighEvent::trigger(Simulator * pSimulator) {
	Truck * pTruck = (Truck *)owner;
	//调度结束转运事件
	const double travelTime = pSimulator->getRandom()->nextDiscrete(pTravelTimeCDF);
	EndTravelEvent* pEvent = new EndTravelEvent(pSimulator->getClock() + travelTime,pTruck);
	sprintf(nameBuffer,"卡车%d转运结束事件",pTruck->truckID);
	pEvent->setName(nameBuffer);
	pSimulator->scheduleEvent(pEvent);
	//称重台空闲
	Wt = 0;

	//计算称重时间
	BS += pSimulator->getClock() - pTruck->beginWeighTime;

	pSimulator->print("CLOCK=%3d:\t完成了对Truck %d 的称重,开始转运，转运结束时间为%d\n",(int)pSimulator->getClock(),
		pTruck->truckID,(int)pEvent->getTime());
}

//结束转运(确定事件)
EndTravelEvent::EndTravelEvent(double time,Truck * truck):EventNotice(time) {
	owner = truck;
}

void EndTravelEvent::trigger(Simulator * pSimulator) {
	Truck * pTruck = (Truck *)owner;
	//卡车进入装载队列
	LoaderQueue.enqueue(pTruck);
	MLQ = max(MLQ,LoaderQueue.getCount());
	pSimulator->print("CLOCK=%3d:\tTruck %d完成了转运\n",(int)pSimulator->getClock(),pTruck->truckID);
}

void PrintReport(Simulator * pSimulator) {
	const double loaderUtil  = BL/2/StopSimulationTime;
	const double scaleUtil   = BS/StopSimulationTime;

	pSimulator->print( "六辆卡车转运煤炭仿真 - 三段扫描法\n");
	pSimulator->print( "\t仿真时长(分钟)      %f\n",pSimulator->getClock());
	pSimulator->print( "\t装载车工作时间      %f\n",BL);
	pSimulator->print( "\t装载车利用率        %f\n",loaderUtil);
	pSimulator->print( "\t最大装载队长        %d\n",MLQ );
	pSimulator->print( "\t称重台工作时间      %f\n",BS );
	pSimulator->print( "\t称重台利用率        %f\n",scaleUtil );
	pSimulator->print( "\t最大称重队长        %d\n",MWQ );
}

void Initialization(Simulator * pSimulator){
	//第1辆卡车
	Truck* truck1 = new Truck(1);
	EventNotice* pEvent = new EndWeighEvent(pSimulator->getClock() + pSimulator->getRandom()->nextDiscrete(pWeighingTimeCDF),truck1);
	sprintf(nameBuffer,"卡车%d称重结束事件",truck1->truckID);
	pEvent->setName(nameBuffer);
	pSimulator->scheduleEvent(pEvent);
	//称重台占用
	Wt = 1;

	//第2辆卡车
	Truck* truck2 = new Truck(2);
	pEvent = new EndLoadEvent(pSimulator->getClock()  + pSimulator->getRandom()->nextDiscrete(pLoadingTimeCDF),truck2);
	sprintf(nameBuffer,"卡车%d装载结束事件",truck2->truckID);
	pEvent->setName(nameBuffer);
	pSimulator->scheduleEvent(pEvent);

	//第3辆卡车
	Truck* truck3 = new Truck(3);
	pEvent = new EndLoadEvent(pSimulator->getClock() + pSimulator->getRandom()->nextDiscrete(pLoadingTimeCDF),truck3);
	sprintf(nameBuffer,"卡车%d装载结束事件",truck3->truckID);
	pEvent->setName(nameBuffer);
	pSimulator->scheduleEvent(pEvent);

	//装载车占用
	Lt = 2;

	//第4,5,6辆卡车
	LoaderQueue.enqueue(new Truck(4));
	LoaderQueue.enqueue(new Truck(5));
	LoaderQueue.enqueue(new Truck(6));

	//称重队长
	MWQ = 0;

	//装载队长
	MLQ = LoaderQueue.getCount();

	pSimulator->scheduleConditionalEvent(new BeginLoadEvent());
	pSimulator->scheduleConditionalEvent(new BeginWeighEvent());
}

int main(int argc, char* argv[]){
	Simulator * pSimulator = new Simulator(1234567,"dumptruck_3p.txt");
	Initialization(pSimulator);
	pSimulator->setDebug();
	pSimulator->run(StopSimulationTime);
	PrintReport(pSimulator);
	return 0;
}

