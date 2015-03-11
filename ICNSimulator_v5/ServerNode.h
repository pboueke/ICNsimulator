/*
 * ServerNode.h
 *
 *  Created on: 10/11/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef SERVERNODE_H_
#define SERVERNODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <pthread.h>

#include "Geral.h"
#include "BaseNode.h"

using namespace std;

class ServerNode: public BaseNode
{
public:
	ServerNode(stNodeParams* config);
	virtual ~ServerNode();

	virtual void Start();
	virtual void Stop();

	virtual void ProcessTx();
	virtual void ProcessRx(stPackage* pack);
};

#endif /* SERVERNODE_H_ */
