/*
 * StringUtility.cpp
 *
 *  Created on: 26/08/2013
 *      Author: Diego Vargas Jannibelli
 */

#include "StringUtility.h"

StringUtility::StringUtility()
{
}

StringUtility::~StringUtility()
{
}

vector<string> StringUtility::split(string &line, char delim)
{
	stringstream ss(line);
	string item;

	vector<string> elems;
	while (getline(ss, item, delim))
		elems.push_back(item);

	return elems;
}

string StringUtility::toString(double value)
{
	ostringstream convert;

	convert << value;
	return convert.str();
}

string StringUtility::toString(int value)
{
	ostringstream convert;

	convert << value;
	return convert.str();
}

string StringUtility::toString(unsigned int value)
{
	ostringstream convert;

	convert << value;
	return convert.str();
}

string StringUtility::toEventString(int eventType)
{
	if(eventType == 0)
		return "Evento::GerarPacote";
	if(eventType == 1)
		return "Evento::EnviarPacote";
	if(eventType == 2)
		return "Evento::ProcessarPacote";
	if(eventType == 3)
		return "Evento::DecrementarRc";

	return "";
}
