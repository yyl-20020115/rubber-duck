/**
 * @file Queue.h
 * @brief 队列模板Queue
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-02
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#include <queue>

#ifndef QUEUE_H_
#define QUEUE_H_

template< class T >
/**
 * @brief 队列模板，按照排队操作封装了标准C++的queue模板
 */
class Queue : public std::queue<T>{
public:
	/**
	 * @brief 在队列中添加数据
	 * @param  pData    数据对象
	 */
	void enqueue( T pData ){
		this->push(pData);
	}
	/**
	 * @brief 获取队列中的第一个数据对象
	 * @return T 队列中的第一个数据对象
	 */
	T dequeue(){
		T pData = this->front();

		this->pop();

		return pData;
	}
	/**
	 * @brief 获取队列中的数据对象数量
	 * @return int 数据对象数量
	 */
	int getCount()const{
		return (int)this->size();
	}
};

#endif /* QUEUE_H_ */