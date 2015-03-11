/*
 * RouterNode.cpp
 *
 *  Created on: 19/11/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "RouterBaseNode.h"
#include "OperationManager.h"

RouterBaseNode::RouterBaseNode(stNodeParams* config) : BaseNode(config)
{
	if(OperationManager::instance()->IsDebug(config_->iType))
	{
		string filename = "./Output/" + string(config->sName) + "_PhiUp";
		myLogPhiup_.open(filename.c_str(), ofstream::out);

		filename = "./Output/" + string(config->sName) + "_Prob";
		myLogProb_.open(filename.c_str(), ofstream::out);

		filename = "./Output/" + string(config->sName) + "_ProbTTL-Time";
		myLogProbTTL_.open(filename.c_str(), ofstream::out);

		filename = "./Output/" + string(config->sName) + "_ProbContent-Time";
		myLogProbContent_.open(filename.c_str(), ofstream::out);
	}

	pPackToDelete_ = NULL;
}

RouterBaseNode::~RouterBaseNode()
{
}

void RouterBaseNode::Start()
{
	BaseNode::Start();

	numPckPerTTL_ = new unsigned long int[OperationManager::instance()->GetMaxTTL() + 1];
	numPckFilteredPerTTL_ = new unsigned long int[OperationManager::instance()->GetMaxTTL() + 1];

	for(int i = 0; i <= OperationManager::instance()->GetMaxTTL(); i++)
	{
		numPckPerTTL_[i] = 0;
		numPckFilteredPerTTL_[i] = 0;
	}

	pRCLimits_ = OperationManager::instance()->GetRCLimits();
	numPckPerContents_ = new unsigned long int[OperationManager::instance()->GetNumberOfContents()];
	numFilteredPckPerContents_ = new unsigned long int[OperationManager::instance()->GetNumberOfContents()];
	numPackageInPerContent_ = new unsigned long int[OperationManager::instance()->GetNumberOfContents()];

	pCacheMemory_ = (stCacheItem*) malloc(sizeof(stCacheItem) * OperationManager::instance()->GetNumberOfContents());
	for(int i = 0; i < OperationManager::instance()->GetNumberOfContents(); i++)
	{
		pCacheMemory_[i].iClassType = i;
		pCacheMemory_[i].isInCache = false;
		pCacheMemory_[i].tStartCache = 0.0;
		pCacheMemory_[i].tTimeInCache = 0.0;
		pCacheMemory_[i].dProbability = DistUtility::instance()->zpifProb(distToContent_->params[0], OperationManager::instance()->GetNumberOfContents(), i+1);
		pCacheMemory_[i].dRate = 0.0;

		numPckPerContents_[i] = 0;
		numFilteredPckPerContents_[i] = 0;
		numPackageInPerContent_[i] = 0;
	}

	StoreToLogFile("Router started.", "-");
}

void RouterBaseNode::Stop()
{
	BaseNode::Stop();

	if(OperationManager::instance()->IsDebug(config_->iType))
	{
		myLogPhiup_.close();
		myLogProb_.close();
		myLogProbTTL_.close();
		myLogProbContent_.close();
	}

	DELETE_PTR(numPckPerTTL_);
	DELETE_PTR(numPckFilteredPerTTL_);
	DELETE_PTR(numPckPerContents_);
	DELETE_PTR(numFilteredPckPerContents_);
	DELETE_PTR(pCacheMemory_);
	DELETE_PTR(numPackageInPerContent_);

	StoreToLogFile("Router stopped.", "-");
}

void RouterBaseNode::ProcessEvent(int iEventType)
{
	BaseNode::ProcessEvent(iEventType);
}

void RouterBaseNode::ProcessRx(stPackage* pack)
{
	InitProcess(pack);

	// Verificar se o conteúdo está na cache
	if(CheckCache(pack->contentInfo->iClassType))
	{
		RemovePackFromSystem(pack);
		return;
	}

	BaseNode::ProcessRx(pack);
}

void RouterBaseNode::InitProcess(stPackage* pack)
{
	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "PROCESS_RX,PACK_TTL," << pack->iTTLTag;
	Logger::instance()->LogTrace(msg.str());

	// TODO Inserir na tabela do Bread crumb
	DELETE_PTR(pPackToDelete_);

	numPackageTotal_++;
	numPckPerContents_[pack->contentInfo->iClassType]++;
	numPckPerTTL_[pack->iTTLTag]++;

	// Contagem do número de pacotes do domínio abaixo
	if(pack->iTTLTag == 0)
	{
		numPackageIn_++;
		numPackageInPerContent_[pack->contentInfo->iClassType]++;

		pack->tStartSearch = TimeHandler::instance()->getCurrentTime();

		IncrementCacheAlgorithm(pack->contentInfo->iClassType);

		stringstream msg;
		msg << TimeHandler::instance()->getCurrentTime() << " " << (pCacheMemory_[pack->contentInfo->iClassType].tTimeInCache / TimeHandler::instance()->getCurrentTime()) << endl;
		StoreDebugInfo(&myLogPhiup_, msg.str());
	}
}

void RouterBaseNode::RemovePackFromSystem(stPackage* pack)
{
	if(OperationManager::instance()->IsDebug(config_->iType))
	{
		probInCachoPerTime_++;
		myLogProb_ << TimeHandler::instance()->getCurrentTime() << " " << ((double)probInCachoPerTime_ / (double)numPackageTotal_) << endl;

		stringstream msg;
		msg << TimeHandler::instance()->getCurrentTime() << " ";
		for(int i = 0; i <= OperationManager::instance()->GetMaxTTL(); i++)
		{
			if(numPckPerTTL_[i] == 0)
				msg << "0 ";
			else
				msg << (double)numPckFilteredPerTTL_[i]/(double)numPckPerTTL_[i] << " ";
		}

		myLogProbTTL_ << msg.str() << endl;
	}

	numFilteredPckPerContents_[pack->contentInfo->iClassType]++;
	numPckFilteredPerTTL_[pack->iTTLTag]++;

	CalculateTimeInDomain(pack->tStartSearch, pack->contentInfo->iClassType);
	CalculateTimeToFind(pack->contentInfo->iClassType, pack->tStartTotal);

	OperationManager::instance()->AddProbabilityPerContent(config_->iDomainIdent, pack->contentInfo->iClassType, (double)numFilteredPckPerContents_[pack->contentInfo->iClassType] / (double)numPckPerContents_[pack->contentInfo->iClassType]);
	OperationManager::instance()->IncrementFilteredLoadPerContent(config_->iDomainIdent, pack->contentInfo->iClassType);

	pPackToDelete_ = pack;

	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "PACK_FILTERED,PACK_TLL," << pack->iTTLTag;
	Logger::instance()->LogTrace(msg.str());
}

void RouterBaseNode::ProcessTx()
{
	BaseNode::ProcessTx();

	stPackage *pck = buffer_[0];
	buffer_.erase(buffer_.begin());

	meanTimeToWait_ += TimeHandler::instance()->getCurrentTime() - pck->tStartQueue;

	pck->iTTLTag++;

	// Verificar se o TTL é menor do que o limite
	if(pck->iTTLTag < pck->iTTLMax)
		SendPackageToNextNode(pck);
	else
		SendPackageToNextDomain(pck);

	pck = NULL;
}

bool RouterBaseNode::CheckCache(int classType)
{
	return false;
}

void RouterBaseNode::IncrementCacheAlgorithm(int classType)
{
	return;
}

void RouterBaseNode::StoreMeasurements()
{
	BaseNode::StoreMeasurements();

	stringstream msg;
	msg << endl;

	for(int i = 0; i <= OperationManager::instance()->GetMaxTTL(); i++)
	{
		msg << "Number of filtered packages per TLL," << i << ",";
		msg << numPckFilteredPerTTL_[i] << "," << numPckPerTTL_[i] << "," << ((double)numPckFilteredPerTTL_[i]/(double)numPckPerTTL_[i]) << endl;
	}

	msg << endl;

	for(int i = 0; i < OperationManager::instance()->GetNumberOfContents(); i++)
	{
		msg << "Number of packages received from class," << i << "," << numPckPerContents_[i] << endl;
		msg << "Number of filtered packages received from class," << i << "," << numFilteredPckPerContents_[i] << endl;
		msg << "Probability content is available in cache," << i << "," << (pCacheMemory_[i].tTimeInCache / OperationManager::instance()->GetSimulationTime()) << endl;
	}

	msg << endl;

	StoreToLogFile(msg.str(), "");
}
