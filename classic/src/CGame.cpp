/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CGame.cpp
//
// Desc:
//
//
//-----------------------------------------------------------------------------

#include "OpenZelda_private.h"

#include "CGame.h"
#include "SaveLoadFunctions.h"

#include "Global.h"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern bool			g_fWindowed;
extern float		g_rDelta;
extern int			g_wScreenWidth;
extern int			g_wScreenHeight;
extern float		g_rFPS;
extern bool			g_bShowFPS;


//-----------------------------------------------------------------------------
// Name:
// Desc:
//
//-----------------------------------------------------------------------------
CGame::CGame()
{
	m_pDayNight            = NULL;
	m_pSaveLoad            = NULL;
	m_pWipe                = NULL;
	m_pDisplay            = NULL;
	m_pMap                = NULL;
	m_pGraphics            = NULL;
	m_pSound            = NULL;
	m_pFade                = NULL;
	m_pPauseStack        = NULL;
	m_pFontList            = NULL;
	m_pGameOver            = NULL;
	m_pEntityList        = NULL;
	m_pTextBox            = NULL;
	m_lWorldX            = 0;
	m_lWorldY            = 0;
	m_lNumDynamicAnim    = 0;
	m_lTempWorldX        = 0;
	m_lTempWorldY        = 0;
	m_fLoadingEntities    = false;
	m_rQKeyTimeOut        = 0;
	m_bControlsEnabled    = true;

	// Fonts
	m_pTextBoxFont        = NULL;
	m_pScreenTextFont    = NULL;

	m_cKeys[0]            = 8;
	m_cKeys[1]            = 0;
	m_cKeys[2]            = 0;

	strncpy(m_szFileName, DEFAULT_QUEST, 512);
	strncpy(m_szPlayerName, "Link", 255);

	// Create all Font Objects
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::Initialise() - Calling CreateFonts()");

	m_pFontList = new CFontList();
	m_pTextBoxFont = m_pFontList->CreateFont( 16 );
	m_pScreenTextFont = m_pFontList->CreateFont( 12 );

	LoadingMessage( "Loading", 10.0, 10.0, 10);

	// Format the Restart Table
	ClearRestartTable();
	ResetKeys();

	// Create the Main List of Entities
	m_pEntityList = new CEntityList();

	// Initialise the Animation and Counter Lists
	InitialiseLists();

	FILE * pOut;
	std::string path(FS_Gamedir());
	path.append("testing");

	this->fatalError = !(pOut = fopen(path.c_str(), "wb"));
	fclose(pOut);

}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//
//-----------------------------------------------------------------------------
CGame::~CGame()
{
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::~CGame() - Deleting all Objects");

	SAFE_DELETE( m_pDayNight );
	SAFE_DELETE( m_pDisplay );
	SAFE_DELETE( m_pMap );
	SAFE_DELETE( m_pGraphics );
	SAFE_DELETE( m_pFade );
	SAFE_DELETE( m_pWipe );
	SAFE_DELETE( m_pTextBox );
	SAFE_DELETE( m_pSaveLoad );
	SAFE_DELETE( m_pSound );
	SAFE_DELETE( m_pPauseStack );
	SAFE_DELETE( m_pFontList );
	SAFE_DELETE( m_pGameOver );
	SAFE_DELETE( m_pEntityList );

	// SAFE_DELETEFonts
	SAFE_DELETE( m_pTextBoxFont );
	SAFE_DELETE( m_pScreenTextFont );

	// Free Linked Lists
	m_pAvailableEntities.free();
	m_pAnimList.free();
	m_pCounterList.free();
}

//-----------------------------------------------------------------------------
// Name: CreateLists()
// Desc: Sets up the lists of animations entities etc..
//
//-----------------------------------------------------------------------------
void CGame::InitialiseLists()
{
	// Initialise the animation list
	CreateAnimations();
	CreateCounters();
}

//-----------------------------------------------------------------------------
// Name: Initialise()
// Desc: Should only be called once
//
//-----------------------------------------------------------------------------
bool CGame::Initialise(const char* szCommandLine)
{
	// Create the Pause Stack
	m_pPauseStack = new CPauseStack();

	// Get the Quest name
	if ( strlen(szCommandLine) > 2 )
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Command Line is: %s\n", szCommandLine);
		SetQuestFileName(szCommandLine); // Set Quest name to the one supplied
	}
	else
		SetQuestFileName(DEFAULT_QUEST); // Load the default QST file

	// ================================
	// Allocate Memory for each object
	// ================================
	// Create the Map Object
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::Initialise() - Allocating memory for CQuestLoader Object");
	m_pMap = new CQuestLoader();

	// Load the appropriate quest file
	if (!m_pMap->LoadQSTFile(GetQuestFileName()))
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "Couldn't load Quest from Command Line - switching to default");
		SetQuestFileName(DEFAULT_QUEST);

		// The map file supplied couldn't be found
		m_pMap->LoadQSTFile(DEFAULT_QUEST);
	}

	// Create the DisplayInterface
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::CGame() - Allocating memory for CDisplay object");
	m_pDisplay = new CDisplay();

	// Create the Fading Object
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::CGame() - Allocating memory for CFade Object");
	m_pFade = new CFade();

	// Create the Wiping Object
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::CGame() - Allocating memory for CWipe Object");
	m_pWipe = new CWipe();

	// Create the Day/Night Object
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::CGame() - Allocating memory for CNightDay Object");
	m_pDayNight = new CNightDay;
	ResetKeys();

	// Create the SaveLoad object
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::CGame() - Allocating memory for CSaveLoad object");
	m_pSaveLoad = new CSaveLoad();

	// Create the GameOver object
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::CGame() - Allocating memory for CGameOver object");
	m_pGameOver = new CGameOver();

	// Create Sound object
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::CGame() - Allocating memory for CSound object");
	m_pSound = new CSound();

	// Create the Text Box
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::Initialise() - Allocating memory for CTextBox Object");
	m_pTextBox = new CTextBox();

	// Initialise the new QST file
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::Initialise() - calling CQuestLoader::Initialise()");
	m_pMap->Initialise();

	// If this is a finished Quest then display a Load Screen
	if (m_pMap->isFinished())
	{
		m_fFirstTimeLoad = true;
		m_pSaveLoad->PreLoad();
		m_pSaveLoad->DoSaveLoad();
	}

	return true;
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::Save(FILE* sFile)
{
	int n;

	// =====================
	// Write the Player Name
	// =====================
	WriteSTRING(sFile, "PlayerName", m_szPlayerName);


	// ====================
	// Save all Entities
	// ====================
	m_pEntityList->Save( sFile );

	// ====================
	// Save QuestLoader
	// ====================
	m_pMap->Save(sFile);

	// ======================
	// Save the Restart Table
	// ======================
	WriteSlotHeader(sFile, "[RESTARTTAB]");

	// Put the number of Restart points
	Writeint(sFile, "numrest", MAX_RESTART);
	for (n = 0; n < MAX_RESTART; n++)
	{
		WritelongNoHeader(sFile, m_sRestartTable[n].lStartX);
		WritelongNoHeader(sFile, m_sRestartTable[n].lStartY);
		WriteSTRINGNoHeader(sFile, m_sRestartTable[n].szName);
	}

	// ======================
	// Save the Day/Night
	// ======================
	m_pDayNight->Save(sFile);

}


//-----------------------------------------------------------------------------
// Name: Load()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::Load(FILE* sFile)
{
	int n;
	int m;
	char szTemp[128];
	m_fLoadingEntities = true;

	// =====================
	// Read the Player Name
	// =====================
	ReadSTRING(sFile, "PlayerName", szTemp);

	// Delete all animations
	m_pAnimList.free();

	// Recreate the animations hash table
	CreateAnimations();

	// =====================
	// Load Entities
	// =====================
	m_pEntityList->Load( sFile );

	// ====================
	// Load QuestLoader
	// ====================
	m_pMap->Load(sFile);

	// ======================
	// Save the Restart Table
	// ======================
	FindSlotHeader(sFile, "[RESTARTTAB]");

	// read the number of Restart points
	Readint(sFile, "numrest", m);
	for (n = 0; n < m; n++)
	{
		ReadlongNoHeader(sFile, m_sRestartTable[n].lStartX);
		ReadlongNoHeader(sFile, m_sRestartTable[n].lStartY);
		ReadSTRINGNoHeader(sFile, m_sRestartTable[n].szName);
	}

	// Re-Run all the entities, note that becuase the LoadingEntities
	// Flag is set the entities are not allowed to call certain functions
	// this is to prevent them from duplicating things and wasting memory.
	m_pEntityList->StartEntities();
	m_fLoadingEntities    = false;
	m_bControlsEnabled    = true;

	// ======================
	// Load the Day/Night
	// ======================
	m_pDayNight->Load(sFile);
}


//-----------------------------------------------------------------------------
// Name: EncryptString()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::EncryptString(char *String)
{
	int n,m;

	// Encrypt or Decrypt the String using 3 stages of XOR encryption
	// Go though the string and Decrypt each charater
	for (n = 0; n < 3; n++)
	{
		for (m = 0; m < strlen(String); m++)
		{
			// XOR the string with the keys
			String[m] ^= GetEncryptionKey(n);
		}
	}
}


//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::InitDeviceObjects()
{
	// Create the Set of Sprite Sheets
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::RestoreSurfaces() - Allocating memory for CGraphics object");
	m_pGraphics = new CGraphics();
	m_pGraphics->LoadMasks();

	// We should reload the current screens as well
	if ( m_pMap )
		m_pMap->RestoreScreens();
}


//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::DeleteDeviceObjects()
{

	// Delete the Set of Sprite Sheets if it exists, we
	// will just create it again and reload all images
	delete ( m_pGraphics );

	// Make sure that no old pointers will exist to new sprites
	if (m_pDisplay)
		m_pDisplay->FlushSpriteBuffers();
}

//-----------------------------------------------------------------------------
// Name: RestoreSurfaces()
// Desc: Creates Display and Restores surfaces
//
//-----------------------------------------------------------------------------
bool CGame::RestoreSurfaces()
{
	return true;

	// Delete the Set of Sprite Sheets if it exists, we
	// will just create it again and reload all images
	SAFE_DELETE( m_pGraphics );

	// Invalidate the Font objects
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::RestoreSurfaces() - InvalidateFonts()");
	if (m_pScreenTextFont)
		InvalidateFonts();


	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::RestoreSurfaces() - Calling CDisplay::PostInitialize()");
	m_pDisplay->PostInitialize(g_wScreenWidth, g_wScreenHeight);




	// If the fonts have been initialised already then restore them
	if (m_pScreenTextFont)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::RestoreSurfaces() - Calling RestoreFonts()");
		RestoreFonts();
	}

	// Create the Set of Sprite Sheets
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "CGame::RestoreSurfaces() - Allocating memory for CGraphics object");
	m_pGraphics = new CGraphics();

	return true;
}


//-----------------------------------------------------------------------------
// Name: ResetKeys()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::ResetKeys()
{
	m_fEnterKey    = false;
	m_fLeftKey    = false;
	m_fRightKey    = false;
	m_fUpKey    = false;
	m_fDownKey    = false;
	m_fQKey        = false;
	m_fWKey        = false;
	m_fAKey        = false;
	m_fSKey        = false;
	m_fEKey        = false;
	m_fDKey        = false;
	m_fZKey        = false;
	m_fXKey        = false;
	m_fCKey        = false;
	m_fRKey        = false;
	m_fFKey        = false;
	m_fVKey        = false;
	m_fTKey        = false;
	m_fGKey        = false;
}

//-----------------------------------------------------------------------------
// Name: SetEnableControls()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::SetEnableControls(bool val)
{
	m_bControlsEnabled = val;
}

//-----------------------------------------------------------------------------
// Name: SetRestartTable()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::SetRestartTable(int index, long x, long y, char* szDesc)
{
	if (index < 0 || index >= MAX_RESTART)
		return;

	m_sRestartTable[index].lStartX = x;
	m_sRestartTable[index].lStartY = y;
	strcpy(m_sRestartTable[index].szName, szDesc);
}


//-----------------------------------------------------------------------------
// Name: ClearRestartTable()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::ClearRestartTable()
{
	int n;
	for (n = 0; n < MAX_RESTART; n++)
	{
		m_sRestartTable[n].lStartX = 320;
		m_sRestartTable[n].lStartY = 240;
		strcpy(m_sRestartTable[n].szName, "");
	}
}


//-----------------------------------------------------------------------------
// Name: SetRestartPoint()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::SetRestartPoint(long x, long y)
{
	m_pMap->SetRestartX(x);
	m_pMap->SetRestartY(y);
}


//-----------------------------------------------------------------------------
// Name: RestartFrom()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::RestartFrom(int wIndex)
{
	long x,y;

	m_pFade->FadeTo(255,255,255,100);
	m_pPauseStack->PurgeStack();

	// If the parameter is -1 then start from StartX,StartY
	if (wIndex == -1)
	{
		x = m_pMap->GetRestartX();
		y = m_pMap->GetRestartY();
	}
	else
	{
		x = m_sRestartTable[wIndex].lStartX;
		y = m_sRestartTable[wIndex].lStartY;
	}

	m_pMap->SetRestartX( x );
	m_pMap->SetRestartY( y );

	// Set the Player's Position and facing
	// Try and get a pointer to the player entitiy
	CEntity* pTemp = m_pEntityList->GetPlayerEntity();
	if (pTemp)
	{
		pTemp->SetXPos( x );
		pTemp->SetYPos( y );
		pTemp->SetDirection(SOUTH);
		pTemp->SetState(0);

		UpdateWorldCo(x,y);
		SwapTempWorldCo();
	}

	m_pMap->RestoreScreens();
}
//-----------------------------------------------------------------------------
// Name: CombineColors()
// Desc:
//
//-----------------------------------------------------------------------------
uint32_t CGame::CombineColors(uint32_t dwInputColor)
{
	uint32_t dwOutput;

	// Get each component of the supplied color
	int alpha = GetAAValue(dwInputColor);
	int red = GetARValue(dwInputColor);
	int green = GetAGValue(dwInputColor);
	int blue = GetABValue(dwInputColor);

	if (m_pMap->GetCurrentScreen())
	{
		if ( !m_pMap->GetCurrentScreen()->inside())
		{
			m_pDayNight->CombineColor(&red, 0);
			m_pDayNight->CombineColor(&green, 1);
			m_pDayNight->CombineColor(&blue, 2);
		}
	}

	// Take into account the current fade (if any)
	m_pFade->CombineColor(&red, 0);
	m_pFade->CombineColor(&green, 1);
	m_pFade->CombineColor(&blue, 2);

	dwOutput = ARGB(alpha, red, green, blue);
	return dwOutput;
}

//-----------------------------------------------------------------------------
// Name: WorldToDispX()
// Desc: Converts given world coordinate to a coodinate relative to the Upper-left
//         Point of the actual Screen the player is viewing
//
//-----------------------------------------------------------------------------
float CGame::WorldToDispX(float x)
{
	return x - m_lWorldX;
}


//-----------------------------------------------------------------------------
// Name: WorldToDispY()
// Desc:
//
//-----------------------------------------------------------------------------
float CGame::WorldToDispY(float y)
{
	return y - m_lWorldY;
}


//-----------------------------------------------------------------------------
// Name: GetWidthHeight()
// Desc: Returns the Width or Height of an image in the image list
//
//-----------------------------------------------------------------------------
int CGame::GetWidthHeight(char* szCode, bool fWidth)
{
	if (!m_pGraphics)
		return 1;

	if (szCode == NULL)
		return 1;

	CSprite* pSprite        = NULL;

	// Return the Sprite object with a matching code
	pSprite = m_pGraphics->GetSprite(szCode, false);

	if (pSprite)
	{
		if (fWidth)
			return pSprite->m_wWidth;
		else
			return pSprite->m_wHeight;
	}

	// This shouldn't happen, a correct sprite should always be found
	return 1;
}


//-----------------------------------------------------------------------------
// Name: PutSprite()
// Desc: Places a sprite into the sprite buffers
//
//-----------------------------------------------------------------------------
void CGame::PutSprite(long x, long y, long wDepth, char* szCode,
					 int r, int g, int b, int a, int wScale, int wRot, int nLayer, int flip)
{
	if (!m_pGraphics)
		return;

	CSprite* pSprite = NULL;
	SDL_Color colour;
	colour.r = r;
	colour.b = b;
	colour.g = g;
	colour.a = a;

	// Return the Sprite object with a matching code
	pSprite = m_pGraphics->GetSprite(szCode, false);

	if (pSprite)
	{
		// Check this is on the screen
		if ( x > m_lWorldX + VSCREEN_WIDTH || y > m_lWorldY + VSCREEN_HEIGHT)
			return;

		if ( x + pSprite->m_wWidth < 0 || y + pSprite->m_wHeight < 0)
			return;

		m_pDisplay->PutSprite( (int)WorldToDispX((float)x),
							 (int)WorldToDispY((float)y),
							 pSprite, wDepth, colour, wScale, wRot, nLayer, flip);
	}
}


//-----------------------------------------------------------------------------
// Name: SimpleTileCode()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::SimpleTileCode(long x, long y, long width, long height, char* szCode,
						int r, int g, int b, int a)
{
	if (!m_pGraphics)
		return;

	CSprite * pSprite = NULL;
	SDL_Color color;
	color.r = r;
	color.b = b;
	color.g = g;
	color.a = a;

	SDL_Point dest = {x,y};
	SDL_Point centre;
	centre.x = 0;
	centre.y = 0;

	// Return the Sprite object with a matching code
	pSprite = m_pGraphics->GetSprite(szCode, false);

	if (pSprite)
	{
		Texture * texture = pSprite->GetTexture(0.0);
		m_pDisplay->BlitTiled(texture, &dest, &centre, 1.0, 0.0, width, height, color);
	}
}


//-----------------------------------------------------------------------------
// Name: PutImage()
// Desc: Places a sprite in the image buffers
//
//-----------------------------------------------------------------------------
void CGame::PutImage( long x, long y, char* szCode, int r, int g, int b, int a,
					  int wScale, int wRot, int flip)
{
	if (!m_pGraphics)
		return;

	CSprite * pSprite = NULL;

	SDL_Color color;
	color.r = r;
	color.b = b;
	color.g = g;
	color.a = a;

	// Return the Sprite object with a matching code
	pSprite = m_pGraphics->GetSprite(szCode, false);

	if ( pSprite )
	{
		m_pDisplay->PutImage( x, y, pSprite, color, wScale, wRot, flip);
	}
}


//-----------------------------------------------------------------------------
// Name: SimpleDrawCode()
// Desc: Draws an image Directly to the screen using its sprite code
//
//-----------------------------------------------------------------------------
void CGame::SimpleDrawCode(long x, long y, char* szCode)
{
	if (!m_pGraphics)
		return;

	CSprite * pSprite = NULL;
	SDL_Color color;
	color.r = 255;
	color.b = 255;
	color.g = 255;
	color.a = 255;
	// Return the Sprite object with a matching code
	pSprite = m_pGraphics->GetSprite(szCode, false);

	if (pSprite)
		m_pDisplay->SimpleDraw(x, y, pSprite, color, 1, 0);
}


//-----------------------------------------------------------------------------
// Name: SimpleDrawCode()
// Desc: Draws an image Directly to the screen with RGBA using its sprite code
//
//-----------------------------------------------------------------------------
void CGame::SimpleDrawCode(long x, long y, char* szCode,
						int r, int g, int b, int a)
{
	if (!m_pGraphics)
		return;

	CSprite * pSprite = NULL;
	SDL_Color color;

	color.r = r;
	color.b = b;
	color.g = g;
	color.a = a;

	// Return the Sprite object with a matching code
	pSprite = m_pGraphics->GetSprite(szCode, false);

	if (pSprite)
		m_pDisplay->SimpleDraw(x, y, pSprite, color, 1, 0);
}


//-----------------------------------------------------------------------------
// Name: SimpleDrawCodeLarge()
// Desc: Same as SimpleDrawCode, but everything is 4x larger
//
//-----------------------------------------------------------------------------
void CGame::SimpleDrawCodeLarge(long x, long y, char* szCode,
						int r, int g, int b, int a, int scale)
{
	if (!m_pGraphics)
		return;

	CSprite * pSprite = NULL;
	SDL_Color color;
	color.r = r;
	color.b = b;
	color.g = g;
	color.a = a;
	// Return the Sprite object with a matching code
	pSprite = m_pGraphics->GetSprite(szCode, false);

	if (pSprite)
	{
		m_pDisplay->SimpleDraw(x << 1, y << 1,
								 pSprite, color, scale, 0, 2);
	}
}


//-----------------------------------------------------------------------------
// Name: UpdateWorldCo()
// Desc: Moves the World Coordinates
//
//-----------------------------------------------------------------------------
void CGame::UpdateWorldCo(float x, float y, bool forceUpdate)
{
	CVirtualFile* pTemp1;
	CVirtualFile* pTemp2;
	bool fCheck = false;


	static float LastX = 0;
	static float LastY = 0;

	if (!forceUpdate)
	{
		// Dont set anything while we are wiping
		if (m_pWipe->isWiping() || GetPauseLevel() > 2)
			return;
	}
	else
	{
		LastX = -99;
		LastX = -99;
	}

	// Check if we have moved a significant amount since last time
	if ( ((abs(x - LastX) > 40) || (abs(y - LastY) > 40)) || forceUpdate )
	{
		pTemp1 = m_pMap->ReturnScreen(x, y);
		pTemp2 = m_pMap->ReturnScreen(LastX, LastY);

		// Check if we have crossed Screens
		if (pTemp1 != pTemp2)
		{

			// Get any new boundaries
			m_pMap->GetGroupSet()->GetBoundary(x, y);

			// Get next TempWorldX, TempWorldY coordinates
			ChangeWorldCo(x, y);

			m_lWorldX = m_lTempWorldX;
			m_lWorldY = m_lTempWorldY;

			m_pMap->RestoreScreens();
		}
	}
	else
	{
		ChangeWorldCo(x, y);
	}


/*
	// Check if we have moved a significant amount since last time
	if ( (abs(m_lTempWorldX - m_lWorldX) > 20) || (abs(m_lTempWorldY - m_lWorldY) > 20))
	{
		// We have moved quite a large amount so we should check if we are on a new screen
		pTemp1 = m_pMap->ReturnScreen(m_lTempWorldX, m_lTempWorldY);
		pTemp2 = m_pMap->ReturnScreen(m_lWorldX, m_lWorldX);

		// Check if we have crossed Screens
		if (pTemp1 != pTemp2)
		{
			m_lWorldX = m_lTempWorldX;
			m_lWorldY = m_lTempWorldY;

			// We have moved more than 20 pixels onto a new screen, we
			// didnt walk here so chances are its not loaded properly.
			m_pMap->RestoreScreens();
		}
	}
*/
	// Record x and y for next time
	LastX = x;
	LastY = y;
}

//-----------------------------------------------------------------------------
// Name: ChangeWorldCo()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::ChangeWorldCo(float x, float y)
{
	if (x < m_pMap->GetBounds().left + (VSCREEN_WIDTH / 2))
		m_lTempWorldX = m_pMap->GetBounds().left;
	else
		m_lTempWorldX = x - (VSCREEN_WIDTH / 2);

	if (y < m_pMap->GetBounds().top + (VSCREEN_HEIGHT / 2))
		m_lTempWorldY = m_pMap->GetBounds().top;
	else
		m_lTempWorldY = y - (VSCREEN_HEIGHT / 2);

	if (x > m_pMap->GetBounds().right - (VSCREEN_WIDTH / 2))
		m_lTempWorldX = m_pMap->GetBounds().right - VSCREEN_WIDTH;

	if (y > (m_pMap->GetBounds().bottom - (VSCREEN_HEIGHT / 2)) )
		m_lTempWorldY = m_pMap->GetBounds().bottom - VSCREEN_HEIGHT;
}


//-----------------------------------------------------------------------------
// Name: CheckTempXWorld()
// Desc:
//
//-----------------------------------------------------------------------------
long CGame::CheckTempXWorld( long x )
{
	if (x < m_pMap->GetBounds().left + (VSCREEN_WIDTH / 2))
		return m_pMap->GetBounds().left;
	else if (x > m_pMap->GetBounds().right - (VSCREEN_WIDTH / 2))
		return (m_pMap->GetBounds().right - VSCREEN_WIDTH);
	else
		return (x - (VSCREEN_WIDTH / 2));
}

//-----------------------------------------------------------------------------
// Name: CheckTempYWorld()
// Desc:
//
//-----------------------------------------------------------------------------
long CGame::CheckTempYWorld( long y )
{
	if (y < m_pMap->GetBounds().top + (VSCREEN_HEIGHT / 2))
		return m_pMap->GetBounds().top;
	else if (y > (m_pMap->GetBounds().bottom - (VSCREEN_HEIGHT / 2)) )
		return (m_pMap->GetBounds().bottom - VSCREEN_HEIGHT);
	else
		return( y - (VSCREEN_HEIGHT / 2));
}


//-----------------------------------------------------------------------------
// Name: AddAvailableEntity()
// Desc: Adds an entity to the list of available entities
//
//-----------------------------------------------------------------------------
void CGame::AddAvailableEntity(char* szCode, bool fLibrary)
{
	_Entity* pNewEnt = new _Entity();
	pNewEnt->fLibrary = fLibrary;

	// Fill in a new Entity Structure and add it to the list
	strncpy(pNewEnt->szCode, szCode, MAX_CHARS-1);
	m_pAvailableEntities.additem(pNewEnt, szCode);

	if (fLibrary)
	{
		// If this Entity is a Library Entity then make 1 instance of it
		FindEntity(szCode, -100, -100, szCode, false);
	}
}

void CGame::SetQuestFileName(const char * name)
{
	int len = strlen(name);
	int c = 0;
	int lastslash = 0, lastdot = 0;
	while (c < len)
	{
		if ( name[c] == '\\')
			lastslash = c;
		if ( name[c] == '/')
			lastslash = c;
		if ( name[c] == '.')
			lastdot = c;
		c++;
	}

	memset(m_szFileName, 0, 512);
	strncpy(m_szFileName, name, 512);

	name += lastslash;
	memcpy(m_szName, name, lastdot-lastslash);


}

//-----------------------------------------------------------------------------
// Name: FindEntity()
// Desc:
//
//-----------------------------------------------------------------------------
CEntity* CGame::FindEntity(char *String, long x, long y, char *szIdent, bool fDynamic)
{
	_Entity* pTemp;
	size_t LengthOfCode;
	size_t nCount = 0;
	bool bParam;

	// Search the Available Entities
	if (m_pAvailableEntities.isempty())
		return NULL;

	m_pAvailableEntities.tostart();
	do
	{
		bParam = false;
		pTemp = m_pAvailableEntities.getcurrent();

		// Get the string length of the current entities code
		LengthOfCode = strlen(pTemp->szCode);

		// Check if the current entity has a parameter
		if ( pTemp->szCode[LengthOfCode - 1] == '#')
		{
			bParam = true;
			nCount = LengthOfCode - 1;
		}
		else
		{
			nCount = LengthOfCode;
		}

		// Compare the Code given with the Code of the current Entity
		if (!strncmp( pTemp->szCode, String, nCount) && (LengthOfCode == strlen(String)))
		{
			// Add an entity to the list
			return m_pEntityList->AddEntity( pTemp->fLibrary, x, y, szIdent,
											 fDynamic, bParam, String );
		}

	}while( m_pAvailableEntities.advance() );
	return NULL;
}

//-----------------------------------------------------------------------------
// Name: SwapTempWorldCo()
// Desc:
//
//-----------------------------------------------------------------------------
void CGame::SwapTempWorldCo()
{
	m_lWorldX = m_lTempWorldX;
	m_lWorldY = m_lTempWorldY;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Direct3D Rendering Loop
//
//-----------------------------------------------------------------------------
void CGame::Render()
{
	SDL_Color whi = {255,255,255,128};
	SDL_Color bla = {0,0,0,128};
	SDL_SetRenderDrawColor(GetRendererObject(), 64, 64, 64, 255);
	SDL_RenderClear( GetRendererObject() );
	SetJoyKeys();


	/* Checkerboard Background */
	const static SDL_Rect rects[150] = { \
		{0, 0, 32, 32}, {64, 0, 32, 32}, {128, 0, 32, 32}, {192, 0, 32, 32}, {256, 0, 32, 32}, {320, 0, 32, 32}, {384, 0, 32, 32}, {448, 0, 32, 32}, {512, 0, 32, 32}, {576, 0, 32, 32},\
		{0, 64, 32, 32}, {64, 64, 32, 32}, {128, 64, 32, 32}, {192, 64, 32, 32}, {256, 64, 32, 32}, {320, 64, 32, 32}, {384, 64, 32, 32}, {448, 64, 32, 32}, {512, 64, 32, 32}, {576, 64, 32, 32},\
		{0, 128, 32, 32}, {64, 128, 32, 32}, {128, 128, 32, 32}, {192, 128, 32, 32}, {256, 128, 32, 32}, {320, 128, 32, 32}, {384, 128, 32, 32}, {448, 128, 32, 32}, {512, 128, 32, 32}, {576, 128, 32, 32},\
		{0, 192, 32, 32}, {64, 192, 32, 32}, {128, 192, 32, 32}, {192, 192, 32, 32}, {256, 192, 32, 32}, {320, 192, 32, 32}, {384, 192, 32, 32}, {448, 192, 32, 32}, {512, 192, 32, 32}, {576, 192, 32, 32},\
		{0, 256, 32, 32}, {64, 256, 32, 32}, {128, 256, 32, 32}, {192, 256, 32, 32}, {256, 256, 32, 32}, {320, 256, 32, 32}, {384, 256, 32, 32}, {448, 256, 32, 32}, {512, 256, 32, 32}, {576, 256, 32, 32},\
		{0, 320, 32, 32}, {64, 320, 32, 32}, {128, 320, 32, 32}, {192, 320, 32, 32}, {256, 320, 32, 32}, {320, 320, 32, 32}, {384, 320, 32, 32}, {448, 320, 32, 32}, {512, 320, 32, 32}, {576, 320, 32, 32},\
		{0, 384, 32, 32}, {64, 384, 32, 32}, {128, 384, 32, 32}, {192, 384, 32, 32}, {256, 384, 32, 32}, {320, 384, 32, 32}, {384, 384, 32, 32}, {448, 384, 32, 32}, {512, 384, 32, 32}, {576, 384, 32, 32},\
		{0, 448, 32, 32}, {64, 448, 32, 32}, {128, 448, 32, 32}, {192, 448, 32, 32}, {256, 448, 32, 32}, {320, 448, 32, 32}, {384, 448, 32, 32}, {448, 448, 32, 32}, {512, 448, 32, 32}, {576, 448, 32, 32},\
		{32, 32, 32, 32}, {96, 32, 32, 32}, {160, 32, 32, 32}, {224, 32, 32, 32}, {288, 32, 32, 32}, {352, 32, 32, 32}, {416, 32, 32, 32}, {480, 32, 32, 32}, {544, 32, 32, 32}, {608, 32, 32, 32},\
		{32, 96, 32, 32}, {96, 96, 32, 32}, {160, 96, 32, 32}, {224, 96, 32, 32}, {288, 96, 32, 32}, {352, 96, 32, 32}, {416, 96, 32, 32}, {480, 96, 32, 32}, {544, 96, 32, 32}, {608, 96, 32, 32},\
		{32, 160, 32, 32}, {96, 160, 32, 32}, {160, 160, 32, 32}, {224, 160, 32, 32}, {288, 160, 32, 32}, {352, 160, 32, 32}, {416, 160, 32, 32}, {480, 160, 32, 32}, {544, 160, 32, 32}, {608, 160, 32, 32},\
		{32, 224, 32, 32}, {96, 224, 32, 32}, {160, 224, 32, 32}, {224, 224, 32, 32}, {288, 224, 32, 32}, {352, 224, 32, 32}, {416, 224, 32, 32}, {480, 224, 32, 32}, {544, 224, 32, 32}, {608, 224, 32, 32},\
		{32, 288, 32, 32}, {96, 288, 32, 32}, {160, 288, 32, 32}, {224, 288, 32, 32}, {288, 288, 32, 32}, {352, 288, 32, 32}, {416, 288, 32, 32}, {480, 288, 32, 32}, {544, 288, 32, 32}, {608, 288, 32, 32},\
		{32, 352, 32, 32}, {96, 352, 32, 32}, {160, 352, 32, 32}, {224, 352, 32, 32}, {288, 352, 32, 32}, {352, 352, 32, 32}, {416, 352, 32, 32}, {480, 352, 32, 32}, {544, 352, 32, 32}, {608, 352, 32, 32},\
		{32, 416, 32, 32}, {96, 416, 32, 32}, {160, 416, 32, 32}, {224, 416, 32, 32}, {288, 416, 32, 32}, {352, 416, 32, 32}, {416, 416, 32, 32}, {480, 416, 32, 32}, {544, 416, 32, 32}, {608, 416, 32, 32}\
	};

	SDL_SetRenderDrawColor(GetRendererObject(), 128, 128, 128, 255);
	SDL_RenderFillRects( GetRendererObject(), rects, 150 );


	Update();

	GetFontList()->DrawText( 11, 471, 8, bla, "Classic Edition Beta" );
	GetFontList()->DrawText( 10, 470, 8, whi, "Classic Edition Beta" );


	SDL_RenderPresent( GetRendererObject() );


	return;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Updates and Draws the Next Frame
//
//-----------------------------------------------------------------------------
void CGame::Update()
{

	RECT2D sSrc = {0,0,VSCREEN_WIDTH, VSCREEN_HEIGHT};

	SwapTempWorldCo();

	// Update the Music object
	m_pSound->DoMusic();

	// Update the Fade
	m_pFade->DoFade();

	// Update any wipes
	m_pWipe->DoWipe();

	// Update day/night
	if(m_Inside)
		m_pDayNight->UpdateDayNight();

	// Move all the Counters
	UpdateAllCounters();

	// Reset all the sprites Animations
	if (m_pGraphics)
		m_pGraphics->ResetAnimations();


	// Execute the Screen and Group Scripts
	m_pMap->ExecScripts();

	// Run all the Entity Scripts
	m_pEntityList->RunEntities( m_lWorldX, m_lWorldY );

	// Clear the Collidion Rect List
	m_pMap->ClearCollisionRects();

	// Add Every Entity Collision Rectangle to the scene
	m_pEntityList->AddEntityRects();

	// Check For Scrolling Screens
	m_pMap->CheckScreens();

	// Draw the level 0 sprite buffer
	m_pDisplay->SortSprites(0);//-1 layer images

	// Draw the Ground layers
	m_pMap->DrawScreenLayers(1); //fills and tiles that were placed in the screen

	// Draw the Top layers
	m_pDisplay->SortSprites(1); //layer 0 images
	m_pMap->DrawScreenLayers(2); //ontop tiles like trees

	// Draw the level 1-2 sprite buffers
	m_pDisplay->SortSprites(2); //layer 1+ images
	m_pDisplay->SortSprites(3); //layer 2+ images


	// Update the TextBox
	m_pTextBox->HandleTextBox();

	// Draw all shapes
	m_pDisplay->GetShapeBuffer()->DrawShapes();

	// Draw the Image buffers
	m_pDisplay->DrawImages();

	m_pDisplay->GetTextBuffer()->DrawTexts();
	m_pDisplay->GetBitmapTextBuffer()->DrawTexts();


	if (GetPauseLevel() == 3)
		m_pSaveLoad->DoSaveLoad();

	if (GetPauseLevel() == 4)
		m_pGameOver->DoGameOver();

	// Handle the Key Timeouts
	if (m_rQKeyTimeOut > 0)
		m_rQKeyTimeOut-= 1 * g_rDelta;

}

SDL_Texture * intro_message = NULL;

SDL_Rect intro_message_rect = { 192, 416, 256, 32};

#include "IntroMessage.h"

SDL_Texture * LoadIntroMessage()
{
	SDL_Texture * t = SDL_CreateTexture( GetRendererObject(), SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, 256, 32 );
	SDL_SetTextureBlendMode( t, SDL_BLENDMODE_NONE );
	SDL_UpdateTexture( t, NULL, intro_message_raw, 768);

	return t;
}


void CGame::LoadingMessage(char* message, float x, float y, uint32_t waitTime)
{
	SDL_Color whi = {0, 0, 0,255};
	SDL_Color yel = {255, 215, 0,255};
	if ( intro_message == NULL )
	{
		intro_message = LoadIntroMessage();
	}
	SDL_SetRenderDrawColor( GetRendererObject(), 255,255,255, 255);
	SDL_RenderClear( GetRendererObject() );
	GetFontList()->DrawText( x+12, y+3, 8, whi, message );

	GetFontList()->DrawText( x, y, 8, yel, "\x1e" );
	GetFontList()->DrawText( x-4, y+5, 8, yel, "\x1e" );
	GetFontList()->DrawText( x+4, y+5, 8, yel, "\x1e" );

	if ( intro_message )
	{
		SDL_RenderCopy( GetRendererObject(), intro_message, NULL, &intro_message_rect );
	}

	SDL_RenderPresent( GetRendererObject() );

	if ( waitTime )
		SDL_Delay(waitTime);

}
