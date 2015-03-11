/*
 * BaseNode.cpp
 *
 *  Created on: 26/08/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "BaseNode.h"
#include "OperationManager.h"

BaseNode::BaseNode(stNodeParams* config)
{
	config_ = config;
	distToContent_ = OperationManager::instance()->GetDistributionParams(Enums::DIST_CONTENT);
	distToSend_ = OperationManager::instance()->GetTransDistParams(config->iDomainIdent);
	distToRequest_ = OperationManager::instance()->GetDistributionParams(Enums::DIST_REQUEST);
	distToRandom_ = OperationManager::instance()->GetDistributionParams(Enums::DIST_RANDOM);
	distToRC_ = OperationManager::instance()->GetDistributionParams(Enums::DIST_RC);

	string filename = "./Output/" + string(config->sName) + "_" + TimeHandler::instance()->getLocalTimeStrToFileName() + ".csv";
	myLog_.open(filename.c_str(), ofstream::out);
	myLog_ << "Time,Direction,Event" << endl;

	lastEvTime_ = 0.0;
	numDepartures_ = 0;
	numPackageTotal_ = 0;
	numPackageIn_ = 0;
	numPackageOut_ = 0;
	meanTimeToWait_ = 0.0;
	idleTime_ = 0.0;
	queueMean_ = 0.0;
}

BaseNode::~BaseNode()
{
	config_ = NULL;
}

void BaseNode::Start()
{
}

void BaseNode::Stop()
{
	StoreMeasurements();
	myLog_.close();

	for(size_t i = 0; i < buffer_.size(); i++)
	{
		DELETE_PTR(buffer_[i]);
	}

	buffer_.clear();
	nodesConnectedAbove_.clear();
	nodesConnectedDomain_.clear();
}

void BaseNode::ProcessEvent(int iEventType)
{
	string evStr = StringUtility::toEventString(iEventType);

	stringstream msg;
	msg << TimeHandler::instance()->getCurrentTime() << "," << evStr << endl;
	//StoreDebugInfo(&myLog_, msg.str());

	if(iEventType == Enums::SEND_PACKAGE)
		ProcessTx();
}

void BaseNode::ProcessTx()
{
	numDepartures_++;

	CalculateEN(buffer_.size());

	if(buffer_.size() > 1)
	{
		double nextEventToSend = GetNextEventTimeByDistribution(distToSend_);
		OperationManager::instance()->EnqueueEvent(this, nextEventToSend, Enums::SEND_PACKAGE);
	}

	lastEvTime_ = TimeHandler::instance()->getCurrentTime();
}

void BaseNode::ProcessRx(stPackage* pack)
{
	CalculateU();
	CalculateEN((double)buffer_.size());

	double nextEventToSend = GetNextEventTimeByDistribution(distToSend_);
	pack->tStartSending = nextEventToSend;
	pack->tStartQueue = TimeHandler::instance()->getCurrentTime();
	buffer_.push_back(pack);

	if(buffer_.size() == 1)
	{
		OperationManager::instance()->EnqueueEvent(this, nextEventToSend, Enums::SEND_PACKAGE);
	}

	lastEvTime_ = TimeHandler::instance()->getCurrentTime();
}

int* BaseNode::GetNodesConnectedAbove(int* nodesAbove)
{
	*nodesAbove = config_->iNumNodesConnectedAbove;
	return config_->iNodesConnectedAbove;
}

int* BaseNode::GetNodesConnectedDomain(int* nodesDomain)
{
	*nodesDomain = config_->iNumNodesConnectedDomain;
	return config_->iNodesConnectedDomain;
}

void BaseNode::InsertNodesConnectedAbove(BaseNode* node)
{
	stringstream msg;
	msg << "BaseNode::InsertNodesConnectedAbove - Connecting node " << config_->sName << " to node " << node->GetName();
	Logger::instance()->LogInfo(msg.str());
	StoreDebugInfo(&myLog_, msg.str()+"\n\r");

	nodesConnectedAbove_.push_back(node);
}

void BaseNode::InsertNodesConnectedDomain(BaseNode* node)
{
	stringstream msg;
	msg << "BaseNode::InsertNodesConnectedDomain - Connecting node " << config_->sName << " to node " << node->GetName();
	Logger::instance()->LogInfo(msg.str());
	StoreDebugInfo(&myLog_, msg.str()+"\n\r");

	nodesConnectedDomain_.push_back(node);
}

double BaseNode::GetNextEventTimeByDistribution(stDistributionParams* params)
{
	double nextEventTime = -1;
	if(params->iType == Enums::EXP)
		nextEventTime = DistUtility::instance()->exponential((1.0 / (double)params->params[0]));
	else if(params->iType == Enums::NORMAL)
		nextEventTime = DistUtility::instance()->normal(params->params[0], params->params[1]);
	else if(params->iType == Enums::FIXO)
		nextEventTime = params->params[0];
	else if(params->iType == Enums::UNIFORME)
		nextEventTime = DistUtility::instance()->uniform(params->params[0], params->params[1]);

	return nextEventTime + TimeHandler::instance()->getCurrentTime();
}

double BaseNode::GetRandomByDistribution(stDistributionParams* params)
{
	double random = -1.0;
	if(params->iType == Enums::UNIFORME)
		random = DistUtility::instance()->uniform(params->params[0], params->params[1]);

	return random;
}

int BaseNode::GetContentByDistribution(stDistributionParams* param, int numContents)
{
	int contentIndex = -1;
	if(param->iType == Enums::ZPIF)
		contentIndex = DistUtility::instance()->zpif(param->params[0], numContents);

	return contentIndex;
}

double BaseNode::GetZpifProb(stDistributionParams* param, int numContents, int classType)
{
	return DistUtility::instance()->zpifProb(param->params[0], numContents, classType);
}

void BaseNode::StoreToLogFile(string info, string direction)
{
	if(myLog_.is_open())
	{
		myLog_ << TimeHandler::instance()->getCurrentTime() << "," << direction << "," << info << endl;
		myLog_.flush();
	}

	//cout << info << endl;
}

void BaseNode::StoreMeasurements()
{
	double totalTime = OperationManager::instance()->GetSimulationTime();

	stringstream msg;
	msg << "Packages in the buffer, " << buffer_.size() << endl;
	msg << "Number of package, " << numPackageTotal_ << endl;
	msg << "Number of package Input," << numPackageIn_ << endl;
	msg << "Number of package Output," << numPackageOut_ << endl;
	msg << "Number of transmissions, " << numDepartures_ << endl;
	msg << "Utilization, " << (1.0 - (idleTime_/ totalTime)) << endl;
	msg << "E[N], " << (queueMean_ / totalTime) << endl;
	msg << "W, " << (meanTimeToWait_ / (double)numDepartures_) << endl;

	StoreToLogFile(msg.str(), "");
}

void BaseNode::CalculateW(double startTime)
{
	meanTimeToWait_ += TimeHandler::instance()->getCurrentTime() - startTime;
}

void BaseNode::CalculateEN(double queueSize)
{
	queueMean_ += (TimeHandler::instance()->getCurrentTime() - lastEvTime_ ) * queueSize;
}

void BaseNode::CalculateU()
{
	if(buffer_.size() == 0)
		idleTime_ += TimeHandler::instance()->getCurrentTime() - lastEvTime_;
}

void BaseNode::CalculateTimeInDomain(double startTime, int hoops)
{
	double timeInDomain = TimeHandler::instance()->getCurrentTime() - startTime;
	OperationManager::instance()->AddTimeInDomain(config_->iDomainIdent, hoops, timeInDomain);
}

void BaseNode::CalculateTimeToFind(int classType, double startTime)
{
	double timeToFind = TimeHandler::instance()->getCurrentTime() - startTime;
	OperationManager::instance()->AddTimeToFind(config_->iDomainIdent, classType, timeToFind);
}

void BaseNode::SendPackageToNextNode(stPackage* pck)
{
	double probRouterToSend = GetRandomByDistribution(distToRandom_);
	size_t totalRouter = nodesConnectedDomain_.size();
	int routerToSend = ceil(probRouterToSend * (double)totalRouter - 1);

	BaseNode* router = nodesConnectedDomain_[routerToSend];

	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "SEND_PACKAGE TO NEXT NODE," << router->GetConfig()->sName;
	Logger::instance()->LogTrace(msg.str());

	router->ProcessRx(pck);
}

void BaseNode::SendPackageToNextDomain(stPackage* pck)
{
	double probRouterToSend = GetRandomByDistribution(distToRandom_);
	size_t totalRouter = nodesConnectedAbove_.size();
	int routerToSend = ceil(probRouterToSend * (double)totalRouter - 1);
	BaseNode* router = nodesConnectedAbove_[routerToSend];

	CalculateTimeInDomain(pck->tStartSearch, pck->contentInfo->iClassType);
	pck->iTTLTag = 0;
	pck->iSourceRouter = router->GetConfig()->iIdent;

	// Contagem de pacotes que saem do domínio
	numPackageOut_++;

	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "SEND_PACKAGE TO NEXT DOMAIN," << router->GetConfig()->sName;
	Logger::instance()->LogTrace(msg.str());

	router->ProcessRx(pck);
}

void BaseNode::StoreDebugInfo(ofstream* file, string info)
{
	if(OperationManager::instance()->IsDebug(config_->iType) && file != NULL)
		(*file) << info;
}
