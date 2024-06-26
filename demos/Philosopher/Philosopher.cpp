/**
 * @file Philosopher.cpp
 * @brief 哲学家就餐问题的Petri网仿真模型
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
    PetriNet * pNet = new PetriNet("Philosopher");
    int id = 0;
    Place * P0 = new Place(id ++,"P0",1);
    Place * P1 = new Place(id ++,"P1",1);
    Place * P2 = new Place(id ++,"P2",1);
    Place * P3 = new Place(id ++,"P3",1); 
    Place * P4 = new Place(id ++,"P4",1);
    Place * P5 = new Place(id ++,"P5",1);
    Place * P6 = new Place(id ++,"P6",1);
    Place * P7 = new Place(id ++,"P7",1);
    Place * P8 = new Place(id ++,"P8",1);
    Place * P9 = new Place(id ++,"P9",1);

    Place * P10 = new Place(id ++,"P10",1);
    Place * P11 = new Place(id ++,"P11",1);
    Place * P12 = new Place(id ++,"P12",1);
    Place * P13 = new Place(id ++,"P13",1); 
    Place * P14 = new Place(id ++,"P14",1);

    pNet->addPlace(P0);
    pNet->addPlace(P1);
    pNet->addPlace(P2);
    pNet->addPlace(P3);
    pNet->addPlace(P4);
    pNet->addPlace(P5);
    pNet->addPlace(P6);
    pNet->addPlace(P7);
    pNet->addPlace(P8);
    pNet->addPlace(P9);
    pNet->addPlace(P10);
    pNet->addPlace(P11);
    pNet->addPlace(P12);
    pNet->addPlace(P13);
    pNet->addPlace(P14);

    Transition * t0 = new Transition(id ++,"t0",stepDuration);
    Transition * t1 = new Transition(id ++,"t1",stepDuration);
    Transition * t2 = new Transition(id ++,"t2",stepDuration);
    Transition * t3 = new Transition(id ++,"t3",stepDuration);
    Transition * t4 = new Transition(id ++,"t4",stepDuration);
    Transition * t5 = new Transition(id ++,"t5",stepDuration);
    Transition * t6 = new Transition(id ++,"t6",stepDuration);
    Transition * t7 = new Transition(id ++,"t7",stepDuration);
    Transition * t8 = new Transition(id ++,"t8",stepDuration);
    Transition * t9 = new Transition(id ++,"t9",stepDuration);

    pNet->addTransition(t0);
    pNet->addTransition(t1);
    pNet->addTransition(t2);
    pNet->addTransition(t3);
    pNet->addTransition(t4);
    pNet->addTransition(t5);
    pNet->addTransition(t6);
    pNet->addTransition(t7);
    pNet->addTransition(t8);
    pNet->addTransition(t9);

    pNet->addConnection(P0->getID(),t0->getID());
    pNet->addConnection(P0->getID(),t4->getID());

    pNet->addConnection(P1->getID(),t0->getID());

    pNet->addConnection(P2->getID(),t0->getID());
    pNet->addConnection(P2->getID(),t1->getID());

    pNet->addConnection(P3->getID(),t1->getID());

    pNet->addConnection(P4->getID(),t1->getID());
    pNet->addConnection(P4->getID(),t2->getID());

    pNet->addConnection(P5->getID(),t2->getID());

    pNet->addConnection(P6->getID(),t2->getID());
    pNet->addConnection(P6->getID(),t3->getID());

    pNet->addConnection(P7->getID(),t3->getID());

    pNet->addConnection(P8->getID(),t3->getID());
    pNet->addConnection(P8->getID(),t4->getID());

    pNet->addConnection(P9->getID(),t4->getID());

    pNet->addConnection(P10->getID(),t5->getID());
    pNet->addConnection(P11->getID(),t6->getID());
    pNet->addConnection(P12->getID(),t7->getID());
    pNet->addConnection(P13->getID(),t8->getID());
    pNet->addConnection(P14->getID(),t9->getID());

    pNet->addConnection(t0->getID(),P10->getID());
    pNet->addConnection(t1->getID(),P11->getID());
    pNet->addConnection(t2->getID(),P12->getID());
    pNet->addConnection(t3->getID(),P13->getID());
    pNet->addConnection(t4->getID(),P14->getID());

    pNet->addConnection(t5->getID(),P0->getID());
    pNet->addConnection(t5->getID(),P1->getID());
    pNet->addConnection(t5->getID(),P2->getID());

    pNet->addConnection(t6->getID(),P2->getID());
    pNet->addConnection(t6->getID(),P3->getID());
    pNet->addConnection(t6->getID(),P4->getID());

    pNet->addConnection(t7->getID(),P4->getID());
    pNet->addConnection(t7->getID(),P5->getID());
    pNet->addConnection(t7->getID(),P6->getID());

    pNet->addConnection(t8->getID(),P6->getID());
    pNet->addConnection(t8->getID(),P7->getID());
    pNet->addConnection(t8->getID(),P8->getID());

    pNet->addConnection(t9->getID(),P8->getID());
    pNet->addConnection(t9->getID(),P9->getID());
    pNet->addConnection(t9->getID(),P0->getID());

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