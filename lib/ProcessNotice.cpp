/**
 * @file ProcessNotice.cpp
 * @brief 
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-02
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#include "ProcessNotice.h"
#include "Simulator.h"

using namespace rubber_duck;

//重载EventNotice事件处理函数，执行进程推进函数
void ProcessNotice::trigger(Simulator * pSimulator){
	if(pSimulator->isDebug()){
		pSimulator->print("仿真时间=%f 进程[%s]从复活点[%s]推进进程。 \n",pSimulator->getClock(),getName(),getPhaseName());
	}
	//推进进程，直到被锁住
	activateTime = runToBlocked(pSimulator);
	//如果进程进入无条件延迟
	if(activateTime >= 0){
		time = activateTime;
		if(pSimulator->isDebug()){
			pSimulator->print("仿真时间=%f 进程[%s]进入无条件延迟，复活点[%s]。\n",pSimulator->getClock(),getName(),getPhaseName());
		}
		//调度无条件延迟进程对象
		pSimulator->scheduleEvent(this);
	}else{//进程进入条件延迟
		//如果进程结束，则设置reserved为false，由SImulator删除进程对象
		if(terminated){
			if(pSimulator->isDebug()){
				pSimulator->print("仿真时间=%f 进程[%s]运行结束！ \n",pSimulator->getClock(),getName());
			}
			reserved = false;
		}else{//如果进程为条件延迟
			if(pSimulator->isDebug()){
				pSimulator->print("仿真时间=%f 进程[%s]进入条件延迟，复活点[%s]。\n",pSimulator->getClock(),getName(),getPhaseName());
			}
			//将进程对象添加到条件事件列表
			pSimulator->scheduleConditionalEvent(this);
		}
	}
}


