/**
 * @file Queue.cpp
 * @brief 单通道排队系统事件调度批量运行仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-01
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#include <queue>
#include <list>
#include <string>
#include <stdio.h>
#include "Random.h"

using namespace std;
using namespace rubber_duck;

/**
 * @brief 事件结构：Event，包含发生时间、事件类型和用户标识属性
 */
struct Event{
	int type;	 //事件类型
	double time; //发生时间
	int id;		 //用户标识
};

#define ARRIVAL 	1	//到达事件类型
#define DEPARTURE 	2 	//服务完成事件类型

/**
 * @brief 实体：Customer，仅考察到达时间和开始服务时间属性
 */
struct Customer{
	int customerID;//属性：CustomerID
	double arrivalTime;//属性：到达时间
	double startServiceTime;//属性：到达时间
};

/**
 * @brief 随机变量生成器
 */
Random * pStream;

/**
 * @brief 新顾客标识
 */
int customerID = 0;

/**
 * @brief 仿真时间
 */
double simClock = 0;

std::list<Event *> FutureEventList;
std::queue<Customer*> customers;

Customer * CustomerInService = NULL;

double MeanInterArrivalTime, MeanServiceTime, SIGMA;
double TotalBusy, MaxQueueLength, SumResponseTime;

long NumberOfCustomers, QueueLength, TotalCustomers, NumberOfDepartures, LongService;

/**
 * @brief 
 * @param  pEvent           新插入的事件
 */
void Enqueue(Event *pEvent){
	//前面有更大时间戳的事件，则插入其之前
	for (std::list<Event *>::iterator it = FutureEventList.begin(); it != FutureEventList.end(); it++){
		Event *pItEvent = *it;
		if (pEvent->time < pItEvent->time){
			FutureEventList.insert(it, pEvent);
			return;
		}
	}

	//否则插入末尾
	FutureEventList.push_back(pEvent);
}

/**
 * @brief 移出下一未来事件记录
 * @return Event* 下一未来事件记录
 */
Event *Dequeue(){
	if (FutureEventList.size() == 0)
		return NULL;
	Event *pEvent = *(FutureEventList.begin());		//第一个事件记录
	FutureEventList.erase(FutureEventList.begin()); //从列表中删除该记录
	return pEvent;
}

/**
 * @brief 删除清理未来事件表和顾客队列中的记录
 */
void Clear(){
	for (std::list<Event *>::iterator it = FutureEventList.begin(); it != FutureEventList.end(); it++){
		Event *pItEvent = *it;
		delete pItEvent;
	}
	FutureEventList.clear();
	while(!customers.empty()){
		Customer * pCustomer = customers.front();
		delete pCustomer;
		customers.pop();
	}
}
/**
 * @brief 初始化仿真模型参数、模型状态和统计变量
 * @param  meanInterArrivalTime 指数分布的平均到达间隔
 * @param  meanServiceTime  正态分布的服务时间均值
 * @param  sigma            正态分布的服务时间方差
 * @param  totalCustomers   控制仿真运行结束的离开顾客数量
 */
void Initialization(double meanInterArrivalTime,double meanServiceTime,double sigma,long totalCustomers){
	MeanInterArrivalTime = meanInterArrivalTime; MeanServiceTime = meanServiceTime;
	SIGMA                = sigma; TotalCustomers  = totalCustomers;
	TotalBusy = 0 ;
	MaxQueueLength = 0;
	SumResponseTime = 0;
	QueueLength = 0;
	CustomerInService = NULL;
	MaxQueueLength = 0;
	NumberOfDepartures = 0;
	LongService = 0;
	simClock = 0;
	Clear();
	pStream = new Random(123456);
	Event *pEvent = new Event();
	pEvent->time = simClock + pStream->nextExponential() * MeanInterArrivalTime;
	pEvent->type = ARRIVAL;
	pEvent->id = customerID;
	Enqueue(pEvent);
	customerID++;
}

void ScheduleDeparture();

/**
 * @brief 到达事件处理函数
 * @param  pArrivalEvent    发生的到达事件
 */
void ProcessArrival(Event *pArrivalEvent){
	//printf("ProcessArrival Event for CustomerID:%d\n",pArrivalEvent->id);
	QueueLength++;
	Customer * customer = new Customer();
	customer->customerID = pArrivalEvent->id;
	customer->arrivalTime = simClock;
	customer->startServiceTime = 0;
	customers.push(customer);
	// if the server is idle, fetch the event, do statistics
	// and put into service
	if (CustomerInService == NULL){
		ScheduleDeparture();
	}

	// adjust max queue length statistics
	if (MaxQueueLength < QueueLength){
		MaxQueueLength = QueueLength;
	}

	// schedule the next arrival
	Event *pEvent = new Event();
	pEvent->time = simClock + pStream->nextExponential() * MeanInterArrivalTime;
	pEvent->type = ARRIVAL;
	pEvent->id = customerID;
	customerID++;
	Enqueue(pEvent);
}

/**
 * @brief 调度生成离开事件
 */
void ScheduleDeparture(){
	CustomerInService = customers.front();
	customers.pop();
	CustomerInService->startServiceTime = simClock;
	Event * pEvent = new Event();
	pEvent->time = simClock + pStream->nextNormal(MeanServiceTime, SIGMA);
	pEvent->type = DEPARTURE;
	pEvent->id = CustomerInService->customerID;
	Enqueue(pEvent);
	QueueLength--;
}

/**
 * @brief 离开事件处理函数
 * @param  pDepartureEvent   发生的离开事件
 */
void ProcessDeparture(Event *pDepartureEvent){
	//printf("ProcessDeparture Event for CustomerID:%d\n",pDepartureEvent->id);
	NumberOfDepartures++;
	double response = (simClock - CustomerInService->arrivalTime);
	SumResponseTime += response;
	if( response > 4.0 ) LongService++;
	TotalBusy += (simClock - CustomerInService->startServiceTime);
	delete CustomerInService;
	// if there are customers in the queue then schedule
	// the departure of the next one
	if (QueueLength > 0){
		ScheduleDeparture();
	}
	else{
		CustomerInService = NULL;
	}
}

std::vector<std::string *> reports;

/**
 * @brief 仿真运行结束的统计结果报告
 */
void ReportGeneration(){
	double RHO   = TotalBusy/simClock;
	double AVGR  = SumResponseTime/TotalCustomers;
	double PC4   = ((double)LongService)/TotalCustomers;
	printf("SINGLE SERVER QUEUE SIMULATION - GROCERY STORE CHECKOUT COUNTER\n");
	printf( "\tMEAN INTERARRIVAL TIME                         %f\n",MeanInterArrivalTime );
	printf( "\tMEAN SERVICE TIME                              %f\n",MeanServiceTime );
	printf( "\tSTANDARD DEVIATION OF SERVICE TIMES            %f\n",SIGMA );
	printf( "\tNUMBER OF CUSTOMERS SERVED                     %ld\n",TotalCustomers );
	printf("\n");
	printf( "\tSERVER UTILIZATION                             %f\n",RHO );
	printf( "\tMAXIMUM LINE LENGTH                            %f\n",MaxQueueLength );
	printf( "\tAVERAGE RESPONSE TIME                          %f  MINUTES\n",AVGR);
	printf( "\tPROPORTION WHO SPEND FOUR \n");
	printf( "\t MINUTES OR MORE IN SYSTEM                     %f\n",PC4);
	printf( "\tSIMULATION RUNLENGTH                           %f MINUTES\n", simClock);
	printf( "\tNUMBER OF DEPARTURES                            %ld\n",TotalCustomers );
	char report[256];
	sprintf(report,"%f,\t%f,\t%f,\t%f,\t%f,\t%f,\t%f",MeanInterArrivalTime,MeanServiceTime,SIGMA,RHO,MaxQueueLength,AVGR,PC4);
	std::string * pReport = new std::string(report);
	reports.push_back(pReport);
}
/**
 * @brief 执行指定参数的仿真运行
 * @param  meanInterArrivalTime 指数分布的平均到达间隔
 * @param  meanServiceTime  正态分布的服务时间均值
 * @param  sigma            正态分布的服务时间方差
 * @param  totalCustomers   控制仿真运行结束的离开顾客数量
 */
void Run(double meanInterArrivalTime,double meanServiceTime,double sigma,long totalCustomers){
	Initialization(meanInterArrivalTime,meanServiceTime,sigma,totalCustomers);
	// Loop until first "TotalCustomers" have departed
	while (NumberOfDepartures < TotalCustomers && FutureEventList.size() > 0){
		Event *pEvent = Dequeue(); // get imminent event
		simClock = pEvent->time;		// advance simulation time
		//printf("Simulation Clock:%f\n",clock);
		switch(pEvent->type){
			case ARRIVAL:
				ProcessArrival(pEvent);
				break;
			case DEPARTURE:
				ProcessDeparture(pEvent);
				break;
		}
		delete pEvent;
	}
	ReportGeneration();
}

/**
 * @brief 主函数
 * @param  argc             参数个数
 * @param  argv             参数数量
 * @return int 运行结束标志
 */
int main(int argc, char *argv[]){
	for(double a = 2; a <= 7;a += 0.5){
		Run(a,3.2,0.6,1000);
	}
	printf("\tMeanInterArrivalTime,\tMeanServiceTime,\tSIGMA,\tSERVER UTILIZATION,\tMaxQueueLength,\tAVGR,\tPC4\n");
	for (std::vector<std::string *>::iterator it = reports.begin();it != reports.end();it ++){
		std::string * pReport = *it;
		printf("%s\n",pReport->c_str());
		delete pReport;
	}
	return 0;
}
