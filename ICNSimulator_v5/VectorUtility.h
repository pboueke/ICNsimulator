/*
 * VectorHelper.h
 *
 *  Created on: 07/09/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef VECTORUTILITY_H_
#define VECTORUTILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>

#include "Geral.h"

using namespace std;

class VectorUtility {
public:
	VectorUtility();
	virtual ~VectorUtility();

	static double Mean(vector<double> array);
	static double Max(vector<double> array);
	static double Min(vector<double> array);
	static double StdDev(vector<double> array);
	static double Var(vector<double> array);
	static double Cov(vector<double> array);
};

#endif /* VECTORUTILITY_H_ */
