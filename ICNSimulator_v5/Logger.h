/*
 * Logger.h
 *
 *  Created on: 01/09/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "TimeHandler.h"
#include "Geral.h"

using namespace std;

class Logger {
public:
	Logger();
	virtual ~Logger();

	static Logger* instance();

	void LogInfo(string info);
	void LogError(string error);
	void LogTrace(string trace);

private:
	static Logger* aLogger_;

	ofstream myLog_;
	ofstream myTrace_;
};

#endif /* LOGGER_H_ */
