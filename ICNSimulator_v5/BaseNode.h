/*
 * BaseNode.h
 *
 *  Created on: 26/08/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef BASENODE_H_
#define BASENODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <pthread.h>
#include <string>
#include <sstream>

#include "Geral.h"
#include "Logger.h"
#include "DistUtility.h"

using namespace std;

class BaseNode
{
public:
	BaseNode(stNodeParams* config);
	virtual ~BaseNode();

	stNodeParams* GetConfig()	{ return config_; };
	int GetIdent() 				{ return config_->iIdent; };
	string GetName()			{ return config_->sName; };

	virtual void Start();
	virtual void Stop();

	virtual void ProcessEvent(int iEventType);
	virtual void ProcessTx();
	virtual void ProcessRx(stPackage* pack);

	void InsertNodesConnectedAbove(BaseNode* node);
	void InsertNodesConnectedDomain(BaseNode* node);

	int* GetNodesConnectedAbove(int* nodesAbove);
	int* GetNodesConnectedDomain(int* nodesDomain);

	int	GetNumTotalPackages()	{ return numPackageTotal_; };

protected:
	void StoreToLogFile(string info, string direction);
	void StoreDebugInfo(ofstream* file, string info);
	virtual void StoreMeasurements();

	void CalculateTimeInDomain(double startTime, int hoops);
	void CalculateTimeToFind(int classType, double startTime);
	void CalculateW(double startTime);
	void CalculateEN(double queueSize);
	void CalculateU();

	double	GetNextEventTimeByDistribution(stDistributionParams* params);
	double 	GetRandomByDistribution(stDistributionParams* params);
	double	GetZpifProb(stDistributionParams* param, int numContents, int classType);
	int 	GetContentByDistribution(stDistributionParams* param, int numContents);

	void SendPackageToNextNode(stPackage* pck);
	void SendPackageToNextDomain(stPackage* pck);
//	void SendPackageToSourceDomainNode(stPackage* pck);

	static bool Sorter(stPackage* ev01, stPackage* ev02) {return ev01->tStartSending < ev02->tStartSending;};

protected:
	stNodeParams* config_;
	ofstream myLog_;

	stDistributionParams* distToSend_;
	stDistributionParams* distToRequest_;
	stDistributionParams* distToRandom_;
	stDistributionParams* distToContent_;
	stDistributionParams* distToRC_;

	vector<BaseNode*> nodesConnectedAbove_;
	vector<BaseNode*> nodesConnectedDomain_;

	vector<stPackage *> buffer_;

	double lastEvTime_;
	double meanTimeToWait_;
	double idleTime_;
	double queueMean_;
	unsigned long int numPackageTotal_;
	unsigned long int numDepartures_;
	unsigned long int numPackageIn_;
	unsigned long int numPackageOut_;
};

#endif /* BASENODE_H_ */
