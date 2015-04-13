/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CDisplay.h
//
// Desc: Direct Draw Display Class
//
//
//-----------------------------------------------------------------------------
#ifndef CDISPLAY_H
#define CDISPLAY_H

#include <SDL2/SDL.h>
#include "CSpriteSheet.h"
#include "ShapeBuffer.h"
#include "TextBuffer.h"
#include "BitmapTextBuffer.h"

#define DEPTH_BUFFER_SIZE 800
#define SPRITE_LAYERS 4


//-----------------------------------------------------------------------------
// Name: struct _DepthBuffer
// Desc: Structure for holding a depth buffer to sprite sprites
//-----------------------------------------------------------------------------
struct _DepthBuffer{
	long            zBufferYPos[DEPTH_BUFFER_SIZE];        // The Z position
	int				zBufferX[DEPTH_BUFFER_SIZE];        // The x and y coordinates
	int             zBufferY[DEPTH_BUFFER_SIZE];
	SDL_Color       sColors[DEPTH_BUFFER_SIZE];            // Any Colors to apply
	int             wScaleFactor[DEPTH_BUFFER_SIZE];    // Scaling factor
	int             wRotation[DEPTH_BUFFER_SIZE];        // Rotation Amount
	CSprite*        pzSprite[DEPTH_BUFFER_SIZE];        // CSprite and CSheet pointers
	int             NumSprites;                            // Num of Sprites in the buffers
	int             mFlip[DEPTH_BUFFER_SIZE];           //Flipping factor
};



//-----------------------------------------------------------------------------
// Name: class CDisplay
// Desc: Class to handle all D3D aspects of a 2D display.
//
//-----------------------------------------------------------------------------
class CDisplay
{
	protected:

	// Internal variables for the state of the app
	bool                    m_bWindowed;
	bool                    m_bActive;
	bool                    m_bReady;

	// Main objects used for creating and rendering the 3D scene
	uint32_t             m_dwCreateFlags;     // Indicate sw or hw vertex processing
	uint32_t             m_dwWindowStyle;     // Saved window style for mode switches
	RECT2D             m_rcWindowBounds;    // Saved window bounds for mode switches
	RECT2D             m_rcWindowClient;    // Saved client area size for mode switches

	// Depth Buffers
	_DepthBuffer            m_sDBuf[SPRITE_LAYERS];        // The depth buffer to hold this frames sprites
	_DepthBuffer            m_sImages;        // Sprite Buffer for overlaid images
	CShapeBuffer*            m_pShapeBuffer;    // Buffer containing lots of shapes
	CTextBuffer*            m_pTextBuffer;
	CBitmapTextBuffer*        m_pBitmapTextBuffer;

	// Internal error handling function
	int32_t DisplayErrorMsg( int32_t hr, uint32_t dwType );
	int32_t InitDeviceObjects();
	int32_t RestoreDeviceObjects();
	int32_t InvalidateDeviceObjects();
	int32_t DeleteDeviceObjects();

	// =========================================
	// Public Functions
	// =========================================
	public:
	CDisplay();
	~CDisplay();

	void PostInitialize(long WindowWidth, long WindowHeight);

	void Blit2(Texture * pTexture,
			 SDL_Point * pDest,
			 SDL_Point* pCentre,
			 double rScale,
			 double angle,
			 SDL_Color colour, int flip);

	void BlitTiled(Texture * pTexture, SDL_Point * pDest,
				 SDL_Point* pCentre, double rScale,
				 double angle, int width, int height, SDL_Color colour);

	int32_t PutSprite( int x, int y, CSprite* pSprite, long wDepth, SDL_Color sCol,
					 int wScale, int rRotation, int nLayer = 0, int flip = 0);

	int32_t PutImage( int x, int y, CSprite* pSprite, SDL_Color sCol,
					 int wScale, int rRotation, int flip = 0);
	void    FlushSpriteBuffers();

	int32_t SortSprites( int nLayer = 0 );
	int32_t DrawImages();
	void SimpleDraw(int x, int y, CSprite* pSprite,
					SDL_Color modulate, float rScale, float rRotation, float fExpectedScale = 1, int flip = 0);

	void SimpleTile(int x, int y,int width, int height, CSprite* pSprite,
					SDL_Color modulate);

	void Present();
	int32_t UserSelectNewDevice();
	int32_t ToggleFullscreen();
	void Rectangle(RECT2D pDest, float angle, SDL_Color colour, bool bNeedCenter = false);
	void Line( long x1, long y1, long x2, long y2, int nWid, SDL_Color colour);


	// ======================
	// Access functions
	// ======================
	CShapeBuffer* GetShapeBuffer(){ return m_pShapeBuffer;}
	CTextBuffer* GetTextBuffer(){ return m_pTextBuffer;}
	CBitmapTextBuffer* GetBitmapTextBuffer(){ return m_pBitmapTextBuffer;}

};

#endif
