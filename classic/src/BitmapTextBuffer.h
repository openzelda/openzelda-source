// BitmapTextBuffer.h: interface for the CTextBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITMAPTEXTBUFFER_H__89B46B4E_4376_40A9_BD40_16DEE4A45915__INCLUDED_)
#define AFX_BITMAPTEXTBUFFER_H__89B46B4E_4376_40A9_BD40_16DEE4A45915__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "GeneralData.h"
#define MAX_BIT_TEXTS 100


// Name: struct _BitmapTextBuffer
struct _BitmapTextBuffer{
		char	szText[256];
		SDL_Point	sPoint;
		SDL_Color	dwColour;
		int		nStyle;
		int		nScale;
};



class CBitmapTextBuffer
{
	_BitmapTextBuffer	m_Text[MAX_BIT_TEXTS];
	int					m_nTexts;

public:
	CBitmapTextBuffer();
	virtual ~CBitmapTextBuffer();

	void AddText( char* szText, SDL_Point point, SDL_Color dwColour, int nStyle, int nScale);
	void DrawTexts();
	void MakeSpriteName(char cCharacter, char* szBuffer, int nStyle);
};

#endif // !defined(AFX_BITMAPTEXTBUFFER_H__89B46B4E_4376_40A9_BD40_16DEE4A45915__INCLUDED_)
