/*
 * VectorHelper.cpp
 *
 *  Created on: 07/09/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "VectorUtility.h"

VectorUtility::VectorUtility()
{
}

VectorUtility::~VectorUtility()
{
}

double VectorUtility::Max(vector<double> array)
{
	double max = array[0];

	for(size_t i = 0; i < array.size(); i++)
	{
		if(array[i] > max)
			max = array[i];
	}

	return max;
}

double VectorUtility::Min(vector<double> array)
{
	double min = array[0];

	for(size_t i = 0; i < array.size(); i++)
	{
		if(array[i] < min)
			min = array[i];
	}

	return min;
}

double VectorUtility::Mean(vector<double> array)
{
	double mean = 0.0;

	for(size_t i = 0; i < array.size(); i++)
	{
		mean += array[i] / (double) array.size();
	}

	return mean;
}

double VectorUtility::Var(vector<double> array)
{
	double sum1 = 0.0;
	double sum2 = 0.0;

	for(size_t i = 0; i < array.size(); i++)
	{
		sum1 += array[i] / (double) array.size();
		sum2 += (array[i] * array[i]) / (double) array.size();
	}

	return sum2 - (sum1 * sum1);
}

double VectorUtility::StdDev(vector<double> array)
{
	return sqrt(Var(array));
}

double VectorUtility::Cov(vector<double> array)
{
	double sum1 = 0.0;
	double sum2 = 0.0;

	for(size_t i = 0; i < array.size(); i++)
	{
		sum1 += array[i] / (double) array.size();
		sum2 += (array[i] * array[i]) / (double) array.size();
	}

	return sqrt(sum2 - (sum1 * sum1)) / sum1;
}

