//============================================================================
// Name        : ICNSimulator.cpp
// Author      : Diego Vargas Jannibelli
// Version     :
// Copyright   : Your copyright notice
// Description : Simulator Main Entry
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include "OperationManager.h"
#include "TestUnits.h"

using namespace std;

int main(int argc, char *argv[])
{
	string command;
	bool bStatus = false;

	if(argc > 1)
	{
		OperationManager::instance()->SetRunNumber(atoi(argv[1]));
		if(!OperationManager::instance()->LoadConfiguration("./SimConfig.txt") || !OperationManager::instance()->Start())
		{
			cout << "Configuration or Starting error" << endl;
			return EXIT_FAILURE;
		}

		Logger::instance()->LogInfo("Simulation finished");
		OperationManager::instance()->Stop();

		return EXIT_SUCCESS;
	}

	Logger::instance()->LogInfo("PESC/COPPE - UFRJ");
	Logger::instance()->LogInfo("ICN Simulator - Version 5.0.0.0");/*
	cout << "Command:" << endl;
	cin >> command;

	while(command.find("exit") == command.npos)
	{
		if(command.find("load") != command.npos)
		{
			string file;
			cin >> file;
			bStatus = OperationManager::instance()->LoadConfiguration(file);
		}

		else if(command.find("start") != command.npos)
		{
			bStatus = OperationManager::instance()->Start();
		}

		else if(command.find("stop") != command.npos)
		{
			bStatus = OperationManager::instance()->Stop();
		}

		else if(command.find("testd") != command.npos)
		{
			string cmdType;
			int numPoints, distType, numParams;
			cin >> numPoints;
			cin >> distType;
			cin >> numParams;

			double* params = (double*) malloc(numParams * sizeof(double));
			for(int i = 0; i < numParams; i++)
				cin >> params[i];

			bStatus = TestUnits::instance()->RunDistributionTest(numPoints, distType, params, numParams);
			DELETE_PTR(params);
		}

		else if(command.find("zipf") != command.npos)
		{
			string cmdType;
			double alpha;
			int listLength;
			cin >> alpha;
			cin >> listLength;

			bStatus = TestUnits::instance()->RunZipfTest(alpha, listLength);
		}

		else if(command.find("help") != command.npos)
		{
			cout << "load [configuration file path] - Load the configuration file into simulator." << endl;
			cout << "start - Start the simulator." << endl;
			cout << "stop - Stop the simulator." << endl;
			cout << "testd [number of samples] [type of distribution] [number of distribution parameters] [parameter1] ... [parameter n] - Test Unit of the distribution class." << endl;
			cout << "Type of distribution:" << endl;
			cout << "0 - Exponecial." << endl;
			cout << "1 - Normal." << endl;
			cout << "2 - Uniforme." << endl;
			cout << "3 - ZPIF." << endl;
			cout << "4 - Fixo." << endl;

			cout << endl;
			cout << "Configuration File:" << endl;
			cout << "SimTime paramter - simulation time in seconds." << endl;
			cout << "TTLLmits parameter - maximum hoops which the packages can do during the Random Walking." << endl;
			cout << "CacheAlgorithm parameter - the type of the storage algorithm which the simulation will use (0 = RC and 1 = Fixed Probability)." << endl;
			cout << "DebugClient parameter - it is a flag which indicates if some clients log information will be storage in the log files (1 = ON, 0 = OFF)." << endl;
			cout << "DebugRouter parameter - it is a flag which indicates if some routers log information will be storage in the log files (1 = ON, 0 = OFF)." << endl;
			cout << "RCLimits parameter - the bound of RC. When the RC value is greater or equal than UPPER value, the content is storaged in the cache." << endl;
			cout << "\t And the RC value is lower than LOWER value, the content is removed." << endl;
			cout << "DistParams parameter - it is which distribution will be used in the simulation events." << endl;
			cout << "\t Type can be 0 - exponencial distribution, 1 - normal distribution, 2 - uniform distribution, 3 - ZPIF distribution, 4 - fixed value." << endl;
			cout << "\t Params is the distribution parameters. It must be separated by ';'" << endl;
			cout << "NodesNumber parameter - number of nodes which will be simulated." << endl;
			cout << "DomainNumber parameter - number of domains." << endl;
			cout << "NodeParams parameter - the parameters of each nodes." << endl;
			cout << "\t Type can be 0 - Client, 1 - Router, 2 - NRS" << endl;
			cout << "\t Name is the node name" << endl;
			cout << "\t Ident is the identification in the topology" << endl;
			cout << "\t DomainIdent is the domain which it belongs" << endl;
			cout << "\t CacheSize is the size of cache;router cache replacement type (can be random or older)." << endl;
			cout << "\t NodesAboveConnected are the nodes connected in the domain above. It must be separated by ';'" << endl;
			cout << "\t DomainNodesConnected are the nodes connected in the same domain. It must be separated by ';'" << endl;
		}

		if(bStatus)
			cout << "Command " << command << " executed successfully." << endl;
		else
			cout << "Command " << command << " failed. See the log file to see the error." << endl;

		cout << "Command:" << endl;
		cin >> command;
	}

	OperationManager::instance()->Stop();

	return EXIT_SUCCESS;*/
}
