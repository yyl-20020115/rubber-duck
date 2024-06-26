/**
 * @file Inventory.cpp
 * @brief 库存问题进程交互仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2023-03-30
 * 
 * @copyright Copyright(C) 2023 liqun
 */
#include "CProcess.h"

using namespace std;
using namespace rubber_duck;

int Demands[4] = {1,2,3,4};
double DemandsProb[4] = {1/6.0,1/3.0,1/3.0,1/6.0};
CDFDiscreteTable * pDemandsCDF = makeDiscreteCDFTable(4,Demands,DemandsProb);

//仿真时间长度
double NumberOfMonth = 120;
//单位商品保管成本
double H = 2;
//单位缺货成本
double PI = 5;
//需求平均时间间隔
double MeanInterDemandTime = 0.1;
//最小在架寿命
double MinShelfLife = 1.5;
//最大在架寿命
double MaxShelfLife = 2.5;
//最小滞后时间
double MinDeliveryLag = 1.0;
//最大滞后时间
double MaxDeliveryLag = 3.0;
//(s,S)库存策略中的s
double LittleS = 20;
//(s,S)库存策略中的S
double BigS = 40;
//订货固定成本
double SetupCost = 32.0;
//订货单位成本
double CostPerItem = 3.0;
//初始库存
double InitialInventory = 60.0;
//累积商品需求量
int numberOfBacklogged = 0; 
//订货量
int numberOnOrder = (int)InitialInventory;
//仓库货物集合
std::list<double> storedItems;
//订货成本统计
Tally *  orderingCost = new Tally("订货费用");
//库存数量统计
Accumulate * numberInStorage = new Accumulate("库存数量");
//缺货数量统计
Accumulate * numberInBacklog = new Accumulate("缺货数量");
Random * stream = new Random(12345678);
//事件名称缓冲区
char nameBuffer[128];

double numberInStorageSamples[120];
int samples = 0;

class OrderArrival:public CProcess{
private:
    int orderSize;
public:
	//time复活时间
	OrderArrival(int orderSize):CProcess(0) {
		this->orderSize = orderSize;
		sprintf(nameBuffer,"orderSize%d",orderSize);
		setName(nameBuffer);
	};

	virtual void actions(){
        for(int i = 0;i < this->orderSize;i ++){
			if(numberOfBacklogged == 0){
				double spoilTime = Time() + stream->nextDouble(MinShelfLife,MaxShelfLife);
				storedItems.push_back(spoilTime);
			}
			else{
				numberOfBacklogged --;
			}
		}
		numberInBacklog->update(numberOfBacklogged,Time());
		numberInStorage->update((double)storedItems.size(),Time());
		numberOnOrder = numberOnOrder - this->orderSize;
    };
};

class DemandGenerator:public CProcess{
public:
	DemandGenerator():CProcess(0) {
		setName("DemandGenerator");
	};
	virtual void actions(){
		while(Time() < NumberOfMonth){
			wait(stream->nextExponential() * MeanInterDemandTime);

			for (list<double>::iterator it = storedItems.begin();it != storedItems.end();){
				double spoilTime = *it;
				if(spoilTime <= Time()) {
			        it = storedItems.erase(it);
			    }
            	else{
               		it ++;
				}
			}
			
			int demandSize = stream->nextDiscrete(pDemandsCDF);
			for(int i = 0;i < demandSize;i ++){
				bool found = false;
				for (list<double>::iterator it = storedItems.begin();it != storedItems.end();){
					double spoilTime = *it;
					if(spoilTime > Time()) {
						found = true;
						it = storedItems.erase(it);
						break;
					}
					else{
						it ++;
					}
				}
				if(!found){
					numberOfBacklogged ++;
				}
			}
			numberInBacklog->update(numberOfBacklogged,Time());
			numberInStorage->update((double)storedItems.size(),Time());
		}
	};
};

class InventoryReview:public CProcess{
public:
	InventoryReview():CProcess(0) {
		setName("InventoryReview");
	};
	virtual void actions(){
		while(Time() < NumberOfMonth){
			int inventoryLevel = (int)storedItems.size() - numberOfBacklogged + numberOnOrder;
			double orderCost = 0;
			if(inventoryLevel < LittleS){
				int orderSize = (int)BigS - inventoryLevel;
				numberOnOrder = numberOnOrder + orderSize;
				orderCost = SetupCost + (CostPerItem * orderSize);
				activate(new OrderArrival(orderSize), Time() + stream->nextDouble(MinDeliveryLag,MaxDeliveryLag));
			}else{
				orderCost = 0;
			}
			orderingCost->update(orderCost,Time());
			if(samples < 120){
				numberInStorageSamples[samples] = inventoryLevel;
				samples ++;
			}

			wait(1);
		}
	};
};

void Initialization(double LS,double BS){
	LittleS = LS;
	BigS = BS;
	numberOfBacklogged = 0; 
	numberOnOrder = (int)InitialInventory;
	storedItems.clear();
	orderingCost->reset(0);
	numberInStorage->reset(0);
	numberInBacklog->reset(0);
	delete stream;
	stream = new Random(12345678);

	orderingCost->update(0,0);
	numberInBacklog->update(0,0);
	numberInStorage->update(0,0);

	CProcess::getSimulator()->reset(12345678,"Inventory.txt");
	CProcess::activateNow(new OrderArrival((int)InitialInventory));
	CProcess::activateNow(new DemandGenerator());
	CProcess::activateNow(new InventoryReview());
}

void ReportGeneration(Simulator * pSimulator){
	double avgHoldingCost = H * numberInStorage->mean();
	double avgShortingCost = PI * numberInBacklog->mean();
	double avgTotalCost = orderingCost->mean() + avgHoldingCost + avgShortingCost;
	pSimulator->print("LittleS: %f BigS: %f 平均费用: %f 平均订购费用: %f 平均保管费用: %f 平均缺货费用: %f\n",
		LittleS, BigS, avgTotalCost, orderingCost->mean(), avgHoldingCost, avgShortingCost);
	pSimulator->print("First %d numberInStorageSamples, \n", 120);
	for(int i = 0;i < 120;i ++){
//		pSimulator->print("%f,", jobTotalDelayInQueue[i]);
		if(i == 120 - 1){
			printf("%f\n", numberInStorageSamples[i]);
		}else{
			printf("%f, ", numberInStorageSamples[i]);
		}
	}
	pSimulator->print("\n");
}

int main(int argc, char* argv[]){
	Simulator * pSimulator = new Simulator(12345678,"Inventory.txt");
	CProcess::init(pSimulator,1280000,12800);
	//pSimulator->setDebug();
	for(double BS = 40;BS <= 100;BS += 20){
		Initialization(20,BS);
		pSimulator->run();
		ReportGeneration(pSimulator);
	}

	for(double BS = 60;BS <= 100;BS += 20){
		Initialization(40,BS);
		pSimulator->run();
		ReportGeneration(pSimulator);
	}

	for(double BS = 80;BS <= 100;BS += 20){
		Initialization(60,BS);
		pSimulator->run();
		ReportGeneration(pSimulator);
	}

	return 0;
}
