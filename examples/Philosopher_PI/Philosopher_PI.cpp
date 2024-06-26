/**
 * @file Philosopher_PI.cpp
 * @brief 哲学家就餐问题进程交互仿真模型
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
#include "ProcessNotice.h"
#include "Simulator.h"

using namespace std;
using namespace rubber_duck;

#define NUM_OF_PHILOS 	5
#define SIM_TIME 		(24.0*7)	/* one week! */
#define THINK_TIME		5.0			/* five hours */
#define EAT_TIME		2.0			/* two hours */

#define LEFT_CHOPSTICK(id)	chopsticks[id]
#define RIGHT_CHOPSTICK(id)	chopsticks[(id + 1) % NUM_OF_PHILOS]

#define AWAIT		1	//等待就餐复活点
#define THINK_END	2	//思考结束复活点
#define EAT_END		3	//就餐结束复活点

class Philosopher;

bool chopsticks[NUM_OF_PHILOS];
Philosopher * philosophers[NUM_OF_PHILOS];

//事件名称缓冲区
char nameBuffer[128];

//实体：Customer进程，仅考察到达时间属性
class Philosopher:public ProcessNotice{
public:
	int id;
	double awaitStartTime;
	double thinkTime;
	double awaitTime;

	//time复活时间
	Philosopher(int id,double time):ProcessNotice(time,AWAIT) {
		this->id = id;
		sprintf(nameBuffer,"哲学家%d",id);
		setName(nameBuffer);
		thinkTime = 0;
		awaitStartTime = 0;
		awaitTime = 0;
	};

	//获得当前复活点名称
	virtual const char * getPhaseName(){
		switch(phase){
			case AWAIT: //等待就餐
				return "等待就餐";
			case THINK_END://思考结束
				return "思考结束";
			case EAT_END://就餐结束
				return "就餐结束";
		}
		return NULL;
	};

	//进程推进函数，进程复活后的推进函数，返回复活时间
	virtual double runToBlocked(Simulator * pSimulator){
		switch(phase){
			case AWAIT: //等待就餐
				return await(pSimulator);
			case THINK_END://思考结束
				return think(pSimulator);
			case EAT_END://就餐结束
				return eat(pSimulator);
		}
		return 0;
	};

	//[等待就餐]复活点进程推进
	double await(Simulator * pSimulator){
		int index = id;
		/* No longer waiting for chop sticks      */
		RIGHT_CHOPSTICK(index) = false;
		LEFT_CHOPSTICK(index) = false;
		phase = EAT_END;
		awaitTime += pSimulator->getClock() - awaitStartTime;
		double t = pSimulator->getRandom()->nextExponential() * EAT_TIME;
		pSimulator->print("CLOCK=%f:\t哲学家%d开始就餐，结束时刻为%g\n",pSimulator->getClock(),index,pSimulator->getClock() + t);
		return pSimulator->getClock() + t;
	};

	//[就餐结束]复活点进程推进
	double eat(Simulator * pSimulator){
		int philosopherID = id;
		LEFT_CHOPSTICK(philosopherID) = true;
		RIGHT_CHOPSTICK(philosopherID) = true;
		double t = pSimulator->getRandom()->nextExponential() * THINK_TIME;
		thinkTime += t;
		phase = THINK_END;
		pSimulator->print("CLOCK=%f:\t哲学家%d开始思考，结束时刻为%g\n",pSimulator->getClock(),philosopherID,pSimulator->getClock() + t);
		return pSimulator->getClock() + t;
	};

	//[思考结束]复活点进程推进
	double think(Simulator * pSimulator){
		phase = AWAIT;
		awaitStartTime = pSimulator->getClock();
		return -1;
	};

	//如果进程处于条件延迟状态，确定当前进程延迟条件是否满足
	virtual bool isConditionalBlocking(Simulator * pSimulator){
		//判断是否处于开始服务条件延迟
		if(phase == AWAIT){
			if(RIGHT_CHOPSTICK(id) && LEFT_CHOPSTICK(id)){ 
				return false;
			}
			return true;
		}
		return false;
	};
};

void PrintReport(Simulator * pSimulator) {
	pSimulator->print("\n\nPercentage think time (max = %f)\n\n",100.0*THINK_TIME/(THINK_TIME+EAT_TIME));
	for (int i = 0;i < NUM_OF_PHILOS;i++) {
    	pSimulator->print("Philosopher %d Think Time:  %f\n", i + 1,100.0 * philosophers[i]->thinkTime / SIM_TIME);
		pSimulator->print("Philosopher %d Await Time:  %f\n", i + 1,100.0 * philosophers[i]->awaitTime / SIM_TIME);
  	}
}

void Initialization(Simulator * pSimulator){
	for (int i = 0;i < NUM_OF_PHILOS;i++) {
    	chopsticks[i] = true;
		philosophers[i] = new Philosopher(i,-1);
		pSimulator->await(philosophers[i]);
	}
}

int main(int argc, char* argv[]){
	Simulator * pSimulator = new Simulator(12345678,"philosopher_pi.txt");
	pSimulator->setDebug();
	Initialization(pSimulator);
	pSimulator->run(SIM_TIME);
	PrintReport(pSimulator);
	return 0;
}



