/**
 * @file EventList.h
 * @brief 事件表类EventList
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-03
 * 
 * @copyright Copyright(C) 2020 liqun
 */

#ifndef EVENTLIST_H_
#define EVENTLIST_H_

#include <list>
#include <vector>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include "EventNotice.h"
#include "Error.h"

namespace rubber_duck{

//浮点数相等的比较
#define fequal(a,b) (fabs((a)-(b))<0.000001)

/**
 * @brief 事件表类EventList
 */
class EventList : public std::list<EventNotice*>{
public:
	/**
	 * @brief 事件表中是否存在事件
	 * @return true 存在事件
	 * @return false 不存在事件
	 */
	bool isEmpty(){
		return size() == 0;
	}
	/**
	 * @brief 插入新事件
	 * @param  pEvent    事件对象指针
	 */
	void insertEvent(EventNotice* pEvent){
		//前面有更大时间戳的事件，则插入其之前
		for (iterator it = begin();it != end();it ++){
			EventNotice* pItEvent = *it;
			if (pEvent->getTime() < pItEvent->getTime()){
				insert(it,pEvent);
				return;
			}
		}
		//否则插入末尾
		push_back(pEvent);
	}

	/**
	 * @brief 删除事件
	 * @param  pEvent    事件对象指针
	 */
	void removeEvent(EventNotice* pEvent){
		for (iterator it = begin();it != end();it ++){
			EventNotice* pItEvent = *it;
			if (pItEvent == pEvent){
				this->erase(it);
				return;
			}
		}
	}

	/**
	 * @brief 是否包含事件
	 * @param  pEvent    事件对象指针
	 */
	bool hasEvent(EventNotice* pEvent){
		for (iterator it = begin();it != end();it ++){
			EventNotice* pItEvent = *it;
			if (pItEvent == pEvent){
				return true;
			}
		}
		return false;
	}

	/**
	 * @brief 获取下一最早未来事件时间
	 * @return double 下一最早未来事件时间
	 */
	double getImminentEventTime()const{
		//调用该函数时确保里面有事件
		if(size() == 0){
			warn(EVENTLISTNULL);
		}
		return (*begin())->getTime();//第一个事件就是下一未来事件
	}
	/**
	 * @brief 移出下一最早未来事件对象
	 * @return EventNotice* 下一最早未来事件对象指针
	 */
	EventNotice* popImminentEvent(){
		if (size() == 0) return NULL;
		EventNotice* pEvent = *begin();//第一个事件记录
		erase(begin());//从列表中删除该记录
		return pEvent;
	}
	/**
	 * @brief 移出所有最早未来事件对象
	 * @return std::vector<EventNotice*> 最早未来事件对象集合
	 */
	std::vector<EventNotice*> popImminentEventList(){
		std::vector<EventNotice*> ImminentEventList;
		if (size() == 0) return ImminentEventList;

		//下一未来事件的时间
		const double ImminentEventTime = (*begin())->getTime();
		double nextEventTime;//临时变量，下一事件时间
		do {
			ImminentEventList.push_back(*begin());
			erase(begin());
			if (empty()) break;//已经没有事件了，停止循环
			nextEventTime = (*begin())->getTime();
		} while (nextEventTime == ImminentEventTime);
		return ImminentEventList;
	}
	/**
	 * @brief 移出所有最早未来事件对象，并按照事件优先级由大到小排序
	 * @return std::vector<EventNotice*> 最早未来事件对象集合
	 */
	std::vector<EventNotice*> popImminentEventListPriority(){
		std::vector<EventNotice*> ImminentEventList;
		if (size() == 0) return ImminentEventList;
		//下一未来事件的时间
		const double ImminentEventTime = (*begin())->getTime();
		double nextEventTime;//临时变量，下一事件时间
		do{
			ImminentEventList.push_back(*begin());
			erase(begin());
			if (empty()) break;//已经没有事件了，停止循环
			nextEventTime = (*begin())->getTime();
		} while (nextEventTime == ImminentEventTime);

		//排序
		std::sort(ImminentEventList.begin(),ImminentEventList.end(),EventList::largeTypeFirst);

		return ImminentEventList;
	}

	void removeAll(){
		for (iterator it = begin();it != end();it ++){
			EventNotice* pItEvent = *it;
			delete pItEvent; 
		}
		clear();
	}

private:
	//
	/**
	 * @brief 类型排序函数：由大到小
	 * @param  elem1            第一个元素
	 * @param  elem2            第二个元素
	 * @return true 第一个元素大于第二个元素
	 * @return false 第一个元素不大于第二个元素
	 */
	static bool largeTypeFirst ( EventNotice* elem1, EventNotice* elem2 ){
		return elem1->getPriority() > elem2->getPriority();
	}
};

}

#endif /* EVENTLIST_H_ */
