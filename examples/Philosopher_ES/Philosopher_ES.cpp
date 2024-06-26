/**
 * @file Philosopher_ES.cpp
 * @brief 哲学家就餐问题事件调度仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-10-13
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#include <vector>
#include <stdio.h>
#include <algorithm>
#include "Queue.h"
#include "EventNotice.h"
#include "Simulator.h"

using namespace std;
using namespace rubber_duck;

#define NUM_OF_PHILOS 	5
#define SIM_TIME 		(24.0*7)	/* one week! */
#define THINK_TIME		5.0			/* five hours */
#define EAT_TIME		2.0			/* two hours */

#define LEFT_CHOPSTICK(id)	chopsticks[id]
#define RIGHT_CHOPSTICK(id)	chopsticks[(id + 1) % NUM_OF_PHILOS]

#define AWAIT	1	//等待就餐状态
#define THINK	2	//思考状态
#define EAT		3	//就餐状态

typedef struct{
	int id;
	int state;
	double awaitStartTime;
	double thinkTime;
	double awaitTime;
}Philosopher;

bool chopsticks[NUM_OF_PHILOS];
Philosopher philosophers[NUM_OF_PHILOS];

//事件名称缓冲区
char nameBuffer[128];

class EatCompleteEvent;
class ThinkCompleteEvent;

void scheduleEatEvent(Simulator * pSimulator);

//思考完成事件
class ThinkCompleteEvent:public EventNotice{
public:
	ThinkCompleteEvent(Philosopher * pPhilosopher,double time):EventNotice(time) {
		setOwner(pPhilosopher);
	};

	virtual void trigger(Simulator * pSimulator){
		Philosopher * pPhilosopher = (Philosopher *)getOwner();
		pPhilosopher->state = AWAIT;
		pPhilosopher->awaitStartTime = pSimulator->getClock();
		scheduleEatEvent(pSimulator);
	};
};

class EatCompleteEvent:public EventNotice{
public:
	EatCompleteEvent(Philosopher * pPhilosopher,double time):EventNotice(time) {
		setOwner(pPhilosopher);
	};

	virtual void trigger(Simulator * pSimulator){
		Philosopher * pPhilosopher = (Philosopher *)getOwner();
		int philosopherID = pPhilosopher->id;
		LEFT_CHOPSTICK(philosopherID) = true;
		RIGHT_CHOPSTICK(philosopherID) = true;
		double t = pSimulator->getRandom()->nextExponential() * THINK_TIME;
		pPhilosopher->thinkTime += t;
		pPhilosopher->state = THINK;
		ThinkCompleteEvent * thinkCompleteEvent = new ThinkCompleteEvent(pPhilosopher,pSimulator->getClock() + t);
		sprintf(nameBuffer,"哲学家%d思考完成事件",philosopherID);
		thinkCompleteEvent->setName(nameBuffer);
		pSimulator->scheduleEvent(thinkCompleteEvent);
		pSimulator->print("CLOCK=%f:\t哲学家%d开始思考，结束时刻为%g\n",pSimulator->getClock(),philosopherID,thinkCompleteEvent->getTime());
		scheduleEatEvent(pSimulator);
	};
};

void scheduleEatEvent(Simulator * pSimulator){
	for(int i = 0;i < NUM_OF_PHILOS;i ++){
		int index = i;
		if(philosophers[index].state == AWAIT && RIGHT_CHOPSTICK(index) && LEFT_CHOPSTICK(index)){ 
			/* No longer waiting for chop sticks      */
			RIGHT_CHOPSTICK(index) = false;
			LEFT_CHOPSTICK(index) = false;
			philosophers[index].state = EAT;
			philosophers[index].awaitTime += pSimulator->getClock() - philosophers[index].awaitStartTime;
			double t = pSimulator->getRandom()->nextExponential() * EAT_TIME;
			EatCompleteEvent * eatCompleteEvent = new EatCompleteEvent(&(philosophers[index]),pSimulator->getClock() + t);
			sprintf(nameBuffer,"哲学家%d就餐完成事件",index);
			eatCompleteEvent->setName(nameBuffer);
			pSimulator->scheduleEvent( eatCompleteEvent );
			pSimulator->print("CLOCK=%f:\t哲学家%d开始就餐，结束时刻为%g\n",pSimulator->getClock(),index,eatCompleteEvent->getTime());
		}
	}
}

void PrintReport(Simulator * pSimulator) {
	pSimulator->print("\n\nPercentage think time (max = %f)\n\n",100.0*THINK_TIME/(THINK_TIME+EAT_TIME));
	for (int i = 0;i < NUM_OF_PHILOS;i++) {
    	pSimulator->print("Philosopher %d Think Time:  %f\n", i + 1,100.0 * philosophers[i].thinkTime / SIM_TIME);
		pSimulator->print("Philosopher %d Await Time:  %f\n", i + 1,100.0 * philosophers[i].awaitTime / SIM_TIME);
  	}
}

void Initialization(Simulator * pSimulator){
	for (int i = 0;i < NUM_OF_PHILOS;i++) {
    	chopsticks[i] = true;
		philosophers[i].id = i;
		philosophers[i].state = AWAIT;
		philosophers[i].thinkTime = 0;
		philosophers[i].awaitStartTime = 0;
		philosophers[i].awaitTime = 0;
	}
	scheduleEatEvent(pSimulator);
}

int main(int argc, char* argv[]){
	Simulator * pSimulator = new Simulator(12345678,"philosopher_es.txt");
	Initialization(pSimulator);
	pSimulator->setDebug();
	pSimulator->run(SIM_TIME);
	PrintReport(pSimulator);
	return 0;
}



