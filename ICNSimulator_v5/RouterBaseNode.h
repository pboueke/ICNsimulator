/*
 * RouterNode.h
 *
 *  Created on: 19/11/2013
 *      Author: diego
 */

#ifndef ROUTERBASENODE_H_
#define ROUTERBASENODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <pthread.h>

#include "Geral.h"
#include "BaseNode.h"

using namespace std;

class RouterBaseNode: public BaseNode
{
public:
	RouterBaseNode(stNodeParams* config);
	virtual ~RouterBaseNode();

	virtual void Start();
	virtual void Stop();

	virtual void ProcessEvent(int iEventType);
	virtual void ProcessRx(stPackage* pack);
	virtual void ProcessTx();

protected:
	virtual bool CheckCache(int classType);

	virtual void StoreMeasurements();
	virtual void IncrementCacheAlgorithm(int classType);

	void InitProcess(stPackage* pack);
	void RemovePackFromSystem(stPackage* pack);

protected:
	unsigned long int* numPckPerTTL_;
	unsigned long int* numPckFilteredPerTTL_;
	unsigned long int* numPckPerContents_;
	unsigned long int* numFilteredPckPerContents_;
	unsigned long int* numPackageInPerContent_;

	int probInCachoPerTime_;

	ofstream myLogPhiup_;
	ofstream myLogProb_;
	ofstream myLogProbTTL_;
	ofstream myLogProbContent_;

	stCacheItem* pCacheMemory_;
	stLimits* pRCLimits_;

	stPackage* pPackToDelete_;
};

#endif /* ROUTERBASENODE_H_ */
