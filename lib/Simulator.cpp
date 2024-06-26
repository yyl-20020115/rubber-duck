/**
 * @file Simulator.cpp
 * @brief 仿真引擎对象类
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-01
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#include <vector>
#include <stdarg.h>
#include "platdefs.h"
#include "Error.h"
#include "Simulator.h"

using namespace rubber_duck;

Simulator::Simulator(unsigned long int seed,const char * printFileName){
	//初始化随机数流
	random = new Random(seed);
	#ifdef _WIN32
		system("chcp 65001");
	#endif
	//创建输出打印文件
	if (printFileName != NULL){
		printFile = fopen(printFileName,"w");
		if (printFile == NULL){
			printf("错误：打开文件失败（%s），请检查路径或访问权限\n",printFileName);
			exit(0);
		}
	}
	//为避免中文乱码，需要在Windows的命令行终端输入"chcp 65001"命令，将编码改为utf-8
}

Simulator::~Simulator(){
	//关闭输出打印文件
	if (printFile != NULL){
		fclose(printFile);
	}
	delete random;
	futureEventList.removeAll();
	conditionalEventList.removeAll();
}

void Simulator::reset(unsigned long int seed,const char * printFileName){
	//初始化随机数流
	if(random != NULL){
		delete random;
		random = new Random(seed);
	}
	//创建输出打印文件
	if (printFileName != NULL){
		if(printFile != NULL){
			fclose(printFile);
		}
		printFile = fopen(printFileName,"w");
		if (printFile == NULL){
			printf("错误：打开文件失败（%s），请检查路径或访问权限\n",printFileName);
			exit(0);
		}
	}
	clock = 0;
	duration = -1;
	futureEventList.removeAll();
	conditionalEventList.removeAll();
}

void Simulator::cancelEvent(EventNotice * pEvent){
	futureEventList.removeEvent(pEvent);
	conditionalEventList.removeEvent(pEvent);
}

bool Simulator::hasEvent(EventNotice * pEvent){
	if(futureEventList.hasEvent(pEvent)){
		return true;
	}
	if(conditionalEventList.hasEvent(pEvent)){
		return true;
	}
	return false;
}

void Simulator::scheduleEvent(EventNotice * pEvent){
	//不允许添加小于当前仿真时间的仿真事件
	if(pEvent->getTime() < clock){
		printf("仿真时间=%f，事件：%s 发生时间小于当前仿真时间！\n",clock,pEvent->getName());
		exit(0);
	}
	//按照事件时间添加仿真事件
	futureEventList.insertEvent(pEvent);
	if(debug){
		print("仿真时间=%f 在FEL中添加未来事件(%s),发生时间：%f。\n",clock,pEvent->getName(),pEvent->getTime());
	}
}

void Simulator::scheduleConditionalEvent(EventNotice * pEvent){
	//添加条件事件
	conditionalEventList.insertEvent(pEvent);
	if(debug){
		print("仿真时间=%f 在CEL中添加条件事件{%s}。\n",clock,pEvent->getName());
	}
}

bool Simulator::isEnd(){
	if(terminated){
		return true;
	}
	if(futureEventList.isEmpty() && conditionalEventList.isEmpty()){
		return true;
	}
	return false;
}

void Simulator::triggerEvent(EventNotice* pEvent){
	//Step2: 将CLOCK推进至该事件的时间
	clock = pEvent->getTime();
	if(debug){
		print("仿真时间=%f 发生事件(%s)。\n",clock,pEvent->getName());
	}
	//Step3: 执行该事件，更新Snapshot表
	//Step4: 如果必要，调度新的未来事件
	pEvent->trigger(this);
	//如果不需要保留事件，则删除当前事件
	if(!pEvent->isReserved()){
		//删除事件
		delete pEvent;
	}
}

void Simulator::scanFutureEvents(bool CEL){
	if(futureEventList.size() == 0){
		return;
	}
	if(CEL){
		//Step1: 从FEL中移出imminent事件记录
		std::vector<EventNotice*> CEL = futureEventList.popImminentEventListPriority(); assert(CEL.size()>0);
		//Step2: 将CLOCK推进至该事件的时间
		clock = CEL[0]->getTime();
		//按照事件优先级排序调度时间相同的事件
		for (unsigned i = 0;i < CEL.size();i ++){
			EventNotice* pEvent = CEL[i];
			triggerEvent(pEvent);
		}
	}else{
		//Step1: 从FEL中移出imminent事件记录
		EventNotice* pEvent = futureEventList.popImminentEvent();
		triggerEvent(pEvent);
	}
}

class EndEvent:public EventNotice{
public:
	EndEvent(double time):EventNotice(time) {
        setName("仿真结束事件");
	};
	virtual void trigger(Simulator * pSimulator){
         pSimulator->stop();
    };
};

void Simulator::run(double duration,bool bCEL){
	this->duration = duration;

	//扫描调度条件事件,避免仿真模型初始化时不存在确定事件，仅存在条件事件
	scanConditionalEvents();

	if(duration > 0){
		scheduleEvent(new EndEvent(duration));
	}

	//按照解结规则执行相同时间的仿真事件
	while(!isEnd()){
		//扫描调度未来事件
		scanFutureEvents(bCEL);
		//扫描调度条件事件
		scanConditionalEvents();
	}
}

void Simulator::scanConditionalEvents(){
	bool bRoutineExecuted = true;
	//如果执行了条件事件，则需要重新扫描条件事件表
	while(bRoutineExecuted){
		bRoutineExecuted = false;
		EventList::iterator it;
		//扫描条件事件表
		for (it = conditionalEventList.begin();
				it != conditionalEventList.end();
				it++){
			EventNotice* pEvent = (*it);
			//如果满足事件执行条件，则结束扫描，执行条件事件
			if(pEvent->canTrigger(this)){
				bRoutineExecuted = true;
				break;
			}
		}
		if(bRoutineExecuted){
			//获取当前满足条件的条件事件
			EventNotice* pEvent = (*it);
			//删除执行的条件事件
			conditionalEventList.remove(pEvent);
			if(debug){
				print("仿真时间=%f 发生条件事件{%s}\n",clock,pEvent->getName());
			}
			//执行条件事件，如果必要，调度新的未来事件
			pEvent->trigger(this);
			//如果不需要保留事件，则删除当前条件事件
			if(!pEvent->isReserved()){
				//删除事件
				delete pEvent;
			}
		}
	}
}

//打印输出函数
void Simulator::print(const char* strFormat,...){
	char szTemp[4096];

	va_list ap;
	va_start(ap, strFormat);
	vsprintf(szTemp, strFormat, ap);
	va_end(ap);

	//打印到控制台
	printf(szTemp);

	//打印到文件
	if (printFile != NULL){
		fprintf(printFile,szTemp);
	}
}

void Simulator::activate(ProcessNotice * pProcess,double time){
	if(time < 0){
		scheduleEvent(pProcess);
	}else{
		pProcess->setTime(time);
		scheduleEvent(pProcess);
	}
}

void Simulator::suspend(ProcessNotice * pProcess){
	if(hasEvent(pProcess)){
		cancelEvent(pProcess);
	}else{
		err(PROCUSCH);
	}
}

void Simulator::resume(ProcessNotice * pProcess){
	if(hasEvent(pProcess)){
		err(PROCSCH);
		return;
	}
	pProcess->setTime(getClock());
	scheduleEvent(pProcess);
}

void Simulator::await(ProcessNotice * pProcess){
	if(hasEvent(pProcess)){
		err(PROCSCH);
	}else{
		scheduleConditionalEvent(pProcess);
	}
}