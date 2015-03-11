/*
 * DistUtility.cpp
 *
 *  Created on: 01/09/2013
 *      Author: Diego Vargas Jannibelli
 *
 *  NOTE: The algorithms for all distribution were based on http://www.cse.usf.edu/~christen/tools/toolpage.html
 */

#include "DistUtility.h"

DistUtility* DistUtility::aDistribution_ = NULL;

DistUtility::DistUtility()
{
	srand(time(NULL));
}

DistUtility::~DistUtility()
{
}

DistUtility* DistUtility::instance()
{
	if(!aDistribution_)
		aDistribution_ = new DistUtility();

	return aDistribution_;
}

double DistUtility::GenerateRandomVariable(int type, double* params, int length)
{
	switch(type)
	{
		case Enums::EXP:
			return exponential(1.0 / params[0]);
			break;
		case Enums::NORMAL:
			return normal(params[0],params[1]);
			break;
		case Enums::UNIFORME:
			return uniform((int)params[0],(int)params[1]);
			break;
		case Enums::ZPIF:
			return zpif(params[0], (int)params[1]);
			break;
		case Enums::FIXO:
			return params[0];
			break;
	}

	return INVALID_VALUE;
}

double DistUtility::zpif(double alpha, int n)
{
	double random = rand_val();

	double c = 0; 		// Normalization constant
	double sum_prob = 0.0;    // Sum of probabilities
	double zipf_value = 0.0;  // Computed exponential value to be returned
	int i;	            // Loop counter

	for (i = 1; i <= n; i++)
	  c += pow((double) i, alpha * -1.0);
	c = 1.0 / c;

	sum_prob = 0;
	for (i = 1; i <= n; i++)
	{
		sum_prob += c / pow((double) i, alpha);
		if (sum_prob >= random)
		{
		  zipf_value = i;
		  break;
		}
	}

	return zipf_value;
}

double DistUtility::zpifProb(double alpha, int n, int index)
{
	double c = 0; 		// Normalization constant
	int i;	            // Loop counter

	for (i = 1; i <= n; i++)
	  c = c + (1.0 / pow((double) i, alpha));
	c = 1.0 / c;

	return c / pow((double) index, alpha);
}

double DistUtility::exponential(double mean)
{
	double random = rand_val();

	double exp_value = 0.0;             // Computed exponential value to be returned

	// Compute exponential random variable using inversion method
	exp_value = -mean * log(random);

	return(exp_value);
}

double DistUtility::normal(double mean, double stdDev)
{
	double u = rand_val();
	double v = rand_val();

	double   r = 0.0, theta = 0.0;        // Variables for Box-Muller method
	double   x = 0.0;                     // Normal(0, 1) rv
	double   norm_rv = 0.0;               // The adjusted normal rv

	// Compute r
	r = sqrt(-2.0 * log(u));

	// Generate theta
	theta = 2.0 * PI * v;

	// Generate x value
	x = r * cos(theta);

	// Adjust x value for specified mean and variance
	norm_rv = (x * stdDev) + mean;

	// Return the normally distributed RV value
	return norm_rv;
}

double DistUtility::uniform(int max, int min)
{
	double unif_value = 0.0;            // Computed uniform value to be returned

	// Pull a uniform random value (0 < z < 1)
	double random = rand_val();

	// Compute uniform continuous random variable using inversion method
	unif_value = random * (max - min) + min;

	return(unif_value);
}

double DistUtility::rand_val()
{
	return (double) rand() / (double)RAND_MAX;
/*	const long  a =      16807;  // Multiplier
	const long  m = 2147483647;  // Modulus
	const long  q =     127773;  // m div a
	const long  r =       2836;  // m mod a
	static long x = 5;           // Random int value
	long        x_div_q;         // x divided by q
	long        x_mod_q;         // x modulo q
	long        x_new;           // New x value

	// RNG using integer arithmetic
	x_div_q = x / q;
	x_mod_q = x % q;
	x_new = (a * x_mod_q) - (r * x_div_q);
	if (x_new > 0)
	x = x_new;
	else
	x = x_new + m;

	// Return a random value between 0.0 and 1.0
	return((double) x / m);
*/
}

