/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CQuestLoader.cpp
//
// Desc:
//
//
//-----------------------------------------------------------------------------
#define STRICT
#include <stdio.h>
#include "CQuestLoader.h"
#include "GeneralData.h"
#include "CGame.h"
#include "Script.h"
#include "SaveLoadFunctions.h"
#include "OpenZelda_private.h"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame*		 g_pGame;
extern bool		 g_stopScript;
extern uint32_t		 g_dwLastTick;

//-----------------------------------------------------------------------------
// CQuestLoader Constructor
//-----------------------------------------------------------------------------
CQuestLoader::CQuestLoader()
{
	int n;

	// Create the archive object
	m_pArchive = new CVirtualArchive();

	// Create the Screen Objects
	for (n = 0 ; n < NUM_SCREENS ; n++)
		m_pScreenObject[n] = new CScreen();

	// Create the group Set
	m_pGroupSet = new CGroup();

	m_sBounds.left			= 0;
	m_sBounds.right			= 999999999;
	m_sBounds.top			= 0;
	m_sBounds.bottom		= 999999999;

	m_fFinishedQuest		= false;
	m_wCRects				= 0;
	m_pAmx					= NULL;
	m_pProgram				= NULL;
	m_sLowerLevel.x			= 0;
	m_sLowerLevel.y			= 0;
	m_bJustReloadedAllScreens	= true;
	m_pCurrentScreen		= NULL;
}

//-----------------------------------------------------------------------------
// CQuestLoader destructor
//-----------------------------------------------------------------------------
CQuestLoader::~CQuestLoader()
{
	int n;

	// Delete all the screen objects
	for (n = 0 ; n < NUM_SCREENS ; n++)
		SAFE_DELETE(m_pScreenObject[n]);

	SAFE_DELETE(m_pArchive);
	SAFE_DELETE(m_pGroupSet);
	SAFE_DELETE(m_pAmx);
	free(m_pProgram);

	m_VirtualScreens.free(true);
}


//-----------------------------------------------------------------------------
// Name: Initialise()
// Desc: Called When new Quest is Loaded
//
//-----------------------------------------------------------------------------
void CQuestLoader::Initialise()
{
	int n;

	g_pGame->UpdateWorldCo( m_lPlayerStartX, m_lPlayerStartY, true);

	// Reset all globals to 0
	for (n = 0; n < MAXGLOBAL; n++)
		m_wGlobal[n] = 0;

	// Initialise all the loaded Entities
	g_pGame->GetEList()->StartEntities();


	// Reload the screens
	RestoreScreens();
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc:
//
//-----------------------------------------------------------------------------
void CQuestLoader::Save(FILE *sfile)
{
	int n;
	// Save all the Important Variables to file
	WriteSlotHeader(sfile, "[QUESTLOADER]");

	// Write the number of globals
	Writeint(sfile, "NumGlobal", MAXGLOBAL);

	// Write the Globals
	for (n = 0; n < MAXGLOBAL; n++)
		WriteintNoHeader(sfile, m_wGlobal[n]);

	// Write the Lower Level
	Writelong(sfile, "m_sLowerLevelx", m_sLowerLevel.x);
	Writelong(sfile, "m_sLowerLevely", m_sLowerLevel.y);
	Writelong(sfile, "m_lRestartX", m_lRestartX);
	Writelong(sfile, "m_lRestartY", m_lRestartY);

	// Save the Group Info
	m_pGroupSet->Save(sfile);
}


//-----------------------------------------------------------------------------
// Name: Load()
// Desc:
//-----------------------------------------------------------------------------
void CQuestLoader::Load(FILE *sfile)
{
	int n, m;
	// Load all the Important Variables from file
	if (!FindSlotHeader(sfile, "[QUESTLOADER]"))
		return;

	// Read the number of globals
	Readint(sfile, "NumGlobal", m);

	// Read the Globals
	for (n = 0; n < m; n++)
		ReadintNoHeader(sfile, m_wGlobal[n]);

	// Read the Lower Level
	Readlong(sfile, "m_sLowerLevelx", (long&)m_sLowerLevel.x);
	Readlong(sfile, "m_sLowerLevely", (long&)m_sLowerLevel.y);
	Readlong(sfile, "m_lRestartX", m_lRestartX);
	Readlong(sfile, "m_lRestartY", m_lRestartY);

	// Load the Group Info
	m_pGroupSet->Load(sfile);
}


//-----------------------------------------------------------------------------
// Name: RestoreScreens()
// Desc:
//
//-----------------------------------------------------------------------------
bool CQuestLoader::RestoreScreens()
{
	CEntity* pTemp;

	// The graphics object has to be loaded for this to work - as each screen
	// needs to preload its pointers to the sprites.
	if (!g_pGame->GetSSheetSet())
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","*** RestoreScreens() - Sprite Sheets are not loaded - aborting ***");
		return false;
	}

	// Go through all 4 screens and initialise them
	m_pScreenObject[0]->PreLoadScreen(g_pGame->GetWorldX(), g_pGame->GetWorldY());
	m_pScreenObject[1]->PreLoadScreen(g_pGame->GetWorldX() + VSCREEN_WIDTH, g_pGame->GetWorldY());
	m_pScreenObject[2]->PreLoadScreen(g_pGame->GetWorldX(), g_pGame->GetWorldY() + VSCREEN_HEIGHT);
	m_pScreenObject[3]->PreLoadScreen(g_pGame->GetWorldX() + VSCREEN_WIDTH, g_pGame->GetWorldY()+ VSCREEN_HEIGHT);
	m_bJustReloadedAllScreens	= true;

	// Get the boundary the player is in
	pTemp = g_pGame->GetEntityList()->GetObjectFromID("player1");
	if (pTemp)
		m_pGroupSet->GetBoundary(pTemp->GetXPos(), pTemp->GetYPos());

	return true;
}


//-----------------------------------------------------------------------------
// Name: ReturnScreen()
// Desc: Return a pointer to a Screen File with a matching x,y coordinate
//
//-----------------------------------------------------------------------------
CVirtualFile* CQuestLoader::ReturnScreen(long Xpoint, long Ypoint)
{
	// If we have no screens then return
	if (m_VirtualScreens.isempty())
		return NULL;

	long sx;
	long sy;

	m_VirtualScreens.tostart();
	do
	{
		CVirtualFile* pTemp = m_VirtualScreens.getcurrent();

		sx = pTemp->GetParam(0) * VSCREEN_WIDTH;
		sy = pTemp->GetParam(1) * VSCREEN_HEIGHT;

		if ( Xpoint >= sx && Xpoint < sx + VSCREEN_WIDTH)
		{
			if ( Ypoint >= sy && Ypoint < sy + VSCREEN_HEIGHT)
			{
				return pTemp;
			}
		}


		//if (pTemp->GetParam(0) == Tempx && pTemp->GetParam(1) == Tempy )
		//	return pTemp;

	}while(m_VirtualScreens.advance());
	return NULL;
}


//-----------------------------------------------------------------------------
// Name: CheckFile()
// Desc: Checks this is a valid Map File
//
//-----------------------------------------------------------------------------
bool CQuestLoader::CheckFile(char * FileName)
{
	bool bIsFile = false;
	FILE* quest;
	char CurrentLine[16];

	// Open the QST file
	if ( !(quest = fopen(FileName, "r") ))
		return false;
	else
	{
		if (fgets(CurrentLine, sizeof(CurrentLine), quest))
		{
			if (!strstr(CurrentLine, "Virtual Archive"))
			{
				// This is not a quest file
				bIsFile = false;
			}
			else
				bIsFile = true;
		}
		else
			bIsFile = false;
	}

	fclose( quest );
	return bIsFile;
}

//-----------------------------------------------------------------------------
// Name: AddScreen()
// Desc: Adds a new screen to the map
//
//-----------------------------------------------------------------------------
void CQuestLoader::AddScreen( CVirtualFile* pFile )
{
	// Add this file to the list of files that represent LNDs
	m_VirtualScreens.additem( pFile, "LND");
}

//-----------------------------------------------------------------------------
// Name: HandleQSSfile()
// Desc:
//
//-----------------------------------------------------------------------------
void CQuestLoader::HandleQSSfile( CVirtualFile* pFile )
{
	int n;
	char szBuffer[128];
	long lScreens;
	long lGroups;

	long x;
	long y;
	bool bInside;

	pFile->Open();

	// Get the QSS header
	pFile->ReadString(szBuffer, 128);

	// Get Number of screens
	lScreens = pFile->ReadLong();

	// Read data from every screen
	for ( n = 0; n < lScreens; n++ )
	{
		// Read The x and y coordinate
		x = pFile->ReadLong();
		y = pFile->ReadLong();
		bInside = (int)pFile->ReadLong();  // is inside or not?

		if (bInside)
			SetScreenInside( x, y, bInside );
	}

	// Get the number of groups
	lGroups = pFile->ReadLong();
	for ( n = 0; n < lGroups; n++ )
	{
		CBoundary* pNew = new CBoundary();
		pNew->lScriptIndex	= pFile->ReadLong();

		pNew->bounds.left	= (pFile->ReadLong() / 128) * 320;
		pNew->bounds.top	= (pFile->ReadLong() / 96 ) * 240;
		pNew->bounds.right	= (pFile->ReadLong() / 128) * 320;
		pNew->bounds.bottom = (pFile->ReadLong() / 96 ) * 240;
		pNew->wLoopBack		= pFile->ReadLong();
		pFile->ReadString( pNew->szMusic, sizeof( pNew->szMusic ));

		m_pGroupSet->AddBoundary( pNew );
	}


	// Get x and y starting coords
	m_lPlayerStartX = pFile->ReadLong();
	m_lPlayerStartY = pFile->ReadLong();

	// Set the restart info
	m_lRestartX = m_lPlayerStartX;
	m_lRestartY = m_lPlayerStartY;

	// Read the Quest title
	pFile->ReadString( m_szQuestName, sizeof( m_szQuestName ));
	m_fFinishedQuest = pFile->ReadLong();

	pFile->Close();
}

void DisplayMessage(char* message, float x, float y, uint32_t wiat);
//-----------------------------------------------------------------------------
// Name: LoadQSTFile()
// Desc:
//
//-----------------------------------------------------------------------------
bool CQuestLoader::LoadQSTFile(char *filename)
{

	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","********************************************");
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","CQuestLoader::LoadQSTFile() - Loading QST...");

	// Load the files from the archive
	m_pArchive->SetFileName( filename );

	if (!m_pArchive->LoadHeader())
		return false;

	// Extract all AMX files
	if (!m_pArchive->ExtractAMXFiles())
		return false;


	// Load screens
	if (!m_pArchive->LoadScreenFiles( this ))
		return false;

	// Load QSS file
	if (!m_pArchive->LoadQSSFile( this ))
		return false;

	// Load the Main Script
	LoadMainScript();

	// Get the current map boundary
	// ** Taken out 31/05/02 - CSound object doesnt exist yet
	//m_pGroupSet->GetBoundary(m_lPlayerStartX, m_lPlayerStartY);

	// Load Entities
	if (!m_pArchive->LoadEntities())
		return false;

	// Create the Player Entity
	CreatePlayerEntity();
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Player Entity Created.");

	// Check every screen for entities
	m_pArchive->CheckForEntities();

	// Write the total number of entities in log
	SDL_Log("Total Entities Loaded: %d", g_pGame->GetEntityList()->GetCount());

	// Re-Hash the Entity List, by now all the normal non-dynamic entities should
	// Have been loaded so we have a better idea of how big the hash table should be
	g_pGame->GetEList()->ReHashEntities( g_pGame->GetEntityList()->GetCount() + 24 );

	// Initialise the Main Script
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","CQuestLoader::LoadQSTFile() - Running main script:");
	RunMainScript();

	char * m_szQuestName = g_pGame->GetMap()->GetQuestName();
/*
	if ( !strcmp(m_szQuestName, "Untitled Quest") )
		SetWindowText(g_hWnd, m_szQuestName);
	else
		SetWindowText(g_hWnd, PRODUCT_NAME" "PRODUCT_VERSION);
*/
	return true;
}

//-----------------------------------------------------------------------------
// Name: LoadMainScript()
// Desc:
//
//-----------------------------------------------------------------------------
void CQuestLoader::LoadMainScript()
{
	// Delete the Abstract machine for the Current Script
	SAFE_DELETE( m_pAmx )

	// Re-create the AMX for a new script
	m_pAmx = new AMX();

	// if the program memory has been reserved then free it
	if (m_pProgram)
		free(m_pProgram);

	// Load the new script
	m_pProgram = loadprogram(m_pAmx, "main.amx");
	//RegisterNatives(*m_pAmx);
}


//-----------------------------------------------------------------------------
// Name: RunMainScript()
// Desc:
//
//-----------------------------------------------------------------------------
void CQuestLoader::RunMainScript()
{
	// Run the Script in FirstRun Mode
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
// Name: CreatePlayerEntity()
// Desc:
//
//-----------------------------------------------------------------------------
void CQuestLoader::CreatePlayerEntity()
{
	g_pGame->FindEntity("player1", m_lPlayerStartX,
						m_lPlayerStartY, "player1", false);

	g_pGame->SetWorldX((float)m_lPlayerStartX);
	g_pGame->SetWorldY((float)m_lPlayerStartY);
}

//-----------------------------------------------------------------------------
// Name: CheckScreens()
// Desc:
//
//-----------------------------------------------------------------------------
bool CQuestLoader::CheckScreens()
{
	int n;
	long lWorldX = g_pGame->GetWorldX();
	long lWorldY = g_pGame->GetWorldY();
	CVirtualFile* pTemp;

	if (m_VirtualScreens.isempty())
		return false;

	// Scan through all the 4 screens and see which
	// is not being displayed - then re-use it
	// for the next screen

	// This function is quite large, but necessary
	// to ensure proper scrolling of the screens

	for ( n = 0; n < 4 ;n++)
	{
		// Left to Right Scroll
		if ( lWorldX > m_pScreenObject[n]->AccessXPos() + VSCREEN_WIDTH)
		{
			// Find the Next Screen to the right
			m_VirtualScreens.tostart();
			do
			{
				pTemp = m_VirtualScreens.getcurrent();

				// Look for a free screen which is to the right of the current one and has the same
				// Y coordinate as this one.
				if (( pTemp->GetX() * VSCREEN_WIDTH ) > lWorldX &&
					(m_pScreenObject[n]->AccessYPos() == ( pTemp->GetY() * VSCREEN_HEIGHT )) )
				{
					// Now Select the Screen Which is the next one to the left
					if ( (( pTemp->GetX() * VSCREEN_WIDTH) - m_pScreenObject[n]->AccessXPos()) == SCREEN_WIDTH)
					{
						m_pScreenObject[n]->SetCurrentScreen( pTemp );
						m_pScreenObject[n]->LoadScreen( pTemp );

						g_dwLastTick = SDL_GetTicks();
						break;
					}
				}

			}while(m_VirtualScreens.advance());
		}

		// Right to Left scroll
		if ( lWorldX < m_pScreenObject[n]->AccessXPos() - VSCREEN_WIDTH)
		{
			// Find the Next Screen to the right
			m_VirtualScreens.tostart();
			do
			{
				pTemp = m_VirtualScreens.getcurrent();

				// Look for a free screen which is to the right of the current one and has the same
				// Y coordinate as this one.
				if (( pTemp->GetX() * VSCREEN_WIDTH) < lWorldX &&
					(m_pScreenObject[n]->AccessYPos() == ( pTemp->GetY() * VSCREEN_HEIGHT)) )
				{
					// Now Select the Screen Which is the next one to the left
					if ( (m_pScreenObject[n]->AccessXPos() - ( pTemp->GetX() * VSCREEN_WIDTH)) == SCREEN_WIDTH)
					{
						m_pScreenObject[n]->SetCurrentScreen( pTemp );
						m_pScreenObject[n]->LoadScreen( pTemp );

						g_dwLastTick = SDL_GetTicks();
						break;
					}
				}

			}while(m_VirtualScreens.advance());
		}

		// Top to Bottom scroll
		if (lWorldY >= m_pScreenObject[n]->AccessYPos() + VSCREEN_HEIGHT)
		{
			// Find the Next Screen to the right
			m_VirtualScreens.tostart();
			do
			{
				pTemp = m_VirtualScreens.getcurrent();

				// Look for a free screen which is to the right of the current one and has the same
				// Y coordinate as this one.
				if (( pTemp->GetY() * VSCREEN_HEIGHT) >= lWorldY &&
					(m_pScreenObject[n]->AccessXPos() == ( pTemp->GetX() * VSCREEN_WIDTH)) )
				{
					// Now Select the Screen Which is the next one to the left
					if ((( pTemp->GetY() * VSCREEN_HEIGHT) - m_pScreenObject[n]->AccessYPos()) == SCREEN_HEIGHT)
					{
						m_pScreenObject[n]->SetCurrentScreen( pTemp );
						m_pScreenObject[n]->LoadScreen(pTemp );
						g_dwLastTick = SDL_GetTicks();
						break;
					}
				}

			}while(m_VirtualScreens.advance());
		}

		// Bottom to Top scroll
		if ( lWorldY < m_pScreenObject[n]->AccessYPos() - VSCREEN_HEIGHT)
		{
			// Find the Next Screen to the right
			m_VirtualScreens.tostart();
			do
			{
				pTemp = m_VirtualScreens.getcurrent();

				// Look for a free screen which is to the right of the current one and has the same
				// Y coordinate as this one.
				if (( pTemp->GetY() * VSCREEN_HEIGHT) < lWorldY &&
					(m_pScreenObject[n]->AccessXPos() == ( pTemp->GetX() * VSCREEN_WIDTH)) )
				{
					// Now Select the Screen Which is the next one to the left
					if ((m_pScreenObject[n]->AccessYPos() - ( pTemp->GetY() * VSCREEN_HEIGHT) ) == SCREEN_HEIGHT)
					{
						m_pScreenObject[n]->SetCurrentScreen( pTemp );
						m_pScreenObject[n]->LoadScreen(pTemp);
						g_dwLastTick = SDL_GetTicks();
						break;
					}
				}

			}while(m_VirtualScreens.advance());
		}
	}
	return false;
}


//-----------------------------------------------------------------------------
// Name: ExecScripts()
// Desc: Runs the Scripts for the Screens, Group etc..
//
//-----------------------------------------------------------------------------
void CQuestLoader::ExecScripts()
{
	static CScreen* pOldScreen = NULL;

	// Run the main Script
	RunMainScript();

	// Work out which screen the Player is on
	CScreen* pTemp = GetScreenOn(g_pGame->GetWorldX() + VSCREEN_WIDTH  / 2,
								 g_pGame->GetWorldY() + VSCREEN_HEIGHT / 2);

	// Record the current screen as a member variable
	m_pCurrentScreen = pTemp;


	// If we have just realoaded all the screens then its gonna think we just crossed a
	// screen, but in fact we just loaded all new ones.
	if (m_bJustReloadedAllScreens)
	{
		m_bJustReloadedAllScreens	= false;
		pOldScreen = NULL;
	}

	if ( pTemp && pTemp != pOldScreen && pOldScreen)
	{
		// Stands to reason that we have just crossed to another screen script
		pTemp->GetScript()->firstrun = true;
	}


	if (pTemp)
	{
		if (pTemp->GetScript())
			pTemp->RunScript();
	}

	// Run the Group Script
	if (m_pGroupSet)
	{
		if (m_pGroupSet->GetScript())
			m_pGroupSet->RunScript();
	}

	pOldScreen = pTemp;
}


//-----------------------------------------------------------------------------
// Name: DrawScreenLayers()
// Desc:
//
//-----------------------------------------------------------------------------
void CQuestLoader::DrawScreenLayers(int wLayer)
{
	int n;

	for (n = 0; n < NUM_SCREENS; n++)
	{
		if (m_pScreenObject[n]->inUse())
		{
			m_pScreenObject[n]->DrawLayer(wLayer);
		}
	}
}


//-----------------------------------------------------------------------------
// Name: WorldToScreenX()
// Desc: Converts given world coordinate to a coodinate relative to the Upper-left
//		 Point of the Screen it is on.
//
//-----------------------------------------------------------------------------
long CQuestLoader::WorldToScreenX(long x, long y)
{
	// Get the Screen this point is on
	CScreen* pTemp = GetScreenOn(x, y);

	if (!pTemp)
		return -1;

	return x - pTemp->AccessXPos();
}

//-----------------------------------------------------------------------------
// Name: WorldToScreenY()
// Desc: Converts given world coordinate to a coodinate relative to the Upper-left
//		 Point of the Screen it is on.
//
//-----------------------------------------------------------------------------
long CQuestLoader::WorldToScreenY(long x, long y)
{
	// Get the Screen this point is on
	CScreen* pTemp = GetScreenOn(x, y);

	if (!pTemp)
		return -1;

	return y - pTemp->AccessYPos();
}


//-----------------------------------------------------------------------------
// Name: GetScreenOn()
// Desc: Returns a pointer to the CScreen object the given coordinates are on
//
//-----------------------------------------------------------------------------
CScreen* CQuestLoader::GetScreenOn(long x, long y)
{
	int n;

	for (n = 0 ;n < NUM_SCREENS; n++)
	{
		if (x >= m_pScreenObject[n]->AccessXPos() && y >= m_pScreenObject[n]->AccessYPos())
		{
			if (x < m_pScreenObject[n]->AccessXPos() + VSCREEN_WIDTH
				&& y < m_pScreenObject[n]->AccessYPos() + VSCREEN_HEIGHT)
				return m_pScreenObject[n];
		}
	}

	return NULL;
}


//-----------------------------------------------------------------------------
// Name: AddColisionRect()
// Desc: Adds a Collision Rectangle to the scene
//
//-----------------------------------------------------------------------------
void CQuestLoader::AddColisionRect(RECT2D sRect)
{
	if (m_wCRects < MAX_COLLIDE_LIST)
	{
		m_sCollideList[m_wCRects].sRect = sRect;
		m_wCRects++;
	}
}


//-----------------------------------------------------------------------------
// Name: SetScreenInside()
// Desc:
//
//-----------------------------------------------------------------------------
void CQuestLoader::SetScreenInside( long x, long y, bool bInside )
{
	CVirtualFile* pCurrent;

	if (m_VirtualScreens.isempty())
		return;

	// Go through all virtual screens and find a matching one
	m_VirtualScreens.tostart();
	do
	{
		pCurrent = m_VirtualScreens.getcurrent();

		if ( pCurrent->GetX() == x && pCurrent->GetY() == y )
		{
			pCurrent->SetParam(2, (long)bInside);
			return;
		}

	}while( m_VirtualScreens.advance() );

}


//-----------------------------------------------------------------------------
// Name: TestCollisionList()
// Desc:
//
//-----------------------------------------------------------------------------
bool CQuestLoader::TestCollisionList(long x, long y)
{
	int n;
	RECT2D sRect;

	// Loop Through every Collision Rect
	for (n = 0; n < m_wCRects; n++)
	{
		sRect = m_sCollideList[n].sRect;
		if (PointInRect(x, y, sRect))
			return true;
	}

	return false;
}
















