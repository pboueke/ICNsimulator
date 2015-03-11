/*
 * SimConfiguration.cpp
 *
 *  Created on: 26/08/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "SimConfiguration.h"

SimConfiguration::SimConfiguration()
{
}

SimConfiguration::~SimConfiguration()
{
	for(size_t i = 0; i < TopologyItems_.size(); i++)
	{
		DELETE_PTR(TopologyItems_[i]);
	}

	for(size_t i = 0; i < ContentsItems_.size(); i++)
	{
		DELETE_PTR(ContentsItems_[i]);
	}

	for(size_t i = 0; i < TransDistParams_.size(); i++)
	{
		DELETE_PTR(TransDistParams_[i]);
	}

	TopologyItems_.clear();
	ContentsItems_.clear();
	TransDistParams_.clear();

	Logger::instance()->LogInfo("Configuration deleted.");
}

stNodeParams* SimConfiguration::GetNodeByIndex(int index)
{
	if(index >= (int) TopologyItems_.size())
		return NULL;

	return TopologyItems_[index];
}

stContentsParams* SimConfiguration::GetContentByIndex(int index)
{
	if(index > (int) ContentsItems_.size())
		return NULL;

	return ContentsItems_[index];
}

stDistributionParams* SimConfiguration::GetTransDistParams(int domain)
{
	if(domain > (int) TransDistParams_.size())
	{
		Logger::instance()->LogError("There is no distribution associated for domain " + StringUtility::toString(domain));
		return NULL;
	}

	return TransDistParams_[domain];
}

bool SimConfiguration::Parser(string configFile)
{
	try
	{
		myConfigFile_.open(configFile.c_str(), fstream::in);
		if (myConfigFile_.is_open())
		{
			int index;
			string line;
			while(!myConfigFile_.eof())
			{
				getline (myConfigFile_, line);

				if(line.find("SimTime") != line.npos)
				{
					index = line.find('=');
					iSimTime_ = atoi(line.substr(index+1).c_str());
				}
				else if(line.find("TTL") != line.npos)
				{
					index = line.find('=');
					iTTLLimit_ = atoi(line.substr(index+1).c_str());
				}
				else if(line.find("CacheAlgorithm") != line.npos)
				{
					index = line.find('=');
					iCacheAlgorithm = atoi(line.substr(index+1).c_str());

					string msg = iCacheAlgorithm == 1 ? "Probability" : "RC";
					Logger::instance()->LogInfo("Storage algorithm: " + msg);
				}
				else if(line.find("DebugOper") != line.npos)
				{
					index = line.find('=');
					iDebugOper_ = atoi(line.substr(index+1).c_str());
				}
				else if(line.find("DebugClient") != line.npos)
				{
					index = line.find('=');
					iDebugClient_ = atoi(line.substr(index+1).c_str());
				}
				else if(line.find("DebugRouter") != line.npos)
				{
					index = line.find('=');
					iDebugRouter_ = atoi(line.substr(index+1).c_str());
				}
				else if(line.find("[Trace]") != line.npos)
				{
					FillTraceParams();
				}
				else if(line.find("[RCLimits]") != line.npos)
				{
					FillLimitsParams(&RCLimits_);
				}
				else if(line.find("[RequestDistParams]") != line.npos)
				{
					FillDistParams(&ReqDistParams_);
				}
				else if(line.find("[TransmissionDistParams]") != line.npos)
				{
					FillTransParams();
				}
				else if(line.find("[RandomDistParams]") != line.npos)
				{
					FillDistParams(&RandomDistParams_);
				}
				else if(line.find("[ContentDistParams]") != line.npos)
				{
					FillDistParams(&ContentDistParams_);
				}
				else if(line.find("[RCDistParams") != line.npos)
				{
					FillDistParams(&RCDistParams_);
				}
				else if(line.find("DomainsNumber") != line.npos)
				{
					int index = line.find('=');
					iDomainsNumber_= atoi(line.substr(index + 1).c_str());

					string msg = "SimConfiguration::Parser - Number of domains = " + StringUtility::toString(iDomainsNumber_);
					Logger::instance()->LogInfo(msg);
				}
				else if(line.find("NodesNumber") != line.npos)
				{
					int index = line.find('=');
					iNodesNumber_= atoi(line.substr(index + 1).c_str());

					string msg = "SimConfiguration::Parser - Number of nodes = " + StringUtility::toString(iNodesNumber_);
					Logger::instance()->LogInfo(msg);
				}
				else if(line.find("[NodeParams]") != line.npos)
				{
					FillNodeParams();
				}
				else if(line.find("ContentsNumber") != line.npos)
				{
					int index = line.find('=');
					iContentsNumber_ = atoi(line.substr(index + 1).c_str());

					string msg = "SimConfiguration::Parser - Number of contents = " + StringUtility::toString(iContentsNumber_);
					Logger::instance()->LogInfo(msg);
				}
				else if(line.find("[ContentParams]") != line.npos)
				{
					FillContentParams();
				}
			}

			myConfigFile_.close();
		}
		else
			return false;
	}
	catch(int ex)
	{
		string error = "SimConfiguration::Parser - Unexpected error number = " + StringUtility::toString(ex);
		Logger::instance()->LogError(error);
		return false;
	}

	return true;
}

void SimConfiguration::FillLimitsParams(stLimits *limit)
{
	string line;
	getline(myConfigFile_, line);
	int index = line.find('=');
	limit->iUpper = atoi(line.substr(index+1).c_str());

	getline(myConfigFile_, line);
	index = line.find('=');
	limit->iLower = atoi(line.substr(index+1).c_str());

	string msg = "SimConfiguration::FillLimitsParams - RC Limits loaded (" + StringUtility::toString(limit->iUpper) + "," + StringUtility::toString(limit->iLower) + ")";
	Logger::instance()->LogInfo(msg);
}

void SimConfiguration::FillDistParams(stDistributionParams *dist)
{
	string line;
	getline(myConfigFile_, line);
	int index = line.find('=');
	dist->iType = atoi(line.substr(index + 1).c_str());

	getline(myConfigFile_, line);
	index = line.find('=');
	string sAux = line.substr(index + 1);
	vector<string> values = StringUtility::split(sAux, ';');

	dist->params = (float*) malloc(values.size() * sizeof(float));
	for (size_t i = 0; i < values.size(); i++)
		dist->params[i] = atof(values[i].c_str());

	string msg = "SimConfiguration::FillDistParams - Distribution Params loaded (" + StringUtility::toString(dist->iType) + "), number of parameters = " + StringUtility::toString((int)values.size());
	Logger::instance()->LogInfo(msg);
	msg = "Parameters:";
	for(size_t i = 0; i < values.size(); i++)
	{
		msg += StringUtility::toString(dist->params[i]) + ",";
	}
	Logger::instance()->LogInfo(msg);
}

void SimConfiguration::FillTransParams()
{
	stDistributionParams* dist = (stDistributionParams*) malloc(sizeof(stDistributionParams));

	string line;
	getline(myConfigFile_, line);
	int index = line.find('=');
	dist->iDomain = atoi(line.substr(index + 1).c_str());

	getline(myConfigFile_, line);
	index = line.find('=');
	dist->iType = atoi(line.substr(index + 1).c_str());

	getline(myConfigFile_, line);
	index = line.find('=');
	string sAux = line.substr(index + 1);
	vector<string> values = StringUtility::split(sAux, ';');

	dist->params = (float*) malloc(values.size() * sizeof(float));
	for (size_t i = 0; i < values.size(); i++)
		dist->params[i] = atof(values[i].c_str());

	string msg = "SimConfiguration::FillDistParams - Distribution Params loaded (" + StringUtility::toString(dist->iType) + "), number of parameters = " + StringUtility::toString((int)values.size());
	Logger::instance()->LogInfo(msg);
	msg = "Parameters:";
	for(size_t i = 0; i < values.size(); i++)
	{
		msg += StringUtility::toString(dist->params[i]) + ",";
	}
	Logger::instance()->LogInfo(msg);

	TransDistParams_.push_back(dist);
}

void SimConfiguration::FillNodeParams()
{
	stNodeParams* stAux = (stNodeParams*) malloc(sizeof(stNodeParams));

	if(stAux != NULL)
	{
		string line;
		getline(myConfigFile_, line);
		int index = line.find('=');
		stAux->iType = atoi(line.substr(index+1).c_str());

		getline(myConfigFile_, line);
		index = line.find('=');
		int index2 = line.find('\r');
		if(index2 == (int)line.npos)
			stAux->sName = strdup(line.substr(index+1).c_str());
		else
			stAux->sName = strndup(line.substr(index+1).c_str(), line.length() - (index + 2));

		getline(myConfigFile_, line);
		index = line.find('=');
		stAux->iIdent = atoi(line.substr(index+1).c_str());

		getline(myConfigFile_, line);
		index = line.find('=');
		stAux->iDomainIdent = atoi(line.substr(index+1).c_str());

		getline(myConfigFile_, line);
		index = line.find('=');
		string sAux = line.substr(index+1);
		vector<string> values = StringUtility::split(sAux, ';');
		stAux->iCacheSize = atoi(values[0].c_str());
		if(values.size() > 1)
			stAux->iReplacementType = atoi(values[1].c_str());
		else
			stAux->iReplacementType = Enums::REPLACEMENT_RANDOM;

		Logger::instance()->LogInfo("Cache replacement type: " + StringUtility::toString(stAux->iReplacementType) + ", Cache size: " + StringUtility::toString(stAux->iCacheSize));

		getline(myConfigFile_, line);
		index = line.find('=');
		sAux = line.substr(index + 1);
		stAux->iNumNodesConnectedAbove = 0;

		if(sAux.length() > 0)
		{
			values = StringUtility::split(sAux, ';');
			stAux->iNodesConnectedAbove = (int*) malloc(values.size() * sizeof(int));
			stAux->iNumNodesConnectedAbove = values.size();
			for (size_t i = 0; i < values.size(); i++)
				stAux->iNodesConnectedAbove[i] = atoi(values[i].c_str());
		}

		getline(myConfigFile_, line);
		index = line.find('=');
		sAux = line.substr(index + 1);
		stAux->iNumNodesConnectedDomain = 0;

		if(sAux.length() > 0)
		{
			values = StringUtility::split(sAux, ';');

			stAux->iNodesConnectedDomain = (int*) malloc(values.size() * sizeof(int));
			stAux->iNumNodesConnectedDomain = values.size();
			for (size_t i = 0; i < values.size(); i++)
				stAux->iNodesConnectedDomain[i] = atoi(values[i].c_str());
		}

		TopologyItems_.push_back(stAux);

		stringstream msg;
		msg << "SimConfiguration::Parser - Node loaded (" << stAux->sName << ")";
		Logger::instance()->LogInfo(msg.str());
	}
	else
		Logger::instance()->LogError("SimConfiguration::FillNodeParams - Memory are not allocated.");
}

void SimConfiguration::FillContentParams()
{
	stContentsParams* stAux = (stContentsParams*) malloc(sizeof(stContentsParams));

	if(stAux != NULL)
	{
		string line;
		getline(myConfigFile_, line);
		int index = line.find('=');
		stAux->sName = strdup(line.substr(index+1).c_str());

		getline(myConfigFile_, line);
		index = line.find('=');
		stAux->iSize = atoi(line.substr(index+1).c_str());

		stAux->iClassType = (int) ContentsItems_.size();

		ContentsItems_.push_back(stAux);

		stringstream msg;
		msg << "SimConfiguration::Parser - Content loaded (" << stAux->sName << ")";
		Logger::instance()->LogInfo(msg.str());
	}
	else
		Logger::instance()->LogError("SimConfiguration::FillContentParams - Memory are not allocated.");
}

void SimConfiguration::FillTraceParams()
{
	string line;
	getline(myConfigFile_, line);
	int index = line.find('=');
	iTrace_ = atoi(line.substr(index+1).c_str());

	getline(myConfigFile_, line);
	index = line.find('=');
	iMinTimeTrace_ = atoi(line.substr(index+1).c_str());

	getline(myConfigFile_, line);
	index = line.find('=');
	iMaxTimeTrace_ = atoi(line.substr(index+1).c_str());

	stringstream msg;
	msg << "SimConfiguration::Parser - TraceParams [" << iMinTimeTrace_ << "," << iMaxTimeTrace_ << "], IsEnabled = " << iTrace_ ;
	Logger::instance()->LogInfo(msg.str());
}

stDistributionParams* SimConfiguration::GetDistributionParams(int iDistType)
{
	stDistributionParams* dist = NULL;

	if(iDistType == Enums::DIST_REQUEST)
		dist = &ReqDistParams_;
	else if(iDistType == Enums::DIST_RANDOM)
		dist = &RandomDistParams_;
	else if(iDistType == Enums::DIST_RC)
		dist = &RCDistParams_;
	else if(iDistType == Enums::DIST_CONTENT)
		dist = &ContentDistParams_;

	return dist;
}

