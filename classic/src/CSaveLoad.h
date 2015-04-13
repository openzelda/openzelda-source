/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CSaveLoad.h
//
// Desc: A Class For Saving & Loading
// 
//-----------------------------------------------------------------------------
#ifndef CSAVELOAD_H
#define CSAVELOAD_H
#include "GeneralData.h"
#define MAX_ITEMS 6
//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CSaveLoad;

struct _slot{
	char    szName[32];
	SDL_Point	sMenuPos;
	int		wHealth;
};

//-----------------------------------------------------------------------------
// Name: class CSaveLoad
// Desc: 
//-----------------------------------------------------------------------------
class CSaveLoad
{
	bool	m_isSaving;
	bool	m_isLoading;
	bool	m_isWhereScreen;
	float	m_rCounter;
	int		m_wSelected;

	// Menu position variables
	int		m_wMenuItems;
	_slot	m_sSlots[MAX_ITEMS];

public:
	
    CSaveLoad();
    ~CSaveLoad();
	
	void DoSaveLoad();
	void PreSave();
	void PreLoad();
	void DoSaveGame();
	void DoLoadGame();
	void HandleKeys();
	void FillSaveGameInfo();
	void DrawHearts(int x, int y, int wNumHearts);
	void WriteQuestTitle();
	void DoWhereStart();

	void DrawMat(int x, int y, int width, int height);
	void DrawTileLine(int StartX, int StartY, int Length, int Direction, char* code);
};


#endif

