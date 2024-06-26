/**
 * @file DumpTruck_PI.cpp
 * @brief 卡车卸货转运系统进程交互仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#include <stdarg.h>
#include "vector"
#include "ProcessNotice.h"
#include "Simulator.h"
#include "Queue.h"

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

class Truck;

//实体集合
Queue<Truck*> LoaderQueue;	//正在排队装载的卡车，按到达时间排序
Queue<Truck*> ScaleQueue;	//正在排队称重的卡车，按到达时间排序

//状态变量
int Lt;	//正在装载的卡车数量 0,1,2
int Wt; //正在称重的卡车数量 0,1

//复活点声明
#define BEGINLOADING	0	//开始装载复活点
#define ENDLOADING		1	//装载完成复活点
#define BEGINWEIGHING	2	//开始称重复活点
#define ENDWEIGHING		3	//称重完成复活点
#define ENDTRAVEL		4	//转运完成复活点

//事件名称缓冲区
char nameBuffer[128];

//卡车进程
class Truck:public ProcessNotice{
public:
	int truckID;//卡车标识
	double beginLoadTime;//开始装载时间
	double beginWeighTime;//开始称重时间
	//ID卡车标识，time复活时间和p复活点
	Truck(int ID,double time,int p):ProcessNotice(time,p){
		truckID = ID;
		beginWeighTime = 0.;
		beginLoadTime = 0.;
		sprintf(nameBuffer,"卡车进程%d",truckID);
		setName(nameBuffer);
	}
	//获得当前复活点名称
	virtual const char * getPhaseName(){
		switch(phase){
			case BEGINLOADING: //开始装载
				return "开始装载";
			case ENDLOADING://装载结束
				return "装载结束";
			case BEGINWEIGHING://开始称重
				return "开始称重";
			case ENDWEIGHING://称重结束
				return "称重结束";
			case ENDTRAVEL://转运结束
				return "转运结束";
		}
		return NULL;
	};

	//进程推进函数，进程复活后的推进函数，返回复活时间
	virtual double runToBlocked(Simulator * pSimulator){
		switch(phase){
			case BEGINLOADING: //开始装载
				return beginLoad(pSimulator);
			case ENDLOADING://装载结束
				return endLoad(pSimulator);
			case BEGINWEIGHING://开始称重
				return beginWeigh(pSimulator);
			case ENDWEIGHING://称重结束
				return endWeigh(pSimulator);
			case ENDTRAVEL://转运结束
				return endTravel(pSimulator);
		}
		return 0;
	};
	//[开始装载]复活点进程推进
	double beginLoad(Simulator * pSimulator){
		LoaderQueue.dequeue();
		beginLoadTime = pSimulator->getClock();
		//占用一个Loader
		Lt++;
		//调度其结束装载事件
		const double loadTime = pSimulator->getRandom()->nextDiscrete(pLoadingTimeCDF);
		pSimulator->print("CLOCK=%3d:\tTruck %d 开始装载，结束时间为%d\n",(int)pSimulator->getClock(),truckID,(int)(pSimulator->getClock() + loadTime));
		//进入无条件延迟，下一复活点为结束装载
		phase = ENDLOADING;
		return pSimulator->getClock() + loadTime;
	}
	//[结束装载]复活点进程推进
	double endLoad(Simulator * pSimulator){
		pSimulator->print("CLOCK=%3d:\t完成了对Truck %3d 的装载\n",(int)pSimulator->getClock(),truckID);

		//进入称重队列
		ScaleQueue.enqueue(this);
		MWQ = max(MWQ,ScaleQueue.getCount());
		//释放一个装载车
		Lt--;

		//计算装载时间
		BL += pSimulator->getClock() - beginLoadTime;
		//进入条件延迟，下一复活点为开始称重
		phase = BEGINWEIGHING;
		return -1;
	}
	//[开始称重]复活点进程推进
	double beginWeigh(Simulator * pSimulator){
		//占用Scale
		Wt = 1;

		//队首卡车
		ScaleQueue.dequeue();
		beginWeighTime = pSimulator->getClock();

		//调度其结束称重事件
		const double weighTime = pSimulator->getRandom()->nextDiscrete(pWeighingTimeCDF);
		pSimulator->print("CLOCK=%3d:\tTruck %d 开始称重，结束时间为%d\n",(int)pSimulator->getClock(),truckID,(int)(pSimulator->getClock() + weighTime));
		//进入无条件延迟，下一复活点为结束称重
		phase = ENDWEIGHING;
		return pSimulator->getClock() + weighTime;
	}
	//[结束称重]复活点进程推进
	double endWeigh(Simulator * pSimulator){
		//称重台空闲
		Wt = 0;

		//计算称重时间
		BS += pSimulator->getClock() - beginWeighTime;

		//调度结束转运事件
		const double travelTime = pSimulator->getRandom()->nextDiscrete(pTravelTimeCDF);

		pSimulator->print("CLOCK=%3d:\t完成了对Truck %d 的称重,开始转运，转运结束时间为%d\n",(int)pSimulator->getClock(),
			truckID,(int)(pSimulator->getClock() + travelTime));
		//进入无条件延迟，下一复活点为结束转运
		phase = ENDTRAVEL;
		return pSimulator->getClock() + travelTime;
	}
	//[结束转运]复活点进程推进
	double endTravel(Simulator * pSimulator){
		//卡车进入装载队列
		LoaderQueue.enqueue(this);
		MLQ = max(MLQ,LoaderQueue.getCount());
		pSimulator->print("CLOCK=%3d:\tTruck %d完成了转运\n",(int)pSimulator->getClock(),truckID);
		//进入条件延迟，下一复活点为开始装载
		phase = BEGINLOADING;
		return -1;
	}

	//如果进程处于条件延迟状态，确定当前进程延迟条件是否满足
	virtual bool isConditionalBlocking(Simulator * pSimulator){
		//判断处于哪个条件延迟，确定是否需要延迟锁定
		//条件延迟：开始装载
		if(phase == BEGINLOADING){
			if (LoaderQueue.getCount() > 0 && Lt < 2){
				if(LoaderQueue.front() == this){
					return false;
				}
			}
			return true;
		}
		//条件延迟：开始称重
		if(phase == BEGINWEIGHING){
			if (ScaleQueue.getCount()>0 && Wt == 0){
				if(ScaleQueue.front() == this){
					return false;
				}
			}
			return true;
		}
		return true;
	};
};

void PrintReport(Simulator * pSimulator) {
	const double loaderUtil  = BL/2/StopSimulationTime;
	const double scaleUtil   = BS/StopSimulationTime;

	pSimulator->print( "六辆卡车转运煤炭仿真 - 进程交互法\n");
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
	Truck* truck1 = new Truck(1,pSimulator->getClock() + pSimulator->getRandom()->nextDiscrete(pWeighingTimeCDF),ENDWEIGHING);
	pSimulator->activate( truck1 );
	//称重台占用
	Wt = 1;

	//第2辆卡车
	Truck* truck2 = new Truck(2,pSimulator->getClock()  + pSimulator->getRandom()->nextDiscrete(pLoadingTimeCDF),ENDLOADING);
	pSimulator->activate( truck2 );

	//第3辆卡车
	Truck* truck3 = new Truck(3,pSimulator->getClock() + pSimulator->getRandom()->nextDiscrete(pLoadingTimeCDF),ENDLOADING);
	pSimulator->activate( truck3 );

	//装载车占用
	Lt = 2;

	//第4,5,6辆卡车
	Truck* truck = new Truck(4,-1,BEGINLOADING);
	pSimulator->await(truck);
	LoaderQueue.enqueue(truck);
	truck = new Truck(5,-1,BEGINLOADING);
	pSimulator->await(truck);
	LoaderQueue.enqueue(truck);
	truck = new Truck(6,-1,BEGINLOADING);
	pSimulator->await(truck);
	LoaderQueue.enqueue(truck);

	//称重队长
	MWQ = 0;

	//装载队长
	MLQ = LoaderQueue.getCount();
}

int main(int argc, char* argv[]){
	Simulator * pSimulator = new Simulator(1234567,"dumptruck_pi.txt");
	Initialization(pSimulator);
	pSimulator->setDebug();
	pSimulator->run(StopSimulationTime);
	PrintReport(pSimulator);
	return 0;
}

