/**
 * @file Queue_ES.cpp
 * @brief 单通道排队系统事件调度仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#include <vector>
#include <stdio.h>
#include "Queue.h"
#include "EventNotice.h"
#include "DataCollection.h"
#include "Simulator.h"

using namespace std;
using namespace rubber_duck;

double MeanInterArrivalTime, MeanServiceTime, SIGMA;
long  NumberOfCustomers, QueueLength, TotalCustomers, 
		NumberOfDepartures, LongService;

Tally responseTally("RESPONSE TIME");
Accumulate queueLengthAccum("QUEUE LENGTH"),busyAccum("SERVER UTILIZATION");

//实体：Customer，仅考察到达时间属性
struct Customer{
	int customerID;//属性：CustomerID
	double arrivalTime;//属性：到达时间
};

//顾客队列
Queue<Customer*> customers;
//随机变量生成器
Random * stream;

//事件名称缓冲区
char nameBuffer[128];
//新顾客标识
int CustomerID = 0;

Customer * CustomerInService = NULL;

//调度离开事件
void ScheduleDeparture(Simulator * pSimulator);

//离开事件
class DepartureEvent:public EventNotice{
public:
	DepartureEvent(double time):EventNotice(time) {
	};

	virtual void trigger(Simulator * pSimulator){
		Customer * finished = customers.dequeue();
		double response = (pSimulator->getClock() - finished->arrivalTime);
		responseTally.update(response,pSimulator->getClock());
		if( response > 4.0 ) LongService ++; // record long service
		NumberOfDepartures ++;
		delete CustomerInService;
		if(NumberOfDepartures >= TotalCustomers){
			pSimulator->stop();
		}
		busyAccum.update(0,pSimulator->getClock());
		if(QueueLength > 0){
			ScheduleDeparture(pSimulator);
		}else{
			CustomerInService = NULL;
		}
	};
};

//调度离开事件
void ScheduleDeparture(Simulator * pSimulator){
	double serviceTime;
	// get the job at the head of the queue
	while ((serviceTime = stream->nextNormal(MeanServiceTime, SIGMA)) < 0);
	DepartureEvent * depart = new DepartureEvent(pSimulator->getClock() + serviceTime);
	CustomerInService = customers.front();
	sprintf(nameBuffer,"顾客%d离开事件",CustomerInService->customerID);
	depart->setName(nameBuffer);
	pSimulator->scheduleEvent(depart);
	QueueLength--;
	busyAccum.update(1,pSimulator->getClock());
	queueLengthAccum.update(QueueLength,pSimulator->getClock());
}

//到达事件
class ArrivalEvent:public EventNotice{
public:
	ArrivalEvent(double time):EventNotice(time) {
	};

	virtual void trigger(Simulator * pSimulator){
		Customer* customer = new Customer();
		CustomerID ++;
		customer->customerID = CustomerID;
		customer->arrivalTime = pSimulator->getClock();
		customers.enqueue(customer);
		QueueLength ++;
		queueLengthAccum.update(QueueLength,pSimulator->getClock());
		//其次调度下一Caller的到达事件
		double nextArrivalInterval = stream->nextExponential() * MeanInterArrivalTime;
		ArrivalEvent* pEvent = new ArrivalEvent(pSimulator->getClock() + nextArrivalInterval);
		sprintf(nameBuffer,"顾客%d到达事件",CustomerID + 1);
		pEvent->setName(nameBuffer);
		pSimulator->scheduleEvent(pEvent);

		if(CustomerInService == NULL){
			ScheduleDeparture(pSimulator);
		}
	};
};

void Initialization(Simulator * pSimulator){
	QueueLength = 0;
	CustomerInService = NULL;
	NumberOfDepartures = 0;
	LongService = 0;
	busyAccum.update(0,pSimulator->getClock());
	queueLengthAccum.update(0,pSimulator->getClock());
	ArrivalEvent* pEvent = new ArrivalEvent(pSimulator->getClock() + stream->nextExponential() * MeanInterArrivalTime);
	sprintf(nameBuffer,"顾客%d到达事件",CustomerID + 1);
	pEvent->setName(nameBuffer);
	pSimulator->scheduleEvent(pEvent);
}

void ReportGeneration(Simulator * pSimulator) {
	double PC4   = ((double)LongService)/TotalCustomers;
	pSimulator->print("SINGLE SERVER QUEUE SIMULATION - GROCERY STORE CHECKOUT COUNTER\n");
	pSimulator->print( "\tMEAN INTERARRIVAL TIME                         %f\n",MeanInterArrivalTime );
	pSimulator->print( "\tMEAN SERVICE TIME                              %f\n",MeanServiceTime );
	pSimulator->print( "\tSTANDARD DEVIATION OF SERVICE TIMES            %f\n",SIGMA );
	pSimulator->print( "\tNUMBER OF CUSTOMERS SERVED                     %d\n",TotalCustomers );
	pSimulator->print("\n");
	pSimulator->print( "\tPROPORTION WHO SPEND FOUR \n");
	pSimulator->print( "\t MINUTES OR MORE IN SYSTEM                     %f\n",PC4 );
	pSimulator->print( "\tSIMULATION RUNLENGTH                           %f MINUTES\n", pSimulator->getClock());
	pSimulator->print( "\tNUMBER OF DEPARTURES                            %d\n",TotalCustomers );
	DataCollection::printHeading();
	responseTally.report();
	queueLengthAccum.report();
	busyAccum.report();
	DataCollection::printEnding();
}

int main(int argc, char* argv[]){
	MeanInterArrivalTime = 4.5; MeanServiceTime = 3.2;
	SIGMA                = 0.6; TotalCustomers  = 100;

	stream = new Random(12345678);
	Simulator * pSimulator = new Simulator(12345678,"Queue_ES.txt");
	pSimulator->setDebug();
	Initialization(pSimulator);
	pSimulator->run();
	ReportGeneration(pSimulator);

	return 0;
}



