/**
 * @file EventNotice.h
 * @brief 事件对象类EventNotice
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-02
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#ifndef EVENTNOTICE_H_
#define EVENTNOTICE_H_

#include <string>
#include <string.h>
#include <stdio.h>

namespace rubber_duck{

class Simulator;

/**
 * @brief 事件记录类
 */
class EventNotice {
public:
	/**
	 * @brief 创建Event Notice对象
	 * @param  ftime            事件发生时间
	 * @param  p                事件优先级，缺省为0
	 */
	EventNotice(double ftime,int p = 0) {
		this->time = ftime;
		name = "event";
		priority = p;
	}
	/**
	 * @brief 析构函数，删除Event Notice对象
	 */
	virtual ~EventNotice() {
	}
	/**
	 * @brief 查询事件优先级
	 * @return int 事件优先级
	 */
	int getPriority() const {
		return priority;
	}
	/**
	 * @brief 查询事件发生时间
	 * @return double 事件发生时间
	 */
	double getTime() const {
		return time;
	}

	void setTime(double t) {
		time = t;
	}
	/**
	 * @brief 事件在触发后是否需要保留
	 * @return true 事件在触发后不会由仿真引擎删除
	 * @return false 事件在触发后由仿真引擎自动删除
	 */
	bool isReserved(){
		return reserved;
	}
	/**
	 * @brief 获得事件所属对象
	 * @return void* 事件所属对象指针
	 */
	void * getOwner(){
		return owner;
	}
	/**
	 * @brief 设置事件所属对象
	 * @param  owner      事件所属对象指针
	 */
	void setOwner(void * owner){
		this->owner = owner;
	}
	/**
	 * @brief 获得事件名称
	 * @return const char* 事件名称字符串
	 */
	const char * getName(){
		return name.c_str();
	}
	/**
	 * @brief 设置事件名称
	 * @param  n   事件名称
	 */
	void setName(const char * n){
		name = n;
	}
	/**
	 * @brief 当前系统状态是否满足事件发生条件
	 * @param  pSimulator       仿真引擎对象指针
	 * @return true 	满足事件发生条件，事件可以发生
	 * @return false 	不满足事件发生条件，事件不能发生
	 */
	virtual bool canTrigger(Simulator * pSimulator){	return true;	};
	/**
	 * @brief 事件发生时的事件处理函数，必须由派生事件类实现
	 */
	virtual void trigger(Simulator *) = 0;
protected:
	/**
	 * @brief 事件时间
	 */
	double time;
	/**
	 * @brief 事件优先级
	 */
	int priority;
	/**
	 * @brief 事件在触发后是否需要保留
	 */
	bool reserved = false;
	/**
	 * @brief 事件所属对象
	 */
	void * owner = NULL;
	/**
	 * @brief 事件名称
	 */
	std::string name;
};

}
#endif /* EVENTNOTICE_H_ */
