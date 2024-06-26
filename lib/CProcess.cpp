/**
 * @file ProcessNotice.cpp
 * @brief 
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2022-10-02
 * 
 * @copyright Copyright(C) 2022 liqun
 */

#include "CProcess.h"

namespace rubber_duck{

Simulator * CProcess::simulator = NULL;

void process(void * param){
	CProcess * pProcess = (CProcess *)param;
	pProcess->actions();
	pProcess->setReserved(false);
	pProcess->processFinished();
}

void CProcess::request(Resource * r,int units){
	if(!r->request(this,units)){
		Coroutine::switchToMain();
	}
}

void CProcess::relinquish(Resource * r,int units){
	r->relinquish(this,units);
}

}