/*
 * RouterProbNode.h
 *
 *  Created on: 19/07/2014
 *      Author: Diego Vargas Jannibelli
 */

#ifndef ROUTERPROBNODE_H_
#define ROUTERPROBNODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <pthread.h>

#include "Geral.h"
#include "RouterBaseNode.h"

class RouterProbNode: public RouterBaseNode {
public:
	RouterProbNode(stNodeParams* config);
	virtual ~RouterProbNode();

protected:
	virtual void IncrementCacheAlgorithm(int classType);

	virtual bool CheckCache(int classType);

	void AdjustProbability(int classType);
};

#endif /* ROUTERPROBNODE_H_ */
