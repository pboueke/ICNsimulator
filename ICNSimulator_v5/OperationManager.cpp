/*
 * OperationManager.cpp
 *
 *  Created on: 26/08/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "OperationManager.h"

OperationManager* OperationManager::aOperation_ = NULL;

OperationManager::OperationManager()
{
	config_ = new SimConfiguration();
	bInterrupted = false;
}

OperationManager::~OperationManager()
{
}

OperationManager* OperationManager::instance()
{
	if(!aOperation_)
		aOperation_ = new OperationManager();

	return aOperation_;
}

bool OperationManager::LoadConfiguration(string config)
{
	if(!config_->Parser(config))
	{
		Logger::instance()->LogError("OperationManager::LoadConfiguration - Configuration Load Failed.");
		return false;
	}

	Logger::instance()->LogInfo("OperationManager::LoadConfiguration - Configuration loaded.");
	return true;
}

bool OperationManager::Start()
{
	if(config_ == NULL)
	{
		Logger::instance()->LogError("OperationManager::Start - The operation cannot start because there is configuration loaded.");
		return false;
	}

	TimeHandler::instance()->startCountingSimTime();

	countEvents_ = 0;

	totalLoadPerContent_ = new unsigned long int[config_->GetNumberOfContents()];
	timeToFindPerContent_ = new double[config_->GetNumberOfContents()];
	iPckTimeToFindPerContent_ = new unsigned long int[config_->GetNumberOfContents()];
	filteredLoadPerContent_ = new unsigned long int[config_->GetNumberOfContents()];

	dPckTimeInDomain_ = (double**) malloc(sizeof(double*) * (config_->GetNumberOfDomains() + 1));
	iPckNumInDomain_ = (unsigned long int**) malloc(sizeof(unsigned long int*) * (config_->GetNumberOfDomains() + 1));
	timeToFindPerContentDom_ = (double**) malloc(sizeof(double*) * (config_->GetNumberOfDomains() + 1));
	iPckTimeToFindPerContentDom_ = (unsigned long int**) malloc(sizeof(unsigned long int*) * (config_->GetNumberOfDomains() + 1));
	probabilityPerContent_ = (double**) malloc(sizeof(double*) * (config_->GetNumberOfDomains() + 1));
	iPckProbabilityPerContent_ = (unsigned long int**) malloc(sizeof(double*) * (config_->GetNumberOfDomains() + 1));
	filteredLoadPerContentDom_ = (unsigned long int**) malloc(sizeof(unsigned long int*) * (config_->GetNumberOfDomains() + 1));

	for(int i = 0; i <= config_->GetNumberOfDomains(); i++)
	{
		dPckTimeInDomain_[i] = new double[config_->GetNumberOfContents()];
		iPckNumInDomain_[i] = new unsigned long int[config_->GetNumberOfContents()];
		timeToFindPerContentDom_[i] = new double[config_->GetNumberOfContents()];
		iPckTimeToFindPerContentDom_[i] = new unsigned long int[config_->GetNumberOfContents()];
		probabilityPerContent_[i] = new double[config_->GetNumberOfContents()];
		iPckProbabilityPerContent_[i] = new unsigned long int[config_->GetNumberOfContents()];
		filteredLoadPerContentDom_[i] = new unsigned long int[config_->GetNumberOfContents()];
	}

	for(int i = 0; i <= config_->GetNumberOfDomains(); i++)
	{
		totalLoadPerContent_[i] = 0;

		for(int j = 0; j < config_->GetNumberOfContents(); j++)
		{
			dPckTimeInDomain_[i][j] = 0.0;
			iPckNumInDomain_[i][j] = 0;
			timeToFindPerContentDom_[i][j] = 0.0;
			iPckTimeToFindPerContentDom_[i][j] = 0;
			probabilityPerContent_[i][j] = 0.0;
			iPckProbabilityPerContent_[i][j] = 0;
			filteredLoadPerContentDom_[i][j] = 0.0;

			timeToFindPerContent_[j] = 0.0;
			iPckTimeToFindPerContent_[j] = 0;
			filteredLoadPerContent_[j] = 0;

		}
	}

	CreateNodes();
	LinkReferences();
	StartNodes();

	//PrintEventList(pStartEvents);

	ThreadEvHandler();

	TimeHandler::instance()->stopCountingSimTime();

	return true;
}

void OperationManager::PrintEventList(stEvent* ev)
{
	if(ev == NULL)
		return;

	Logger::instance()->LogInfo("Node = " + (string) ev->oNode->GetConfig()->sName + ", Event = " + StringUtility::toString(ev->iTypeEvent) + ", Time = " + StringUtility::toString(ev->time));

	PrintEventList(ev->nextEv);
}

bool OperationManager::Stop()
{
	Logger::instance()->LogInfo("OperationManager::Stop - Operation stopped.");

	for(int i = 0; i <= config_->GetNumberOfDomains(); i++)
	{
		DELETE_PTR(dPckTimeInDomain_[i]);
		DELETE_PTR(iPckNumInDomain_[i]);
		DELETE_PTR(timeToFindPerContentDom_[i]);
		DELETE_PTR(iPckTimeToFindPerContentDom_[i]);
		DELETE_PTR(probabilityPerContent_[i]);
		DELETE_PTR(iPckProbabilityPerContent_[i]);
		DELETE_PTR(filteredLoadPerContentDom_[i]);
	}

	DELETE_PTR(dPckTimeInDomain_);
	DELETE_PTR(iPckNumInDomain_);
	DELETE_PTR(timeToFindPerContentDom_);
	DELETE_PTR(iPckTimeToFindPerContentDom_);
	DELETE_PTR(timeToFindPerContent_);
	DELETE_PTR(iPckTimeToFindPerContent_);
	DELETE_PTR(probabilityPerContent_);
	DELETE_PTR(iPckProbabilityPerContent_);
	DELETE_PTR(filteredLoadPerContentDom_);
	DELETE_PTR(totalLoadPerContent_);
	DELETE_PTR(filteredLoadPerContent_);

	DeleteEventList(pStartEvents);

	//StopNodes();

	for(size_t i = 0; i < nodes_.size(); i++)
	{
		stringstream msg;
		msg << "OperationManager::Dispose - Deleting node " << nodes_[i]->GetName();
		Logger::instance()->LogInfo(msg.str());

		DELETE_PTR(nodes_[i]);
	}

	nodes_.clear();
	DELETE_PTR(config_);
	DELETE_PTR(aOperation_);

//	StopThreadEventHandler();

	return true;
}

void OperationManager::DeleteEventList(stEvent* ev)
{
	if(ev == NULL)
		return;

	DeleteEventList(ev->nextEv);
	ev->previousEv->nextEv = NULL;
	ev->previousEv = NULL;
	DELETE_PTR(ev);
}

void OperationManager::StartNodes()
{
	for(size_t i = 0; i < nodes_.size(); i++)
	{
		nodes_[i]->Start();
	}
}

void OperationManager::StopNodes()
{
	for(size_t i = 0; i < nodes_.size(); i++)
	{
		nodes_[i]->Stop();
	}
}


void OperationManager::EnqueueEvent(BaseNode* node, double time, int iTypeEv)
{
	stEvent* event = (stEvent*) malloc(sizeof(stEvent));
	event->time = time;
	event->iTypeEvent = iTypeEv;
	event->oNode = node;
	event->nextEv = NULL;
	event->previousEv = NULL;

	countEvents_++;

//	Logger::instance()->LogInfo("OperationManager::EnqueueEvent - Enqueuing the event time " + StringUtility::toString(time) +
//			", Queue size = " + StringUtility::toString(countEvents_) + ", Event = " + StringUtility::toString(iTypeEv) + ", Node = " + node->GetConfig()->sName);

/*	if(pStartEvents == NULL)
		pStartEvents = pMiddleEvents = pEndEvents = event;
	else
	{
		if(time > pStartEvents->time)
			FindEventForward(pStartEvents, event);
		else
		{
			event->nextEv = pStartEvents;
			pStartEvents->previousEv = event;
			pStartEvents = event;
		}
	}
*/
//	if(pStartEvents->previousEv != NULL)
//		pStartEvents = pStartEvents->previousEv;

//	if(pEndEvents->nextEv != NULL)
//		pEndEvents = pEndEvents->nextEv;

//	event = NULL;

//	pthread_mutex_lock(&mutexEventHandler_);
	eventList_.Add(event);
	//eventList_.push_back(event);
	//sort(eventList_.begin(), eventList_.end(), OperationManager::Sorter);

//	pthread_cond_signal(&signalToProcessEv_);
//	pthread_mutex_unlock(&mutexEventHandler_);
}

void OperationManager::FindEventForward(stEvent* source, stEvent* toFind)
{
	if(source->nextEv == NULL)
	{
		source->nextEv = toFind;
		toFind->previousEv = source;
	}

	else if(source->time < toFind->time)
		FindEventForward(source->nextEv, toFind);

	else
	{
		toFind->nextEv = source;
		toFind->previousEv = source->previousEv;
		source->previousEv->nextEv = toFind;
		source->previousEv = toFind;
	}

//	if(countEvents_ % 2 == 1 && pMiddleEvents->nextEv != NULL)
//		pMiddleEvents = pMiddleEvents->nextEv;
}

void OperationManager::FindEventBackward(stEvent* source, stEvent* toFind)
{
	if(source->previousEv == NULL)
	{
		source->previousEv = toFind;
		toFind->nextEv = source;
	}

	else if(source->time > toFind->time)
		FindEventBackward(source->previousEv, toFind);

	else
	{
		toFind->nextEv = source->nextEv;
		toFind->previousEv = source;
		source->nextEv->previousEv = toFind;
		source->nextEv = toFind;
	}
}

stEvent* OperationManager::DequeueEvent()
{
/*	if(countEvents_ > 0)
	{
		stEvent* ev = pStartEvents;
		pStartEvents = pStartEvents->nextEv;
		countEvents_--;

//		if(countEvents_ % 2 == 1 && pMiddleEvents->previousEv != NULL)
//			pMiddleEvents = pMiddleEvents->previousEv;

//		Logger::instance()->LogInfo("OperationManager::DequeueEventAtCurrentTime - Event at " + StringUtility::toString(ev->time) + " deleted. Array size = " + StringUtility::toString(countEvents_));
		return ev;
	}*/

	if(eventList_.Size() > 0)
	{
		stEvent* ev = eventList_.Remove_Head();
		//stEvent* ev = eventList_[0];
		//eventList_.erase(eventList_.begin());

		//Logger::instance()->LogInfo("OperationManager::DequeueEventAtCurrentTime - Event at " + StringUtility::toString(ev->time) + " deleted. Array size = " + StringUtility::toString((int)eventList_.size()));
		return ev;
	}

	return NULL;
}

void OperationManager::StartThreadEventHandler()
{
	bInterrupted = false;
	pthread_mutex_init(&mutexEventHandler_, NULL);
	pthread_cond_init(&signalToProcessEv_, NULL);
	pthread_create(&threadEventHandler_, NULL, ProxyThreadFunction, this);
}

void OperationManager::StopThreadEventHandler()
{
	bInterrupted = true;
	pthread_join(threadEventHandler_, NULL);
}

void OperationManager::ThreadEvHandler()
{
	while(1)
	{
		if(IsSimulationTerminated())
			break;

//		if(((int)TimeHandler::instance()->getCurrentTime() % 10000) == 0)
//			Logger::instance()->LogInfo("OperationManager::ThreadEvHandler - Simulation time: " + StringUtility::toString(TimeHandler::instance()->getCurrentTime()));

//		pthread_mutex_lock(&mutexEventHandler_);

		if(countEvents_ == 0)
		{
			Logger::instance()->LogInfo("Event list is empty. Waiting next event...");
//			pthread_cond_wait(&signalToProcessEv_, &mutexEventHandler_);
			continue;
		}

		stEvent* ev = DequeueEvent();

//		pthread_mutex_unlock(&mutexEventHandler_);

		if(ev != NULL)
		{

//			Logger::instance()->LogInfo("OperationManager::ThreadEvHandler - (" + StringUtility::toString(ev->time) + ") Processing the event " +
//					StringUtility::toString(ev->iTypeEvent) + " node " + ev->oNode->GetConfig()->sName + ", Domain " + StringUtility::toString(ev->oNode->GetConfig()->iDomainIdent));

			TimeHandler::instance()->setCurrentTime(ev->time);
			ev->oNode->ProcessEvent(ev->iTypeEvent);
			DELETE_PTR(ev);

			Logger::instance()->LogTrace("NEXT EVENT...");
		}
	}

	StopNodes();

	stringstream msg;
	for(int i = 1; i <= config_->GetNumberOfDomains(); i++)
	{
		for(int j= 0; j < config_->GetNumberOfContents(); j++)
		{
			msg << "Mean time in domain " << i << " content " << j << " = " << (dPckTimeInDomain_[i][j] / (double)iPckNumInDomain_[i][j]) << endl;
		}
	}

	Logger::instance()->LogInfo(msg.str());

	if(config_->IsDebugOper())
		CreateGraphicFiles();

	Logger::instance()->LogInfo("Simulation finished successfully.");
	Logger::instance()->LogInfo("Time line size = " + StringUtility::toString(countEvents_));
}

void OperationManager::CreateNodes()
{
	for(int i = 0; i < config_->GetNumberOfNodes();i++)
	{
		BaseNode* node = NULL;
		stNodeParams* param = config_->GetNodeByIndex(i);

		stringstream msg;

		if(param->iType == Enums::CLIENT)
		{
			msg << "OperationManager::CreateNodes - Creating client node " << param->sName;
			node = new ClientNode(param, config_->GetContents());
		}
		else if(param->iType == Enums::ROUTER && config_->GetTypeCacheAlgorithm() == Enums::ALGORITHM_PROB)
		{
			msg<< "OperationManager::CreateNodes - Creating Prob router node " << param->sName;
			node = new RouterProbNode(param);
		}
		else if(param->iType == Enums::ROUTER && config_->GetTypeCacheAlgorithm() == Enums::ALGORITHM_RC_NO_QUEUE_NO_CACHE_FIN)
		{
			msg << "OperationManager::CreateNodes - Creating RC router node (NO QUEUE & NO CACHE) " << param->sName;
			node = new RouterRcNodeNQueue(param, false);
		}
		else if(param->iType == Enums::ROUTER && config_->GetTypeCacheAlgorithm() == Enums::ALGORITHM_RC_NO_QUEUE_CACHE_FIN)
		{
			msg << "OperationManager::CreateNodes - Creating RC router node (NO QUEUE & CACHE)" << param->sName;
			node = new RouterRcNodeNQueue(param, true);
		}
		else if(param->iType == Enums::ROUTER && config_->GetTypeCacheAlgorithm() == Enums::ALGORITHM_RC_QUEUE_NO_CACHE_FIN)
		{
			msg << "OperationManager::CreateNodes - Creating RC router node (QUEUE & NO CACHE)" << param->sName;
			node = new RouterRcNodeQueue(param, false);
		}
		else if(param->iType == Enums::ROUTER && config_->GetTypeCacheAlgorithm() == Enums::ALGORITHM_RC_QUEUE_CACHE_FIN)
		{
			msg << "OperationManager::CreateNodes - Creating RC router node (QUEUE & CACHE)" << param->sName;
			node = new RouterRcNodeQueue(param, true);
		}
		else
		{
			msg << "OperationManager::CreateNodes - Creating NRS server node " << param->sName;
			node = new ServerNode(param);
		}

		nodes_.push_back(node);

		Logger::instance()->LogInfo(msg.str());
	}
}

void OperationManager::LinkReferences()
{
	for(int i = 0; i < config_->GetNumberOfNodes(); i++)
	{
		BaseNode* nodeSource = nodes_[i];
		int *nodesAbove = NULL, *nodesDomain = NULL, numNodesAbove, numNodesDomain;
		nodesAbove = nodeSource->GetNodesConnectedAbove(&numNodesAbove);
		nodesDomain = nodeSource->GetNodesConnectedDomain(&numNodesDomain);

		for(int j = 0; j < numNodesAbove; j++)
		{
			for(int k = 0; k < config_->GetNumberOfNodes(); k++)
			{
				BaseNode* node = nodes_[k];
				if(nodesAbove[j] == node->GetIdent())
					nodeSource->InsertNodesConnectedAbove(node);
			}
		}

		for(int j = 0; j < numNodesDomain; j++)
		{
			for(int k = 0; k < config_->GetNumberOfNodes(); k++)
			{
				BaseNode* node = nodes_[k];
				if(nodesDomain[j] == node->GetIdent())
					nodeSource->InsertNodesConnectedDomain(node);
			}
		}
	}
}

bool OperationManager::IsSimulationTerminated()
{
	if(TimeHandler::instance()->getCurrentTime() >= config_->GetSimulationTime())
		return true;

	return false;
}

void OperationManager::AddTimeInDomain(int domain, int classType, double time)
{
	if(domain > config_->GetNumberOfDomains())
		return;

	dPckTimeInDomain_[domain][classType] += time;
	iPckNumInDomain_[domain][classType]++;
}

void OperationManager::AddTimeToFind(int domain, int classType, double time)
{
	if(domain > config_->GetNumberOfDomains() || classType >= config_->GetNumberOfContents())
		return;

	timeToFindPerContentDom_[domain][classType] += time;
	iPckTimeToFindPerContentDom_[domain][classType]++;

	timeToFindPerContent_[classType] += time;
	iPckTimeToFindPerContent_[classType]++;
}

void OperationManager::AddProbabilityPerContent(int domain, int classType, double prob)
{
	if(domain > config_->GetNumberOfDomains() || classType >= config_->GetNumberOfContents())
		return;

	probabilityPerContent_[domain][classType] += prob;
	iPckProbabilityPerContent_[domain][classType]++;
}

void OperationManager::IncrementTotalLoadPerContent(int classType)
{
	if(classType >= config_->GetNumberOfContents())
		return;

	totalLoadPerContent_[classType] ++;
}

void OperationManager::IncrementFilteredLoadPerContent(int domain, int classType)
{
	if(domain > config_->GetNumberOfDomains() || classType >= config_->GetNumberOfContents())
		return;

	filteredLoadPerContentDom_[domain][classType]++;
	filteredLoadPerContent_[classType]++;
}

void OperationManager::CreateGraphicFiles()
{
	for(int i = 1; i <= config_->GetNumberOfDomains(); i++)
	{
		ofstream fileCarga, fileResp, fileTempoPerDom;
		string filename = "./Output/Tempo_medio_resposta_" + StringUtility::toString(config_->GetMaxTTL()) + "-" + StringUtility::toString(i) + "_" + StringUtility::toString(iRunNumber_) + "_" + TimeHandler::instance()->getLocalTimeStrToFileName();
		fileResp.open(filename.c_str(), ofstream::out);
		filename = "./Output/Carga_Filtrada_" + StringUtility::toString(config_->GetMaxTTL()) + "-" + StringUtility::toString(i) + "_" + StringUtility::toString(iRunNumber_) + "_" + TimeHandler::instance()->getLocalTimeStrToFileName();
		fileCarga.open(filename.c_str(), ofstream::out);
		filename = "./Output/Tempo_dominio_" + StringUtility::toString(config_->GetMaxTTL()) + "-" + StringUtility::toString(i) + "_" + StringUtility::toString(iRunNumber_) + "_" + TimeHandler::instance()->getLocalTimeStrToFileName();
		fileTempoPerDom.open(filename.c_str(), ofstream::out);

		fileResp << config_->GetMaxTTL() << " ";
		fileCarga << config_->GetMaxTTL() << " ";
		fileTempoPerDom << config_->GetMaxTTL() << " ";

		for(int j = 0; j < config_->GetNumberOfContents(); j++)
		{
			fileResp << timeToFindPerContentDom_[i][j] / (double)iPckTimeToFindPerContentDom_[i][j] << " ";
			fileCarga << (double)filteredLoadPerContentDom_[i][j]/(double)totalLoadPerContent_[j] << " ";
			fileTempoPerDom << (double)dPckTimeInDomain_[i][j] / (double) iPckNumInDomain_[i][j] << " ";
		}

		fileResp.close();
		fileCarga.close();
		fileTempoPerDom.close();
	}

	ofstream fileProb, fileCargaTot, fileTempoTot;
	string filename = "./Output/Probabilidade_" + StringUtility::toString(config_->GetMaxTTL()) + "_" + StringUtility::toString(iRunNumber_) + "_" + TimeHandler::instance()->getLocalTimeStrToFileName();
	fileProb.open(filename.c_str(), ofstream::out);

	filename = "./Output/Carga_Filtrada_Total_" + StringUtility::toString(config_->GetMaxTTL()) + "_" + StringUtility::toString(iRunNumber_) + "_" + TimeHandler::instance()->getLocalTimeStrToFileName();
	fileCargaTot.open(filename.c_str(), ofstream::out);

	filename = "./Output/Tempo_resposta_Total_" + StringUtility::toString(config_->GetMaxTTL()) + "_" + StringUtility::toString(iRunNumber_) + "_" + TimeHandler::instance()->getLocalTimeStrToFileName();
	fileTempoTot.open(filename.c_str(), ofstream::out);

	fileCargaTot << config_->GetMaxTTL() << " ";
	fileTempoTot << config_->GetMaxTTL() << " ";

	for(int j = 0; j < config_->GetNumberOfContents(); j++)
	{
		fileProb << j + 1 << " ";

		for(int i = 1; i <= config_->GetNumberOfDomains(); i++)
		{
			fileProb << probabilityPerContent_[i][j] / (double)iPckProbabilityPerContent_[i][j] << " ";
		}

		fileProb << endl;

		fileCargaTot << (double)filteredLoadPerContent_[j]/(double)totalLoadPerContent_[j] << " ";
		fileTempoTot << (double) timeToFindPerContent_[j]/(double)iPckTimeToFindPerContent_[j] << " ";
	}

	fileProb.close();
	fileTempoTot.close();
	fileCargaTot.close();
}

bool OperationManager::IsDebug(int iNode)
{
	if(iNode == Enums::CLIENT)
		return config_->IsDebugClient();
	else
		return config_->IsDebugRouter();

	return false;
}

//void OperationManager::AddContentTimeInCache(int domain, int classType, double time)
//{
//	double diff = TimeHandler::instance()->getCurrentTime() - time;
//	timeInCachePerContent_[domain][classType] += diff;
//	iPckTimeInCachePerContent_[domain][classType]++;
//}
