/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CPlayer.h
//
// Desc: 
//       
//-----------------------------------------------------------------------------
#ifndef CPLAYER_H
#define CPLAYER_H

#include "CEntity.h"
#include "CAnimation.h"

enum PlayerState{standing, walking};


//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CPlayer;


//-----------------------------------------------------------------------------
// Name: class CPlayer
// Desc:  
//-----------------------------------------------------------------------------
class CPlayer : public CEntity 
{
	CAnimation*		m_pStandn;
	CAnimation*		m_pStande;
	CAnimation*		m_pStands;
	CAnimation*		m_pStandw;
	CAnimation*		m_pWalkn;
	CAnimation*		m_pWalke;
	CAnimation*		m_pWalks;
	CAnimation*		m_pWalkw;

	PlayerState		m_eState;

public:
    CPlayer();
    ~CPlayer();

	void Update();
	void Stand();
};


#endif

