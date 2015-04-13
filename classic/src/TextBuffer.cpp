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

#include "TextBuffer.h"
#include "CGame.h"

extern CGame*		g_pGame;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextBuffer::CTextBuffer()
{
	m_nTexts = 0;

}

CTextBuffer::~CTextBuffer()
{

}

void CTextBuffer::AddText( char* szText, SDL_Point point, SDL_Color dwColour)
{
	if (m_nTexts >= MAX_TEXTS )
		return;

	strcpy( m_Text[m_nTexts].szText, szText );
	m_Text[m_nTexts].sPoint   = point;
	m_Text[m_nTexts].dwColour = dwColour;
	m_nTexts++;
}


void CTextBuffer::DrawTexts()
{
	int n;

	if (!g_pGame->GetFontList()->GetCurrentFont())
		return;

	for ( n = 0 ; n < m_nTexts; n++ )
	{
		g_pGame->GetFontList()->GetCurrentFont()->DrawText( m_Text[n].sPoint.x,
															m_Text[n].sPoint.y,
															m_Text[n].dwColour,
															m_Text[n].szText);
	}

	m_nTexts = 0;
}
