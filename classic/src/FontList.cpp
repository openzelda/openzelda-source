/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "Global.h"
#include "FontList.h"
#include "BitFont.h"

CFontList::CFontList()
{
	m_pCurrentFont = NULL;

	this->LoadBitmapFont();

	this->CreateFont( 8 );
	this->SetFontSize( 8 );
}


CFontList::~CFontList()
{
	m_pFonts.erase(m_pFonts.begin(), m_pFonts.end());
	this->UnloadBitmapFont();
}


void CFontList::LoadBitmapFont()
{
	uint8_t * font_point = &gfxPrimitivesFontdata[0];
	uint8_t i = 0, q;
	uint16_t * charflip = new uint16_t[64];

	for ( uint8_t c = 0; c < 128; c++)
	{
		for (i = 0; i < 8; i++)
		{
			for (q = 0; q < 8; q++)
			{
				charflip[(i*8) + q] =  (!!(font_point[i] & (1 << (8-q))) ? 0xFFFF : 0x000F);
			}
		}
		this->font[c] = SDL_CreateTexture( GetRendererObject(), SDL_PIXELFORMAT_ARGB1555, SDL_TEXTUREACCESS_STATIC, 8, 8 );
		if ( this->font[c] )
		{
			//SDL_SetTextureAlphaMod(native_font[c],255);
			SDL_SetTextureBlendMode(font[c],SDL_BLENDMODE_BLEND);
			SDL_UpdateTexture(font[c], NULL, charflip, 16);
		}

		font_point += 8;
	}
	delete charflip;
}

void CFontList::UnloadBitmapFont()
{
	for ( uint8_t c = 0; c < 128; c++)
	{
		SDL_DestroyTexture( this->font[c] );
	}
}

void CFontList::SetFontSize( int nSize )
{
	TextFont * pLast = NULL;
	m_pCurrentFont = NULL;

	// Go through the list and get the closest size
	if ( m_pFonts.empty() )
		return;


	for ( std::list<TextFont*>::iterator it = m_pFonts.begin(); it != m_pFonts.end(); ++it )
	{
		if ( (*it)->GetSize() > nSize )
			break;
		pLast = (*it);
	}

	if ( pLast )
		m_pCurrentFont = pLast;

}

TextFont * CFontList::GetCurrentFont()
{
	return m_pCurrentFont;
}

SDL_Texture * CFontList::GetTexture(char q)
{
	if ( q < 128 )
	{
		return font[q];
	}
	return font['?'];
}


uint32_t CFontList::DrawText( int x, int y, int scale, SDL_Color dwColor, char * strText )
{
	SDL_Rect dest = { x, y, 8, scale };
	for ( uint32_t i = 0; strText[i]; i++ )
	{
		SDL_Texture * pTexture = GetTexture(strText[i]);

		SDL_SetTextureColorMod(pTexture,dwColor.r,dwColor.g,dwColor.b);
		//SDL_SetTextureAlphaMod(pTexture,dwColor.a);

		SDL_RenderCopy( GetRendererObject(), pTexture, NULL, &dest );

		dest.x += 8;
	}


	return 0;
}

TextFont * CFontList::CreateFont( int nSize )
{
	TextFont* font = new TextFont( QUEST_FONT, nSize );
	m_pFonts.push_back( font );
	return font;
}

