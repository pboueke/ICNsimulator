/*
 * Logger.cpp
 *
 *  Created on: 01/09/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "OperationManager.h"
#include "TimeHandler.h"
#include "Logger.h"

Logger* Logger::aLogger_ = NULL;

Logger::Logger()
{
	string filename = "./Output/LogApp_" + TimeHandler::instance()->getLocalTimeStrToFileName() + ".txt";
	myLog_.open(filename.c_str(), ofstream::out);

	filename = "./Output/Trace_" + TimeHandler::instance()->getLocalTimeStrToFileName() + ".csv";
	myTrace_.open(filename.c_str(), ofstream::out);

	myTrace_ << "Time,Domain,Node,Event" << endl;
}

Logger::~Logger()
{
	if(aLogger_)
	{
		myTrace_.flush();
		myLog_.flush();

		myLog_.close();
		myTrace_.close();
	}

	DELETE_PTR(aLogger_);
}

Logger* Logger::instance()
{
	if(!aLogger_)
		aLogger_ = new Logger();

	return aLogger_;
}

void Logger::LogError(string error)
{
	myLog_ << "Time: " << TimeHandler::instance()->getCurrentTime() << " - Error: " << error << endl;
	myLog_.flush();

	cout << error << endl;
}

void Logger::LogInfo(string info)
{
	myLog_ << "Current Time: " << TimeHandler::instance()->getCurrentTime() << " - Info: " << info << endl;
	myLog_.flush();

	cout << info << endl;
}

void Logger::LogTrace(string trace)
{
	if(OperationManager::instance()->IsTrace() &&
		TimeHandler::instance()->getCurrentTime() >= OperationManager::instance()->GetMinTimeTrace() &&
		TimeHandler::instance()->getCurrentTime() <= OperationManager::instance()->GetMaxTimeTrace())
	{
		myTrace_ << TimeHandler::instance()->getCurrentTime() << "," << trace << endl;
		myTrace_.flush();
	}
}

