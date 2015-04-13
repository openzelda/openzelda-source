/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CMusic.cpp
//
// Desc:
//
//
//-----------------------------------------------------------------------------
#define STRICT
#include "CMusic.h"
#include "GeneralData.h"
//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CMusic Constructor
//-----------------------------------------------------------------------------
CMusic::CMusic(char* szFileName, bool free)
{
	SetName("none");
	m_wLoopBack = 0;
	bFree = free;

	// Load the Track provided
	//m_pMod = FMUSIC_LoadSong(szFileName);

	std::string path(FS_Gamedir());
	path.append(szFileName);

	m_pMod = Mix_LoadMUS(path.c_str());
}


//-----------------------------------------------------------------------------
// CMusic destructor
//-----------------------------------------------------------------------------
CMusic::~CMusic()
{
	/*
	if (FMUSIC_IsPlaying(m_pMod))
		FMUSIC_StopSong(m_pMod);

	FMUSIC_FreeSong(m_pMod);
	*/

	Mix_FreeMusic(m_pMod);


}
