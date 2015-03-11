/*
 * TimeHandler.h
 *
 *  Created on: 01/09/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef TIMEHANDLER_H_
#define TIMEHANDLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include "Geral.h"
#include "StringUtility.h"
#include "Logger.h"

using namespace std;

class TimeHandler {
public:
	TimeHandler();
	virtual ~TimeHandler();

	static TimeHandler* instance();

	string getLocalTimeStr();
	string getLocalTimeStrToFileName();
	string toString(time_t time);

	void setCurrentTime(double time) { currentTime_ = time; };
	double getCurrentTime()			 { return currentTime_; };

	void startCountingSimTime();
	void stopCountingSimTime();

private:
	static TimeHandler* aTimeHandler_;

	double currentTime_;
	time_t startSim_;
};

#endif /* TIMEHANDLER_H_ */
