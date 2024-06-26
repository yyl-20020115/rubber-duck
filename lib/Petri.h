/**
 * @file Petri.h
 * @brief 该文件定义了随机Petri网仿真模型框架
 * Node为网节点对象
 * Place为库所节点对象类
 * Transition为变迁对象类
 * Connection为输入输出关系对象类
 * PetriNet为Petri网对象类
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2022-10-15
 * 
 * @copyright Copyright(C) 2023 liqun
 */
#ifndef PETRI_H_
#define PETRI_H_

#include <climits>
#include <iostream>
#include "EventNotice.h"
#include "Simulator.h"

using namespace std;
using namespace rubber_duck;
class Transition;
class Connection;

#define PLACE_NODE 0
#define TRANSITION_NODE 1

/**
 * @brief Petri网节点对象类
 */
class Node{
protected:
    /**
     * @brief 节点标识
     */
    int id;
    /**
     * @brief 节点名称
     */
    std::string name;
    /**
     * @brief 节点输入连接关系集合
     */
    std::list<Connection *> inputConnections;
    /**
     * @brief 节点输出连接关系集合
     */
    std::list<Connection *> outputConnections;
    /**
     * @brief 节点类型，PLACE_NODE为0，TRANSITION_NODE为1
     */
    int type;
public:
	/**
	 * @brief 创建Node对象
	 * @param  id    节点唯一标识
     * @param  name  节点名称
     * @param  type  节点类型
	 */
    Node(int id,const char * name,int type){
        this->id = id;
        this->name = name;
        this->type = type;
    };
	/**
	 * @brief 获得节点标识
	 * @return int 节点唯一标识
	 */
    int getID(){
        return id;
    };
	/**
	 * @brief 获得节点名称
	 * @return const char * 节点名称
	 */
    const char * getName(){
        return name.c_str();
    };
	/**
	 * @brief 添加该节点的输入连接关系
	 * @param  pConnection 连接关系对象指针
	 */
    void addInputConnection(Connection * pConnection){
        inputConnections.push_back(pConnection);
    };
	/**
	 * @brief 添加该节点的输出连接关系
	 * @param  pConnection 连接关系对象指针
	 */
    void addOutputConnection(Connection * pConnection){
        outputConnections.push_back(pConnection);
    };
	/**
	 * @brief 获得节点类型
	 * @return int 节点类型
	 */
    int getType(){
        return type;
    }
};
/**
 * @brief Petri网库所对象类
 */
class Place:public Node{
private:
    /**
     * @brief 库所Token数量
     */
    int tokens;
    /**
     * @brief 库所容量
     */
    int capacity;
    /**
     * @brief 库所保留的Token数量
     */
    int reservedTokens;
    /**
     * @brief 库所输入变迁集合
     */
    std::list<Transition *> inputs;
    /**
     * @brief 库所输出变迁集合
     */
    std::list<Transition *> outputs;
public:
	/**
	 * @brief 创建Place对象
	 * @param  id       库所唯一标识
     * @param  name     库所名称
     * @param  tokens   库所初始Token数量
     * @param  capacity 库所容量
	 */
    Place(int id,const char * name,int tokens,int capacity = INT_MAX):Node(id,name,PLACE_NODE) {
		this->tokens = tokens;
        this->reservedTokens = 0;
        this->capacity = capacity;
	};
	/**
	 * @brief 添加该库所的输入变迁对象
	 * @param  pTransition 变迁对象指针
	 */
    void addInput(Transition * pTransition){
        inputs.push_back(pTransition);
    };
	/**
	 * @brief 添加该库所的输出变迁对象
	 * @param  pTransition 变迁对象指针
	 */
    void addOutput(Transition * pTransition){
        outputs.push_back(pTransition);
    };
	/**
	 * @brief 获得库所当前Token数量
	 * @return int Token数量
	 */
    int getTokens(){
        return tokens;
    };

    friend class Transition;
};

/**
 * @brief 变迁延迟函数指针类型定义
 * @return double 变迁延迟时间
*/
typedef double (* DurationFunction)();

class PetriNet;
/**
 * @brief Petri网变迁对象类
 */
class Transition:public Node{
public:
    /**
     * @brief 变迁输入库所集合
     */
    std::list<Place *> inputs;
    /**
     * @brief 变迁输出库所集合
     */
    std::list<Place *> outputs;
    /**
     * @brief 变迁延迟函数
     */    
    DurationFunction duration;
    /**
     * @brief 所属的Petri网对象
     */    
    PetriNet * pParent;
public:
	/**
	 * @brief 创建Transition对象
	 * @param  id       变迁唯一标识
     * @param  name     变迁名称
     * @param  duration 变迁延迟函数指针
	 */
    Transition(int id,const char * name,DurationFunction duration):Node(id,name,TRANSITION_NODE) {
        this->duration = duration;
	};
	/**
	 * @brief 添加该变迁的输入库所对象
	 * @param  pPlace 库所对象指针
	 */
    void addInput(Place * pPlace){
        inputs.push_back(pPlace);
    };
	/**
	 * @brief 添加该变迁的输出库所对象
	 * @param  pPlace 库所对象指针
	 */
    void addOutput(Place * pPlace){
        outputs.push_back(pPlace);
    };
	/**
	 * @brief 当前变迁是否满足点火条件
	 * @return bool 是否可以点火
	 */
    bool canFire();
    /**
	 * @brief 变迁开始点火计算
	 * @param  pSimulator 仿真引擎对象指针
	 */
    void beginFire(Simulator * pSimulator);
    /**
	 * @brief 变迁结束点火计算
	 * @param  pSimulator 仿真引擎对象指针
	 */
    void fire(Simulator * pSimulator);
     /**
	 * @brief 设置该变迁所属的Petri网对象
	 * @param  pParent Petri网对象指针
	 */   
    void setParent(PetriNet * pParent){
        this->pParent = pParent;
    };
};
/**
 * @brief 连接对象类
 */
class Connection{
private:
    /**
     * @brief 连接权值
     */
    int weight;
    /**
     * @brief 连接起始节点
     */
    Node * startNode;
    /**
     * @brief 连接终止节点
     */
    Node * endNode;
public:
	/**
	 * @brief 创建Connection对象
	 * @param  startNode   起始节点对象指针
     * @param  endNode     终止节点对象指针
     * @param  weight      权值
	 */
    Connection(Node * startNode,Node * endNode,int weight){
        if(startNode == NULL || endNode == NULL){
            cerr << endl << "运行错误 : " << "Petri网连接节点为空！" << endl;
	        exit(0);
        }
        if(startNode->getType() == endNode->getType()){
            cerr << endl << "运行错误 : " << "Petri网连接节点类型相同！" << endl;
	        exit(0);
        }
        this->startNode = startNode;
        this->endNode = endNode;
        this->weight = weight;
	};
	/**
	 * @brief 获得起始节点对象
	 * @return Node * 起始节点对象指针
	 */
    Node * getStartNode(){
        return startNode;
    };
	/**
	 * @brief 获得终止节点对象
	 * @return Node * 终止节点对象指针
	 */
    Node * getEndNode(){
        return endNode;
    };
	/**
	 * @brief 获得权值
	 * @return int 权值
	 */
    int getWeight(){
        return weight;
    }
};
/**
 * @brief Petri网对象类
 */
class PetriNet{
private:
    /**
     * @brief Petri网名称
     */
    std::string name;
    /**
     * @brief 连接关系集合
     */
    std::list<Connection *> connections;
    /**
     * @brief 节点集合
     */    
    std::list<Node *> nodes;
    /**
     * @brief 仿真引擎对象
     */
    Simulator * pSimulator;
    /**
     * @brief 上次状态更新时间
     */    
    double lastTime;
public:
	/**
	 * @brief 创建PetriNet对象
	 * @param  name   Petri网名称
	 */
    PetriNet(const char * name){
        this->name = name;
    };
	/**
	 * @brief 删除PetriNet对象
	 */
    ~PetriNet();
	/**
	 * @brief 添加库所对象
	 * @param  id       库所唯一标识
     * @param  name     库所名称
     * @param  tokens   库所初始Token数量
     * @param  capacity 库所容量
	 */
    void addPlace(int id,const char * name,int tokens,int capacity = INT_MAX);
	/**
	 * @brief 添加库所对象
	 * @param  pPlace   库所对象指针
	 */
    void addPlace(Place * pPlace);
 	/**
	 * @brief 添加变迁对象
	 * @param  id       变迁唯一标识
     * @param  name     变迁名称
     * @param  duration 变迁延迟函数指针
	 */   
    void addTransition(int id,const char * name,DurationFunction duration);
 	/**
	 * @brief 添加变迁对象
	 * @param  pTransition  变迁对象指针
	 */ 
    void addTransition(Transition * pTransition);
  	/**
	 * @brief 添加连接关系
	 * @param  startID  起始节点唯一标识
     * @param  endID    终止节点唯一标识
     * @param  weight   关系权值
	 */    
    void addConnection(int startID,int endID,int weight = 1);
	/**
	 * @brief 获得指定标识的节点对象
     * @param  id       节点唯一标识
	 * @return Node *   节点对象指针
	 */
    Node * getNode(int id);
	/**
	 * @brief 生成Petri网节点和关系数据
	 */
    void build();
	/**
	 * @brief 初始化Petri网开始点火事件
     * @param  pSimulator  仿真引擎对象指针
	 */
    void initialize(Simulator * pSimulator);
 	/**
	 * @brief 打印Petri网当前状态
	 */   
    void print();
};
/**
 * @brief Petri网开始点火事件类
 */
class BeginFireEvent:public EventNotice{
private:
    /**
     * @brief 执行开始点火的变迁对象
     */
    Transition * pTransition;
public:
	/**
	 * @brief 创建开始点火事件
	 * @param  pTransition   开始点火的变迁对象指针
	 */
	BeginFireEvent(Transition * pTransition):EventNotice(-1) {
        this->pTransition = pTransition;
        char nameBuffer[128];
        sprintf(nameBuffer,"Transition%s开始点火事件",pTransition->getName());
        setName(nameBuffer);
	};
	/**
	 * @brief 开始点火事件发生条件判断函数
	 * @param  pSimulator   仿真引擎对象指针
     * @return bool 事件是否满足发生条件
	 */
	virtual bool canTrigger(Simulator * pSimulator){
        return pTransition->canFire();
    };
	/**
	 * @brief 开始点火事件发生执行函数
	 * @param  pSimulator   仿真引擎对象指针
	 */
	virtual void trigger(Simulator * pSimulator){
        pTransition->beginFire(pSimulator);
    };
};
/**
 * @brief Petri网结束点火事件类
 */
class EndFireEvent:public EventNotice{
private:
    /**
     * @brief 结束点火的变迁对象
     */
    Transition * pTransition;
public:
	/**
	 * @brief 创建结束点火事件
	 * @param  pTransition   结束点火的变迁对象指针
	 */
	EndFireEvent(Transition * pTransition,double time):EventNotice(time) {
        this->pTransition = pTransition;
        char nameBuffer[128];
        sprintf(nameBuffer,"Transition%s点火结束事件",pTransition->getName());
        setName(nameBuffer);
	};
	/**
	 * @brief 结束点火事件发生执行函数
	 * @param  pSimulator   仿真引擎对象指针
	 */
	virtual void trigger(Simulator * pSimulator){
         pTransition->fire(pSimulator);
    };
};

#endif /* PETRI_H_ */