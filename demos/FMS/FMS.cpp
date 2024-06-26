/**
 * @file FMS.cpp
 * @brief 车间柔性制造系统进程交互仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2023-03-30
 * 
 * @copyright Copyright(C) 2023 liqun
 */

#include "CProcess.h"

using namespace std;
using namespace rubber_duck;
//仿真时间长度（小时）
double SimLength = 2920;
//工件的平均到达间隔时间（小时）
double MeanInterArrivalTime = 0.25;
//工件类型随机变量离散取值
int Types[3]		= {1,2,3};
//工件类型随机变量离散取值概率
double TypesProbs[3]	= {0.3,0.5,0.2};
//工件类型随机变量离散分布表
CDFDiscreteTable * pTypeCDF = makeDiscreteCDFTable(3,Types,TypesProbs);
//运输装置的移动速度（英尺/秒）
double TPSpeed = 5.0;
//输入输出站序号
int IOStation = 5;
//工作站之间的距离数组（英尺）
double Distances[6][6] = {
	{0.0,90.0,100.0,180.0,200.0,270.0},
	{90.0,0.0,100.0,200.0,180.0,270.0},
	{100.0,100.0,0.0,100.0,100.0,180.0},
	{180.0,200.0,100.0,0.0,90.0,100.0},
	{200.0,180.0,100.0,90.0,0.0,100.0},
	{270.0,270.0,180.0,100.0,100.0,0.0},
};
//工作站资源
Resource * WorkStations[5] = {new Resource("WS1",3),new Resource("WS2",3),new Resource("WS3",4),
                    new Resource("WS4",4),new Resource("WS5",1)};
//每类工件类型加工任务数量
int TaskSizes[3] = {4,3,5};
//第一类工件的加工任务工序序列
int Tasks1[4] = {3,1,2,5};
//第二类工件的加工任务工序序列
int Tasks2[3] = {4,1,3};
//第三类工件的加工任务工序序列
int Tasks3[5] = {2,5,1,4,3};
//由Tasks1、Tasks2和Tasks3组成的所有工件的加工任务序列数组
int * Tasks[3] = {Tasks1,Tasks2,Tasks3};
//第一类工件加工任务的每项工序的平均服务时间
double MeanServiceTimes1[4] = {0.5,0.6,0.85,0.5};
//第二类工件加工任务的每项工序的平均服务时间
double MeanServiceTimes2[3] = {1.10,0.80,0.75};
//第三类工件加工任务的每项工序的平均服务时间
double MeanServiceTimes3[5] = {1.20,0.25,0.70,0.90,1.00};
//由MeanServiceTimes1、MeanServiceTimes1和MeanServiceTimes1组成的所有工件的每项工序的平均服务时间数组
double * MeanServiceTimes[3] = {MeanServiceTimes1,MeanServiceTimes2,MeanServiceTimes3};
//运输装置资源
Resource * transporter = new Resource("Transporter",1);
//运输装置当前所处的工作站序号
int tpLocation = IOStation;
//随机变量生成对象
Random * stream = NULL;
//每类工件的加工队列滞留时间统计对象
Tally * jobDelaysInQueue[3] = {new Tally("Type1"),new Tally("Type2"),new Tally("Type3")};
//每类工件的运输队列滞留时间统计对象
Tally * jobTransporterDelays[3] = {new Tally("Type1"),new Tally("Type2"),new Tally("Type3")};
//每个工作站的工作状态统计对象
Accumulate * machineWorking[5] = {new Accumulate("WS1"),new Accumulate("WS2"),new Accumulate("WS3"),new Accumulate("WS4"),new Accumulate("WS5")};
//每类工作站的工作状态数组
int numMachineWorking[5] = {0,0,0,0,0};
//工件在特定工作站的滞留时间统计对象
Tally * delayInQueueforWS[5] = {new Tally("WS1"),new Tally("WS2"),new Tally("WS3"),new Tally("WS4"),new Tally("WS5")};
//工件标识号
int id = 0;
//事件名称缓冲区
char nameBuffer[128];

#define MAX_JOB_SIZE 250
double jobTotalDelayInQueue[MAX_JOB_SIZE];
int jobs = 0;

class Job:public CProcess{
private:
    int type;
public:
	//time复活时间
	Job(double time):CProcess(time) {
		type = stream->nextDiscrete(pTypeCDF) - 1;
		//cout << "type " << type << endl;
		sprintf(nameBuffer,"Job%d",id ++);
		setName(nameBuffer);
	};

	virtual void actions(){
		double delayInTransporter = 0.0;
    	double totalDelayInQueue = 0.0;
		double requestTime = getSimulator()->getClock();
		request(transporter,1);
		double travelTime = Distances[tpLocation][IOStation] / (TPSpeed * 3600);
		wait(travelTime);
		delayInTransporter = delayInTransporter + (Time()- requestTime);
		int nextStation = Tasks[type][0] - 1;
		travelTime = Distances[IOStation][nextStation] / (TPSpeed * 3600);
		//cout << "travelTime2 " << travelTime << endl;
		wait(travelTime);
		tpLocation = nextStation;
		relinquish(transporter,1);
		for (int i = 0; i < TaskSizes[type]; i++) {
			int currentStation = Tasks[type][i] - 1;
			double arrivalTime = Time();
			request(WorkStations[currentStation],1);
			delayInQueueforWS[currentStation]->update(Time() - arrivalTime,Time());
			totalDelayInQueue = totalDelayInQueue + (Time() - arrivalTime);
			numMachineWorking[currentStation] ++;
			machineWorking[currentStation]->update(numMachineWorking[currentStation],Time());
			//Work(random.nextGamma(2.0,1/MeanServiceTimes[type][i]));?
			work(stream->nextErlang(2,MeanServiceTimes[type][i]));
			numMachineWorking[currentStation] --;
			machineWorking[currentStation]->update(numMachineWorking[currentStation],Time());
			requestTime = Time();
			request(transporter,1);
			travelTime = Distances[tpLocation][currentStation] / (TPSpeed * 3600);
			wait(travelTime);
			delayInTransporter  = delayInTransporter + (Time() - requestTime);
			relinquish(WorkStations[currentStation],1);
			if(i == TaskSizes[type] - 1){
				nextStation = IOStation;
			}else{
				nextStation = Tasks[type][i + 1] - 1;;
			}
			travelTime = Distances[currentStation][nextStation] / (TPSpeed * 3600);
			wait(travelTime);
			tpLocation = nextStation;
			relinquish(transporter,1);
		}
		if(jobs < MAX_JOB_SIZE){
			jobTotalDelayInQueue[jobs] = totalDelayInQueue;
			jobs ++;
		}
		jobDelaysInQueue[type]->update(totalDelayInQueue,Time());
		jobTransporterDelays[type]->update(delayInTransporter,Time());
	}
};

class ArrivalGenerator:public CProcess{
public:
	ArrivalGenerator(double time):CProcess(time) {
		setName("ArrivalGenerator");
	};
	virtual void actions(){
		while(Time() < SimLength){
			wait(stream->nextExponential() * MeanInterArrivalTime);
			activateNow(new Job(0));
		}
	};
};

void ReportGeneration(Simulator * pSimulator,FILE* csvFile) {
	double propBlocked = 0.0;
	double propIdle = 0.0;
	double propWorking = 0.0;
	for(int i = 0;i < 3;i ++){
		pSimulator->print("Job Type:%d Mean Job Delay In Queue:%f Mean Job Transporter Delay: %f \n" ,
		 (i+1), jobDelaysInQueue[i]->mean() , jobTransporterDelays[i]->mean());
	}
	for(int i = 0;i < 5;i ++){
		propWorking = machineWorking[i]->mean() / WorkStations[i]->getCapacity();
		propIdle = 1 - WorkStations[i]->getWorkingAccumulate()->mean();
		propBlocked = 1 - propWorking - propIdle;
		pSimulator->print("Work Station: %d Avg Num In Queue: %f Avg Delay In Queue: %f Prop Time Working: %f \
		Prop Time Blocked: %f Prop Time Idle: %f\n", (i+1) , WorkStations[i]->getWaitingAccumulate()->mean() ,
		delayInQueueforWS[i]->mean() , propWorking , propBlocked , propIdle);
	}
	pSimulator->print("Transporter Utility:%f\n" , transporter->getWorkingAccumulate()->mean());
	pSimulator->print("First %d Job Total Delay In Queue:\n", MAX_JOB_SIZE);
	for(int i = 0;i < MAX_JOB_SIZE;i ++){
//		pSimulator->print("%f,", jobTotalDelayInQueue[i]);
		if(i == MAX_JOB_SIZE - 1){
			fprintf(csvFile,"%f\n", jobTotalDelayInQueue[i]);
		}else{
			fprintf(csvFile,"%f, ", jobTotalDelayInQueue[i]);
		}
	}
	pSimulator->print("\n");

}

int main(int argc, char* argv[]){
	int runs = 0;
	int seed = 12345678;
	FILE* csvFile = NULL;
	
	if(argc > 1){
		runs = atoi(argv[1]);
		seed = atoi(argv[2]);
	}

	if(runs == 0){
		csvFile = fopen("result.csv","w");
		fprintf(csvFile,"Runs, ");
		for(int i = 0;i < MAX_JOB_SIZE;i ++){
			if(i == MAX_JOB_SIZE - 1){
				fprintf(csvFile,"%d\n", i + 1);
				}else{
				fprintf(csvFile,"%d, ", i + 1);
			}
		}
	}else{
		csvFile = fopen("result.csv","a");
	}

	if (csvFile == NULL){
		printf("错误：打开文件失败（esult.csv），请检查路径或访问权限\n");
		exit(0);
	}
	fprintf(csvFile,"%d, ",runs + 1);

//	fprintf(csvFile,"Runs, %d, ", runs);
	Simulator * pSimulator = new Simulator(seed,"FMS.txt");
	stream = new Random(seed);
	CProcess::init(pSimulator,12800000,128000);
	CProcess::activateNow(new ArrivalGenerator(0));
	//pSimulator->setDebug();
	pSimulator->run();
	ReportGeneration(pSimulator,csvFile);

	if (csvFile != NULL){
		fclose(csvFile);
	}
	return 0;
}
