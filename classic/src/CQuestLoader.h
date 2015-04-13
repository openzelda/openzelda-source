/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CQuestLoader.h
//
// Desc: Class which encapsulates loading and displaying QST files
//
//-----------------------------------------------------------------------------
#ifndef CQUESTLOAD_H
#define CQUESTLOAD_H

#define NUM_SCREENS 4
#define MAXGLOBAL 5000
#define MAX_COLLIDE_LIST 1000

#include <stdio.h>
#include "CScreen.h"
#include "CGroup.h"
#include "LList.h"
#include "CVirtualArchive.h"
#include "GeneralData.h"

//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CQuestLoader;
class CVirtualArchive;
class CVirtualFile;
class CScreen;


//-----------------------------------------------------------------------------
// Name: class CQuestLoader
// Desc:  
//-----------------------------------------------------------------------------
class CQuestLoader
{
	CVirtualArchive*		m_pArchive;			// Represents QST file

	// ==================
	// Screen Information
	// ==================
	LList< CVirtualFile >		m_VirtualScreens;				// List of files which hold data on every screen
	CGroup*						m_pGroupSet;					// Holds all the grouping information
	CScreen*					m_pScreenObject[NUM_SCREENS];	// There are 4 possible screen objects active at one time
	CScreen*					m_pCurrentScreen;

	// ======================
	// Boundary Information
	// ======================
	RECT2D	m_sBounds;


	// ======================
	// Misc Quest Information
	// ======================
	char m_szQuestName[256];		// Quest Name
	long m_lPlayerStartX;			// The posistion the Player starts at
	long m_lPlayerStartY;
	long m_lRestartX;				// coordinates to restart from when you die
	long m_lRestartY;
	bool m_fFinishedQuest;
	SDL_Point m_sLowerLevel;

	
	// ======================
	// Collision List
	// ======================
	ColRect	m_sCollideList[MAX_COLLIDE_LIST];	// List of Collision Rectangles
	int		m_wCRects;							// Number of Collision Rects in use

	// ======================
	// Gloabls
	// ======================
	int m_wGlobal[MAXGLOBAL];	
	
	// ===================
	// Script Variables
	// ===================
	AMX*	 m_pAmx;						// The Abstract Machine to run the script
	void*	 m_pProgram;					// Pointer to the location of the script

	// Misc Flags
	bool	m_bJustReloadedAllScreens;		// Called after RestoreScreens - so that a screen
											// Script doesnt called firstrun twice in a row

public:
    CQuestLoader();
    ~CQuestLoader();

	void AddScreen( CVirtualFile* pFile );
	bool LoadQSTFile(char *filename);

	void HandleQSSfile( CVirtualFile* pFile );
	void Initialise();
	bool CheckScreens();
	bool RestoreScreens();
	CVirtualFile* ReturnScreen(long Xpoint, long Ypoint);
	void ExecScripts();
	void ClearCollisionRects(){m_wCRects = 0;}
	bool CheckFile(char *FileName);
	void Save(FILE *sfile);
	void Load(FILE *sfile);
	void CheckBoundaryLimits();
	CScreen* GetScreenOn(long x, long y);
	long WorldToScreenX(long x, long y);
	long WorldToScreenY(long x, long y);
	void AddColisionRect(RECT2D sRect);
	void LoadMainScript();
	void RunMainScript();
	void DrawScreenLayers(int wLayer);
	void CreatePlayerEntity();
	void SetScreenInside( long x, long y, bool bInside );
	bool TestCollisionList(long x, long y);

	// Data Broker Functions
	void SetBounds( RECT2D sRect){ m_sBounds = sRect; }
	void SetRestartX( long val ){m_lRestartX = val;}
	void SetRestartY( long val ){m_lRestartY = val;}
	void SetGlobal(int index, int val){if (index < MAXGLOBAL && index >= 0){m_wGlobal[index] = val;}}
	void SetLowerLevel(long x, long y){m_sLowerLevel.x = x; m_sLowerLevel.y = y;}

	// Access Functions
	RECT2D  GetBounds(){return m_sBounds;}
	CScreen* AccessScreen(int wIndex){return m_pScreenObject[wIndex];}
	long GetPlayerStartX(){return m_lPlayerStartX;}
	long GetPlayerStartY(){return m_lPlayerStartY;}
	long GetRestartX(){ return m_lRestartX;}
	long GetRestartY(){ return m_lRestartY;}
	int	 GetCollisionNum(){return m_wCRects;}
	RECT2D GetCollideRect(int wIndex){return  m_sCollideList[wIndex].sRect;}
	CGroup* GetGroupSet(){return m_pGroupSet;}
	char* GetQuestName(){return m_szQuestName;}
	int GetGlobal(int index){return m_wGlobal[index];}
	bool isFinished(){return m_fFinishedQuest;}
	SDL_Point GetLowerLevel(){return m_sLowerLevel;}
	CVirtualArchive* GetArchive(){ return m_pArchive;}
	CScreen* GetCurrentScreen(){ return m_pCurrentScreen;}

};


#endif

