/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// VirtualArchive.cpp: implementation of the CVirtualArchive class.
//
//////////////////////////////////////////////////////////////////////

#include "CVirtualArchive.h"
#include "CGame.h"


#include "Global.h"
#include <zlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVirtualArchive::CVirtualArchive()
{
	memset(m_szFileName, 0, 512);
	m_lHeaderSize	= 0;

	// Create Encryption Keys
	m_cEncryptionKeys[0] = 8;
	m_cEncryptionKeys[1] = 0;
	m_cEncryptionKeys[2] = 0;
}

CVirtualArchive::~CVirtualArchive()
{
	m_Files.clear();
}


//-----------------------------------------------------------------------------
// Name: LoadHeader()
// Desc: Loads all virtual files into memory
//       Set the filename prior to calling this function
//-----------------------------------------------------------------------------
bool CVirtualArchive::LoadHeader()
{
	char szBuffer[128];
	gzFile pIn;
	long lFiles = 0;
	long n;

	m_Files.clear();

	// Open the file

	if (!(pIn = gzopen( m_szFileName, "r")))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
								 "Error",
								 "Couldn't Open Archive File!",
								 NULL);
		return false;
	}

	/*
	if (!(pIn = fopen( m_szFileName, "r")))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
								 "Error",
								 "Couldn't Open Archive File!",
								 NULL);
		return false;
	}
*/
	// Load the File Header and make sure its an archive file
	if (!GetNextEntry(szBuffer, sizeof(szBuffer), pIn))
		return false;

	// Check First entry is the archive header
	if ( strcmp(szBuffer, "Virtual Archive File"))
	{

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
								 "Not a valid QST file",
								 m_szFileName,
								 NULL);

		return false;
	}

	// Get the number of file in archive
	GetNextEntry(szBuffer, sizeof(szBuffer), pIn);
	lFiles = atol(szBuffer);

	if (lFiles < 1)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
								 "No files in archive!",
								 m_szFileName,
								 NULL);
		return false;
	}

	// Load each file entry
	for ( n = 0; n < lFiles; n++ )
		LoadFile( pIn );

	// Get the Size of the header
	//m_lHeaderSize = ftell( pIn );
	m_lHeaderSize = gztell( pIn );

	//fclose( pIn );
	gzclose( pIn );
	return true;
}


//-----------------------------------------------------------------------------
// Name: LoadFile()
// Desc:
//
//-----------------------------------------------------------------------------
void CVirtualArchive::LoadFile( gzFile pIn)
//void CVirtualArchive::LoadFile( FILE *pIn)
{
	char szBuffer[32];
	char szVirtName[64];
	bool bEncrypted;
	long lStart;
	long lEnd;

	// Get the Virtual File name
	GetNextEntry(szVirtName, sizeof(szVirtName), pIn);

	// Get the encryption status
	GetNextEntry(szBuffer, sizeof(szBuffer), pIn);
	bEncrypted = atoi( szBuffer );

	// Get the starting pos
	GetNextEntry(szBuffer, sizeof(szBuffer), pIn);
	lStart = atol( szBuffer );

	// Get the end pos
	GetNextEntry(szBuffer, sizeof(szBuffer), pIn);
	lEnd = atol( szBuffer );

	// Create a new file
	CVirtualFile* pNew = new CVirtualFile( lStart, lEnd, bEncrypted, szVirtName );
	pNew->ClassifyFile();
	pNew->SetOwner( this );
	pNew->SetEncryptionKeys( m_cEncryptionKeys );

	m_Files.push_back( pNew );
}

//-----------------------------------------------------------------------------
// Name: GetNextEntry()
// Desc: Gets the next string between two ~ chars
//
//-----------------------------------------------------------------------------
//bool CVirtualArchive::GetNextEntry(char *szString, size_t sLen, FILE *in)
bool CVirtualArchive::GetNextEntry(char *szString, size_t sLen, gzFile in)
{

	int m;
	char c;

	// Clear the supplied buffer
	memset(szString, 0, sLen);

	// This function gets the next line from a file
	// decrypts it and places it in the supplied buffer
	for (m = 0; m < sLen; m++)
	{
		// Get the Next Character
		//c = fgetc( in );
		c  = gzgetc(in);
		if (c == EOF)
			return false;

		if (c == '~')
		   return true;

		szString[m] = c;
	}

	return false;
}


//-----------------------------------------------------------------------------
// Name: LoadScreenFiles()
// Desc:
//
//-----------------------------------------------------------------------------
bool CVirtualArchive::LoadScreenFiles( CQuestLoader* pQuest)
{
	// Get all files of type lnd, and load their scripts
	// file 2-2.lnd / LS2-2.amx

	if ( m_Files.empty() )
		return false;

	for ( std::list<CVirtualFile*>::iterator it = m_Files.begin(); it != m_Files.end(); ++it )
	{
		GetGameObject()->LoadingMessage("Extracting Screen", 10, 10, 0);
		CVirtualFile* pTemp = *it;

		// Check if this file is an lnd file
		if ( pTemp->GetType() == lnd)
		{
			// Add a new screen to the quest
			pQuest->AddScreen( pTemp );
		}
	}

	return true;
}


//-----------------------------------------------------------------------------
// Name: ExtractAMXFiles()
// Desc:
//-----------------------------------------------------------------------------
bool CVirtualArchive::ExtractAMXFiles()
{
	if (m_Files.empty())
		return false;


	for ( std::list<CVirtualFile*>::iterator it = m_Files.begin(); it != m_Files.end(); ++it )
	{
		GetGameObject()->LoadingMessage("Extracting Script", 10, 10, 0);
		CVirtualFile* pTemp = *it;

		// Check if this file is an lnd file
		if ( pTemp->GetType() == lndScript ||
			 pTemp->GetType() == groupScript ||
			 pTemp->GetType() == entityScript ||
			 pTemp->GetType() == mainScript)
		{
			// extract this script
			pTemp->ExtractAMX();
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: FindFile()
// Desc:
//-----------------------------------------------------------------------------
CVirtualFile* CVirtualArchive::FindFile( char* szName )
{
	if ( m_Files.empty() )
		return NULL;

	for ( std::list<CVirtualFile*>::iterator it = m_Files.begin(); it != m_Files.end(); ++it )
	{
		CVirtualFile* pTemp = *it;

		if (!strcmp(pTemp->GetVirtualName(), szName))
			return pTemp;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Name: LoadQSSFile()
// Desc:
//-----------------------------------------------------------------------------
bool CVirtualArchive::LoadQSSFile( CQuestLoader* pQuest )
{
	CVirtualFile* pFile = FindFile( "qss" );

	if ( !pFile )
		return false;

	pQuest->HandleQSSfile(pFile);
	return true;
}


//-----------------------------------------------------------------------------
// Name: LoadEntities()
// Desc:
//-----------------------------------------------------------------------------
bool CVirtualArchive::LoadEntities()
{
	long lEntCount = 0;
	bool bLib;
	char szBuffer[64];
	CVirtualFile* pEnt = FindFile( "entities.txt" );

	if (!pEnt)
		return false;

	pEnt->Open();

	for (;;)
	{
		GetGameObject()->LoadingMessage("Extracting Script", 10, 10, 0);

		// Get the entity name
		if (!pEnt->ReadString(szBuffer, sizeof(szBuffer)))
			break;

		// Get the library flag
		bLib = pEnt->ReadLong();

		GetGameObject()->AddAvailableEntity(szBuffer, bLib);
		lEntCount++;
	}

	pEnt->Close();

	// Record the number of entities in the Log
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s %d\n", "Number of Avaiable Entities:", lEntCount);

	return true;
}


//-----------------------------------------------------------------------------
// Name: GetSheetsFile()
// Desc:
//-----------------------------------------------------------------------------
CVirtualFile* CVirtualArchive::GetSheetsFile()
{
	return FindFile( "sheets.txt" );
}


//-----------------------------------------------------------------------------
// Name: ExtractFile()
// Desc: Find a file and extract it to a location
//-----------------------------------------------------------------------------
bool CVirtualArchive::ExtractFile( char* szName, char* szLocation )
{
	CVirtualFile* pFile = FindFile( szName );

	if (!pFile)
		return false;

	pFile->ExtractTo( szLocation );
	return true;

}


//-----------------------------------------------------------------------------
// Name: CheckForEntities()
// Desc: Checks every LND file for entities
//
//-----------------------------------------------------------------------------
void CVirtualArchive::CheckForEntities()
{
	long n;
	long lSprites;
	long x,y;
	char szCode[ MAX_CHARS ];
	char szIdent[ MAX_CHARS ];

	if ( m_Files.empty() )
		return;


	for ( std::list<CVirtualFile*>::iterator it = m_Files.begin(); it != m_Files.end(); ++it )
	{
		CVirtualFile* pTemp = *it;

		if (pTemp->GetType() == lnd)
		{

			// This screen is an LND, go through each sprite code
			// if its an entity then add it to the active list
			pTemp->Open();

			// Get the number of sprites
			lSprites = pTemp->ReadLong();

			for (n = 0; n < lSprites; n++ )
			{

				x = (pTemp->ReadLong() / 2) + pTemp->GetX() * 320;
				y = (pTemp->ReadLong() / 2) + pTemp->GetY() * 240;
				pTemp->ReadString( szCode, MAX_CHARS + 5 );


				// If the ident is blank then its not an entity
				// if its just a * then its an entity but clear the ident
				pTemp->ReadString( szIdent, MAX_CHARS );


				if ( szIdent != NULL && szIdent[0] > 32  )
				{
					if ( szIdent[0] == '*')
						GetGameObject()->FindEntity(szCode, x, y, "", false);
					else
						GetGameObject()->FindEntity(szCode, x, y, szIdent, false);
				}
			}

			pTemp->Close();
		}
	}

}
