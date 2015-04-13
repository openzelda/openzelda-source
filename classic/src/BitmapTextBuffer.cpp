/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// TextBuffer.cpp: implementation of the CTextBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include <ctype.h>
#include "BitmapTextBuffer.h"
#include "CGame.h"

extern CGame*		g_pGame;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitmapTextBuffer::CBitmapTextBuffer()
{
	m_nTexts = 0;
}

CBitmapTextBuffer::~CBitmapTextBuffer()
{

}

void CBitmapTextBuffer::AddText( char* szText, SDL_Point point, SDL_Color dwColour, int nStyle, int nScale)
{
	if (m_nTexts >= MAX_BIT_TEXTS )
		return;

	strcpy( m_Text[m_nTexts].szText, szText );
	m_Text[m_nTexts].sPoint   = point;
	m_Text[m_nTexts].dwColour = dwColour;
	m_Text[m_nTexts].nStyle   = nStyle;
	m_Text[m_nTexts].nScale   = nScale;
	m_nTexts++;
}


void CBitmapTextBuffer::DrawTexts()
{
	int n;
	int m;
	char szBuffer[11];
	int x;
	int y;
	int wid = 8;
	CSprite* pSprite		= NULL;
	int spacer = 8;

	for ( n = 0 ; n < m_nTexts; n++ )
	{
		x = m_Text[n].sPoint.x;
		y = m_Text[n].sPoint.y;
		sprintf(szBuffer, "_char%d%d", m_Text[n].nStyle, 32);
		//WriteCheckTextVal(szBuffer, m_Text[n].nStyle);
		spacer = g_pGame->GetWidthHeight(szBuffer, true);

		wid = spacer * (m_Text[n].nScale / 100);

		for ( m = 0 ; m < strlen(m_Text[n].szText); m++ )
		{
			// Make a sprite name out of each character
			MakeSpriteName( m_Text[n].szText[m], szBuffer, m_Text[n].nStyle);

			if ( g_pGame->GetSSheetSet() )
			{
				// Return the Sprite object with a matching code
				pSprite = g_pGame->GetSSheetSet()->GetSprite(szBuffer, true);

				if (pSprite)
				{
					SDL_Color whie = {255,255,255,255};
					g_pGame->GetDisplay()->PutImage( x, y, pSprite, m_Text[n].dwColour, m_Text[n].nScale, 0 );
					x += wid;
				}
			}
		}
	}

	m_nTexts = 0;
}


void CBitmapTextBuffer::MakeSpriteName(char cCharacter, char* szBuffer, int nStyle)
{
	sprintf(szBuffer, "_char%d%d", nStyle, toupper(cCharacter));
}
