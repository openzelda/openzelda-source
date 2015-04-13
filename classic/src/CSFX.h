/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CSFX.h
//
// Desc: 
//
//-----------------------------------------------------------------------------
#ifndef CSFX_H
#define CSFX_H

//#include "FMOD/fmod.h"
#include <SDL2/SDL_mixer.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CSFX;

//-----------------------------------------------------------------------------
// Name: class CSFX
// Desc:
//-----------------------------------------------------------------------------
class CSFX
{
	Mix_Chunk *m_pSample;
	char		   m_szName[64];	// Name of Sound File

public:
    CSFX(char* szFileName);
    ~CSFX();

	// Access Functions
	Mix_Chunk * GetSample(){return m_pSample;}
	char* GetName(){return m_szName;}

	// Data Broker Functions
	void SetName(char* name){strcpy(m_szName, name);}
};


#endif

