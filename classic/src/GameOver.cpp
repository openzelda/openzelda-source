/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// GameOver.cpp: implementation of the CGameOver class.
//
//////////////////////////////////////////////////////////////////////
#include "GameOver.h"
#include "CGame.h"

extern CGame*		g_pGame;
extern float		g_rDelta;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameOver::CGameOver()
{
	// Set up the menu items
	strcpy(m_sMenuItems[0].szName, "Continue");
	strcpy(m_sMenuItems[1].szName, "Exit Game");

	m_sMenuItems[0].sMenuPos.x = 270;
	m_sMenuItems[1].sMenuPos.x = 270;
	
	m_sMenuItems[0].sMenuPos.y = 200;
	m_sMenuItems[1].sMenuPos.y = 230;

	m_nSelected = 0;
	m_bInUse    = false;
}

CGameOver::~CGameOver()
{

}

//-----------------------------------------------------------------------------
// Name: PreGameOver()
// Desc: Prepares for Game Over
//
//-----------------------------------------------------------------------------
void CGameOver::PreGameOver()
{
	if (m_bInUse)
		return;

	g_pGame->ResetKeys();
	m_bInUse  = true;
	m_nSelected = 0;
	g_pGame->SetPauseLevel(4, -1, NULL);
}


//-----------------------------------------------------------------------------
// Name: DoGameOver()
// Desc: 
//
//-----------------------------------------------------------------------------
void CGameOver::DoGameOver()
{
	int n;

	// Draw each option
	for ( n = 0 ; n < MAX_MENU_ITEMS; n++ )
	{
	/*	g_pGame->GetTextBoxFont()->DrawText( m_sMenuItems[n].sMenuPos.x, m_sMenuItems[n].sMenuPos.y, 
			                                  0xFFFFFFFF, m_sMenuItems[n].szName );*/
	}

	// handle Up Key
	if (g_pGame->GetUpKey())
	{
		g_pGame->SetUpKey(false);
		m_nSelected--;
		g_pGame->GetSound()->PlaySoundFX("_menuselect.wav", 250);
		
		if (m_nSelected < 0)
			m_nSelected = MAX_MENU_ITEMS - 1;
	}

	// handle Down Key
	if (g_pGame->GetDownKey())
	{
		g_pGame->SetDownKey(false);
		m_nSelected++;
		g_pGame->GetSound()->PlaySoundFX("_menuselect.wav", 250);
		
		if (m_nSelected >= MAX_MENU_ITEMS)
			m_nSelected = 0;
	}

	// Handle Enter Key
	if (g_pGame->GetEnterKey() || g_pGame->GetQKey() || g_pGame->GetAKey() || 
		g_pGame->GetWKey() || g_pGame->GetSKey())
	{
		g_pGame->GetSound()->PlaySoundFX("_menuselect.wav", 250);
		g_pGame->ResetKeys();
		HandleChoice();
	}


	g_pGame->SimpleDrawCodeLarge(115,(m_sMenuItems[m_nSelected].sMenuPos.y / 2) + 3,
		                         "__arow2", 255, 255, 255, 255);

}


//-----------------------------------------------------------------------------
// Name: HandleChoice()
// Desc: 
//
//-----------------------------------------------------------------------------
void CGameOver::HandleChoice()
{	
	m_bInUse  = false;

	if (m_nSelected == 0)
	{
		g_pGame->SetPauseLevel(0, -1, NULL);

		// Continue from the last know death point
		g_pGame->RestartFrom(-1);
	}


	if (m_nSelected == 1)
	{
		// Exit Game
		exit(0);
	}
}
