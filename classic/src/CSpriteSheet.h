/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CSpriteSheet.h
//
// Desc: Loads Sprite Sheets and holds the data about them
//       each sprite sheet comes with a config file (.spt) with
//       info on each individual sprite.
//
//-----------------------------------------------------------------------------
#ifndef CSPRITESHEET_H
#define CSPRITESHEET_H

#include <SDL2/SDL.h>
#include "GeneralData.h"
#include "CSprite.h"
#include "LList.h"
#include "Hash.h"


#define DXTEST(expr) expr


//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CSpriteSheet;
class CSprite;


//-----------------------------------------------------------------------------
// Name: class CSpriteSheet
// Desc: Holds data on a sprite sheet 
//-----------------------------------------------------------------------------
class CSpriteSheet
{
	SDL_Surface * m_lpSurface;   // Surface used for loading textures off

	char	   m_szSPTName[128];	// The name of the SPT file which goes with this sheet
	char	   m_szBitmapName[128];	// The name for this sheets bitmap 
	char	   m_szRealName[128];	// The name for this sheets bitmap 
	int		   m_wNumSprites;		// The Number of Sprites in this sheet;
	int		   m_wWidth;			// Dimensions of the Sheet
	int		   m_wHeight;
	bool	   m_fBackGround;		// Does this sheet contain BackGround tiles?
	bool	   m_fHasAnims;			// Does this Sheet have any animated tiles?

	LList<CSprite> m_pSprites;		// Linked List of Sprites

public:

    CSpriteSheet(char* BitMapName, char *SPTname, char * realName);
    ~CSpriteSheet();

	int32_t LoadBitmap();			// Loads bitmap into CSurface
    int		LoadSPT();				// Loads SPT file and creates CSprite objects
	CSprite* GetSprite(char *szCode, bool fOnlyBackground);
	void    ResetAnim();			// Reset all the anim flags of the sprites
	void    BuildMasks();
	void    HashSprites( CHash* pHash );
	long    CountSprites();

	// Access Functions
	bool GetBackground(){return m_fBackGround;}
	int GetWidth() {return m_wWidth;}
	int GetHeight(){return m_wHeight;}
	bool HasAnim(){return m_fHasAnims;}

};


#endif

