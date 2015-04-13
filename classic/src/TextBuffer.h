/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// TextBuffer.h: interface for the CTextBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTBUFFER_H__89B46B4E_4376_40A9_BD40_16DEE4A45915__INCLUDED_)
#define AFX_TEXTBUFFER_H__89B46B4E_4376_40A9_BD40_16DEE4A45915__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "GeneralData.h"
#define MAX_TEXTS 100


// Name: struct _TextBuffer
struct _TextBuffer{
		char	szText[256];
		SDL_Point	sPoint;
		SDL_Color	dwColour;
};



class CTextBuffer
{
	_TextBuffer		m_Text[MAX_TEXTS];
	int				m_nTexts;

public:
	CTextBuffer();
	virtual ~CTextBuffer();

	void AddText( char* szText, SDL_Point point, SDL_Color dwColour);
	void DrawTexts();
};

#endif // !defined(AFX_TEXTBUFFER_H__89B46B4E_4376_40A9_BD40_16DEE4A45915__INCLUDED_)
