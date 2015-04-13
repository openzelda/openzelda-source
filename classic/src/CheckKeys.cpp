/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CheckKeys.h
//
// Desc: Handles Key Input from the user
//       
// apr 2010 [luke] added custom key support
//-----------------------------------------------------------------------------
#define STRICT

#include <SDL2/SDL_keycode.h>
#include "GeneralData.h"
#include "CGame.h"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame * g_pGame;

uint32_t keyset[20] = { \
	SDLK_RETURN,\
	SDLK_LEFT,\
	SDLK_RIGHT,\
	SDLK_UP,\
	SDLK_DOWN,\
	SDLK_q,\
	SDLK_w,\
	SDLK_a,\
	SDLK_s,\
	SDLK_e,\
	SDLK_d,\
	SDLK_z,\
	SDLK_x,\
	SDLK_c,\
	SDLK_RSHIFT,\
	SDLK_r,\
	SDLK_f,\
	SDLK_v,\
	SDLK_t,\
	SDLK_g\
};

//-----------------------------------------------------------------------------
// Name: CheckKeysDown()
// Desc: 
//
//-----------------------------------------------------------------------------
void CheckKeysDown(int32_t wParam)
{
	if (!g_pGame)
		return;

	if ( wParam == keyset[0] )
		g_pGame->m_fEnterKey = true;
	else if ( wParam == keyset[1] )
		g_pGame->m_fLeftKey=true;
	else if ( wParam == keyset[2] )
		g_pGame->m_fRightKey=true;
	else if ( wParam == keyset[3] )
		g_pGame->m_fUpKey=true;
	else if ( wParam == keyset[4] )
		g_pGame->m_fDownKey=true;
	else if ( wParam == keyset[5] )
	{
		if (g_pGame->m_rQKeyTimeOut <= 0)
			g_pGame->m_fQKey=true;
	}
	else if ( wParam == keyset[6] )
		g_pGame->m_fWKey=true;
	else if ( wParam == keyset[7] )
		g_pGame->m_fAKey=true;
	else if ( wParam == keyset[8] )
		g_pGame->m_fSKey=true;
	else if ( wParam == keyset[9] )
		g_pGame->m_fEKey=true;
	else if ( wParam == keyset[10] )
		g_pGame->m_fDKey=true;
	else if ( wParam == keyset[11] )
		g_pGame->m_fZKey=true;
	else if ( wParam == keyset[12] )
		g_pGame->m_fXKey=true;
	else if ( wParam == keyset[13] )
		g_pGame->m_fCKey=true;
	else if ( wParam == keyset[14] )
		g_pGame->m_fShift=true;
	else if ( wParam == keyset[15] )
		g_pGame->m_fRKey=true;
	else if ( wParam == keyset[16] )
		g_pGame->m_fFKey=true;
	else if ( wParam == keyset[17] )
		g_pGame->m_fVKey=true;
	else if ( wParam == keyset[18] )
		g_pGame->m_fTKey=true;
	else if ( wParam == keyset[19] )
		g_pGame->m_fGKey=true;

	g_pGame->m_fkey = wParam;
}

//-----------------------------------------------------------------------------
// Name: CheckKeysUp()
// Desc: 
//
//-----------------------------------------------------------------------------
void CheckKeysUp(int32_t wParam)
{
	if (!g_pGame)
		return;

	if ( wParam == keyset[0] )
		g_pGame->m_fEnterKey = false;
	else if ( wParam == keyset[1] )
		g_pGame->m_fLeftKey=false;
	else if ( wParam == keyset[2] )
		g_pGame->m_fRightKey=false;
	else if ( wParam == keyset[3] )
		g_pGame->m_fUpKey=false;
	else if ( wParam == keyset[4] )
		g_pGame->m_fDownKey=false;
	else if ( wParam == keyset[5] )
		g_pGame->m_fQKey=false;
	else if ( wParam == keyset[6] )
		g_pGame->m_fWKey=false;
	else if ( wParam == keyset[7] )
		g_pGame->m_fAKey=false;
	else if ( wParam == keyset[8] )
		g_pGame->m_fSKey=false;
	else if ( wParam == keyset[9] )
		g_pGame->m_fEKey=false;
	else if ( wParam == keyset[10] )
		g_pGame->m_fDKey=false;
	else if ( wParam == keyset[11] )
		g_pGame->m_fZKey=false;
	else if ( wParam == keyset[12] )
		g_pGame->m_fXKey=false;
	else if ( wParam == keyset[13] )
		g_pGame->m_fCKey=false;
	else if ( wParam == keyset[14] )
		g_pGame->m_fShift=false;
	else if ( wParam == keyset[15] )
		g_pGame->m_fRKey=false;
	else if ( wParam == keyset[16] )
		g_pGame->m_fFKey=false;
	else if ( wParam == keyset[17] )
		g_pGame->m_fVKey=false;
	else if ( wParam == keyset[18] )
		g_pGame->m_fTKey=false;
	else if ( wParam == keyset[19] )
		g_pGame->m_fGKey=false;
		


}

