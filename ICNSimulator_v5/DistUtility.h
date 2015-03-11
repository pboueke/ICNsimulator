/*
 * DistUtility.h
 *
 *  Created on: 01/09/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef DISTUTILITY_H_
#define DISTUTILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "Geral.h"

using namespace std;

class DistUtility {
public:
	DistUtility();
	virtual ~DistUtility();

	static DistUtility* instance();

	double GenerateRandomVariable(int type, double* params, int length);
	double zpif(double alpha, int n);
	double zpifProb(double alpha, int n, int index);
	double exponential(double mean);
	double normal(double mean, double stdDev);
	double uniform(int max, int min);

private:
	static DistUtility* aDistribution_;

	double rand_val();
};

#endif /* DISTUTILITY_H_ */
