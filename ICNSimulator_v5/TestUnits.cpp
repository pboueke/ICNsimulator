/*
 * TestUnits.cpp
 *
 *  Created on: 01/09/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "TestUnits.h"

TestUnits* TestUnits::aTestUnit_ = NULL;

TestUnits::TestUnits()
{
}

TestUnits::~TestUnits()
{
	testLog_.close();

	DELETE_PTR(aTestUnit_);
}

TestUnits* TestUnits::instance()
{
	if(!aTestUnit_)
		aTestUnit_ = new TestUnits();

	return aTestUnit_;
}

/*
<numPoints> <type> <params1> <params2> ... <paramsN>
*/
bool TestUnits::RunDistributionTest(int numPoints, int type, double* params, int paramsLength)
{
	vector<double> serie;
	string filename;
	filename = string("./Output/TestLog_") + TimeHandler::instance()->getLocalTimeStrToFileName() + string(".txt");

	testLog_.open(filename.c_str(), ofstream::out);

	testLog_ << "Starting Distribution Simulation" << endl;
	testLog_ << "Distribution type: " << type << endl << endl;

	for(int i = 0; i < numPoints; i++)
	{
		double rv = DistUtility::instance()->GenerateRandomVariable(type, params, paramsLength);
		testLog_ << i << " " << rv << endl;

		serie.push_back(rv);
	}

	cout << "Serie with " << numPoints << " points. Mean = " << VectorUtility::Mean(serie) << endl;

	testLog_.flush();
	testLog_.close();

	return true;
}

bool TestUnits::RunZipfTest(double alpha, int listLength)
{
	string filename;
	filename = string("./Output/TestLog_") + TimeHandler::instance()->getLocalTimeStrToFileName() + string(".txt");

	testLog_.open(filename.c_str(), ofstream::out);

	for(int i = 1; i <= listLength; i++)
	{
		double rv = DistUtility::instance()->zpifProb(alpha, listLength, i);
		testLog_ << i << " " << rv << endl;
	}

	testLog_.flush();
	testLog_.close();

	return true;
}



