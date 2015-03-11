/*
 * StringUtility.h
 *
 *  Created on: 26/08/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef STRINGUTILITY_H_
#define STRINGUTILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class StringUtility {
public:
	StringUtility();
	virtual ~StringUtility();

	static vector<string> split(string &line, char delim);
	static string toString(double value);
	static string toString(int value);
	static string toString(unsigned int value);
	static string toEventString(int eventType);
};

#endif /* STRINGUTILITY_H_ */
