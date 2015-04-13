/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: SaveEntity.cpp
//
// Desc: Functions for Saving/Loading Entity Data
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include "CEntity.h"
#include "SaveLoadFunctions.h"
#include "CGame.h"


extern CGame*		 g_pGame;
//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Saves all Entity Data to the file Specified
//  
//-----------------------------------------------------------------------------
void CEntity::Save(FILE* sFile, char* Ident)
{
	int n;
	WriteSlotHeader(sFile, "[ENTITY]");

	// Save the Code - so we know what script to load
	WriteSTRING(sFile, "m_szCode", m_szCode);

	// Save the Ident - so we can identify it
	WriteSTRING(sFile, "IDENT", Ident);

	// Write the Dynamic flag
	Writeint(sFile, "m_fDynamic", m_fDynamic);

	// Write Collision Rects
	Writeint(sFile, "MAX_COLRECT", MAX_COLRECT);
	for (n = 0; n < MAX_COLRECT; n++)
	{
		WriteintNoHeader(sFile, m_sRect[n].fSolid);
		WritelongNoHeader(sFile, m_sRect[n].sRect.top);
		WritelongNoHeader(sFile, m_sRect[n].sRect.left);
		WritelongNoHeader(sFile, m_sRect[n].sRect.bottom);
		WritelongNoHeader(sFile, m_sRect[n].sRect.right);	
	}

	// Save the Entity Values
	Writeint(sFile, "MAX_VALUES", MAX_VALUES);
	for (n = 0; n < MAX_VALUES; n++)
       WritelongNoHeader(sFile, m_wValue[n]);

	// Save the Entity Strings
	Writeint(sFile, "numstring", m_pStrings.GetCount());
	
	// Make sure we have some strings
	if (!m_pStrings.isempty())
	{
		m_pStrings.tostart();

		// Save the Entity String Length
		Writeint(sFile, "strlen", m_wStringLength);

		// Write each string
		do
		{
			m_pStrings.getcurrent()->Save(sFile);
		}while(m_pStrings.advance());
	}

	Writeint(sFile, "m_fisOpen",  m_fisOpen);
	Writeint(sFile, "m_fisTaken", m_fisTaken);
	Writeint(sFile, "m_fisDead",  m_fisDead);
	Writeint(sFile, "m_fisVisible", m_fisVisible);
	Writeint(sFile, "m_fisActive", m_fisActive);
	Writeint(sFile, "m_fisPickable", m_fisPickable);
	Writeint(sFile, "m_fisPushed", m_fisPushed);
	Writeint(sFile, "m_fisInteracting", m_fisInteracting);
	Writeint(sFile, "m_fisLarge", m_fisLarge);
	Writeint(sFile, "m_fisCuttable", m_fisCuttable);
	Writeint(sFile, "m_fisOwned", m_fisOwned);


	Writelong(sFile, "m_lActiveDist", m_lActiveDist);
	Writefloat(sFile,"m_x", m_x);
	Writefloat(sFile,"m_y", m_y);
	Writelong(sFile,		"m_lInitialX", m_lInitialX); // Should preserve this
	Writelong(sFile,		"m_lInitialY", m_lInitialY);
	
	Writefloat(sFile,	"m_rSpeed", m_rSpeed);
	Writefloat(sFile,	"m_rSpeedMod", m_rSpeedMod);
	Writefloat(sFile,	"m_rMoveAngle", m_rMoveAngle);
	Writeint(sFile,		"m_wDirection", m_wDirection);
	Writeint(sFile,		"m_wHealth", m_wHealth);
	Writeint(sFile,		"m_wMaxHealth", m_wMaxHealth);
	Writeint(sFile,		"m_wType", m_wType);
	Writeint(sFile,		"m_wState", m_wState);
	Writeint(sFile,		"m_wLiftLevel", m_wLiftLevel);
	Writefloat(sFile,	"m_rRespawnCount", m_rRespawnCount);
	Writelong(sFile,	"m_lRespawnTarget", m_lRespawnTarget);
	WriteSTRING(sFile,	"m_szItemString", m_szItemString);
	WriteSTRING(sFile,	"m_szImageString", m_szImageString);
	Writeint(sFile,		"m_wWeight", m_wWeight);
	Writeint(sFile,		"m_wBounce", m_wBounce);
	Writeint(sFile,		"m_wDamage", m_wDamage);


	// Write the Parent's Ident
	if ( m_pParent )
		Writelong(sFile,	"parent", m_pParent->GetCount());
	else
		Writelong(sFile,	"parent", -1);

	// Write the Entities counter value - its index in the list
	Writelong(sFile, "m_lCount", m_lCount);

	// Write the entities parameter
	Writeint(sFile,	 "m_nParam", m_nParam);

	// Write the ActiveInAllGroups boolean value
	Writeint(sFile, "m_bActiveinAll", m_bActiveinAllGroups);
		
	// Save the Entity floate
	Writeint(sFile, "MAX_FLOATS", MAX_FLOATS);
	for (n = 0; n < MAX_FLOATS; n++)
       WritefloatNoHeader(sFile, m_fValue[n]);
}


//-----------------------------------------------------------------------------
// Name: Load()
// Desc: 
//  
//-----------------------------------------------------------------------------
void CEntity::Load(FILE* sFile, char* Ident)
{
	FindSlotHeader(sFile, "[ENTITY]");

	// Read the Code - so we know what script to load
	ReadSTRING(sFile, "m_szCode", m_szCode);

	// Clear the supplied buffer
	memset(Ident, 0, sizeof(Ident));

	// Read the Ident into the buffer supplied
	ReadSTRING(sFile, "IDENT", Ident);

	Readint(sFile, "m_fDynamic", (int &)m_fDynamic);
}


//-----------------------------------------------------------------------------
// Name: Load2()
// Desc: 
//  
//-----------------------------------------------------------------------------
void CEntity::Load2(FILE* sFile)
{
	char szString[24];
	long lCount;
	int n;
	int m;

	// Read Collision Rects
	Readint(sFile, "MAX_COLRECT", m);
	for (n = 0; n < m; n++)
	{
		ReadintNoHeader(sFile, (int&)m_sRect[n].fSolid);
		ReadlongNoHeader(sFile, m_sRect[n].sRect.top);
		ReadlongNoHeader(sFile, m_sRect[n].sRect.left);
		ReadlongNoHeader(sFile, m_sRect[n].sRect.bottom);
		ReadlongNoHeader(sFile, m_sRect[n].sRect.right);	
	}

	// Read the Entity Values
	Readint(sFile, "MAX_VALUES", m);
	for (n = 0; n < m; n++)
       ReadlongNoHeader(sFile,  m_wValue[n]);

	// Read the Entity Strings
	Readint(sFile, "numstring", m);
	
	if (m > 0)
	{
		// Read the String Length
		Readint(sFile, "strlen", m_wStringLength);

		// Allocate Strings
		CreateStrings(m, m_wStringLength);

		// Go through and set each strings message
		if (!m_pStrings.isempty())
		{
			m_pStrings.tostart();

			// Load each string
			do
			{
				m_pStrings.getcurrent()->Load(sFile);
			}while(m_pStrings.advance());
		}
	}

	Readint(sFile, "m_fisOpen",  (int &)m_fisOpen);
	Readint(sFile, "m_fisTaken", (int &)m_fisTaken);
	Readint(sFile, "m_fisDead",  (int &)m_fisDead);
	Readint(sFile, "m_fisVisible", (int &)m_fisVisible);
	Readint(sFile, "m_fisActive", (int &)m_fisActive);
	Readint(sFile, "m_fisPickable", (int &)m_fisPickable);
	Readint(sFile, "m_fisPushed", (int &)m_fisPushed);
	Readint(sFile, "m_fisInteracting", (int &)m_fisInteracting);
	Readint(sFile, "m_fisLarge", (int &)m_fisLarge);
	Readint(sFile, "m_fisCuttable", (int &)m_fisCuttable);
	Readint(sFile, "m_fisOwned", (int &)m_fisOwned);

	Readlong(sFile,     "m_lActiveDist", m_lActiveDist);
	Readfloat(sFile,	"m_x", m_x);
	Readfloat(sFile,	"m_y", m_y);
	Readlong(sFile,		"m_lInitialX", m_lInitialX); // Should preserve this
	Readlong(sFile,		"m_lInitialY", m_lInitialY);
	
	Readfloat(sFile,	"m_rSpeed",    m_rSpeed);
	Readfloat(sFile,	"m_rSpeedMod", m_rSpeedMod);
	Readfloat(sFile,	"m_rMoveAngle",m_rMoveAngle);
	Readint(sFile,		"m_wDirection",m_wDirection);
	Readint(sFile,		"m_wHealth",  m_wHealth);
	Readint(sFile,		"m_wMaxHealth", m_wMaxHealth);
	Readint(sFile,		"m_wType", m_wType);
	Readint(sFile,		"m_wState", m_wState);
	Readint(sFile,		"m_wLiftLevel", m_wLiftLevel);
	Readfloat(sFile,	"m_rRespawnCount", m_rRespawnCount);
	Readlong(sFile,		"m_lRespawnTarget", m_lRespawnTarget);
	ReadSTRING(sFile,	"m_szItemString", m_szItemString);
	ReadSTRING(sFile,	"m_szImageString", m_szImageString);
	Readint(sFile,		"m_wWeight", m_wWeight);
	Readint(sFile,		"m_wBounce", m_wBounce);
	Readint(sFile,		"m_wDamage", m_wDamage);

	// Read the parent long
	Readlong(sFile,	"parent", lCount );

	// Find the parent entity already loaded with a matching count
	if ( lCount >= 0)
	{
		sprintf(szString, "%d", lCount );
		m_pParent = g_pGame->GetEList()->GetEntityWithCount( szString );
	}

	Readlong(sFile, "m_lCount", m_lCount);
	Readint(sFile,	"m_nParam", (int &)m_nParam);
	Readint(sFile, "m_bActiveinAll", (int &)m_bActiveinAllGroups);


	// Read the Entity Floats
	Readint(sFile, "MAX_FLOATS", m);
	for (n = 0; n < m; n++)
       ReadfloatNoHeader(sFile,  m_fValue[n]);
}

