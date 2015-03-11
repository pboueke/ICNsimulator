/*
 * TimeHandler.cpp
 *
 *  Created on: 01/09/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "TimeHandler.h"

TimeHandler* TimeHandler::aTimeHandler_ = NULL;

TimeHandler::TimeHandler()
{
}

TimeHandler::~TimeHandler()
{
	DELETE_PTR(aTimeHandler_);
}

TimeHandler* TimeHandler::instance()
{
	if(!aTimeHandler_)
		aTimeHandler_ = new TimeHandler();

	return aTimeHandler_;
}

void TimeHandler::startCountingSimTime()
{
	currentTime_ = 0.0;

	startSim_ = time(0);
	tm now = *localtime(&startSim_);

	char buf[80];
	strftime(buf, sizeof(buf), "%x %X", &now);
	Logger::instance()->LogInfo("TimeHandler::startCountingSimTime - Start simulation at " + string(buf));
}

void TimeHandler::stopCountingSimTime()
{
	currentTime_ = 0.0;

	time_t     stopSim = time(0);
	tm now = *localtime(&stopSim);

	char buf[80];
	strftime(buf, sizeof(buf), "%x %X", &now);
	Logger::instance()->LogInfo("TimeHandler::stopCountingSimTime - Stop simulation at " + string(buf));

	double seconds = difftime(startSim_, stopSim);
	Logger::instance()->LogInfo("TimeHandler::stopCountingSimTime - Simulation time in minutes" + StringUtility::toString(seconds / 60));
}

string TimeHandler::getLocalTimeStr()
{
	time_t     now = time(0);
	char       buf[80];
	struct tm currentTime = *localtime(&now);

	// Visit http://www.cplusplus.com/reference/clibrary/ctime/strftime/
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%x %X", &currentTime);

	return string(buf);
}

string TimeHandler::toString(time_t time)
{
	char       buf[80];
	struct tm currentTime = *localtime(&time);

	strftime(buf, sizeof(buf), "%x %X", &currentTime);

	return string(buf);
}

string TimeHandler::getLocalTimeStrToFileName()
{
	time_t     now = time(0);
	char       buf[80];
	struct tm currentTime = *localtime(&now);

	strftime(buf, sizeof(buf), "%d%m%y_%H%M%S", &currentTime);

	return string(buf);
}

