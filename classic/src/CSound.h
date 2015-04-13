/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CSound.h
//
// Desc: Class which encapsulates all Sound Files
//
//-----------------------------------------------------------------------------
#ifndef CSOUND_H
#define CSOUND_H

#include <stdio.h>
#include "CMusic.h"
#include "CSFX.h"
#include "LList.h"
//#include "FMOD/fmod.h"
#include <SDL2/SDL_mixer.h>

//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CSound;

//-----------------------------------------------------------------------------
// Name: class CSound
// Desc: Class to house all sound files 
//-----------------------------------------------------------------------------
class CSound
{
	LList<CMusic> m_pMusic;     			// Linked List of Music objects
	LList<CSFX>   m_pSFX;     		    	// Linked List of Sound Effects objects
	int			  m_wSounds;

	// ================
	// Music Variables
	// ================
	CMusic		 *m_pCurrentMusic;			// Current Music Module
	CMusic		 *m_pNextMusic;				// Next music in line
	float		  m_rMusicFade;
	float		  m_rFadeSpeed;
	float		  m_rMusicTimeOut;
	int			  m_wMusicVolume;

	// ================
	// SFX variables
	// ================

public:
    CSound();
    ~CSound();
	
	bool    InitSound();
	void	LoadCustomSounds(char* FileName);	// Loads sounds from the Current QST

	// SFX
	CSFX*   GetSFX(char *szCode);
	bool    PlaySoundFX(char *szCode, int wVolume);
	bool    StopSoundFX(char *szCode);

	// Music
	void	MusicFinished();
	CMusic* GetMusic(char *szCode);
	void    SwitchMusic(char* szName, int loopBack, bool fFade = true);
	void    DoMusic();
	void    FadeMusic();
	char*   GetMusicString();
	void    SetMusicVolume(int wVolume);
	int		GetMusicVolume(){return m_wMusicVolume;}
	void    SetMusicFadeSpeed(long wVal){m_rFadeSpeed = (float)wVal;}
};


#endif

