/**
 * @file Queue_PI2.cpp
 * @brief 单通道排队系统进程交互仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2021-09-06
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#include <vector>
#include <stdio.h>
#include "Queue.h"
#include "CProcess.h"
#include "DataCollection.h"
#include "Simulator.h"

using namespace std;
using namespace rubber_duck;

double MeanInterArrivalTime, MeanServiceTime, SIGMA;
long  NumberOfCustomers, QueueLength, TotalCustomers, 
		NumberOfDepartures, LongService;

Tally responseTally("RESPONSE TIME");

class Customer;

//顾客队列
Queue<Customer*> customers;
//随机变量生成器
Random * stream;

//新顾客标识
int CustomerID = 0;

//事件名称缓冲区
char nameBuffer[128];

Resource * r = new Resource("Server",1);

//实体：Customer进程，仅考察到达时间属性
class Customer:public CProcess{
public:
	int id;//属性：CustomerID
	double arrivalTime;//属性：到达时间
	//time复活时间
	Customer(double time):CProcess(time) {
		arrivalTime = time;
		id = CustomerID ++;
		sprintf(nameBuffer,"顾客%d",id);
		setName(nameBuffer);
	};

	virtual void actions(){
		arrivalTime = getSimulator()->getClock();
		request(r,1);
		work(stream->nextNormal(MeanServiceTime, SIGMA));
		relinquish(r,1);
		double response = (getSimulator()->getClock() - arrivalTime);
		responseTally.update(response,getSimulator()->getClock());
	}
};

class Generator:public CProcess{
public:
	Generator(double time):CProcess(time) {
		setName("Generator");
	};

	virtual void actions(){
		for(int i = 0;i < 10;i ++){
			activateNow(new Customer(0));
			delay(stream->nextExponential() * MeanInterArrivalTime);
		}
	}
};

void Initialization(Simulator * pSimulator){
	CProcess::init(pSimulator,1280000,128000);
	QueueLength = 0;
	NumberOfDepartures = 0;
	LongService = 0;
	CProcess::activateNow(new Generator(0));
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
	r->getWorkingAccumulate()->report();
	r->getWaitingAccumulate()->report();	
	DataCollection::printEnding();
}

int main(int argc, char* argv[]){
	MeanInterArrivalTime = 4.5; MeanServiceTime = 3.2;
	SIGMA                = 0.6; TotalCustomers  = 100;

	stream = new Random(12345678);

	Simulator * pSimulator = new Simulator(12345678,"PI.txt");
	Initialization(pSimulator);
	pSimulator->setDebug();
	pSimulator->run();
	ReportGeneration(pSimulator);

	return 0;
}



