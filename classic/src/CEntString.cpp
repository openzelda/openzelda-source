/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CEntString.cpp
//
// Desc: 
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include "CEntString.h"
#include "SaveLoadFunctions.h"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CEntString::CEntString(int wMessageSize)
{
	int n;
	
	// Allocate Memory for String
	pMessage = new char[wMessageSize];

	// Clear the String
	pMessage[0] = 0; // Modified by luke - apr 2010

	// Set some Default Values
	wNumNext	= 0;
	fGotoNext	= false;

	for (n = 0; n < MAX_ANSWERS; n++)
		cNextMessage[n] = 0;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CEntString::~CEntString()
{
	// Delete String
	delete pMessage;
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc:
//  
//-----------------------------------------------------------------------------
void CEntString::Save(FILE* sFile)
{
	int n;
	
	// Write the String
	WriteSTRING(sFile, "message", GetMessage());

	// Write the next strings
	for (n = 0; n < MAX_ANSWERS; n++)
		WriteintNoHeader(sFile, GetNextMessage(n));

	Writeint(sFile, "NumNext", GetNumNext());
	Writeint(sFile, "fGotoNext", GetGotoNext());
}


//-----------------------------------------------------------------------------
// Name: Load()
// Desc:
//  
//-----------------------------------------------------------------------------
void CEntString::Load(FILE* sFile)
{
	int n;
	
	// Read the String
	ReadSTRING(sFile, "message", pMessage);

	// Write the next strings
	for (n = 0; n < MAX_ANSWERS; n++)
		ReadintNoHeader(sFile, (int &)cNextMessage[n]);

	Readint(sFile, "NumNext", wNumNext);
	Readint(sFile, "fGotoNext", (int &)fGotoNext);
}


//-----------------------------------------------------------------------------
// Name: GetNextMessage()
// Desc:
//  
//-----------------------------------------------------------------------------
int CEntString::GetNextMessage(int index)
{
	if (index < 0)
		index = 0;
	if (index >= MAX_ANSWERS)
		index = MAX_ANSWERS - 1;
	
	return (int)cNextMessage[index];
}


//-----------------------------------------------------------------------------
// Name: SetNextMessage()
// Desc:
//  
//-----------------------------------------------------------------------------
void CEntString::SetNextMessage(int wIndex, int wVal)
{
	if (wIndex < 0)
		wIndex = 0;
	if (wIndex >= MAX_ANSWERS)
		wIndex = MAX_ANSWERS - 1;

	cNextMessage[wIndex] = wVal;
}
