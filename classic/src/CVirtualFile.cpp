/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// VirtualFile.cpp: implementation of the CVirtualFile class.
//
//////////////////////////////////////////////////////////////////////
#include "CVirtualFile.h"
#include "GeneralData.h"
#include "Global.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVirtualFile::CVirtualFile( long lStart, long lFinish, bool bEncrypted, char* szFileName )
{
	m_sFilePointer	= NULL;
	m_pOwner		= NULL;
	m_lStart		= lStart;
	m_lFinish		= lFinish;
	m_bEncrypted	= bEncrypted;
	m_sType			= normal;
	m_pEncryptionKeys = NULL;
	m_lCurrentPos	= 0;

	strncpy( m_szFileName, szFileName, 64 );
}

CVirtualFile::~CVirtualFile()
{
}

//-----------------------------------------------------------------------------
// Name: ExtractTo()
// Desc:
//-----------------------------------------------------------------------------
bool CVirtualFile::ExtractTo( char* szDest )
{
	FILE* pOut;
	int c;

	// open the output file
	std::string path(FS_Gamedir());
	path.append(szDest);

	if ( !( pOut = fopen(path.c_str(), "wb")))
	{
		fprintf(stderr, "\r\nCouldn't extract File! - %s", (char*)path.c_str() );
		GetGameObject()->LoadingMessage("Couldn't extract file, so we may crash. :)", 5,5, 10);
		return false;
	}

	Open();
	long lFilePos = m_lStart;

	for(;;)
	{
		if (lFilePos == m_lFinish)
			break;

		c = ReadNextChar();
		fputc(c, pOut);

		lFilePos++;
	}

	Close();
	fclose( pOut );
	return true;
}

//-----------------------------------------------------------------------------
// Name: ExtractAMX()
// Desc: extract to temp\\name.amx
//-----------------------------------------------------------------------------
void CVirtualFile::ExtractAMX()
{
	int nStart = 0;
	int n;
	char szBuffer[64];
	char szBuffer2[64];

	memset(szBuffer, 0, sizeof(szBuffer));
	memset(szBuffer2, 0, sizeof(szBuffer2));

	if ( m_sType != mainScript)
		nStart = 2;

	// Take off the first 2 chars
	for ( n = nStart; n < strlen( m_szFileName ); n++ )
		szBuffer[n - nStart] = m_szFileName[n];

	strcpy(szBuffer2, szBuffer);

	ExtractTo( szBuffer2 );
}

//-----------------------------------------------------------------------------
// Name: ReadString()
// Desc:
//-----------------------------------------------------------------------------
bool CVirtualFile::ReadString( char* szString, size_t sLen )
{
	bool bDone = true;
	int n;
	int c;

	// Get the next string from the file
	for ( n = 0; n < sLen - 1; n ++)
	{
		c = ReadNextChar();

		// Check if this is the end of file
		if ( m_lCurrentPos > m_lFinish )
		{
			bDone = false;
			break;
		}

		if ( c == EOF || c == '\n' || c == 0 )
			break;
		szString[n] = c;


	}

	if (bDone)
		szString[n - 1] = 0;

	return bDone;
}

//-----------------------------------------------------------------------------
// Name: ReadLong()
// Desc:
//-----------------------------------------------------------------------------
long CVirtualFile::ReadLong()
{
	bool bDone = false;
	char szLong[32];
	int c;
	int n;

	// Get the next string from the file
	for ( n = 0; n < 30; n ++)
	{
		c = ReadNextChar();

		// Check if this is the end of file
		if ( m_lCurrentPos >= m_lFinish )
		{
			bDone = true;
			break;
		}

		if ( c == EOF || c == '\n' || c == 0 )
			break;

		szLong[n] = c;
	}

	szLong[n] = 0;
	return atol(szLong);
}

//-----------------------------------------------------------------------------
// Name: ReadNextChar()
// Desc:
//-----------------------------------------------------------------------------
int CVirtualFile::ReadNextChar()
{
	int n;
	char c = gzgetc(m_sFilePointer);

	// Increment the file position
	m_lCurrentPos++;

	if ( c == EOF )
		return EOF;

	// Decrypt if necessary
	if (m_bEncrypted)
	{
		for (n = 0; n < NUM_ENC_KEYS; n++)
			c ^= m_pEncryptionKeys[n];  // XOR the char with the Encryption keys
	}

	return (int)c;
}


//-----------------------------------------------------------------------------
// Name: Open()
// Desc:
//-----------------------------------------------------------------------------
bool CVirtualFile::Open()
{
	if (!m_pOwner)
		return false;

	// Open the Archive file
	if (!( m_sFilePointer = gzopen( m_pOwner->GetFileName(), "rb")))
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "Couldn't Open Archive file, tried to load screen file");
		return false;
	}

	// Seek this files place
	gzseek(m_sFilePointer, m_lStart + m_pOwner->GetHeaderLength(), SEEK_SET);
	m_lCurrentPos = m_lStart;

	// archive is open
	return true;
}

//-----------------------------------------------------------------------------
// Name: Close()
// Desc:
//-----------------------------------------------------------------------------
void CVirtualFile::Close()
{
	if ( m_sFilePointer )
		gzclose( m_sFilePointer );
}

//-----------------------------------------------------------------------------
// Name: ClassifyFile()
// Desc: Attempt to classify a file based on its name
//-----------------------------------------------------------------------------
void CVirtualFile::ClassifyFile()
{
	char szExt[32];

	// Look at the file extension
	GetFileExtension( szExt, sizeof(szExt) );

	// Try and classify files based on their extensions
	if (!strcmp(szExt, "txt"))
		m_sType = textFile;
	if (!strcmp(szExt, "tps"))
		m_sType = spt;
	if (!strcmp(szExt, "pmb"))
		m_sType = bmp;
	if (!strcmp(szExt, "vaw"))
		m_sType = sound;
	if (!strcmp(szExt, "ti"))
		m_sType = music;
	if (!strcmp(szExt, "dim"))
		m_sType = music;
	if (!strcmp(szExt, "dom"))
		m_sType = music;
	if (!strcmp(szExt, "mx"))
		m_sType = music;
	if (!strcmp(szExt, "tgs"))
		m_sType = music;

	if (!strcmp(szExt, "dnl"))
	{
		m_sType = lnd;

		// Get the coordinates of the file
		m_lParams[0] = GetLNDParam(0);
		m_lParams[1] = GetLNDParam(1);
	}

	if (!strcmp(szExt, "xma"))
	{
		// Look at the first 2 chars of the filename
		// See if its an LND script
		if ( m_szFileName[0] == 'L' &&
			 m_szFileName[1] == 'S')
		{
			m_lParams[0] = GetLNDParam(0, 2);
			m_lParams[1] = GetLNDParam(1, 2);
			m_sType = lndScript;
		}
		// Check for Group Script
		if ( m_szFileName[0] == 'G' &&
			 m_szFileName[1] == 'S')
		{
			// Get the group number
			m_lParams[0] = GetLNDParam(0, 2);
			m_sType = groupScript;
		}
		// Check for Entity Script
		if ( m_szFileName[0] == 'E' &&
			 m_szFileName[1] == 'S')
		{
			m_sType = entityScript;
		}

		if ( !strcmp(m_szFileName, "main.amx"))
		{
			m_sType = mainScript;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: GetLNDParam()
// Desc:
//-----------------------------------------------------------------------------
long CVirtualFile::GetLNDParam( int nPos, int nExtra )
{
	char szTemp[64];
	unsigned int n,m;
	int nDashCount = 0;
	long lVal;

	for (n = nExtra; n < strlen(m_szFileName); n++)
	{
		if ( m_szFileName[n] == '-')
			nDashCount++;

		if ( nDashCount == nPos)
			break;
	}

	if (nPos != 0)
		n++;

	memset(szTemp, 0, sizeof(szTemp));
	for ( m = n; m < strlen(m_szFileName); m++)
	{
		if (m_szFileName[m] == '-' || m_szFileName[m] == '.' || m_szFileName[m] == NULL)
			break;

		szTemp[m - n] = m_szFileName[m];
	}

	lVal = atol( szTemp );

	if (lVal < 0)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "Error - LNDParam less than 0");
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", m_szFileName);
	}

	return lVal;
}


//-----------------------------------------------------------------------------
// Name: GetFileExtension()
// Desc:
//-----------------------------------------------------------------------------
void CVirtualFile::GetFileExtension( char* szString, size_t sLen )
{
	int n;
	int m = 0;

	memset( szString, 0, sizeof(sLen) );

	// Work through the string backwards until we hit a . char
	for (n = strlen( m_szFileName ) - 1; n > 0; n--)
	{
		if (m_szFileName[n] == '.')
			break;

		szString[m] = m_szFileName[n];
		m++;
	}
}
