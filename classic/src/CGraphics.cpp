/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CGraphics.cpp
//
// Desc:
//
//
//-----------------------------------------------------------------------------
#define STRICT
#include "CGraphics.h"
#include "CGame.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame*		g_pGame;


//-----------------------------------------------------------------------------
// CGraphics Constructor
//-----------------------------------------------------------------------------
CGraphics::CGraphics()
{
	m_pSpriteHashTable = NULL;

	// Load Graphics from the Current Quest
	if (g_pGame)
		LoadCustomSheets(g_pGame->GetQuestFileName());

}


//-----------------------------------------------------------------------------
// CGraphics destructor
//-----------------------------------------------------------------------------
CGraphics::~CGraphics()
{
	// Clear all Sprite Sheets
	m_pSheets.free();
	SAFE_DELETE( m_pSpriteHashTable );
}


//-----------------------------------------------------------------------------
// Name: LoadCustomSheets()
// Desc:
//
//-----------------------------------------------------------------------------
void CGraphics::LoadCustomSheets(char* FileName)
{
	char szBuffer[128];
	char szBuffer2[128];
	char szBuffer3[128];

	if (!g_pGame->GetMap())
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Error - LoadCustomSheets - Couldn't access archive");
		return;
	}

	// Load the sheets.txt from the quest archive file
	CVirtualArchive* pTemp = g_pGame->GetMap()->GetArchive();
	CVirtualFile* pFile = pTemp->GetSheetsFile();

	if (!pFile)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Error - LoadCustomSheets - Couldn't get sheets.txt");
		return;
	}

	// Clear all Sprite Sheets
	m_pSheets.free();

	// Open the list of sheets
	pFile->Open();

	// Get each line of the file and make a new sprite sheet
	for (;;)
	{

		if (!pFile->ReadString(szBuffer, sizeof(szBuffer)))
			break;

		snprintf(szBuffer2, 127, "%s.spt", szBuffer);
		snprintf(szBuffer3, 127, "%s.bmp", szBuffer);

		pTemp->ExtractFile(szBuffer2, szBuffer2);
		pTemp->ExtractFile(szBuffer3, "sheet.bmp");

		// Create a new Sheet Object and add it to the list
		CSpriteSheet* pTempSheet = new CSpriteSheet("sheet.bmp", szBuffer2, szBuffer3);

		m_pSheets.additem(pTempSheet, szBuffer);
	}

	pFile->Close();


	// Delete the old hash table
	SAFE_DELETE( m_pSpriteHashTable );

	// Create the sprite hash table, make it as big as the number of sprites
	m_pSpriteHashTable = new CHash( CountSprites() );

	// Insert all sprites into the hash table
	HashSprites();
}

//-----------------------------------------------------------------------------
// Name: CountSprites()
// Desc:
//
//-----------------------------------------------------------------------------
long CGraphics::CountSprites()
{
	long lTotal = 0;

	// Go through every sprite and count the sprites
	if (m_pSheets.isempty())
		return 0;

	m_pSheets.tostart();
	do
	{
		lTotal += m_pSheets.getcurrent()->CountSprites();
	}while(m_pSheets.advance());

	// Record the number of sprites in the log
	SDL_Log("Number of avaiable Sprites in Quest: %d", lTotal);
	return lTotal;
}

//-----------------------------------------------------------------------------
// Name: HashSprites()
// Desc:
//
//-----------------------------------------------------------------------------
void CGraphics::HashSprites()
{
	// Go through every sprite and add it to the hash table
	if (m_pSheets.isempty())
		return;

	m_pSheets.tostart();
	do
	{
		m_pSheets.getcurrent()->HashSprites( m_pSpriteHashTable );
	}while(m_pSheets.advance());
}


//-----------------------------------------------------------------------------
// Name: LoadMasks()
// Desc:
//
//-----------------------------------------------------------------------------
void CGraphics::LoadMasks()
{
	if (m_pSheets.isempty())
		return;

	m_pSheets.tostart();
	do
	{
		m_pSheets.SavePosition();
		m_pSheets.getcurrent()->BuildMasks();
		m_pSheets.LoadPosition();

	}while(m_pSheets.advance());

}


//-----------------------------------------------------------------------------
// Name: ResetAnimations()
// Desc:
//
//-----------------------------------------------------------------------------
void CGraphics::ResetAnimations()
{
	// Since all the animated tiles are grouped together onto 1 sheet we
	// only need to re-set the animation flags for all the sprites on that sheet.
	if (m_pSheets.isempty())
		return;

	m_pSheets.tostart();
	do
	{
		if (m_pSheets.getcurrent()->HasAnim())
			m_pSheets.getcurrent()->ResetAnim();

	}while(m_pSheets.advance());
}


//-----------------------------------------------------------------------------
// Name: GetSprite()
// Desc: Returns the Sprite Object with the same code as the one supplied
//		 This can be from any Sheet in the list.
//
//-----------------------------------------------------------------------------
CSprite* CGraphics::GetSprite(char *szCode, bool fOnlyBackground)
{
	// Look in the hash table


	return (CSprite*)m_pSpriteHashTable->LookupString(szCode);
}


//-----------------------------------------------------------------------------
// Name: GetSpriteSheet()
// Desc:
//
//-----------------------------------------------------------------------------
CSpriteSheet* CGraphics::GetSpriteSheet(CSprite* pSprite)
{
	if (!pSprite)
		return NULL;

	return pSprite->m_pParent;
}






