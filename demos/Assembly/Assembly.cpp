/**
 * @file Assembly.cpp
 * @brief 装配问题Petri网仿真模型
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2023-03-30
 * 
 * @copyright Copyright(C) 2023 liqun
 */
#include "Petri.h"

using namespace std;
using namespace rubber_duck;

double stepDuration(){
    return 1;
}

void Initialization(Simulator * pSimulator){
    PetriNet * pNet = new PetriNet("Assemble");
    int id = 0;
    Place * s1 = new Place(id ++,"S1",10);
    Place * s2 = new Place(id ++,"S2",100,100);
    Place * s3 = new Place(id ++,"S3",1000,1000);
    Place * s4 = new Place(id ++,"S4",0,5); 
    Place * s5 = new Place(id ++,"S5",100,100);
    Place * s6 = new Place(id ++,"S6",0);
    Place * s7 = new Place(id ++,"S7",1);
    pNet->addPlace(s1);
    pNet->addPlace(s2);
    pNet->addPlace(s3);
    pNet->addPlace(s4);
    pNet->addPlace(s5);
    pNet->addPlace(s6);
    pNet->addPlace(s7);

    Transition * t1 = new Transition(id ++,"T1",stepDuration);
    Transition * t2 = new Transition(id ++,"T2",stepDuration);
    pNet->addTransition(t1);
    pNet->addTransition(t2);

    pNet->addConnection(s1->getID(),t1->getID());
    pNet->addConnection(s2->getID(),t1->getID());
    pNet->addConnection(s3->getID(),t1->getID(),2);
    pNet->addConnection(s7->getID(),t1->getID());

    pNet->addConnection(t1->getID(),s4->getID());
    pNet->addConnection(t1->getID(),s7->getID());
    
    pNet->addConnection(s3->getID(),t2->getID(),3);
    pNet->addConnection(s4->getID(),t2->getID());
    pNet->addConnection(s7->getID(),t2->getID());
    pNet->addConnection(s5->getID(),t2->getID());

    pNet->addConnection(t2->getID(),s6->getID());
    pNet->addConnection(t2->getID(),s7->getID());

	pNet->build();
	pNet->initialize(pSimulator);
}

int main(int argc, char* argv[]){
	Simulator * pSimulator = new Simulator(12345678,"Assembly.txt");
	Initialization(pSimulator);
	//pSimulator->setDebug();
	pSimulator->run(100);
    delete pSimulator;
	return 0;
}