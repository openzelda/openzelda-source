/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CScreen.h
//
// Desc: Represents a single sreen in a Quest
//
//-----------------------------------------------------------------------------
#ifndef CSCREEN_H
#define CSCREEN_H

#include "GeneralData.h"
#include "CSpriteSheet.h"
#include "CVirtualFile.h"
#include "LList.h"
#include "AMX/amx.h"

class CVirtualFile;

#define MAX_HOLES        300
#define MAX_SPRITES        800
#define MAX_FILLS        800
#define MASKSIZE        77000


// Structures used in the LND file format
// Sprite Structure
typedef struct _sprite{
	int X;
	int Y;
	CSprite* pSprite;        // Pointer to the sprite to draw
}Sprite;


// Fill Structure
typedef struct _fill{
	CSprite* pSprite;        // Pointer to the sprite to draw
	RECT2D FillRect;
}Fill;

// Structure for holding info on Holes
typedef struct _Hole
{
	char    Type;
	RECT2D    holeRect;
}Hole;

class MaskChar {
	unsigned char v8;
	unsigned char v5;
	unsigned char value();
	void setValue( unsigned char value );
};



//-----------------------------------------------------------------------------
// Classes defined in this header file
//-----------------------------------------------------------------------------
class CScreen;


//-----------------------------------------------------------------------------
// Name: class CScreen
// Desc:
//
//-----------------------------------------------------------------------------
class CScreen
{
	MaskChar     m_mask[MASKSIZE];    // The collision detection mask for this screen
	long            m_x;                // The X position of the screen
	long            m_y;                // The Y position of the screen
	bool            m_inUse;
	bool            m_fInside;

	long            m_lNumSprites;                    // num of Sprites on screen
	Sprite            m_Sprites[MAX_SPRITES];            // Array of Sprites

	long            m_lFills;
	Fill            m_Fills[MAX_FILLS];                // Array of fills

	long            m_lNumHoles;                    // Num of holes on screen
	Hole            m_Holes[MAX_HOLES];                // Array of Holes

	CVirtualFile*    m_pCurrentScreen;    // The File this screen references

	// ===================
	// Script Variables
	// ===================
	AMX*     m_pAmx;                        // The Abstract Machine to run the script
	void*     m_pProgram;                    // Pointer to the location of the script

public:

	CScreen();
	~CScreen();

	void Initialise();
	bool PreLoadScreen(long Xp, long Yp);
	bool LoadScreen( CVirtualFile* pFile);
	void DrawLayers();
	bool CompareAndCopy(CSprite* pSprite, int wIndex, SDL_Color dwBaseCol );
	bool CompareAndCopyFill(CSprite* pSprite, int wIndex, RECT2D sFill, SDL_Color dwBaseCol );
	void CopySpriteMask(int x, int y, CSprite* pSprite);
	void FillMask(int x, int y, int w, int h, unsigned char c);
	void DisplayMask();
	void ClearMask();
	bool CheckMask(int x, int y);
	void AddHole(RECT2D rect, int Type);
	int CheckForHoleRECT(RECT2D rect);
	int CheckForHoleXY(long x, long y);
	int GetMaskValue(long lIndex);
	bool DrawLayer(int wLayer);
	bool LoadScript();
	void LoadMask();
	void RunScript();

	// Access Functions
	int AccessXPos(){return m_x;}
	int AccessYPos(){return m_y;}
	AMX* GetScript(){ return m_pAmx; }
	bool inUse(){return m_inUse;}
	bool inside(){ return m_fInside;}

	// Data Broker
	void SetCurrentScreen(CVirtualFile* pFile){ m_pCurrentScreen = pFile; }

};


#endif
