/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CDisplay.cpp
//
// Desc: Direct Draw Display Class
//
//
//-----------------------------------------------------------------------------
#include <cmath>
#include <cstdio>
#include <iostream>
#include "Global.h"
#include "CDisplay.h"
#include "GeneralData.h"
#include "CGame.h"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern float			g_rDelta;
extern CGame *			g_pGame;
extern SDL_Surface *	g_hWnd;
extern int				g_wScreenWidth;
extern int				g_wScreenHeight;

#define S_OK 1


//-----------------------------------------------------------------------------
// Name: CDisplay()
// Desc:
//-----------------------------------------------------------------------------
CDisplay::CDisplay()
{
	int n;

	// Init Sprite Depth buffers
	for ( n = 0; n < SPRITE_LAYERS; n++ )
		m_sDBuf[n].NumSprites     = 0;

	m_sImages.NumSprites = 0;

	// Create the shape buffer
	m_pShapeBuffer = new CShapeBuffer();
	m_pTextBuffer = new CTextBuffer();
	m_pBitmapTextBuffer = new CBitmapTextBuffer();
	InitDeviceObjects();
}


//-----------------------------------------------------------------------------
// Name: ~CDisplay()
// Desc:
//
//-----------------------------------------------------------------------------
CDisplay::~CDisplay()
{
	m_bActive = false;
	m_bReady = false;


	SAFE_DELETE( m_pShapeBuffer );
	SAFE_DELETE( m_pTextBuffer );
	SAFE_DELETE( m_pBitmapTextBuffer );
}




//----------------------------------------------------------------------------
// Rectangle()
//
//
//----------------------------------------------------------------------------
void CDisplay::Rectangle(RECT2D pDest, float angle, SDL_Color colour, bool bNeedCenter )
{
	SDL_Rect dest = GetRect(pDest);

	if (bNeedCenter)
	{
		//dest.x += dest.w/2;
		//dest.y += dest.h/2;
	}



	SDL_SetRenderDrawBlendMode(GetRendererObject(), SDL_BLENDMODE_BLEND );
	SDL_SetRenderDrawColor(GetRendererObject(),colour.r,colour.g,colour.b,colour.a);
	SDL_RenderFillRect(GetRendererObject(), &dest);
}



//----------------------------------------------------------------------------
// Line()
//
//
//----------------------------------------------------------------------------
void CDisplay::Line( long x1, long y1, long x2, long y2, int nWid, SDL_Color colour)
{
	SDL_SetRenderDrawColor(GetRendererObject(),colour.r,colour.g,colour.b,colour.a);
	SDL_RenderDrawLine(GetRendererObject(), x1, y1, x2, y2);
}


//----------------------------------------------------------------------------
// Blit2()
//
//
//----------------------------------------------------------------------------
void CDisplay::Blit2( Texture * pTexture, SDL_Point * pDest, SDL_Point * pCentre, double rScale, double angle, SDL_Color colour, int flip )
{
	SDL_Point centre;
	SDL_Rect dest;

	dest.x = pDest->x;
	dest.y = pDest->y;
	dest.w = pTexture->w * rScale;
	dest.h = pTexture->h * rScale;

	/*
	if (pCentre)
	{
		centre.x = pCentre->x;
		centre.y = pCentre->y;
	}
	else
	{
		centre.x = dest.w/2;
		centre.y = dest.h/2;
	}
	*/

	SDL_SetTextureColorMod(pTexture->buffer, colour.r, colour.g, colour.b);
	SDL_SetTextureAlphaMod(pTexture->buffer, colour.a);

	if ( pCentre && (pCentre->x || pCentre->y) )
	{
		dest.x -= (pCentre->x * rScale);
		dest.y -= (pCentre->y * rScale);

		SDL_RenderCopyEx( GetRendererObject(), pTexture->buffer, NULL, &dest, angle, NULL, SDL_FLIP_NONE );


		SDL_SetRenderDrawColor(GetRendererObject(), 255, 0, 0, 255);
		SDL_RenderDrawRect( GetRendererObject(), &dest );

	}
	else
	{
		SDL_RenderCopyEx( GetRendererObject(), pTexture->buffer, NULL, &dest, angle, NULL, SDL_FLIP_NONE );
	}


}

//----------------------------------------------------------------------------
// BlitTiled()
//
//
//----------------------------------------------------------------------------
void CDisplay::BlitTiled( Texture * pTexture,  SDL_Point* pDest, SDL_Point* pCentre, double rScale, double angle, int width, int height, SDL_Color colour)
{
	SDL_Point centre;
	SDL_Rect dest,draw;

	dest.x = pDest->x;
	dest.y = pDest->y;
	dest.w = draw.w = pTexture->w * rScale;
	dest.h = draw.h = pTexture->h * rScale;

	width *= rScale;
	height *= rScale;

	centre.x = pDest->x + pCentre->x;
	centre.y = pDest->y + pCentre->y;

	SDL_SetTextureColorMod(pTexture->buffer, colour.r, colour.b, colour.g);
	SDL_SetTextureAlphaMod(pTexture->buffer, colour.a);

	for( int32_t rx = 0; rx < width; rx += dest.w )
	{
		for( int32_t ry = 0; ry < height; ry += dest.h )
		{
			draw.x = dest.x + rx;
			draw.y = dest.y + ry;

			SDL_RenderCopyEx( GetRendererObject(), pTexture->buffer, NULL, &draw, angle, &centre, SDL_FLIP_NONE );

		}
	}
/*
	SDL_Rect area;
	area = dest;
	area.w = width;
	area.h = height;

	SDL_SetRenderDrawColor(GetRendererObject(), 0, 255, 0, 255);
	SDL_RenderDrawRect( GetRendererObject(), &area );
*/
}


//-----------------------------------------------------------------------------
// Name: SimpleDraw()
// Desc: Draws a sprite directly to the screen
//
//-----------------------------------------------------------------------------
void CDisplay::SimpleDraw(int x, int y, CSprite* pSprite, SDL_Color modulate, float rScale, float rRotation, float fExpectedScale, int flip)
{
	SDL_Point dest = {x,y};
	SDL_Point centre ={ 0, 0 };

	if ( rRotation != 0 || rScale != fExpectedScale )
	{
		// Only work out the centre if needed
		centre.x = pSprite->m_wWidth / 2;
		centre.y = pSprite->m_wHeight / 2;
	}

	if ( g_pGame->m_updates )
	{
		dest.x = x + (centre.x*(rScale - 1));
		dest.y = y + (centre.y*(rScale - 1));
	}

	// Save some time here if we know that we are only dealing with the first frame in
	// an animation, all non animated sprites have only 1 frame
	Blit2( pSprite->GetTexture( pSprite->m_rAnimCount ), &dest, &centre, rScale, rRotation, modulate, flip );

}

//-----------------------------------------------------------------------------
// Name: SimpleTile()
// Desc: Tiles a sprite directly to the screen
//
//-----------------------------------------------------------------------------
void CDisplay::SimpleTile(int x, int y,int width, int height, CSprite* pSprite, SDL_Color modulate)
{

	SDL_Point dest;
	dest.x = x;
	dest.y = y;

	SDL_Point centre;
	centre.x = 0;
	centre.y = 0;


	BlitTiled( pSprite->GetTexture( pSprite->m_rAnimCount ), &dest, &centre, 2.0, 0, width, height, modulate);

}


//-----------------------------------------------------------------------------
// Name: PutSprite()
// Desc: Places a sprite in the Sprite Buffer
//-----------------------------------------------------------------------------
int32_t CDisplay::PutSprite( int x, int y, CSprite* pSprite, long wDepth, SDL_Color sCol, int wScale, int rRotation, int nLayer, int flip )
{
	nLayer++;
	if ( nLayer >= SPRITE_LAYERS )
		nLayer = SPRITE_LAYERS - 1;

	// Get the Curent Sprite Buffer
	_DepthBuffer* pBuffer = &m_sDBuf[nLayer];

	if ( pBuffer->NumSprites >= DEPTH_BUFFER_SIZE )
		return S_OK;

	int n = pBuffer->NumSprites;

	// Record all supplied information
	pBuffer->zBufferX[n] = x << ENLARGE_FACTOR;
	pBuffer->zBufferY[n] = y << ENLARGE_FACTOR;
	pBuffer->pzSprite[n] = pSprite;
	pBuffer->sColors[n] = sCol;
	pBuffer->wScaleFactor[n] = wScale << ENLARGE_FACTOR;
	pBuffer->wRotation[n] = rRotation;
	pBuffer->zBufferYPos[n] = wDepth;
	pBuffer->mFlip[n] = flip;
	pBuffer->NumSprites++;
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: PutImage()
// Desc: Places a sprite in the image Buffers
//-----------------------------------------------------------------------------
int32_t CDisplay::PutImage( int x, int y,
					 CSprite* pSprite, SDL_Color sCol,
					 int wScale, int rRotation, int flip)
{
	if ( m_sImages.NumSprites >= DEPTH_BUFFER_SIZE)
		return S_OK;

	int n = m_sImages.NumSprites;

	// Record all supplied information
	m_sImages.zBufferX[n] = x;
	m_sImages.zBufferY[n] = y;
	m_sImages.pzSprite[n] = pSprite;
	m_sImages.sColors[n] = sCol;
	m_sImages.wScaleFactor[n] = wScale;
	m_sImages.wRotation[n] = rRotation;
	m_sImages.mFlip[n] = flip;
	m_sImages.NumSprites++;
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DrawImages()
// Desc: Sorts and displays images in the buffers
//
//-----------------------------------------------------------------------------
int32_t CDisplay::DrawImages()
{
	int n;

	for (n = 0; n < m_sImages.NumSprites; n++)
	{
		SimpleDraw(m_sImages.zBufferX[n],
				 m_sImages.zBufferY[n],
				 m_sImages.pzSprite[n],
				 m_sImages.sColors[n],
				 float(m_sImages.wScaleFactor[n]) / 100,
				 DegToRad(m_sImages.wRotation[n]),
				 1, m_sImages.mFlip[n]);
	}

	m_sImages.NumSprites = 0;
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SortSprites()
// Desc: Sorts and displays sprites in the depth buffers
//
//-----------------------------------------------------------------------------
int32_t CDisplay::SortSprites( int nLayer )
{
	long TempSmall=9999999; // temporary smallest number
	long TempSmall2=0;
	int n, m;

	// Get the Curent Sprite Buffer
	_DepthBuffer* pBuffer = &m_sDBuf[nLayer];

	for (m = 0; m < pBuffer->NumSprites; m++)
	{
		for (n = 0; n < pBuffer->NumSprites; n++)
		{
			if (pBuffer->zBufferYPos[n] < TempSmall && pBuffer->zBufferYPos[n] != -100)
			{
				TempSmall = pBuffer->zBufferYPos[n];
				TempSmall2 = n;
			}
		}

		// Make sure the pointers are valid
		if (pBuffer->pzSprite[TempSmall2])
		{
			// If this isnt an animated sprite then just draw it as normal
			//if (!pBuffer->pzSprite[TempSmall2]->m_fAnimated)
			//{

				SimpleDraw(pBuffer->zBufferX[TempSmall2],
						 pBuffer->zBufferY[TempSmall2],
						 pBuffer->pzSprite[TempSmall2],
						 //g_pGame->CombineColors(pBuffer->sColors[TempSmall2]),
						 pBuffer->sColors[TempSmall2],
						 float(pBuffer->wScaleFactor[TempSmall2]) / 100,
						 DegToRad(pBuffer->wRotation[TempSmall2]),
						 2,pBuffer->mFlip[TempSmall2]);

			//}
		}

		pBuffer->zBufferYPos[TempSmall2]=-100;
		TempSmall=9999999;
		TempSmall2=0;
	}

	pBuffer->NumSprites = 0;
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FlushSpriteBuffers()
// Desc:
//
//-----------------------------------------------------------------------------
void CDisplay::FlushSpriteBuffers()
{
	int n;
	m_sImages.NumSprites = 0;

	for (n = 0; n < SPRITE_LAYERS; n++)
		m_sDBuf[n].NumSprites = 0;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc:
//
//-----------------------------------------------------------------------------
int32_t CDisplay::InitDeviceObjects()
{
	g_pGame->InitDeviceObjects();
	return 1;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc:
//
//-----------------------------------------------------------------------------
int32_t CDisplay::DeleteDeviceObjects()
{
	g_pGame->DeleteDeviceObjects();
	return 1;
}
