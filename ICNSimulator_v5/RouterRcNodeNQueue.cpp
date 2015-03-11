/*
 * RouterRcNode.cpp
 *
 *  Created on: 19/07/2014
 *      Author: Diego Vargas Jannibelli
 */

#include "OperationManager.h"
#include "RouterRcNodeNQueue.h"

RouterRcNodeNQueue::RouterRcNodeNQueue(stNodeParams* config, bool cacheEnabled) : RouterBaseNode(config)
{
	cacheEnabled_ = cacheEnabled;
}

RouterRcNodeNQueue::~RouterRcNodeNQueue()
{
}

void RouterRcNodeNQueue::Start()
{
	RouterBaseNode::Start();

	iCacheIndexPointer_ = 0;
	iCacheGreaterRemoved_ = 0;
	lastEvRcTime_ = 0;
	numPackPerRc_ = new unsigned long int[NUM_MAX_RC_EVAL];
	timePerRc_ = new double[NUM_MAX_RC_EVAL];

	for(int i = 0; i < NUM_MAX_RC_EVAL; i++)
	{
		numPackPerRc_[i] = 0;
		timePerRc_[i] = 0;
	}

	numPackRcPerTTL_ = (unsigned long int**) malloc(sizeof(unsigned long int*) * (OperationManager::instance()->GetMaxTTL() + 1));
	numRcPerTTL_ = new unsigned long int[OperationManager::instance()->GetMaxTTL() + 1];
	pRCperContent_ = new unsigned long int[OperationManager::instance()->GetNumberOfContents()];

	for(int i = 0; i <= OperationManager::instance()->GetMaxTTL(); i++)
	{
		numPackRcPerTTL_[i] = new unsigned long int[NUM_MAX_RC_EVAL];
		for(int j = 0; j < NUM_MAX_RC_EVAL; j++)
			numPackRcPerTTL_[i][j] = 0;
	}

	for(int i = 0; i < OperationManager::instance()->GetNumberOfContents(); i++)
		pRCperContent_[i] = 0;

	Logger::instance()->LogInfo("Node " + (string) config_->sName + " started.");
}

void RouterRcNodeNQueue::Stop()
{
	cacheOccuped_.clear();

	RouterBaseNode::Stop();

	for(int j = 0; j <= OperationManager::instance()->GetMaxTTL(); j++)
	{
		DELETE_PTR(numPackRcPerTTL_[j]);
	}

	DELETE_PTR(numPackRcPerTTL_);
	DELETE_PTR(numRcPerTTL_);
	DELETE_PTR(pRCperContent_);
	DELETE_PTR(numPackPerRc_);
	DELETE_PTR(timePerRc_);
}

void RouterRcNodeNQueue::ProcessEvent(int iEventType)
{
	BaseNode::ProcessEvent(iEventType);

	if(iEventType >= Enums::DEC_RC)
		DecrementRC(iEventType - Enums::DEC_RC);
}

void RouterRcNodeNQueue::ProcessRx(stPackage* pack)
{
	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "PROCESS_RX,PACKAGE RECEIVED (ClassType;TTL;RC)," << pack->contentInfo->iClassType << "," << pack->iTTLTag << "," << pRCperContent_[pack->contentInfo->iClassType];
	Logger::instance()->LogTrace(msg.str());

	InitProcess(pack);

	numRcPerTTL_[pack->iTTLTag] += pRCperContent_[pack->contentInfo->iClassType];

	if(pRCperContent_[pack->contentInfo->iClassType] < NUM_MAX_RC_EVAL)
	{
		numPackPerRc_[pRCperContent_[pack->contentInfo->iClassType]]++;
		numPackRcPerTTL_[pack->iTTLTag][pRCperContent_[pack->contentInfo->iClassType]]++;
	}

	// Verificar se o conteúdo está na cache
	if(CheckCache(pack->contentInfo->iClassType))
	{
		RemovePackFromSystem(pack);
		return;
	}

	CalculateU();
	CalculateEN((double)buffer_.size());

	double nextEventToSend = GetNextEventTimeByDistribution(distToSend_);
	pack->tStartSending = nextEventToSend;
	pack->tStartQueue = TimeHandler::instance()->getCurrentTime();
	buffer_.push_back(pack);
	sort(buffer_.begin(), buffer_.end(), BaseNode::Sorter);

	OperationManager::instance()->EnqueueEvent(this, nextEventToSend, Enums::SEND_PACKAGE);

	lastEvTime_ = TimeHandler::instance()->getCurrentTime();
}

void RouterRcNodeNQueue::ProcessTx()
{
	numDepartures_++;

	CalculateEN(buffer_.size());

	stPackage *pck = buffer_[0];
	buffer_.erase(buffer_.begin());

	meanTimeToWait_ += TimeHandler::instance()->getCurrentTime() - pck->tStartQueue;

	pck->iTTLTag++;

	// Verificar se o TTL é menor do que o limite
	if(pck->iTTLTag < pck->iTTLMax)
		SendPackageToNextNode(pck);
	else
		SendPackageToNextDomain(pck);

	lastEvTime_ = TimeHandler::instance()->getCurrentTime();

	pck = NULL;
}

bool RouterRcNodeNQueue::CheckCache(int classType)
{
	if(pCacheMemory_[classType].isInCache)
		return true;

	return false;
}

void RouterRcNodeNQueue::IncrementCacheAlgorithm(int classType)
{
	IncrementRC(classType);
}

void RouterRcNodeNQueue::IncrementRC(int classType)
{
	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "INCREMENT_RC,CONTENT," << classType << ",RC," << pRCperContent_[classType] << ",to," << pRCperContent_[classType] + 1;
	Logger::instance()->LogTrace(msg.str());

	if(pRCperContent_[classType] < NUM_MAX_RC_EVAL)
		timePerRc_[pRCperContent_[classType]] += (TimeHandler::instance()->getCurrentTime() - lastEvRcTime_);

	lastEvRcTime_ = TimeHandler::instance()->getCurrentTime();

	if(pRCperContent_[classType] >= pRCLimits_->iUpper && !pCacheMemory_[classType].isInCache)
	{
		if(!cacheEnabled_ || ((int)cacheOccuped_.size() < config_->iCacheSize))
		{
			pCacheMemory_[classType].isInCache = true;
			pCacheMemory_[classType].tStartCache = TimeHandler::instance()->getCurrentTime();
			pCacheMemory_[classType].iCacheIndex = cacheOccuped_.size();

			cacheOccuped_.push_back(pCacheMemory_ + classType);
		}
		else
			CacheReplacement(classType);

		stringstream msg;
		msg << config_->iDomainIdent << "," << config_->sName << "," << "STORE_CONTENT,CONTENT," << classType << ",Cache Size," << cacheOccuped_.size();
		Logger::instance()->LogTrace(msg.str());
	}

	pRCperContent_[classType]++;
	if(pRCperContent_[classType] == 1)
	{
		double nextEventToSend = GetNextEventTimeByDistribution(distToRC_);
		OperationManager::instance()->EnqueueEvent(this, nextEventToSend, Enums::DEC_RC + classType);
	}
}

void RouterRcNodeNQueue::DecrementRC(int classType)
{
	if(pRCperContent_[classType] <= 0)
		return;

	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "DECREMENT_RC,CONTENT," << classType << ",RC," << pRCperContent_[classType] << ",to," << pRCperContent_[classType] - 1;
	Logger::instance()->LogTrace(msg.str());

	if(pRCperContent_[classType] < NUM_MAX_RC_EVAL)
		timePerRc_[pRCperContent_[classType]] += (TimeHandler::instance()->getCurrentTime() - lastEvRcTime_);

	lastEvRcTime_ = TimeHandler::instance()->getCurrentTime();

	pRCperContent_[classType]--;
	if(pRCperContent_[classType] < pRCLimits_->iLower && pCacheMemory_[classType].isInCache)
	{
//		OperationManager::instance()->AddContentTimeInCache(config_->iDomainIdent, classType, pCacheMemory_[classType].tStartCache);

		pCacheMemory_[classType].isInCache = false;
		pCacheMemory_[classType].tTimeInCache += TimeHandler::instance()->getCurrentTime() - pCacheMemory_[classType].tStartCache;
		pCacheMemory_[classType].tStartCache = 0;

		cacheOccuped_.erase(cacheOccuped_.begin() + pCacheMemory_[classType].iCacheIndex);
		for(int i = pCacheMemory_[classType].iCacheIndex; i < (int)cacheOccuped_.size(); i++)
			cacheOccuped_[i]->iCacheIndex--;

		stringstream msg;
		msg << config_->iDomainIdent << "," << config_->sName << "," << "REMOVE_CONTENT,CONTENT," << classType;
		Logger::instance()->LogTrace(msg.str());
	}

	if(pRCperContent_[classType] > 0)
	{
		double nextEventToSend = GetNextEventTimeByDistribution(distToRC_);
		OperationManager::instance()->EnqueueEvent(this, nextEventToSend, Enums::DEC_RC + classType);
	}
}

void RouterRcNodeNQueue::CacheReplacement(int classType)
{
	int contentReplace = -1;

	if(config_->iReplacementType == Enums::REPLACEMENT_RANDOM)
	{
		double probContent = GetRandomByDistribution(distToRandom_);
		int totalCache = config_->iCacheSize;
		int indexContentReplace = ceil(probContent * (double)totalCache - 1);

		contentReplace = cacheOccuped_[indexContentReplace]->iClassType;
	}
	else if(config_->iReplacementType == Enums::REPLACEMENTE_RC)
	{
		int contentIndex = 0;
		unsigned int lowerRc = UINT_MAX;
		for(int i = 0; i < config_->iCacheSize; i++)
		{
			int content = cacheOccuped_[i]->iClassType;
			if(pRCperContent_[content] < lowerRc)
			{
				lowerRc = pRCperContent_[content];
				contentIndex = content;
			}
		}

		contentReplace = contentIndex;
	}

	pCacheMemory_[contentReplace].isInCache = false;
	pCacheMemory_[contentReplace].tTimeInCache += TimeHandler::instance()->getCurrentTime() - pCacheMemory_[classType].tStartCache;
	pCacheMemory_[contentReplace].tStartCache = 0;

	cacheOccuped_.erase(cacheOccuped_.begin() + pCacheMemory_[contentReplace].iCacheIndex);

	pCacheMemory_[classType].isInCache = true;
	pCacheMemory_[classType].tStartCache = TimeHandler::instance()->getCurrentTime();
	pCacheMemory_[classType].iCacheIndex = pCacheMemory_[contentReplace].iCacheIndex;

	cacheOccuped_.insert(cacheOccuped_.begin() + pCacheMemory_[contentReplace].iCacheIndex, pCacheMemory_ + classType);

	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "REPLACE_CONTENT,CONTENT," << contentReplace << ",TO," << classType << ",BY REPLACEMENT," << config_->iReplacementType;
	Logger::instance()->LogTrace(msg.str());
}

void RouterRcNodeNQueue::StoreMeasurements()
{
	BaseNode::StoreMeasurements();

	stringstream msg;
	msg << endl;

	for(int i = 0; i <= OperationManager::instance()->GetMaxTTL(); i++)
	{
		msg << "Number of filtered packages per TLL," << i << ",";
		msg << numPckFilteredPerTTL_[i] << "," << numPckPerTTL_[i] << "," << ((double)numPckFilteredPerTTL_[i]/(double)numPckPerTTL_[i]) << ",";
		msg << ((double)numRcPerTTL_[i]/(double)numPckPerTTL_[i]) << endl;
	}

	msg << endl;

	for(int i = 0; i < OperationManager::instance()->GetNumberOfContents(); i++)
	{
		msg << "Number of packages received from class," << i << "," << numPckPerContents_[i] << endl;
		msg << "Number of filtered packages received from class," << i << "," << numFilteredPckPerContents_[i] << endl;
		msg << "Probability content is available in cache," << i << "," << (pCacheMemory_[i].tTimeInCache / OperationManager::instance()->GetSimulationTime()) << endl;
	}

	msg << endl;

	for(int i = 0; i < 10; i++)
		msg << "Time Percent from RC," << i << "," << (timePerRc_[i]/TimeHandler::instance()->getCurrentTime()) << "," << ((double)numPackPerRc_[i]/(double)numPackageTotal_) << endl;

	msg << endl;

	for(int i = 0; i < 10; i++)
	{
		msg << "Time Percent from RC (row) per TTL (column)," << i << ",";
		for(int j = 0; j <= OperationManager::instance()->GetMaxTTL(); j++)
			msg << ((double)numPackRcPerTTL_[j][i]/(double)numPackageTotal_) << ",";

		msg << endl;
	}

	StoreToLogFile(msg.str(), "");
}

