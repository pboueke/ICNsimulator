/*
 * RouterRcNode.h
 *
 *  Created on: 19/07/2014
 *      Author: Diego Vargas Jannibelli
 */

#ifndef ROUTERRCNODENQUEUE_H_
#define ROUTERRCNODENQUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <pthread.h>
#include<limits.h>

#include "Geral.h"
#include "RouterBaseNode.h"

class RouterRcNodeNQueue: public RouterBaseNode
{
public:
	RouterRcNodeNQueue(stNodeParams* config, bool cacheEnabled);
	virtual ~RouterRcNodeNQueue();

	int GetRCValue(int classType)		{ return pRCperContent_[classType]; };

protected:
	virtual void Start();
	virtual void Stop();

	virtual void ProcessRx(stPackage* pack);
	virtual void ProcessTx();
	virtual void ProcessEvent(int iEventType);

	virtual bool CheckCache(int classType);

	virtual void StoreMeasurements();

	virtual void IncrementCacheAlgorithm(int classType);

	void IncrementRC(int classType);
	void DecrementRC(int classType);
	void CacheReplacement(int classType);

protected:
	unsigned long int** numPackRcPerTTL_;
	unsigned long int* pRCperContent_;
	unsigned long int* numRcPerTTL_;
	unsigned long int* numPackPerRc_;
	double* timePerRc_;
	double lastEvRcTime_;

	vector<stCacheItem*> cacheOccuped_;
	bool cacheEnabled_;
	int iCacheIndexPointer_;
	int iCacheGreaterRemoved_;
};

#endif /* ROUTERRCNODENQUEUE_H_ */
