/*
 * ClientNode.cpp
 *
 *  Created on: 10/11/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "ClientNode.h"
#include "OperationManager.h"

ClientNode::ClientNode(stNodeParams* config, vector<stContentsParams*> contents) : BaseNode(config)
{
	contents_ = contents;
}

ClientNode::~ClientNode()
{
}

void ClientNode::Start()
{
	double nextEventTime = GetNextEventTimeByDistribution(distToRequest_);

	OperationManager::instance()->EnqueueEvent(this, nextEventTime, Enums::NEW_PACKAGE);

	contentsTx_ = new int[OperationManager::instance()->GetNumberOfContents()];
	for(int i = 0; i < OperationManager::instance()->GetNumberOfContents(); i++)
		contentsTx_[i] = 0;

	StoreToLogFile("Client started.", "-");
}

void ClientNode::Stop()
{
	BaseNode::Stop();

	DELETE_PTR(contentsTx_);
}

void ClientNode::ProcessEvent(int iEventType)
{
	BaseNode::ProcessEvent(iEventType);

	if(iEventType == Enums::NEW_PACKAGE)
		GenerateNewPackage();
}

void ClientNode::ProcessRx()
{
}

void ClientNode::ProcessTx()
{
	BaseNode::ProcessTx();

	stPackage *pck = buffer_[0];
	buffer_.erase(buffer_.begin());

	CalculateW(pck->tStartQueue);

	SendPackageToNextDomain(pck);

	pck = NULL;
}

stPackage* ClientNode::CreateNewRequest()
{
	stPackage* pck = (stPackage*) malloc(sizeof(stPackage));

	int contentId = GetContentByDistribution(distToContent_, (int) contents_.size());
	for(size_t i = 0; i < contents_.size(); i++)
	{
		if(contentId == (int)(i + 1))
		{
			pck->contentInfo = contents_[i];
			contentsTx_[contents_[i]->iClassType]++;
			break;
		}
	}

	pck->iSourceClient = config_->iIdent;
	pck->iTTLMax = OperationManager::instance()->GetMaxTTL();
	pck->iTypePckg = Enums::REQUEST;
	pck->tStartTotal = TimeHandler::instance()->getCurrentTime();
	pck->tStartQueue = TimeHandler::instance()->getCurrentTime();

	return pck;
}

void ClientNode::GenerateNewPackage()
{
	numPackageTotal_++;

	CalculateU();
	CalculateEN((double)buffer_.size());

	double nextEventToSend = GetNextEventTimeByDistribution(distToSend_);
	double nextEventToGenerate = GetNextEventTimeByDistribution(distToRequest_);

	OperationManager::instance()->EnqueueEvent(this, nextEventToGenerate, Enums::NEW_PACKAGE);

	if(buffer_.size() == 0)
		OperationManager::instance()->EnqueueEvent(this, nextEventToSend, Enums::SEND_PACKAGE);

	stPackage* pck = CreateNewRequest();
	buffer_.push_back(pck);

	lastEvTime_ = TimeHandler::instance()->getCurrentTime();

	OperationManager::instance()->IncrementTotalLoadPerContent(pck->contentInfo->iClassType);

	stringstream msg;
	msg << config_->iDomainIdent << "," << config_->sName << "," << "NEW_PACKAGE," << pck->contentInfo->sName;
	Logger::instance()->LogTrace(msg.str());
}

void ClientNode::StoreMeasurements()
{
	BaseNode::StoreMeasurements();

	stringstream msg;

	for(int i = 0; i < OperationManager::instance()->GetNumberOfContents(); i++)
		msg << "Number of packages from class " << i << "," << contentsTx_[i] << endl;

	myLog_ << msg.str();
}

