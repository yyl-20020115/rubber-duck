/**
 * @file Queue_PI.cpp
 * @brief 单通道排队系统进程交互仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#include <vector>
#include <stdio.h>
#include "Queue.h"
#include "ProcessNotice.h"
#include "DataCollection.h"
#include "Simulator.h"

using namespace std;
using namespace rubber_duck;

double MeanInterArrivalTime, MeanServiceTime, SIGMA;
long  NumberOfCustomers, QueueLength, TotalCustomers, 
		NumberOfDepartures, LongService;

Tally responseTally("RESPONSE TIME");
Accumulate queueLengthAccum("QUEUE LENGTH"),busyAccum("SERVER UTILIZATION");

class Customer;

//顾客队列
Queue<Customer*> customers;
//随机变量生成器
Random * stream;

#define ARRIVAL		1	//到达复活点
#define DEPARTURE	2	//服务完成复活点
#define START		3	//开始服务复活点

Customer * CustomerInService = NULL;

//新顾客标识
int CustomerID = 0;

//事件名称缓冲区
char nameBuffer[128];

//实体：Customer进程，仅考察到达时间属性
class Customer:public ProcessNotice{
public:
	int id;//属性：CustomerID
	double arrivalTime;//属性：到达时间
	//time复活时间
	Customer(double time):ProcessNotice(time,ARRIVAL) {
		arrivalTime = time;
		id = CustomerID ++;
		sprintf(nameBuffer,"顾客%d",id);
		setName(nameBuffer);
	};

	//获得当前复活点名称
	virtual const char * getPhaseName(){
		switch(phase){
			case ARRIVAL: //到达复活点
				return "到达";
			case START://开始服务复活点
				return "开始服务";
			case DEPARTURE://离开复活点
				return "离开";
		}
		return 0;
	};

	//进程推进函数，进程复活后的推进函数，返回复活时间
	virtual double runToBlocked(Simulator * pSimulator){
		switch(phase){
			case ARRIVAL: //到达复活点
				return	Arrival(pSimulator);
			case START://开始服务复活点
				return Start(pSimulator);
			case DEPARTURE://离开复活点
				return Departure(pSimulator);
		}
		return 0;
	};

	//[到达]复活点进程推进
	double Arrival(Simulator * pSimulator){
		arrivalTime = pSimulator->getClock();
		customers.enqueue(this);
		QueueLength++;

		queueLengthAccum.update(QueueLength,pSimulator->getClock());
		
		//其次调度下一顾客的到达事件
		double nextArrivalInterval = stream->nextExponential() * MeanInterArrivalTime;
		Customer* customer = new Customer(pSimulator->getClock() + nextArrivalInterval);
		pSimulator->activate(customer);
		//进入条件延迟，下一复活点为开始服务
		phase = START;
		return -1;
	};
	//[开始服务]复活点进程推进
	double Start(Simulator * pSimulator){
		double serviceTime;
		// get the job at the head of the queue
		while ((serviceTime = stream->nextNormal(MeanServiceTime, SIGMA)) < 0);
		double departTime = pSimulator->getClock() + serviceTime;
		CustomerInService = customers.front();
		QueueLength--;
		busyAccum.update(1,pSimulator->getClock());
		queueLengthAccum.update(QueueLength,pSimulator->getClock());
		//进入无条件延迟，复活点为服务结束
		phase = DEPARTURE;
		return departTime;
	};
	//[服务结束]复活点进程推进
	double Departure(Simulator * pSimulator){
		customers.dequeue();
		CustomerInService = NULL;
		double response = (pSimulator->getClock() - arrivalTime);
		responseTally.update(response,pSimulator->getClock());
		if( response > 4.0 ) LongService ++; // record long service
		NumberOfDepartures ++;
		if(NumberOfDepartures >= TotalCustomers){
			pSimulator->stop();
		}
		busyAccum.update(0,pSimulator->getClock());
		//进程结束
		terminated = true;
		return -1;
	};
	//如果进程处于条件延迟状态，确定当前进程延迟条件是否满足
	virtual bool isConditionalBlocking(Simulator * pSimulator){
		//判断是否处于开始服务条件延迟
		if(phase == START){
			if(customers.front() == this && CustomerInService == NULL){
				return false;
			}
			return true;
		}
		return false;
	};
};

void Initialization(Simulator * pSimulator){
	QueueLength = 0;
	CustomerInService = NULL;
	NumberOfDepartures = 0;
	LongService = 0;
	busyAccum.update(0,pSimulator->getClock());
	queueLengthAccum.update(0,pSimulator->getClock());
	Customer* customer = new Customer(pSimulator->getClock() + stream->nextExponential() * MeanInterArrivalTime);
	pSimulator->activate(customer);
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

	Simulator * pSimulator = new Simulator(12345678,"Queue_PI.txt");
	Initialization(pSimulator);
	pSimulator->setDebug();
	pSimulator->run();

	ReportGeneration(pSimulator);

	return 0;
}



