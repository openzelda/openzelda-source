/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CCounter.cpp
//
// Desc: 
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include "CCounter.h"
#include "CGame.h"
#include "Script.h"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame*		 g_pGame;
extern float		 g_rDelta;



//-----------------------------------------------------------------------------
// Name: Constructor
// Desc:
// 
//-----------------------------------------------------------------------------
CCounter::CCounter(char *szIdent, long lMin, long lMax)
{
	// Copy the supplied Identifier
	strcpy(m_szIdent, szIdent);

	// Set some defaults
	m_lMin		= lMin;
	m_lMax		= lMax;
	m_rValue	= 0;
	m_lTarget	= 0;
	m_lSpeed	= 20;
}


//-----------------------------------------------------------------------------
// Name: Destructor
// Desc: 
//
//-----------------------------------------------------------------------------
CCounter::~CCounter()
{
}


//-----------------------------------------------------------------------------
// Name: SetValue()
// Desc: Sets a Counters Value
//
//-----------------------------------------------------------------------------
void CCounter::SetValue(long lVal)
{
	if (lVal < m_lMin)
		lVal = m_lMin;

	if (lVal > m_lMax)
		lVal = m_lMax;

	m_rValue  = lVal;
	m_lTarget = lVal;
}


//-----------------------------------------------------------------------------
// Name: SetTarget()
// Desc: Sets a Counters Target
//
//-----------------------------------------------------------------------------
void CCounter::SetTarget(long lTarg)
{
	if (lTarg < m_lMin)
		lTarg = m_lMin;

	if (lTarg > m_lMax)
		lTarg = m_lMax;

	m_lTarget = lTarg;
}


//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 
//
//-----------------------------------------------------------------------------
void CCounter::Update()
{
	// Move the Actual Value closer to the target
	if ((long)m_rValue != m_lTarget)
	{
		if ((long)m_rValue < m_lTarget)
		{
			// Increment the Value
			m_rValue += m_lSpeed * g_rDelta;
		}

		if ((long)m_rValue > m_lTarget)
		{
			// Decrement the Value
			m_rValue -= m_lSpeed * g_rDelta;	
		}
	}
}


//-----------------------------------------------------------------------------
// Name: GetCounter()
// Desc:
//  
//-----------------------------------------------------------------------------
CCounter* GetCounter(AMX *amx, cell* params)
{
	char szString[ MAX_CHARS ];
	
	// Check we have some parameters
	if (params[0] == 0)
		return NULL;

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);

	// Return a pointer to the correct animtion
    return g_pGame->GetCounterList()->GetObjectFromID(szString);
}


//-----------------------------------------------------------------------------
// Name: CreateCounter(0, 100, ident)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CreateCounter(AMX *amx, cell *params)
{
	char szString[16];
	cell *cptr;

	// Check we have some parameters
	if (params[0] == 0)
		return 0;

	g_pGame->AddCounter(params[1], params[2], szString);

	// Now we want to place the ident of this Animation in the supplied
	// String Buffer
	amx_GetAddr(amx,params[3],&cptr);
    amx_SetString(cptr, szString, 0);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: CreateCounterWithID(0, 200, "MyCount1")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CreateCounterWithID(AMX *amx, cell *params)
{
	char szString[ MAX_CHARS ];

	// Check we have some parameters
	if (params[0] == 0)
		return 0;

	// Get the String Parameter
	GetStringParam(amx, params[3], szString);

	// Add the Counter
	g_pGame->AddCounterID(params[1], params[2], szString);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: SetCounterMin("MyCount", 0)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetCounterMin(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		pTemp->SetMin(params[2]);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: SetCounterMax("MyCount", 100)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetCounterMax(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		pTemp->SetMax(params[2]);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: SetCounterMin("MyCount", 0)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetCounterMin(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		return pTemp->GetMin();
	return -1;	
}

//-----------------------------------------------------------------------------
// Name: SetCounterMax("MyCount", 100)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetCounterMax(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		return pTemp->GetMax();
	return -1;	
}

//-----------------------------------------------------------------------------
// Name: SetCounterValue("MyCount", 50)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetCounterValue(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		pTemp->SetValue(params[2]);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: SetCounterTarget("MyCount", 75)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetCounterTarget(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		pTemp->SetTarget(params[2]);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: IncCounterTarget("MyCount", -5)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL IncCounterTarget(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		pTemp->SetTarget(pTemp->GetTarget() + params[2]);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: IncCounterValue("MyCount", 10)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL IncCounterValue(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		pTemp->SetValue(pTemp->GetValue() + params[2]);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: GetCounterValue("MyCount")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetCounterValue(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		return pTemp->GetValue();
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetCounterSpeed("MyCount", 20)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetCounterSpeed(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		pTemp->SetSpeed(params[2]);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: DeleteCounter()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DeleteCounter(AMX *amx, cell *params)
{
	CCounter* pTemp = GetCounter(amx, params);

	if (pTemp)
		g_pGame->GetCounterList()->removeitem(pTemp);

	return 0;
}


// Define a List of native Animation functions
extern AMX_NATIVE_INFO counter_Natives[] = {
	{ "CreateCounter", CreateCounter },
	{ "CreateCounterWithID", CreateCounterWithID },
	{ "SetCounterMin", SetCounterMin },
	{ "SetCounterMax", SetCounterMax },
	{ "GetCounterMin", GetCounterMin },
	{ "GetCounterMax", GetCounterMax },
	{ "SetCounterValue", SetCounterValue },
	{ "SetCounterTarget", SetCounterTarget },
	{ "IncCounterTarget", IncCounterTarget },
	{ "IncCounterValue", IncCounterValue },
	{ "GetCounterValue", GetCounterValue },
	{ "SetCounterSpeed", SetCounterSpeed },
    { "DeleteCounter", DeleteCounter },
	{ 0, 0 }      
};
