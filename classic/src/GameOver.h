/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// GameOver.h: interface for the CGameOver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOVER_H__5E6D708A_9AED_4C49_BBFB_6E388F6862D0__INCLUDED_)
#define AFX_GAMEOVER_H__5E6D708A_9AED_4C49_BBFB_6E388F6862D0__INCLUDED_

#include "GeneralData.h"
#define MAX_MENU_ITEMS 2

struct _MenuItem{
	char    szName[64];
	SDL_Point	sMenuPos;
};


class CGameOver  
{
public:
	CGameOver();
	virtual ~CGameOver();

	bool		m_bInUse;
	int			m_nSelected;
	_MenuItem	m_sMenuItems[MAX_MENU_ITEMS];

	void PreGameOver();
	void DoGameOver();
	void HandleChoice();

};

#endif // !defined(AFX_GAMEOVER_H__5E6D708A_9AED_4C49_BBFB_6E388F6862D0__INCLUDED_)
