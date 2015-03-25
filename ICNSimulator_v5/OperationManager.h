/*
 * OperationManager.h
 *
 *  Created on: 26/08/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef OPERATIONMANAGER_H_
#define OPERATIONMANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <sstream>

#include "Geral.h"
#include "Logger.h"
#include "StructureUtilities.h"
#include "SimConfiguration.h"
#include "BaseNode.h"
#include "ClientNode.h"
#include "ServerNode.h"
#include "RouterBaseNode.h"
#include "RouterProbNode.h"
#include "RouterRcNodeNQueue.h"
#include "RouterRcNodeQueue.h"

using namespace std;


struct stEvent
{
	double time;
	BaseNode* oNode;
	int iTypeEvent;
	struct stEvent* nextEv;
	struct stEvent* previousEv;

	//Added for use with Ordered_LinkedList
	static bool operator>(stEvent* s_ev);
	static bool operator<(stEvent* s_ev);

};

class OperationManager
{
public:
	OperationManager();
	virtual ~OperationManager();

	static OperationManager* instance();

	void SetRunNumber(int runNumber)	{iRunNumber_ = runNumber;};

	bool LoadConfiguration(string config);

	stLimits*				GetRCLimits()							{ return config_->GetRCLimits(); };
	stDistributionParams* 	GetDistributionParams(int iDistType)	{ return config_->GetDistributionParams(iDistType); };
	stDistributionParams* 	GetTransDistParams(int iDomain)			{ return config_->GetTransDistParams(iDomain); };
	int						GetMaxTTL()								{ return config_->GetMaxTTL(); };
	int						GetNumberOfContents()					{ return config_->GetNumberOfContents(); };
	int						GetNumberOfNodes()						{ return config_->GetNumberOfNodes(); };
	double					GetSimulationTime()						{ return config_->GetSimulationTime(); };

	bool					IsTrace()			{ return config_->IsTrace(); };
	int						GetMinTimeTrace()	{ return config_->GetMinTimeTrace(); };
	int						GetMaxTimeTrace()	{ return config_->GetMaxTimeTrace(); };

	bool					IsDebug(int iNode);

	bool					SimStopped() { return bInterrupted; };

	bool Start();
	bool Stop();

	void PrintEventList(stEvent* ev);

	void AddTimeInDomain(int domain, int classType, double time);
	void AddTimeToFind(int domain, int classType, double time);
	void AddProbabilityPerContent(int domain, int classType, double prob);

	void IncrementFilteredLoadPerContent(int domain, int classType);
	void IncrementTotalLoadPerContent(int classType);

	void EnqueueEvent(BaseNode* node, double time, int iTypeEv);
	stEvent* DequeueEvent();

	void FindEventForward(stEvent* source, stEvent* toFind);
	void FindEventBackward(stEvent* source, stEvent* toFind);

private:
	void CreateNodes();
	void StartNodes();
	void StopNodes();
	void LinkReferences();
	void ThreadEvHandler();
	void StartThreadEventHandler();
	void StopThreadEventHandler();
	bool IsSimulationTerminated();
	void DeleteEventList(stEvent* ev);

	void CreateGraphicFiles();

	static void* ProxyThreadFunction(void* classPtr) { static_cast<OperationManager*>(classPtr)->ThreadEvHandler(); return NULL; };
	static bool Sorter(stEvent* ev01, stEvent* ev02)
	{
		if(ev01->time == ev02->time)
			ev02->time += 0.001;

		return ev01->time < ev02->time;
	};

private:
	static OperationManager* aOperation_;

	int iRunNumber_;

	SimConfiguration* config_;
	vector<BaseNode*> nodes_;
	Ordered_LinkedList<stEvent*> eventList_; 	//vector<stEvent*> eventList_;
	stEvent* pStartEvents;
	stEvent* pEndEvents;
	stEvent* pMiddleEvents;
	int countEvents_;

	bool bInterrupted;

	pthread_t threadEventHandler_;
	pthread_mutex_t mutexEventHandler_;
	pthread_cond_t signalToProcessEv_;

	double** dPckTimeInDomain_;
	unsigned long int** iPckNumInDomain_;

	double** timeToFindPerContentDom_;
	unsigned long int** iPckTimeToFindPerContentDom_;

	double* timeToFindPerContent_;
	unsigned long int* iPckTimeToFindPerContent_;

	double** probabilityPerContent_;
	unsigned long int** iPckProbabilityPerContent_;

	unsigned long int** filteredLoadPerContentDom_;
	unsigned long int* filteredLoadPerContent_;
	unsigned long int* totalLoadPerContent_;
};

#endif /* OPERATIONMANAGER_H_ */
