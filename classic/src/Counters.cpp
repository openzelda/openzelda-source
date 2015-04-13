/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: Counters.cpp
//
// Desc: 
//       
//
//-----------------------------------------------------------------------------
#define STRICT
#include "CGame.h"

//-----------------------------------------------------------------------------
// Name: CreateCounters()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CGame::CreateCounters()
{
	// Create a hash table for the animation list with a
	// Standard size
	m_pCounterList.CreateHash( 64 );

}

//-----------------------------------------------------------------------------
// Name: AddCounter()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CGame::AddCounter(long lMin, long lMax, char* szString)
{
	// Increment the CreateCounter Call Count
	IncrementDynCounter();

	// Create an Ident Based on the Dynamic Count
	sprintf(szString, "__co%d", GetDynCounterCount());

	// Now Add the Counter Object to the game
	CCounter *pNew = new CCounter(szString, lMin, lMax);
	m_pCounterList.additem(pNew, szString);
}


//-----------------------------------------------------------------------------
// Name: AddCounterID()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CGame::AddCounterID(long lMin, long lMax, char* szString)
{
	CCounter *pNew = new CCounter(szString, lMin, lMax);
	m_pCounterList.additem(pNew, szString);
}


//-----------------------------------------------------------------------------
// Name: UpdateAllCounters()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CGame::UpdateAllCounters()
{
	if (m_pCounterList.isempty())
		return;

	// Move to the Start of the list
	m_pCounterList.tostart();
	do
	{
		m_pCounterList.getcurrent()->Update();
	}while(m_pCounterList.advance());
}