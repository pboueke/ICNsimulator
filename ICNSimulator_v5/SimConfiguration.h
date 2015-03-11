/*
 * SimConfiguration.h
 *
 *  Created on: 26/08/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef SIMCONFIGURATION_H_
#define SIMCONFIGURATION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

#include "Logger.h"
#include "StringUtility.h"
#include "Geral.h"

using namespace std;

class SimConfiguration {
public:
	SimConfiguration();
	virtual ~SimConfiguration();

	bool Parser(string configFile);

	int GetNumberOfNodes()				{ return iNodesNumber_; };
	int GetNumberOfDomains()			{ return iDomainsNumber_; };
	int GetNumberOfContents()			{ return iContentsNumber_; };
	int GetSimulationTime()				{ return iSimTime_; };
	int GetMaxTTL()						{ return iTTLLimit_; };

	bool IsDebugOper()					{ return iDebugOper_ == 1; };
	bool IsDebugClient()				{ return iDebugClient_ == 1; };
	bool IsDebugRouter()				{ return iDebugRouter_ == 1; };
	bool IsTrace()						{ return iTrace_ == 1; };

	int GetMinTimeTrace()				{ return iMinTimeTrace_; };
	int GetMaxTimeTrace()				{ return iMaxTimeTrace_; };

	int GetTypeCacheAlgorithm()			{ return iCacheAlgorithm; };

	stLimits* GetRCLimits()				{ return &RCLimits_; };

	stNodeParams*				GetNodeByIndex(int index);
	stContentsParams*			GetContentByIndex(int index);
	vector<stContentsParams*>	GetContents()	{ return ContentsItems_; };
	stDistributionParams* 		GetDistributionParams(int iDistType);
	stDistributionParams* 		GetTransDistParams(int domain);

private:
	void FillLimitsParams(stLimits *limit);
	void FillDistParams(stDistributionParams *dist);
	void FillTransParams();
	void FillNodeParams();
	void FillContentParams();
	void FillTraceParams();

private:
	ifstream myConfigFile_;

	int iNodesNumber_;
	int iDomainsNumber_;
	int iContentsNumber_;
	int iSimTime_;
	int iTTLLimit_;
	int iDebugOper_;
	int iDebugClient_;
	int iDebugRouter_;
	int iCacheAlgorithm;
	int iTrace_;
	int iMinTimeTrace_;
	int iMaxTimeTrace_;

	stLimits RCLimits_;
	stDistributionParams ReqDistParams_;
	//stDistributionParams TransDistParams_;
	stDistributionParams RandomDistParams_;
	stDistributionParams ContentDistParams_;
	stDistributionParams RCDistParams_;

	vector<stNodeParams*> TopologyItems_;
	vector<stContentsParams*> ContentsItems_;
	vector<stDistributionParams*> TransDistParams_;
};

#endif /* SIMCONFIGURATION_H_ */
