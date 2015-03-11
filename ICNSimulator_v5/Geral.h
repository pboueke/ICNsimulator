/*
 * Geral.h
 *
 *  Created on: 01/09/2013
 *      Author: Diego Vargas Jannibelli
 */

#ifndef GERAL_H_
#define GERAL_H_

#define DELETE_PTR(x)			if( x ) delete x; x = NULL				// Anular o ponteiro, deletar se necessário

#define INVALID_VALUE        	0xffff

#define PI						3.1415926535897932384626433832795
#define CONV_GRAU_RADIANO      	(PI/180.0)
#define CONV_RADIANO_GRAU      	(180.0/PI)
#define NUM_MAX_RC_EVAL			100

#include <stdio.h>
#include <stdlib.h>

using namespace std;

typedef struct
{
	unsigned int iUpper;
	unsigned int iLower;
} stLimits;

typedef struct
{
	int iDomain;
	int iType;
	float *params;
} stDistributionParams;

typedef struct
{
	int iType;
	char* sName;
	int iIdent;
	int iDomainIdent;
	int iNumNodesConnectedAbove;
	int *iNodesConnectedAbove;
	int iNumNodesConnectedDomain;
	int *iNodesConnectedDomain;
	int iCacheSize;
	int iReplacementType;
} stNodeParams;

typedef struct
{
	char* sName;
	int iSize;
	int iClassType;
} stContentsParams;

typedef struct
{
	double tStartTotal;
	double tStartQueue;
	double tStartSearch;
	double tStartSending;
	int iTypePckg;
	int iSourceClient;
	int iSourceRouter;
	int iTTLMax;
	int iTTLTag;
	//bool bNextDomain;
	stContentsParams* contentInfo;
} stPackage;

typedef struct
{
	int iClassType;
	double tStartCache;
	bool isInCache;
	double tTimeInCache;
	double dProbability;
	double dRate;
	int iCacheIndex;
} stCacheItem;

class Enums
{
public:
	typedef enum
	{
		CLIENT = 0,
		ROUTER,
		NRS
	}eNodeType;

	typedef enum
	{
		EXP = 0,
		NORMAL,
		UNIFORME,
		ZPIF,
		FIXO
	}eDistType;

	typedef enum
	{
		REQUEST = 0,
		DATA
	}ePackageType;

	typedef enum
	{
		NEW_PACKAGE = 0,
		SEND_PACKAGE,
		RECEIVE_PACKAGE,
		DEC_RC,
	} eEventType;

	typedef enum
	{
		DIST_REQUEST = 0,
		DIST_SEND,
		DIST_RC,
		DIST_RANDOM,
		DIST_CONTENT
	} eEvDistType;

	typedef enum
	{
		ALGORITHM_RC_NO_QUEUE_NO_CACHE_FIN = 0,
		ALGORITHM_RC_NO_QUEUE_CACHE_FIN,
		ALGORITHM_RC_QUEUE_NO_CACHE_FIN,
		ALGORITHM_RC_QUEUE_CACHE_FIN,
		ALGORITHM_PROB,
	}eTypeCacheAlgorithm;

	typedef enum
	{
		REPLACEMENT_RANDOM = 0,
		REPLACEMENTE_RC
	}eTypeRouterCacheReplacement;
};

#endif /* GERAL_H_ */
