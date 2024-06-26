/**
 * @file Queue.cpp
 * @brief 单通道排队系统事件调度仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-03
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#include <queue>
#include <list>
#include <stdio.h>

using namespace std;

//事件结构：Event，仅考虑发生时间、事件类型和用户标识
struct Event{
	int type;	 //事件类型
	double time; //发生时间
	int id;		 //用户标识
};

#define ARRIVAL 	1	//到达事件类型
#define DEPARTURE 	2 	//服务完成事件类型

#define ARRIVAL_NUM 5

double A[] = {15.0, 32.0, 24.0, 40.0, 22.0};
double S[] = {43.0, 36.0, 34.0, 28.0, 20.0};

//新顾客标识
int customerID = 0;

//仿真时间
double simClock = 0;

std::list<Event *> FutureEventList;
std::queue<int> customers;

long NumberOfCustomers, QueueLength, NumberInService, MaxQueueLength,
	TotalCustomers, NumberOfDepartures;

//插入事件
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

//移出下一未来事件记录
Event *Dequeue(){
	if (FutureEventList.size() == 0)
		return NULL;
	Event *pEvent = *(FutureEventList.begin());		//第一个事件记录
	FutureEventList.erase(FutureEventList.begin()); //从列表中删除该记录
	return pEvent;
}

void Initialization(){
	TotalCustomers = 1000;
	QueueLength = 0;
	NumberInService = 0;
	MaxQueueLength = 0;
	NumberOfDepartures = 0;
	Event *pEvent = new Event();
	pEvent->time = simClock + A[customerID];
	pEvent->type = ARRIVAL;
	pEvent->id = customerID;
	Enqueue(pEvent);
	customerID++;
}

void ScheduleDeparture();

void ProcessArrival(Event *pArrivalEvent){
	printf("ProcessArrival Event for CustomerID:%d\n",pArrivalEvent->id);
	QueueLength++;
	customers.push(pArrivalEvent->id);
	// if the server is idle, fetch the event, do statistics
	// and put into service
	if (NumberInService == 0){
		ScheduleDeparture();
	}

	// adjust max queue length statistics
	if (MaxQueueLength < QueueLength){
		MaxQueueLength = QueueLength;
	}

	if (customerID >= ARRIVAL_NUM){
		return;
	}

	// schedule the next arrival
	Event *pEvent = new Event();
	pEvent->time = simClock + A[customerID];
	pEvent->type = ARRIVAL;
	pEvent->id = customerID;
	customerID++;
	Enqueue(pEvent);
}

//调度离开事件
void ScheduleDeparture(){
	int id = customers.front();
	customers.pop();
	Event * pEvent = new Event();
	pEvent->time = simClock + S[id];
	pEvent->type = DEPARTURE;
	pEvent->id = id;
	Enqueue(pEvent);
	NumberInService = 1;
	QueueLength--;
}

//离开事件

void ProcessDeparture(Event *pDepartureEvent){
	printf("ProcessDeparture Event for CustomerID:%d\n",pDepartureEvent->id);
	NumberOfDepartures++;
	// if there are customers in the queue then schedule
	// the departure of the next one
	if (QueueLength > 0){
		ScheduleDeparture();
	}
	else{
		NumberInService = 0;
	}
}

void ReportGeneration(){
	printf("SINGLE SERVER QUEUE SIMULATION - GROCERY STORE CHECKOUT COUNTER\n");
	printf("\tNUMBER OF CUSTOMERS SERVED                     %ld\n", TotalCustomers);
	printf("\tMAXIMUM LINE LENGTH                            %ld\n", MaxQueueLength);
	printf("\tSIMULATION RUNLENGTH                           %f MINUTES\n", simClock);
	printf("\tNUMBER OF DEPARTURES                            %ld\n", TotalCustomers);
}

int main(int argc, char *argv[]){
	Initialization();
	// Loop until first "TotalCustomers" have departed
	while (NumberOfDepartures < TotalCustomers && FutureEventList.size() > 0){
		Event *pEvent = Dequeue(); // get imminent event
		simClock = pEvent->time;		// advance simulation time
		printf("Simulation Clock:%f\n",simClock);
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
	return 0;
}
