/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CSaveLoad.cpp
//
// Desc: A Class For Saving & Loading
// 
//-----------------------------------------------------------------------------
#define STRICT

#include "CSaveLoad.h"
#include "CGame.h"
#include "SaveLoadFunctions.h"

#define TEXT_COL 0xFF3F2F1D

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define MAX_HEARTS  16
extern CGame*		g_pGame;
extern float		g_rDelta;


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CSaveLoad::CSaveLoad()
{
	m_isSaving		= false;
	m_isLoading		= false;
	m_wMenuItems	= 5;
	m_rCounter		= -1;
	m_wSelected		= 0;
	m_isWhereScreen	= false;
}


//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CSaveLoad::~CSaveLoad()
{
}

//-----------------------------------------------------------------------------
// Name: FillSaveGameInfo()
// Desc: Fills the Save/Load structure with info about saved games
//
//-----------------------------------------------------------------------------
void CSaveLoad::FillSaveGameInfo()
{
	
	FILE *sfile;
	int n = 0;
	char FileName[512];
	char CurrentLine[32];
	
	// Get the filename of the save file
	GetSaveFileName(FileName);

	// Reset all the savegame structures
	for(n = 0; n < MAX_ITEMS; n++)
	{
		m_sSlots[n].wHealth = 0;
		strcpy(m_sSlots[n].szName, "Unused");
	}
	std::string path(FS_Gamedir());
	path.append(FileName);
	// Attempt to open the save file
	if ( !(sfile = fopen(path.c_str(), "r+")))
	{
		// Create the save game and exit
		CreateSaveFile(FileName);
		return;
	}

	n = -1;
	// Now Loop through until we find the player entity player1
	for (;;)
	{
		// Go to the next Slot
		if (FindSlotHeader(sfile, "[SLOT]" ))
			n++;
		else
			break;
		
		// Search for the Player Entity
		if(fgetsEx("PlayerName", CurrentLine, sfile))
		{
			// Copy the Saved Name
			strcpy(m_sSlots[n].szName, CurrentLine);
			
			// Search for the Player Entity
			if(!fgetsEx("player1", CurrentLine, sfile))
				break;
			
			if(!fgetsEx("m_wMaxHealth", CurrentLine, sfile))
				break;
			
			// Copy the Health Value
			m_sSlots[n].wHealth = atoi(CurrentLine);
		}
	}

	fclose( sfile );
}


//-----------------------------------------------------------------------------
// Name: DoSaveLoad()
// Desc: 
//
//-----------------------------------------------------------------------------
void CSaveLoad::DoSaveLoad()
{
	if (m_isSaving)
		DoSaveGame();
	
	if (m_isLoading)
		DoLoadGame();

	if (m_isWhereScreen)
		DoWhereStart();
}


//-----------------------------------------------------------------------------
// Name: PreSave()
// Desc: Prepares for saving the game
//
//-----------------------------------------------------------------------------
void CSaveLoad::PreSave()
{
///	if ( g_pGame->GetWipe()->isWiping())
//		return;

	if (m_isSaving || m_isLoading || m_isWhereScreen)
		return;

	m_wSelected			= 0;
	m_rCounter			= -1;
	m_isSaving			= true;
	g_pGame->SetPauseLevel(3, -1, NULL);
	FillSaveGameInfo();
}


//-----------------------------------------------------------------------------
// Name: DoSaveGame()
// Desc: 
//
//-----------------------------------------------------------------------------
void CSaveLoad::DoSaveGame()
{
	int n;
	int menuX = 70;
	int menuY = 36;
	
	// Draw the mat in the background
	DrawMat(24,16,32,24);

	WriteQuestTitle();

	// Write "SAVE GAME"
	//g_pGame->GetTextBoxFont()->DrawText( 75, 55, TEXT_COL, "Save Game");

	m_wMenuItems = 5;

	// Set the positions for the menu items
	for (n = 0; n < m_wMenuItems; n++)
	{
		m_sSlots[n].sMenuPos.x = menuX;
		m_sSlots[n].sMenuPos.y = menuY;
		menuY += 15;
	}

	// =================
    // Draw icons
	// =================
	for (n = 0 ;n < m_wMenuItems - 1; n++)
	{
		// Draw Pots
		g_pGame->SimpleDrawCodeLarge(m_sSlots[n].sMenuPos.x * 2, 
			                         m_sSlots[n].sMenuPos.y * 2, 
									 "_save01", 255,255,255,255, 2);

		g_pGame->SimpleDrawCodeLarge(155, 
			                         (m_sSlots[n].sMenuPos.y * 2) + 19, 
									 "_save14", 255,255,255,255, 1);

		if (m_sSlots[n].wHealth != 0)
			DrawHearts((m_sSlots[n].sMenuPos.x * 2) + 95, 
			           (m_sSlots[n].sMenuPos.y * 2) + 4, m_sSlots[n].wHealth);
/*
		g_pGame->GetTextBoxFont()->DrawText( (m_sSlots[n].sMenuPos.x * 4) + 38, 
			                                 (m_sSlots[n].sMenuPos.y * 4) + 8, TEXT_COL, m_sSlots[n].szName);*/
	}

	// ===============================
	// Draw a Skull as the last item
	// ===============================
	g_pGame->SimpleDrawCodeLarge(m_sSlots[m_wMenuItems - 1].sMenuPos.x * 2, 
			                     m_sSlots[m_wMenuItems - 1].sMenuPos.y * 2, 
							     "_save02", 255,255,255,255, 2);

	// Write Cancel
/*	g_pGame->GetTextBoxFont()->DrawText( (m_sSlots[m_wMenuItems - 1].sMenuPos.x * 4) + 38, 
			                             (m_sSlots[m_wMenuItems - 1].sMenuPos.y * 4) + 8, TEXT_COL, "Cancel");*/

	// Enter Key has been pressed
	if (m_rCounter != -1)
	{
		// Draw player holding
		g_pGame->SimpleDrawCodeLarge((m_sSlots[m_wSelected].sMenuPos.x * 2) - 24, 
									 (m_sSlots[m_wSelected].sMenuPos.y * 2) - 4, 
									 "_save13", 255,255,255,255, 2);
		m_rCounter += g_rDelta;

		// The CountDown is complete!
		if (m_rCounter > 1)
		{
			m_rCounter = -1;

			// Select a menu item
			if (m_wSelected == m_wMenuItems - 1)
			{
				m_isSaving = false;
				g_pGame->SetPauseLevel(0, -1, NULL);
			}
			if (m_wSelected < m_wMenuItems - 1)
			{
				SaveTheGame(m_wSelected);
				m_isSaving = false;
				g_pGame->SetPauseLevel(0, -1, NULL);
			}		
		}
	}
	else
	{
		// Draw the Player
		g_pGame->SimpleDrawCodeLarge((m_sSlots[m_wSelected].sMenuPos.x * 2) - 24, 
									 (m_sSlots[m_wSelected].sMenuPos.y * 2) - 4, 
									 "_save12", 255,255,255,255, 2);
	}

	// ==================
	// Handle Key Input
	// ==================
	HandleKeys();
}


//-----------------------------------------------------------------------------
// Name: PreLoad()
// Desc: Prepares for Loading the game
//
//-----------------------------------------------------------------------------
void CSaveLoad::PreLoad()
{
//	if ( g_pGame->GetWipe()->isWiping())
//		return;
	
	if (m_isSaving || m_isLoading || m_isWhereScreen)
		return;

	m_wSelected			= 0;
	m_rCounter			= -1;
	m_isLoading			= true;
	g_pGame->SetPauseLevel(3, -1, NULL);
	FillSaveGameInfo();
}


//-----------------------------------------------------------------------------
// Name: DoLoadGame()
// Desc: 
//
//-----------------------------------------------------------------------------
void CSaveLoad::DoLoadGame()
{
	int n;
	int menuX = 70;
	int menuY = 36;
	
	// Draw the mat in the background
	DrawMat(24,16,32,24);

	WriteQuestTitle();

	// Write "LOAD GAME"
	//g_pGame->GetTextBoxFont()->DrawText( 75, 55, TEXT_COL, "Load Game");

	m_wMenuItems = 5;

	// Set the positions for the menu items
	for (n = 0; n < m_wMenuItems; n++)
	{
		m_sSlots[n].sMenuPos.x = menuX;
		m_sSlots[n].sMenuPos.y = menuY;
		menuY += 15;
	}

	// =================
    // Draw icons
	// =================
	for (n = 0 ;n < m_wMenuItems - 1; n++)
	{
		// Draw Pots
		g_pGame->SimpleDrawCodeLarge(m_sSlots[n].sMenuPos.x * 2, 
			                         m_sSlots[n].sMenuPos.y * 2, 
									 "_save01", 255,255,255,255, 2);

		g_pGame->SimpleDrawCodeLarge(155, 
			                         (m_sSlots[n].sMenuPos.y * 2) + 19, 
									 "_save14", 255,255,255,255, 1);

		if (m_sSlots[n].wHealth != 0)
			DrawHearts((m_sSlots[n].sMenuPos.x * 2) + 95, 
			           (m_sSlots[n].sMenuPos.y * 2) + 4, m_sSlots[n].wHealth);

	/*	g_pGame->GetTextBoxFont()->DrawText( (m_sSlots[n].sMenuPos.x * 4) + 38, 
			                                 (m_sSlots[n].sMenuPos.y * 4) + 8, TEXT_COL, m_sSlots[n].szName);*/
	}

	// ===============================
	// Draw a Skull as the last item
	// ===============================
	g_pGame->SimpleDrawCodeLarge(m_sSlots[m_wMenuItems - 1].sMenuPos.x * 2, 
			                     m_sSlots[m_wMenuItems - 1].sMenuPos.y * 2, 
							     "_save02", 255,255,255,255, 2);
/*
	if (!g_pGame->IsFirstTime())
	{
		g_pGame->GetTextBoxFont()->DrawText( (m_sSlots[m_wMenuItems - 1].sMenuPos.x * 4) + 38, 
			                             (m_sSlots[m_wMenuItems - 1].sMenuPos.y * 4) + 8, TEXT_COL, "Cancel");
	}
	else
	{
		g_pGame->GetTextBoxFont()->DrawText( (m_sSlots[m_wMenuItems - 1].sMenuPos.x * 4) + 38, 
			                             (m_sSlots[m_wMenuItems - 1].sMenuPos.y * 4) + 8, TEXT_COL, "New Game");

	}*/

	// Enter Key has been pressed
	if (m_rCounter != -1)
	{
		if (m_sSlots[m_wSelected].wHealth == 0 && m_wSelected != m_wMenuItems - 1 )
		{
			// Cant load this
			m_rCounter = -1;
		}
		else
		{
			// Draw player holding
			g_pGame->SimpleDrawCodeLarge((m_sSlots[m_wSelected].sMenuPos.x * 2) - 24, 
										 (m_sSlots[m_wSelected].sMenuPos.y * 2) - 4, 
										 "_save13", 255,255,255,255, 2);
			m_rCounter += g_rDelta;

			// The CountDown is complete!
			if (m_rCounter > 1)
			{
				m_rCounter = -1;

				// Select a menu item
				if (m_wSelected == m_wMenuItems - 1)
				{
					g_pGame->SetFirstTime(false);
					m_isLoading = false;
					g_pGame->SetPauseLevel(0, -1, NULL);
				}
				if (m_wSelected < m_wMenuItems - 1)
				{
					g_pGame->SetFirstTime(false);
					LoadTheGame(m_wSelected);
					m_wSelected  = 0;
					m_isWhereScreen = true;
					m_isLoading = false;
				}		
			}
		}
	}
	else
	{
		// Draw the Player
		g_pGame->SimpleDrawCodeLarge((m_sSlots[m_wSelected].sMenuPos.x * 2) - 24, 
									 (m_sSlots[m_wSelected].sMenuPos.y * 2) - 4, 
									 "_save12", 255,255,255,255, 2);
	}

	// ==================
	// Handle Key Input
	// ==================
	HandleKeys();
}


//-----------------------------------------------------------------------------
// Name: WhereStartScreen()
// Desc: Displays a "Where do you want to start from?" screen
//
//-----------------------------------------------------------------------------
void CSaveLoad::DoWhereStart()
{
	int n;
	int x, y;
	int menuX = 90;
	int menuY = 70;

	// Gather Data from the Restart Table
	m_wMenuItems = 0;
 
	// Firstly Check if there are any entries in the restart table
	for (n = 0 ; n < MAX_RESTART; n++)
	{
		if (strlen(g_pGame->GetRestartTable(n).szName) > 2)
		{
			m_wMenuItems++;
			m_sSlots[n].sMenuPos.x = menuX;
			m_sSlots[n].sMenuPos.y = menuY;
			menuY += 50;

			strcpy(m_sSlots[n].szName, g_pGame->GetRestartTable(n).szName);
		}
	}

	// If there are no menu items then just leave
	if (m_wMenuItems < 1)
	{
		m_isWhereScreen = false;
		g_pGame->SetPauseLevel(0, -1, NULL);
		g_pGame->RestartFrom(-1);
	}
	
	// Draw the mat in the background
	DrawMat(80,16,20,(3 * m_wMenuItems) + 6);

	// Write "START FROM WHERE?"
	//g_pGame->GetTextBoxFont()->DrawText( 240, 55, TEXT_COL, "Start From Where?");

	// =================
    // Draw Each Entry
	// =================
	for (n = 0 ;n < m_wMenuItems; n++)
	{
		// Draw each place Description next to each entry
		x = (m_sSlots[n].sMenuPos.x * 2) + 30;
		y = (m_sSlots[n].sMenuPos.y) + 60;
		//g_pGame->GetTextBoxFont()->DrawText( x, y, TEXT_COL, m_sSlots[n].szName);

		// Draw brown dividers
		g_pGame->SimpleDrawCodeLarge(100, (y / 2) + 16, 
									 "_save14", 255,255,255,255, 1);
	}

	// Draw arrow next to an entry
	g_pGame->SimpleDrawCodeLarge(90, (((m_sSlots[m_wSelected].sMenuPos.y) + 60) / 2) + 2,
		                         "__arow2", 255, 255, 255, 255);

	// Enter Key has been pressed
	if (m_rCounter != -1)
	{		
		g_pGame->SetPauseLevel(0, -1, NULL);
		m_isWhereScreen = false;
		g_pGame->RestartFrom(m_wSelected);
	}

	// ==================
	// Handle Key Input
	// ==================
	HandleKeys();
}


//-----------------------------------------------------------------------------
// Name: HandleKeys()
// Desc: 
//
//-----------------------------------------------------------------------------
void CSaveLoad::HandleKeys()
{
	// handle Up Key
	if (g_pGame->GetUpKey() && m_rCounter == -1)
	{
		g_pGame->SetUpKey(false);
		m_wSelected--;
		
		if (m_wSelected < 0)
			m_wSelected = m_wMenuItems - 1;
	}

	// handle Down Key
	if (g_pGame->GetDownKey() && m_rCounter == -1)
	{
		g_pGame->SetDownKey(false);
		m_wSelected++;
		
		if (m_wSelected >= m_wMenuItems)
			m_wSelected = 0;
	}

	// Handle Enter Key
	if (g_pGame->GetEnterKey() || g_pGame->GetQKey() || g_pGame->GetAKey() || 
		g_pGame->GetWKey() || g_pGame->GetSKey() && m_rCounter == -1)
	{
		g_pGame->ResetKeys();

		// begin a short count-down
		m_rCounter = 0;
	}
}


//-----------------------------------------------------------------------------
// Name: DrawMat()
// Desc: 
//
//-----------------------------------------------------------------------------
void CSaveLoad::DrawMat(int x, int y, int width, int height)
{
	int swidth = 8;

	// Draw a mat
	DrawTileLine(x + swidth, y, width, EAST, "_save07");
	DrawTileLine(x + swidth, (swidth * (height+1)) + y, width, EAST, "_save09");
	DrawTileLine(x, y + swidth, height, SOUTH, "_save10");
	DrawTileLine((swidth * (width+1)) + x, y + swidth, height, SOUTH, "_save08");

	// Draw the corners
	DrawTileLine(x, y, 1, EAST, "_save03");
	DrawTileLine(x, (swidth * (height+1)) + y, 1, EAST, "_save06");
	DrawTileLine((swidth * (width+1)) + x, y, 1, SOUTH, "_save04");
	DrawTileLine((swidth * (width+1)) + x, (swidth * (height+1)) + y, 1, SOUTH, "_save05");

	// Construct a rectangle for the Brown area of the box
	SDL_Rect Fill = {x + swidth, y + swidth, (swidth * (width+1)) + x, (swidth * (height+1)) + y};

	// Draw the inside of the box Brown
	g_pGame->SimpleTileCode((x + swidth) * 2, 
		                    (y + swidth) * 2, 
							(width * swidth) * 2,
							(height * swidth) * 2,
							"_save11", 
		                    255,255,255,255);
}


//-----------------------------------------------------------------------------
// Name: DrawTileLine()
// Desc: Draws a line of tiles
//
//-----------------------------------------------------------------------------
void CSaveLoad::DrawTileLine(int StartX, int StartY, int Length, int Direction, char* code)
{
	int n;
	int width = 8;
	int height = 8;

	for (n = 0; n < Length; n++)
	{
		switch( Direction )
		{
		case NORTH:
			g_pGame->SimpleDrawCodeLarge(StartX, StartY - (n * height), code, 255,255,255,255, 2);
			break;

		case EAST:
			g_pGame->SimpleDrawCodeLarge(StartX + (n * width), StartY, code, 255,255,255,255, 2);
			break;

		case SOUTH:
			g_pGame->SimpleDrawCodeLarge(StartX, StartY + (n * height), code, 255,255,255,255, 2);
			break;

		case WEST:
			g_pGame->SimpleDrawCodeLarge(StartX - (n * width), StartY, code, 255,255,255,255, 2);
			break;
		};
	}
}


//-----------------------------------------------------------------------------
// Name: DrawHearts()
// Desc: 
//
//-----------------------------------------------------------------------------
void CSaveLoad::DrawHearts(int x, int y, int wNumHearts)
{
	int i;
	int hx,hy;
	int wWidth = 8;  // Heart sprite width

	// Make sure we dont have too many hearts
	if ((wNumHearts / 100) > MAX_HEARTS)
		wNumHearts = MAX_HEARTS * 100;

	for (i = 0; i < wNumHearts / 100 ; i += 1)
	{
		if (i < MAX_HEARTS / 2)
		{
			hx = ( x - 13 ) + ( i * wWidth );
			hy = y;
		}
		else
		{
			hx = (x - 13) + ( i * wWidth ) - ( MAX_HEARTS / 2) * wWidth;
			hy = y + 10;
		}
	
		g_pGame->SimpleDrawCodeLarge(hx, hy, "_heart1", 255,255,255,255, 1);
	}
}


//-----------------------------------------------------------------------------
// Name: WriteQuestTitle()
// Desc: 
//
//-----------------------------------------------------------------------------
void CSaveLoad::WriteQuestTitle()
{
	char szTitle[256];

	// Build a title for the quest
	memset(szTitle, 0 ,sizeof(szTitle));
	szTitle[0] = '"';
	strcat(szTitle, g_pGame->GetMap()->GetQuestName());
	szTitle[strlen(szTitle)] = '"';

		// Draw the Title of the Quest
	int t;
	//g_pGame->GetTextBoxFont()->GetTextExtent(szTitle, &t);
	//g_pGame->GetTextBoxFont()->DrawText( 320 - t.cx / 2, 90, TEXT_COL, szTitle);
}
