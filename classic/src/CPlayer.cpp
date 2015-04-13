/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CPlayer.cpp
// Desc: 
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include "CPlayer.h"
#include "CGame.h"
#include "CAnimation.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame*		g_pGame;
extern float		g_rDelta;


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer() : CEntity()
{
	// Create the Animation Objects
	m_pStandn = new CAnimation("_player_", 5);
	m_pStande = new CAnimation("_player_", 5);
	m_pStands = new CAnimation("_player_", 5);
	m_pStandw = new CAnimation("_player_", 5);

	// Add Frames to the animation objects
	m_pStandn->AddFrame("___psn1", 0, 0);
	m_pStande->AddFrame("___pse1", 0, 0);
	m_pStands->AddFrame("___pss1", 0, 0);
	m_pStandw->AddFrame("___psw1", 0, 0);

	m_eState = standing;
}


//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	SAFE_DELETE( m_pStandn );
	SAFE_DELETE( m_pStande );
	SAFE_DELETE( m_pStands );
	SAFE_DELETE( m_pStandw );
}


//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CPlayer::Update()
{
	switch( m_eState )
	{
	case standing:
		Stand();
		break;
	}
}


//-----------------------------------------------------------------------------
// Name: Stand()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CPlayer::Stand()
{
	CAnimation* pTemp = NULL;

	// Calculate which Animation to use
	switch( m_wDirection )
	{
	case NORTH:
		pTemp = m_pStandn;
		break;
	case EAST:
		pTemp = m_pStande;
		break;
	case SOUTH:
		pTemp = m_pStands;
    	break;
	case WEST:
		pTemp = m_pStandw;
		break;
	}

	pTemp->Draw(m_x, m_y, m_y);
}






