/*
 * RouterRcNode.h
 *
 *  Created on: 19/07/2014
 *      Author: Diego Vargas Jannibelli
 */

#ifndef ROUTERRCNODEQUEUE_H_
#define ROUTERRCNODEQUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <pthread.h>

#include "Geral.h"
#include "RouterRcNodeNQueue.h"

class RouterRcNodeQueue: public RouterRcNodeNQueue
{
public:
	RouterRcNodeQueue(stNodeParams* config, bool cacheEnabled);
	virtual ~RouterRcNodeQueue();

protected:
	virtual void ProcessRx(stPackage* pack);
};

#endif /* ROUTERRCNODEQUEUE_H_ */
