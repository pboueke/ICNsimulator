/*
 * RouterRcNode.cpp
 *
 *  Created on: 19/07/2014
 *      Author: Diego Vargas Jannibelli
 */

#include "OperationManager.h"
#include "RouterRcNodeQueue.h"

RouterRcNodeQueue::RouterRcNodeQueue(stNodeParams* config, bool cacheEnabled) : RouterRcNodeNQueue(config, cacheEnabled)
{
	// TODO Auto-generated constructor stub

}

RouterRcNodeQueue::~RouterRcNodeQueue()
{
	// TODO Auto-generated destructor stub
}

void RouterRcNodeQueue::ProcessRx(stPackage* pack)
{
	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "PROCESS_RX,PACKAGE RECEIVED (ClassType;TTL;RC)," << pack->contentInfo->iClassType << "," << pack->iTTLTag << "," << pRCperContent_[pack->contentInfo->iClassType];
	Logger::instance()->LogTrace(msg.str());

	numRcPerTTL_[pack->iTTLTag] += pRCperContent_[pack->contentInfo->iClassType];

	if(pRCperContent_[pack->contentInfo->iClassType] < NUM_MAX_RC_EVAL)
	{
		numPackPerRc_[pRCperContent_[pack->contentInfo->iClassType]]++;
		numPackRcPerTTL_[pack->iTTLTag][pRCperContent_[pack->contentInfo->iClassType]]++;
	}

	RouterBaseNode::ProcessRx(pack);
}

