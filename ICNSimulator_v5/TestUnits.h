/*
 * TestUnits.h
 *
 *  Created on: 01/09/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef TESTUNITS_H_
#define TESTUNITS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "Geral.h"
#include "TimeHandler.h"
#include "DistUtility.h"
#include "VectorUtility.h"

using namespace std;

class TestUnits {
public:
	TestUnits();
	virtual ~TestUnits();

	static TestUnits* instance();

	bool RunDistributionTest(int numPoints, int type, double* params, int paramsLength);
	bool RunZipfTest(double alpha, int paramsLength);

private:
	static TestUnits* aTestUnit_;

	ofstream testLog_;

};

#endif /* TESTUNITS_H_ */
