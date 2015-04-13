/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef TTFFONT_H
#define TTFFONT_H

#include <SDL2/SDL.h>
//#include <SDL2/SDL_ttf.h>

class TextFont
{
	private:
		//TTF_Font * font;
		uint32_t m_dwFontHeight;
	public:
		TextFont( char * name, uint32_t size );
		~TextFont();
		uint32_t GetSize();
		uint32_t DrawText(int x, int y, SDL_Color dwColor, char * strText );
		uint32_t DrawTextScaled( int x, int y, int z, float fXScale, float fYScale, SDL_Color dwColor, char * strText );
		uint32_t GetTextExtent(char * strText, int32_t * piWidth , int32_t * piHeight);
		Texture * CreateTextTexture( SDL_Color dwColor, char * strText );
};

#endif // TTFFONT_H
