/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CGroup.cpp
//
// Desc: 
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include "CGroup.h"
#include "GeneralData.h"
#include "CGame.h"
#include "Script.h"
#include "SaveLoadFunctions.h"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame*		g_pGame;


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CGroup::CGroup()
{
	m_pAmx				= NULL;
	m_pCurrentBoundary	= NULL;
	m_pLastBoundary		= NULL;
	m_pProgram			= NULL;
	m_wNumBoundary		= 0;

	// Call The Initialiser
	Initialise();
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CGroup::~CGroup()
{
	SAFE_DELETE(m_pAmx);
	free(m_pProgram);
	m_Boundary.free();
}


//-----------------------------------------------------------------------------
// Name: Initialise()
// Desc:
//  
//-----------------------------------------------------------------------------
void CGroup::Initialise()
{
	int n;

	// Initialise Locals
	for (n = 0; n < MAXLOCAL; n++)
		m_wLocal[n] = 0;
}

//-----------------------------------------------------------------------------
// Name: Save()
// Desc: 
//
//-----------------------------------------------------------------------------
void CGroup::Save(FILE *sfile)
{
	int n;
	// Save all the Important Variables to file
	WriteSlotHeader(sfile, "[GROUP]");

	// Write the number of locals
	Writeint(sfile, "NumLocal", MAXLOCAL);

	// Write the Locals
	for (n = 0; n < MAXLOCAL; n++)
		WriteintNoHeader(sfile, m_wLocal[n]);
}


//-----------------------------------------------------------------------------
// Name: Load()
// Desc: 
//
//-----------------------------------------------------------------------------
void CGroup::Load(FILE *sfile)
{
	int n, m;
	// Load all the Important Variables from file
	if (!FindSlotHeader(sfile, "[GROUP]"))
		return;

	// Read the number of globals
	Readint(sfile, "NumLocal", m);

	// Write the Globals
	for (n = 0; n < m; n++)
		ReadintNoHeader(sfile, m_wLocal[n]);
}


//-----------------------------------------------------------------------------
// Name: ReturnBoundary()
// Desc: Returns a pointer to the boundary in the coordinates
//
//-----------------------------------------------------------------------------
CBoundary* CGroup::ReturnBoundary(long x, long y)
{
	RECT2D sRect;

	// If we have no Boundaries then exit
	if (m_Boundary.isempty())
		return NULL;

	// Check if the position given lies in any known boundary
	m_Boundary.tostart();
	do
	{
		sRect = m_Boundary.getcurrent()->bounds;

		if ( x < sRect.left || x > sRect.right)
			continue;

		if ( y < sRect.top || y > sRect.bottom)
			continue;

		return m_Boundary.getcurrent();
		
	}while( m_Boundary.advance());
	return NULL;
}


//-----------------------------------------------------------------------------
// Name: AddBoundary()
// Desc: 
//
//-----------------------------------------------------------------------------
void CGroup::AddBoundary(CBoundary* pGroup)
{
	if (pGroup)
		m_Boundary.additem(pGroup, "_boundary");
}


//-----------------------------------------------------------------------------
// Name: ChangeMusic()
// Desc: 
//
//-----------------------------------------------------------------------------
void CGroup::ChangeMusic()
{
	// If we dont have the sound object then return
	if (!g_pGame->GetSound())
		return;

	// Just changed groups, see if this new group has any music set, if it has
	// and its different to the current music, play this groups music.
    if (strlen(m_pCurrentBoundary->szMusic) > 2)
	{
		// Check if the two music files have different names
		if (strcmp(m_pCurrentBoundary->szMusic, g_pGame->GetSound()->GetMusicString()))
		{
			// They are different - put the newest music on
			g_pGame->GetSound()->SwitchMusic(m_pCurrentBoundary->szMusic, m_pCurrentBoundary->wLoopBack);
		}
	}
}


//-----------------------------------------------------------------------------
// Name: GetBoundary()
// Desc: Sets the Group for the current location
//
//-----------------------------------------------------------------------------
bool CGroup::GetBoundary(long x, long y)
{
	char CurrentLine[32];
	CBoundary* pOldBoundary = m_pCurrentBoundary;

	// If we have no Boundaries then exit
	if (m_Boundary.isempty())
		return false;

	// Dont try and get a new boundary if we have no music object yet
	if (!g_pGame->GetSound())
		return false;

	// Check if the position given lies in any known boundary
	m_Boundary.tostart();
	do
	{
		if (x > m_Boundary.getcurrent()->bounds.left && x < m_Boundary.getcurrent()->bounds.right)
		{
			if (y > m_Boundary.getcurrent()->bounds.top && y < m_Boundary.getcurrent()->bounds.bottom)
			{
				// Record this as the Current Boundary
				m_pCurrentBoundary = m_Boundary.getcurrent();

				// Set the Min and Max values.
				g_pGame->GetMap()->SetBounds(m_Boundary.getcurrent()->bounds);
				
				if (m_pCurrentBoundary != pOldBoundary)
				{
					// record the last boundary in a member variable
					m_pLastBoundary = pOldBoundary;

					// We have crossed boundaries - now we have to load
					// a new script for the new boundary
					
					// Reset the LOCALs
					for (int l = 0; l < MAXLOCAL; l++)
						m_wLocal[l] = 0;

					// Load a new Group Script
					// Load the new Group Script
					sprintf(CurrentLine, "%d.amx", m_pCurrentBoundary->lScriptIndex);
					
					// Delete the Abstract machine for the Current Script
					SAFE_DELETE( m_pAmx );
						
					// Re-create the AM for a new script
					m_pAmx = new AMX();
					
					// if the program memory has been reserved then free it
					if (m_pProgram)
						free(m_pProgram);
					
					// Load the new script
					m_pProgram = loadprogram(m_pAmx, CurrentLine);
					//RegisterNatives(*m_pAmx);

					// We May need to change music
					ChangeMusic();
				}

				return true;
			}
		}

	}while(m_Boundary.advance());

	// The point lies in no boundary
	return false;
}


//-----------------------------------------------------------------------------
// Name: RunScript()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CGroup::RunScript()
{
	cell ret;
	
	if (m_pAmx)
	{
		cell HeapBefore = m_pAmx->hea;
		amx_Exec(m_pAmx, &ret, AMX_EXEC_MAIN, 0);
		m_pAmx->hea = HeapBefore;
		m_pAmx->firstrun = 0;
	}
}


//-----------------------------------------------------------------------------
// Name: ClearEntityLists()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CGroup::ClearEntityLists()
{
	// If we have no Boundaries then exit
	if (m_Boundary.isempty())
		return;

	m_Boundary.tostart();
	do
	{
		m_Boundary.getcurrent()->ClearEntities();
	}while( m_Boundary.advance() );
}


