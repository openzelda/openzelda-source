/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// VirtualArchive.h: interface for the CVirtualArchive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_VIRTUALARCHIVE_H__)
#define _VIRTUALARCHIVE_H__
#include <zlib.h>
#include <list>
#include <cstring>
#include "CVirtualFile.h"
#include "GeneralData.h"
#include "CQuestLoader.h"

class CQuestLoader;
class CVirtualFile;

class CVirtualArchive  
{
	std::list<CVirtualFile*>	m_Files;					// List of virtual files in archive
	char						m_szFileName[512];			// Filename of archive
	long						m_lHeaderSize;				// Length of header portion
	char						m_cEncryptionKeys[ NUM_ENC_KEYS ];	

	//void LoadFile( FILE *pIn);
	void LoadFile(gzFile pIn);
public:
	CVirtualArchive();
	virtual ~CVirtualArchive();

	void CheckForEntities();
	bool ExtractAMXFiles();
	bool LoadHeader();
	//bool GetNextEntry(char *szString, size_t sLen, FILE * in);
	bool GetNextEntry(char *szString, size_t sLen, gzFile in);
	bool LoadScreenFiles( CQuestLoader* pQuest);
	bool LoadQSSFile( CQuestLoader* pQuest );
	CVirtualFile* GetSheetsFile();
	bool ExtractFile( char* szName, char* szLocation );
	CVirtualFile* FindFile( char* szName );
	bool LoadEntities();

	// Data Brokers
	void SetFileName( char* szFileName ){ strncpy(m_szFileName, szFileName, 512);}

	char* GetFileName(){ return m_szFileName;}
	long GetHeaderLength(){ return m_lHeaderSize;}

};

#endif // !defined(AFX_VIRTUALARCHIVE_H__C45F5E63_D946_4462_8651_A34F9B3E251E__INCLUDED_)
