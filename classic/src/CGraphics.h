/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CGraphics.h
//
// Desc: Class which encapsulates all Sprite Sheets
//
//-----------------------------------------------------------------------------
#ifndef CGRAPHICS_H
#define CGRAPHICS_H

#include <stdio.h>
#include "CSpriteSheet.h"
#include "LList.h"
#include "Hash.h"

//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CGraphics;

//-----------------------------------------------------------------------------
// Name: class CGraphics
// Desc: Class to house all sprite sheets    
//-----------------------------------------------------------------------------
class CGraphics
{

	LList<CSpriteSheet> m_pSheets;			// Linked List of Sprite Sheets
	int					m_wNumSheets;		// Number of Sprite Sheets
	CHash*				m_pSpriteHashTable; // Hash Table of sprites

public:
    CGraphics();
    ~CGraphics();
	
	void    HashSprites();
	long    CountSprites();
	void	LoadCustomSheets(char* FileName);	// Loads Sprite Sheets from the Current QST
	void	ResetAnimations();
	CSprite* GetSprite(char *szCode, bool fOnlyBackground);
	CSpriteSheet* GetSpriteSheet(CSprite* pSprite);
	void LoadMasks();
};


#endif

