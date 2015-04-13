/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CSprite.h
//
// Desc: Holds data on just one sprite in a sprite sheet. Note that this does
//       not hold the actual bitmap as that is in the sprite sheet itself.
//
//-----------------------------------------------------------------------------
#ifndef CSPRITE_H
#define CSPRITE_H

#include <vector>
#include <SDL2/SDL.h>
#include "CSpriteSheet.h"
#include "GeneralData.h"


//-----------------------------------------------------------------------------
// Classes defined in this header file
//-----------------------------------------------------------------------------
class CSprite;
class CSpriteSheet;


//-----------------------------------------------------------------------------
// Name: class CSprite
// Desc: Class to house a single sprite
//
//-----------------------------------------------------------------------------
class CSprite
{
public:
	CSprite();
	~CSprite();
	void CreateTexture( SDL_Surface * pSurface, RECT2D * sRect );
	void CopyMask( SDL_Surface * pSurface, RECT2D sRect );
	unsigned char GetCorrectPixelFormat16( SDL_Surface *surface, uint32_t x, uint32_t y );

	Texture * GetTexture( float timer );


	std::vector<Texture*>	 m_pTexture; // Linked list of Textures
	unsigned char* m_pMask;			// Mask Data
	bool m_fEntity;			// Is This Sprite an ENTITY sprite?
	bool m_fBackground;		// Is this a BackGround Sprite?
	int  m_wFrames;			// Frames of animation this BG sprite has
	bool m_fTopLayer;		// Is this sprite on the top layer or not?
	bool m_fAnimated;		// Is this BG Sprite animated?
	char m_szMaskString[ MAX_CHARS ];// If this sprite has a mask then what is it's Code?
	int  m_wSpriteID;		// the ID of this sprite within the sheet;
	char m_szCode[ MAX_CHARS ];		// This sprite might have a code for LND loading
	bool m_fFill;			// Is the Sprite a Fill tile?
	bool m_fHasMask;
	int	 m_wWidth;
	int  m_wHeight;
	uint8_t m_wAlpha;			// Alpha Value
	bool m_fisMask;			// Is this sprite a Mask?
	RECT2D m_sRect;			// Position of texture on sprite sheet
	CSpriteSheet * m_pParent;

	// Animation Details
	bool  m_fBeenAnimated;	// Has this sprite been animated this frame?
	float m_rAnimInc;		// animation increment.
	float m_rAnimCount;		// animation counter.
};


#endif

