/*
 * RouterProbNode.cpp
 *
 *  Created on: 19/07/2014
 *      Author: Diego Vargas Jannibelli
 */

#include "OperationManager.h"
#include "RouterProbNode.h"

RouterProbNode::RouterProbNode(stNodeParams* config) : RouterBaseNode(config)
{
	// TODO Auto-generated constructor stub

}

RouterProbNode::~RouterProbNode()
{
	// TODO Auto-generated destructor stub
}

bool RouterProbNode::CheckCache(int classType)
{
	double probCache = GetRandomByDistribution(distToRandom_);
	if(probCache <= pCacheMemory_[classType].dProbability)
		return true;

	return false;
}

void RouterProbNode::IncrementCacheAlgorithm(int classType)
{
	AdjustProbability(classType);
}

void RouterProbNode::AdjustProbability(int classType)
{
	if(TimeHandler::instance()->getCurrentTime() < 1)
		return;

	pCacheMemory_[classType].dProbability = (double) numPackageInPerContent_[classType] / (double) numPackageIn_;

	OperationManager::instance()->AddProbabilityPerContent(config_->iDomainIdent, classType, pCacheMemory_[classType].dProbability);

	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "PROB_ADJUSTED,CONTENT," << classType << ",PROB," << pCacheMemory_[classType].dProbability;
	Logger::instance()->LogTrace(msg.str());

//	stringstream probs;
//	probs << TimeHandler::instance()->getCurrentTime() << " ";
//	for(int i = 0; i < OperationManager::instance()->GetNumberOfContents(); i++)
//		probs <<  pCacheMemory_[0].dProbability << " ";

//	probs << endl;
//	myLogProbContent_ << probs.str();
}

