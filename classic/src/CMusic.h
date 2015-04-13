/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CMusic.h
//
// Desc: 
//
//-----------------------------------------------------------------------------
#ifndef CMUSIC_H
#define CMUSIC_H

//#include "FMOD/fmod.h"
#include <SDL2/SDL_mixer.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CMusic;

//-----------------------------------------------------------------------------
// Name: class CMusic
// Desc:
//-----------------------------------------------------------------------------
class CMusic
{
	Mix_Music *m_pMod;
	char		   m_szName[64];	// Name of Music File
	int			   m_wLoopBack;		// LoopBack Value
	bool		   bFree;
public:
    CMusic(char* szFileName, bool free = false);
    ~CMusic();

	// Access Functions
	Mix_Music * GetModule(){return m_pMod; }
	char* GetName(){return m_szName;}
	int	  GetLoopBack(){return m_wLoopBack;}

	// Data Broker Functions
	void SetName(char* name){strncpy(m_szName, name, 64);}
	void SetLoopBack(int val){m_wLoopBack = val;}
};


#endif

