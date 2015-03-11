/*
 * ClientNode.h
 *
 *  Created on: 10/11/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef CLIENTNODE_H_
#define CLIENTNODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <pthread.h>
#include <vector>

#include "Geral.h"
#include "BaseNode.h"

using namespace std;

class ClientNode : public BaseNode
{
public:
	ClientNode(stNodeParams* config, vector<stContentsParams*> contents);
	virtual ~ClientNode();

	virtual void Start();
	virtual void Stop();
	virtual void ProcessEvent(int iEventType);
	virtual void ProcessRx();
	virtual void ProcessTx();

	virtual void StoreMeasurements();

private:
	void GenerateNewPackage();
	stPackage* CreateNewRequest();

private:
	vector<stContentsParams*> contents_;
	int* contentsTx_;
};

#endif /* CLIENTNODE_H_ */
