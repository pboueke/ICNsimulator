/*
 * ServerNode.cpp
 *
 *  Created on: 10/11/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "OperationManager.h"
#include "ServerNode.h"

ServerNode::ServerNode(stNodeParams* config) : BaseNode(config)
{
	// TODO Auto-generated constructor stub

}

ServerNode::~ServerNode()
{
	// TODO Auto-generated destructor stub
}

void ServerNode::Start()
{
	BaseNode::Start();
	myLog_ << "NRS started." << endl;
}

void ServerNode::Stop()
{
	BaseNode::Stop();
	myLog_ << "NRS Stopped." << endl;
}

void ServerNode::ProcessRx(stPackage* pack)
{
	CalculateU();
	CalculateEN((double)buffer_.size());

	double nextEventToSend = GetNextEventTimeByDistribution(distToSend_);
	pack->tStartSending = nextEventToSend;
	pack->tStartQueue = TimeHandler::instance()->getCurrentTime();
	buffer_.push_back(pack);

	if(buffer_.size() == 1)
	{
		// Penalização de 10x quando o a requisição chega no roteador
		OperationManager::instance()->EnqueueEvent(this, nextEventToSend * 10, Enums::SEND_PACKAGE);
	}

	lastEvTime_ = TimeHandler::instance()->getCurrentTime();
}

void ServerNode::ProcessTx()
{
	BaseNode::ProcessTx();

	stPackage *pck = buffer_[0];
	buffer_.erase(buffer_.begin());

	CalculateTimeToFind(pck->contentInfo->iClassType, pck->tStartTotal);

//	stringstream msg;
//	msg << "Received request to content " << pck->contentInfo->sName;
//	Logger::instance()->LogInfo("ServerNode::ProcessTx - " + msg.str());
//	StoreToLogFile(msg.str(), "Rx");

	DELETE_PTR(pck);
}
