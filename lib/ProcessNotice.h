/**
 * @file ProcessNotice.h
 * @brief 进程对象类ProcessNotice
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-02
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#ifndef PROCESS_NOTICE_H_
#define PROCESS_NOTICE_H_

#include "EventNotice.h"

namespace rubber_duck{

/**
 * @brief 进程对象类
 */
class ProcessNotice:public EventNotice{
protected:
	/**
	 * @brief 进程复活点
	 */
	int phase;
	/**
	 * @brief 进程复活时间
	 */
	double activateTime;
	/**
	 * @brief 当前进程是否终止
	 */
	bool terminated = false;

public:
	/**
	 * @brief 创建Process Notice对象
	 * @param  ntype            事件类型
	 * @param  ftime            为进程复活时间
	 * @param  p                进程复活点
	 */
	ProcessNotice(double ftime,int p) :EventNotice(ftime){
		reserved = true;
		this->phase = p;
	}
	/**
	 * @brief 删除Process Notice对象
	 */
	virtual ~ProcessNotice() {
	}
	/**
	 * @brief 进程推进函数，进程复活后的推进函数，返回复活时间
	 * 如果复活时间小于0时，
	 * 		如果terminated为false，则进程处于条件延迟状态，进入条件事件表
	 * 		如果terminated为true，则进程运行结束
	 * 如果复活时间大于0，则进程处于无条件延迟状态，进入未来事件表
	 * @param  pSimulator  仿真引擎对象指针
	 * @return double 复活时间
	 */
	virtual double runToBlocked(Simulator * pSimulator) = 0;
	/**
	 * @brief 如果进程处于条件延迟状态，确定当前进程延迟条件是否满足
	 * true表示延迟条件不满足，仍处于条件延迟状态
	 * false表示满足延迟条件，进程可以复活推进，后续将调用runToBlocked函数
	 * @param  pSimulator       仿真引擎对象指针
	 * @return true 	处于条件延迟状态
	 * @return false 	处于无条件延迟状态
	 */
	virtual bool isConditionalBlocking(Simulator * pSimulator) = 0;
	/**
	 * @brief 获得当前进程复活点名称
	 * @return const char* 当前进程复活点名称
	 */
	virtual const char * getPhaseName() = 0;
	/**
	 * @brief 重载EventNotice是否满足事件发生条件函数，判断当前条件延迟进程能否复活
	 * @param  pSimulator       仿真引擎对象指针
	 * @return true 	事件满足发生条件，可以发生
	 * @return false 	事件不满足发生条件
	 */
	virtual bool canTrigger(Simulator * pSimulator){	return !isConditionalBlocking(pSimulator);	};
	/**
	 * @brief 重载EventNotice事件处理函数，执行进程推进函数
	 * @param  pSimulator    仿真引擎对象指针
	 */
	virtual void trigger(Simulator * pSimulator);
};

}
#endif /* PROCESS_NOTICE_H_ */
