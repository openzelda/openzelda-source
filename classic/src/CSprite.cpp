/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CSprite.cpp
//
// Desc: Holds data on just one sprite in a sprite sheet. Note that this does
//       not hold the actual bitmap as that is in the sprite sheet itself.
//
//
//-----------------------------------------------------------------------------
#include "CSprite.h"
#include "CGame.h"
#include "Global.h"



#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
int GetNearestPower( int length );
unsigned char GetNearest8( int n );

uint32_t GetPixel(SDL_Surface *surface, int32_t x, int32_t y)
{
	if ( surface == NULL )
		return 0;

	if ( x > surface->w || y > surface->h || x < 0 || y < 0 )
		return 0;

	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
		case 1:
			return *p;

		case 2:
			return *(Uint16 *)p;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;

		case 4:
			return *(uint32_t *)p;

		default:
			return 0;	   /* shouldn't happen, but avoids warnings */
	}
}


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CSprite::CSprite()
{
	// Set Some Defualts
	m_fEntity		= false;
	m_fBackground	= false;
	m_wFrames		= 0;
	m_fTopLayer		= false;
	m_fAnimated		= false;
	m_wSpriteID		= 0;
	m_fFill			= false;
	m_fBeenAnimated	= false;
	m_rAnimInc		= 0;
	m_rAnimCount	= 0;
	m_fHasMask		= false;
	m_wWidth		= 0;
	m_wHeight		= 0;
	m_wAlpha		= 255;
	m_pMask			= NULL;
	m_fisMask		= false;
	m_pParent		= NULL;

	memset(m_szMaskString, 0, sizeof(m_szMaskString));
	memset(m_szCode, 0, sizeof(m_szCode));
}


//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CSprite::~CSprite()
{
	m_pTexture.clear();

	if ( m_pMask )
	{
		delete [] m_pMask;
	}
}






//-----------------------------------------------------------------------------
// Name: GetNearestPower
// Desc:
//
//-----------------------------------------------------------------------------
int GetNearestPower(int length)
{
	int n = length;
	int m;

	for(m = 1; m < n; m *= 2);
	if(n <= m)
		return m;
	else
		return m/2;
}
//-----------------------------------------------------------------------------
// Name: CreateTexture
// Desc:
//
//-----------------------------------------------------------------------------

void CSprite::CreateTexture(SDL_Surface * pSurface, RECT2D * sRect)
{
	Texture * pTexture = new Texture;
	SDL_Rect rect;

	// Record our width and height
	m_wWidth  = sRect->right - sRect->left;
	m_wHeight = sRect->bottom - sRect->top;

	if( m_wHeight != 0 && m_wWidth != 0)
	{
		rect.x = sRect->left;
		rect.y = sRect->top;
		pTexture->w = rect.w = (m_wWidth);
		pTexture->h = rect.h = (m_wHeight);

		// Create Texture
		pTexture->buffer = SDL_CreateTexture( GetRendererObject(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, rect.w, rect.h );

		uint32_t * pixels = new uint32_t[rect.w*rect.h]();
		if ( pixels )
		{
			for( uint16_t y = 0; y < rect.h; y++ )
			{
				for( uint16_t x = 0; x < rect.w; x++ )
				{
					uint32_t q = (rect.w * y) + x;
					pixels[q] = GetPixel(pSurface, rect.x + x, rect.y + y);
				}
			}
			SDL_SetTextureBlendMode( pTexture->buffer, SDL_BLENDMODE_BLEND );
			SDL_UpdateTexture( pTexture->buffer, NULL, pixels, rect.w * 4 );
		}

	//	SDL_Surface * surf = SDL_CreateRGBSurfaceFrom(pixels, rect.w, rect.h, 32, rect.w*4, 0,0,0,0);
	//	std::string path = FS_Gamedir();
	//	path.append(m_szCode);
	//	path.append(".bmp");
	//	SDL_SaveBMP( surf, path.c_str() );
		delete pixels;


	//	SDL_RenderClear( GetRendererObject() );
	//	SDL_SetTextureColorMod(pTexture->buffer, 255, 255, 255);
	//	SDL_SetTextureAlphaMod(pTexture->buffer, 255);
	//	SDL_RenderCopy( GetRendererObject(), pTexture->buffer, NULL, NULL );
	//	SDL_RenderPresent( GetRendererObject() );
	//	SDL_Delay(10);





		if ( !pTexture->buffer )
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"CreateTexture: '%s' %s [%dx%d]\n", m_szCode, SDL_GetError(), m_wWidth , m_wHeight );
	}
	// Add this texture to the list
	m_pTexture.push_back( pTexture );
}


//-----------------------------------------------------------------------------
// Name: CopyMask
// Desc:
//
//-----------------------------------------------------------------------------

void CSprite::CopyMask(SDL_Surface * pSurface, RECT2D sRect)
{
	uint32_t y1;
	uint32_t x1;
	uint32_t p1;

	// Calculate the Width and Height of the sprite
	int width  = sRect.right - sRect.left;
	int height = sRect.bottom - sRect.top;
	int size = width * height;

	// Allocate memory for the Mask
	m_pMask = new unsigned char[size];

	// Loop through all pixels in the mask image and copy them to the mask array
	for ( y1 = 0; y1 < height; y1++)
	{
		for ( x1 = 0; x1 < width ; x1++)
		{
			p1 = x1 + (y1 * width);
			if ( p1 <= size )
				m_pMask[p1] = GetCorrectPixelFormat16( pSurface, x1 + sRect.left, y1 + sRect.top );
		}
	}


/*
	char tempname[512];
	SDL_Rect srcRect = GetRect(sRect);
	SDL_Surface * destSurface = SDL_CreateRGBSurface(0, width, height,32, 0,0,0,0);

	SDL_BlitSurface(pSurface, &srcRect, destSurface, NULL);

	snprintf(tempname, 511, "%s.bmp", this->m_szCode );

	SDL_SaveBMP( destSurface, tempname );
*/

}


//-----------------------------------------------------------------------------
// Name: GetNearest8
// Desc: gets the closest multiple of 8
//
//-----------------------------------------------------------------------------
unsigned char GetNearest8( int n )
{
	if (n == 0)
		return 0;

	int m;
	for (m=0; m < n; m+=8);

	if ( m == n)return m;
	if ( m > n )
		return m - 8;

	return 0;
}


//-----------------------------------------------------------------------------
// Name: GetCorrectPixelFormat16()
// Desc:
//
//-----------------------------------------------------------------------------
unsigned char CSprite::GetCorrectPixelFormat16(SDL_Surface *surface, uint32_t x, uint32_t y)
{
	uint32_t n = GetPixel(surface, x,  y);
	return (unsigned char)(n & 0xff);
//	return GetARValue(n);
}

//-----------------------------------------------------------------------------
// Name: GetTexture()
// Desc:
//
//-----------------------------------------------------------------------------
Texture * CSprite::GetTexture( float timer )
{
	Texture * texture = m_pTexture.at(0);
	if ( this->m_fAnimated && this->m_rAnimInc )
	{
		int frame = (int)timer;
		if ( frame < m_wFrames )
		{
			texture = m_pTexture.at(frame);
		}

	}

	return texture;
}
