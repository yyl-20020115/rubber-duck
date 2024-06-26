/**
 * @file Petri.cpp
 * @brief 
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2022-10-23
 * 
 * @copyright Copyright(C) 2023 liqun
 */
#include <iostream>
#include "Petri.h"

using namespace std;
using namespace rubber_duck;

bool Transition::canFire(){
    for(list<Connection *>::iterator it = inputConnections.begin();
    it != inputConnections.end();it ++){
        Connection * c = *it;
        Place * pPlace = (Place *)(c->getStartNode());
        if(pPlace->tokens - pPlace->reservedTokens < c->getWeight()){
            return false;
        }
    }
    for (list<Connection *>::iterator it = outputConnections.begin();
    it != outputConnections.end();it ++){
        Connection * c = *it;
        Place * pPlace = (Place *)(c->getEndNode());
        if(c->getWeight() + (pPlace->tokens - pPlace->reservedTokens) > pPlace->capacity){
            return false;
        }
    }
	return true;
}

void Transition::beginFire(Simulator * pSimulator){
    for(list<Connection *>::iterator it = inputConnections.begin();
    it != inputConnections.end();it ++){
        Connection * c = *it;
        Place * pPlace = (Place *)(c->getStartNode());
        pPlace->reservedTokens = pPlace->reservedTokens + c->getWeight();
    }

	//调度其服务完成事件
	EndFireEvent * pEvent = new EndFireEvent(this,pSimulator->getClock() + duration());
	pSimulator->scheduleEvent(pEvent);
}

void Transition::fire(Simulator * pSimulator){
    pParent->print();
    for(list<Connection *>::iterator it = inputConnections.begin();
    it != inputConnections.end();it ++){
        Connection * c = *it;
        Place * pPlace = (Place *)(c->getStartNode());
        pPlace->tokens = pPlace->tokens - c->getWeight();
        pPlace->reservedTokens = pPlace->reservedTokens - c->getWeight();
    }
    for(list<Connection *>::iterator it = outputConnections.begin();
    it != outputConnections.end();it ++){
        Connection * c = *it;
        Place * pPlace = (Place *)(c->getEndNode());
        pPlace->tokens = pPlace->tokens + c->getWeight();
    }
    pSimulator->scheduleConditionalEvent(new BeginFireEvent(this));
}

PetriNet::~PetriNet(){
    for(list<Node *>::iterator it = nodes.begin();it != nodes.end();it ++){
        Node * node = *it;
        delete node;
    }
    for(list<Connection *>::iterator it = connections.begin();it != connections.end();it ++){
        Connection * c = *it;
        delete c;
    }
    nodes.clear();
    connections.clear();
}

Node * PetriNet::getNode(int id){
    for(list<Node *>::iterator it = nodes.begin();it != nodes.end();it ++){
        Node * pNode = *it;
        if(pNode->getID() == id) {
            return pNode;
        }
    }
    return NULL;
}

void PetriNet::addPlace(int id,const char * name,int tokens,int capacity){
    if(getNode(id) != NULL){
        cerr << endl << "运行错误 : " << "Petri网addPlace节点重复！" << endl;
	    exit(0);
    }
    nodes.push_back(new Place(id,name,tokens,capacity));
}

void PetriNet::addPlace(Place * pPlace){
    if(getNode(pPlace->getID()) != NULL){
        cerr << endl << "运行错误 : " << "Petri网addPlace节点重复！" << endl;
	    exit(0);
    }    
    nodes.push_back(pPlace);
}

void PetriNet::addTransition(int id,const char * name,DurationFunction duration){
    if(getNode(id) != NULL){
        cerr << endl << "运行错误 : " << "Petri网addTransition节点重复！" << endl;
	    exit(0);
    }      
    nodes.push_back(new Transition(id,name,duration));
}

void PetriNet::addTransition(Transition * pTransition){
    if(getNode(pTransition->getID()) != NULL){
        cerr << endl << "运行错误 : " << "Petri网addTransition节点重复！" << endl;
	    exit(0);
    }
    nodes.push_back(pTransition);
}

void PetriNet::addConnection(int startID,int endID,int weight){
    for(list<Connection *>::iterator it = connections.begin();it != connections.end();it ++){
        Connection * c = *it;
        if(c->getStartNode()->getID() == startID && c->getEndNode()->getID() == endID){
            cerr << endl << "运行错误 : " << "Petri网连接重复！" << endl;
	        exit(0);
        }
    }
    connections.push_back(new Connection(getNode(startID),getNode(endID),weight));
}

void PetriNet::build(){
	for(list<Connection *>::iterator it = connections.begin();it != connections.end();it ++){
        Connection * c = *it;
        Node * startNode = c->getStartNode();
        Node * endNode = c->getEndNode();
        c->getStartNode()->addOutputConnection(c);
        c->getEndNode()->addInputConnection(c);
        if(startNode->getType() == PLACE_NODE){
            Place * pPlace = (Place *)(startNode);
            Transition * pTransition = (Transition *)(endNode);
            pPlace->addOutput(pTransition);
            pTransition->addInput(pPlace);
        }else{
            Transition * pTransition = (Transition *)(startNode);
            Place * pPlace = (Place *)(endNode);
            pTransition->addOutput(pPlace);
            pPlace->addInput(pTransition);
        }
    }
}

void PetriNet::initialize(Simulator * pSimulator){
    this->pSimulator = pSimulator;
    lastTime = pSimulator->getClock();
    for(list<Node *>::iterator it = nodes.begin();it != nodes.end();it ++){
        Node * pNode = *it;
        if(pNode->getType() == TRANSITION_NODE){
            Transition * pTransition = (Transition *)(pNode);
            pTransition->setParent(this);
            pSimulator->scheduleConditionalEvent(new BeginFireEvent(pTransition));
        }
    }
}

void PetriNet::print(){
    if(lastTime == pSimulator->getClock()){
        return;
    }
    pSimulator->print("仿真时间: %f Petri网 %s 状态:", lastTime,name.c_str());
    for(list<Node *>::iterator it = nodes.begin();it != nodes.end();it ++){
        Node * pNode = *it;
        if(pNode->getType() == PLACE_NODE){
            Place * pPlace = (Place *)(pNode);
            pSimulator->print("%s:%d ", pPlace->getName(),pPlace->getTokens());
        }
    }
    pSimulator->print("\n");
    lastTime = pSimulator->getClock();
}
