/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CSound.cpp
//
// Desc:
//
//
//-----------------------------------------------------------------------------
#define STRICT
#include "CSound.h"
#include "CGame.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame*		g_pGame;
extern float		g_rDelta;


void musicFinished()
{
	g_pGame->GetSound()->MusicFinished();
}

//-----------------------------------------------------------------------------
// CSound Constructor
//-----------------------------------------------------------------------------
CSound::CSound()
{

	// Initialise the Sound;
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","CSound::CSound() Calling InitSound()");
	if ( Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,512) ==-1)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Error initializing SDL_mixer: %s\n", Mix_GetError());
	}
	m_wSounds		= 0;
	m_rMusicFade	= -1;
	m_rFadeSpeed	= 110;
	m_wMusicVolume  = 200;
	m_rMusicTimeOut	= 0;

	// Initialise the current music
	m_pCurrentMusic = NULL;

	// Load Sounds from the Current Quest
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","CSound::CSound() Calling LoadCustomSounds()");
	LoadCustomSounds( g_pGame->GetQuestFileName() );
}

//-----------------------------------------------------------------------------
// CGraphics destructor
//-----------------------------------------------------------------------------
CSound::~CSound()
{
	// Clear all Music
	m_pMusic.free();
	m_pSFX.free();

	// Stop Sound
	Mix_CloseAudio();
}


//-----------------------------------------------------------------------------
// Name: InitSound()
// Desc:
//
//-----------------------------------------------------------------------------
bool CSound::InitSound()
{


	return true;
}


//-----------------------------------------------------------------------------
// Name: LoadCustomSounds()
// Desc:
//
//-----------------------------------------------------------------------------
void CSound::LoadCustomSounds(char* FileName)
{
	long lNumSFX = 0;
	long lNumMusic = 0;

	char szBuffer[128];

	if (!g_pGame->GetMap())
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Error - LoadCustomSounds - Couldn't access archive");
		return;
	}

	// Load the sheets.txt from the quest archive file
	CVirtualArchive* pTemp = g_pGame->GetMap()->GetArchive();
	CVirtualFile* pFile = pTemp->FindFile( "soundlist.txt" );
	CVirtualFile* pTFile;

	if (!pFile)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Error - LoadCustomSounds - Couldn't get soundlist.txt");
		return;
	}

	pFile->Open();

	// Get each line of the file and make a new sprite sheet
	for (;;)
	{
		// Get the name of the file
		if (!pFile->ReadString(szBuffer, sizeof(szBuffer)))
			break;

		// Extract the sound to a temp location
		pTemp->ExtractFile(szBuffer, "__sound");
		pTFile = pTemp->FindFile( szBuffer );

		if (!pTFile)
			continue;

		if ( pTFile->GetType() == sound )
		{
			// Create SFX
			CSFX* pTemp = new CSFX("__sound");
			m_pSFX.additem(pTemp, szBuffer);
			lNumSFX++;
		}
		else
		{
			// Create Music
			CMusic* pTemp = new CMusic("__sound");
			m_pMusic.additem(pTemp, szBuffer);
			lNumMusic++;
		}
	}

	// Write the Number of music and SFX files to the log
	SDL_Log("Music Files in Quest: %d", lNumMusic);
	SDL_Log("Sound Effects Files in Quest:  %d", lNumSFX);

	pFile->Close();
}


//-----------------------------------------------------------------------------
// Name: PlaySoundFX()
// Desc:
//
//-----------------------------------------------------------------------------
bool CSound::PlaySoundFX(char *szCode, int wVolume)
{
	CSFX* pTemp;

	// Get the SFX object with this code
	pTemp = GetSFX(szCode);

	if (!pTemp)
		return false;

	int channel;
	channel = Mix_PlayChannel(-1, pTemp->GetSample(), 0);
	Mix_Volume(channel, wVolume);

	return true;
}


//-----------------------------------------------------------------------------
// Name: GetSFX()
// Desc:
//
//-----------------------------------------------------------------------------
CSFX* CSound::GetSFX(char *szCode)
{
	if (m_pSFX.isempty())
		return NULL;

	CSFX* pTemp = NULL;

	// Loop Through all Lists
	m_pSFX.tostart();
	do
	{
		if ( !strcmp(m_pSFX.GetCurrentIdent(), szCode) )
			return m_pSFX.getcurrent();

	}while(m_pSFX.advance());
	return NULL;
}

//-----------------------------------------------------------------------------
// Name: StopSoundFX()
// Desc:
//
//-----------------------------------------------------------------------------
bool CSound::StopSoundFX(char *szCode)
{
	CSFX* pTemp;

	// Get the SFX object with this code
	pTemp = GetSFX(szCode);

	if (!pTemp)
		return false;

	Mix_HaltChannel(-1);

	return true;
}

void CSound::MusicFinished()
{
	m_rMusicTimeOut = 0;
}



//-----------------------------------------------------------------------------
// Name: SetMusicVolume()
// Desc:
//
//-----------------------------------------------------------------------------
void CSound::SetMusicVolume(int wVolume)
{
	if ( wVolume < 0)
		wVolume = 1;
	if ( wVolume > 255)
		wVolume = 255;

	if (!m_pCurrentMusic)
		return;

	m_wMusicVolume = wVolume;
	Mix_VolumeMusic(wVolume);

}


//-----------------------------------------------------------------------------
// Name: GetMusic()
// Desc:
//
//-----------------------------------------------------------------------------
CMusic* CSound::GetMusic(char *szCode)
{
	if (m_pMusic.isempty())
		return NULL;

	CMusic* pTemp = NULL;

	// Loop Through all Lists
	m_pMusic.tostart();
	do
	{
		if ( !strcmp(m_pMusic.GetCurrentIdent(), szCode) )
			return m_pMusic.getcurrent();

	}while(m_pMusic.advance());

	/* Load music from Open Zelda directory */
	std::string path(FS_Gamedir());
	path.append("music/");
	path.append(szCode);

	pTemp = new CMusic( (char*) path.c_str() );
	if ( pTemp->GetModule() )
	{
		m_pMusic.additem(pTemp, szCode);
		return pTemp;
	}
	delete pTemp;

	return NULL;
}


//-----------------------------------------------------------------------------
// Name: SwitchMusic()
// Desc:
//
//-----------------------------------------------------------------------------
void CSound::SwitchMusic(char* szName, int loopBack, bool fFade)
{
	// Get a pointer to the new music object
	CMusic* pTemp = GetMusic(szName);

	if (!pTemp)
		return;

	// Point the next music to this new music
	m_pNextMusic = pTemp;
	strcpy(m_pNextMusic->GetName(), szName);
	m_pNextMusic->SetLoopBack(loopBack);

	// Fade the Music out
	if (m_pCurrentMusic)
		m_rMusicFade = Mix_VolumeMusic(m_rMusicFade);
	else
		m_rMusicFade = 10;


	if (!fFade)
		m_rMusicFade = 10;
}


//-----------------------------------------------------------------------------
// Name: DoMusic()
// Desc:
//
//-----------------------------------------------------------------------------
void CSound::DoMusic()
{
	// Check if the Music is fading
	if (m_rMusicFade >= 0)
		FadeMusic();

	if (!m_pCurrentMusic)
		return;

	if ( Mix_PlayingMusic() && m_rMusicTimeOut <= 0 )
	{
		Mix_PlayMusic( m_pCurrentMusic->GetModule(), 1 );
		Mix_HookMusicFinished(musicFinished);

		m_rMusicTimeOut = 5;
	}

	/*
	if (m_rMusicTimeOut > 0)
		m_rMusicTimeOut -= g_rDelta;
	*/
}


//-----------------------------------------------------------------------------
// Name: GetMusicString()
// Desc:
//
//-----------------------------------------------------------------------------
char* CSound::GetMusicString()
{
	if (m_rMusicFade >= 0)
	{
		if (m_pNextMusic)
			return m_pNextMusic->GetName();
	}
	else
	{
		if (m_pCurrentMusic)
			return m_pCurrentMusic->GetName();
	}
	return "non";
}


//-----------------------------------------------------------------------------
// Name: FadeMusic()
// Desc:
//
//-----------------------------------------------------------------------------
void CSound::FadeMusic()
{
	m_rMusicFade -= m_rFadeSpeed * g_rDelta;

	// Set the new Volume
	if (m_pCurrentMusic)
		Mix_VolumeMusic( (int)m_rMusicFade );

	// Check if the fade has ended yet
	if (m_rMusicFade <= 5)
	{
		m_rMusicFade = -1;

		if (m_pCurrentMusic)
		{
			// Change the Music
			if (Mix_PlayingMusic())
				Mix_HaltMusic();

		}
		// Point the next music to this new music
		m_pCurrentMusic = m_pNextMusic;
		m_rMusicTimeOut = 5;

		// Start the music playing
		Mix_VolumeMusic( m_wMusicVolume );
		Mix_PlayMusic(m_pCurrentMusic->GetModule(), 1);

	}

}
