/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// VirtualFile.h: interface for the CVirtualFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIRTUALFILE_H__01C4B9CC_699C_4A15_B635_FC8E7EDAB4CD__INCLUDED_)
#define AFX_VIRTUALFILE_H__01C4B9CC_699C_4A15_B635_FC8E7EDAB4CD__INCLUDED_
#include <zlib.h>
#include <stdio.h>
#include "CVirtualArchive.h"
class CVirtualArchive;


enum _FileType { normal, textFile, lnd, lndScript, groupScript, entityScript, mainScript, spt, bmp, sound, music };

class CVirtualFile  
{
	long		m_lStart;		// Positions in archive of this file
	long		m_lFinish;
	bool		m_bEncrypted;		// do we need to decrypt it?
	char		m_szFileName[64];	// the filename stored
	_FileType   m_sType;
	gzFile		m_sFilePointer;
	long		m_lParams[3];
	char*		m_pEncryptionKeys;
	long		m_lCurrentPos;
	CVirtualArchive* m_pOwner;

public:
	CVirtualFile( long lStart, long lFinish, bool bEncrypted, char* szFileName );
	virtual ~CVirtualFile();

	void ExtractAMX();
	bool ReadString( char* szString, size_t sLen  );
	long ReadLong();
	int ReadNextChar();
	void ClassifyFile();
	void GetFileExtension( char* szString, size_t sLen );
	long GetLNDParam( int nPos, int nExtra = 0 );
	bool Open();
	void Close();
	bool ExtractTo( char* szDest );
	void SetParam( int index, long val) {m_lParams[index] = val;}


	// Access
	bool GetEncrypted(){ return m_bEncrypted;}
	char* GetVirtualName(){ return m_szFileName;}
	_FileType GetType(){ return m_sType;}
	long GetParam( int nIndex ) { return m_lParams[nIndex];}
	long GetX(){ return m_lParams[0];}
	long GetY(){ return m_lParams[1];}
	void SetOwner( CVirtualArchive* pArchive){ m_pOwner = pArchive;}
	void SetEncryptionKeys( char* pKeys ){ m_pEncryptionKeys = pKeys;}
};

#endif // !defined(AFX_VIRTUALFILE_H__01C4B9CC_699C_4A15_B635_FC8E7EDAB4CD__INCLUDED_)
