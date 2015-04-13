/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CScreen.cpp
//
// Desc:
//
//
//-----------------------------------------------------------------------------
#define STRICT
#include "CScreen.h"
#include "CGame.h"
#include "GeneralData.h"
#include "Script.h"

extern float	g_rDelta;
extern CGame *	g_pGame;

#include <cmath>

void MaskChar::setValue( unsigned char value )
{
	float v = value;
	v /= (255/31);

	v8 = value;
	v5 = (unsigned char)(v) % 32;

}


unsigned char MaskChar::value()
{
	return v5 * 8;
}

//-----------------------------------------------------------------------------
// CScreen Constructor
//-----------------------------------------------------------------------------
CScreen::CScreen()
{
	m_x                = -VSCREEN_WIDTH;
	m_y                = 0;
	m_pAmx            = NULL;
	m_pProgram        = NULL;
	m_pCurrentScreen= NULL;
	m_inUse            = false;
	m_fInside        = false;
	m_lNumSprites    = 0;
	m_lFills        = 0;
	m_lNumHoles        = 0;

	Initialise();
}

//-----------------------------------------------------------------------------
// CScreen destructor
//-----------------------------------------------------------------------------
CScreen::~CScreen()
{
	SAFE_DELETE(m_pAmx);
	free(m_pProgram);
}


//-----------------------------------------------------------------------------
// Name: Initialise()
// Desc:
//-----------------------------------------------------------------------------
void CScreen::Initialise()
{
	// Clear the Collision Detection Mask
	ClearMask();
}

//-----------------------------------------------------------------------------
// Name: PreLoadScreen()
// Desc:
//-----------------------------------------------------------------------------
bool CScreen::PreLoadScreen(long Xp, long Yp)
{
	// Get the virtual file which represents this screen
	CVirtualFile* pFile = NULL;
	pFile = g_pGame->GetMap()->ReturnScreen( Xp, Yp );

	if (!pFile)
	{
		// The point was not on any screen so find an alternate screen
		if (Xp >= g_pGame->GetMap()->GetBounds().right)
		{
			pFile = g_pGame->GetMap()->ReturnScreen(Xp - SCREEN_WIDTH , Yp );
			if (pFile)goto done;
		}

		if (Xp < g_pGame->GetMap()->GetBounds().left)
		{
			pFile = g_pGame->GetMap()->ReturnScreen(Xp + SCREEN_WIDTH , Yp );
			if (pFile)goto done;
		}

		if (Yp >= g_pGame->GetMap()->GetBounds().bottom)
		{
			pFile = g_pGame->GetMap()->ReturnScreen(Xp, Yp - SCREEN_HEIGHT );
			if (pFile)goto done;
		}

		if (Yp < g_pGame->GetMap()->GetBounds().top)
		{
			pFile = g_pGame->GetMap()->ReturnScreen(Xp, Yp + SCREEN_HEIGHT );
			if (pFile)goto done;
		}

		// If the program reaches here then it doesnt have a screen yet, its probably
		// a bottom right screen.
		pFile = g_pGame->GetMap()->ReturnScreen(Xp - SCREEN_WIDTH , Yp - SCREEN_HEIGHT);
	}

done:

	if (pFile)
	{
		// Load the screens information into the buffers
		LoadScreen(pFile);
		m_inUse = true;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Name: LoadScreen()
// Desc: Loads a LND file from a Virtual File into the screen's buffers
//
//-----------------------------------------------------------------------------
bool CScreen::LoadScreen( CVirtualFile* pFile )
{
	int n;
	int nCount = 0;
	char szBuffer[64];

	// Reset all the screen's data
	m_lNumSprites    = 0;
	m_lFills        = 0;
	m_lNumHoles        = 0;
	m_fInside        = pFile->GetParam(2);

	// Clear the mask
	ClearMask();

	// Load the LND file from the Virtual File
	if (!pFile->Open())
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","*** LoadScreen() - Couldn't Open Virtual File***");
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n",pFile->GetVirtualName());
		return false;
	}

	// Get the number of sprites
	m_lNumSprites = pFile->ReadLong();

	// Read each sprite entry
	for ( n = 0 ; n < m_lNumSprites; n++ )
	{
		m_Sprites[nCount].X = pFile->ReadLong() / 2;
		m_Sprites[nCount].Y = pFile->ReadLong() / 2;

		// Get the Sprite code stored
		pFile->ReadString( szBuffer, 64 );

		// Get a pointer to a sprite and store it
		m_Sprites[nCount].pSprite = g_pGame->GetSSheetSet()->GetSprite( szBuffer, false);

		// If this sprite returned NULL then we dont want it
		if (m_Sprites[nCount].pSprite)
		{
			// Keep entities in - otherwise we cant draw their masks
			nCount ++;
		}

		// Read the id - this is just for entities though so discard it
		pFile->ReadString( szBuffer, 64 );
	}

	// NumSprites may have to be adjusted
	m_lNumSprites = nCount;

	// Do The Fills
	// Get the number of Fills
	m_lFills = pFile->ReadLong();
	nCount = 0;

	for ( n = 0 ; n < m_lFills; n++ )
	{
		// Load the RECT2D
		m_Fills[nCount].FillRect.left = pFile->ReadLong() / 2;
		m_Fills[nCount].FillRect.top    = pFile->ReadLong() / 2;
		m_Fills[nCount].FillRect.right = pFile->ReadLong() / 2;
		m_Fills[nCount].FillRect.bottom = pFile->ReadLong() / 2;

		// Get the Sprite code stored
		pFile->ReadString( szBuffer, 64 );

		// Check for Holes - codes hole001, hole002
		if (strstr (szBuffer, "hole001" ))
		{
			AddHole( m_Fills[nCount].FillRect, 1);
			continue;
		}

		if (strstr (szBuffer, "hole002" ))
		{
			AddHole( m_Fills[nCount].FillRect, 2);
			continue;
		}

		// Get a pointer to a sprite and store it
		m_Fills[nCount].pSprite = g_pGame->GetSSheetSet()->GetSprite( szBuffer, false);

		// If this sprite returned NULL then we dont want it
		if (m_Fills[nCount].pSprite)
		{
			nCount ++;
		}
	}

	// NumFills may have to be adjusted - to account for entities
	m_lFills = nCount;

	pFile->Close();

	m_x = pFile->GetX() * VSCREEN_WIDTH;
	m_y = pFile->GetY() * VSCREEN_HEIGHT;

	// Load the Script file for this screen
	LoadScript();

	// Load this screens mask
	LoadMask();
	return true;
}


//-----------------------------------------------------------------------------
// Name: LoadScript()
// Desc:
//
//-----------------------------------------------------------------------------
bool CScreen::LoadScript()
{
	char szString[32];

	// Now we have loaded the LND file info, load the
	// Script for this Screen.
	// Build the filename of the AMX file
	snprintf(szString, 31, "%d-%d.amx", m_x / VSCREEN_WIDTH,
										 m_y / VSCREEN_HEIGHT);

	// Delete the Abstract machine for the Current Script
	SAFE_DELETE( m_pAmx )

	// Re-create the AMX for a new script
	m_pAmx = new AMX();

	// if the program memory has been reserved then free it
	if (m_pProgram)
		free(m_pProgram);

	// Load the new script
	m_pProgram = loadprogram(m_pAmx, szString);
	//RegisterNatives(*m_pAmx);
	return true;
}


//-----------------------------------------------------------------------------
// Name: RunScript()
// Desc:
//
//-----------------------------------------------------------------------------
void CScreen::RunScript()
{
	cell ret;

	if (m_pAmx)
	{
		cell HeapBefore = m_pAmx->hea;
		amx_Exec(m_pAmx, &ret, AMX_EXEC_MAIN, 0);
		m_pAmx->hea = HeapBefore;
		m_pAmx->firstrun = 0;
	}
}


//-----------------------------------------------------------------------------
// Name: LoadMask()
// Desc: Called when a screen is loaded - this loads the mask layer for the
//         Screen.
//
//-----------------------------------------------------------------------------
void CScreen::LoadMask()
{
	long n;
	int x,y;
	CSprite* pSprite        = NULL;
	CSprite* pMaskSprite    = NULL;
	CSpriteSheet* pMaskSheet= NULL;
	int nWidth;
	int nHeight;

	// ====================
	// Go through all fills
	// ====================
	for (n = 0; n < m_lFills; n++)
	{
		// Return the Sprite object with a matching code
		pSprite = m_Fills[n].pSprite;

		if ( pSprite->m_fHasMask )
		{
			// Return the Sprite object with a matching code to that of this
			// Sprites Mask Code
			pMaskSprite = g_pGame->GetSSheetSet()->GetSprite(pSprite->m_szMaskString, false);

			if (pMaskSprite)
			{
				// Get the Sheet this Sprite is on
				pMaskSheet = g_pGame->GetSSheetSet()->GetSpriteSheet(pMaskSprite);

				if (pMaskSheet)
				{
					nWidth = pSprite->m_wWidth;
					nHeight = pSprite->m_wHeight;

					for (y = m_Fills[n].FillRect.top; y < m_Fills[n].FillRect.bottom; y+= nWidth )
					{
						for (x = m_Fills[n].FillRect.left; x < m_Fills[n].FillRect.right; x+= nHeight)
							CopySpriteMask( x, y, pMaskSprite);
					}
				}
			}
		}
	}

	// ======================
	// Go through all sprites
	// ======================
	for (n = 0; n < m_lNumSprites; n++)
	{
		// Get a pointer to the sprite
		pSprite = m_Sprites[n].pSprite;

		// Check if sprite has mask
		if ( pSprite->m_fHasMask )
		{

			// Return the Sprite object with a matching code to that of this
			// Sprites Mask Code
			pMaskSprite = g_pGame->GetSSheetSet()->GetSprite(pSprite->m_szMaskString, false);

			if (pMaskSprite)
			{
				// Get the Sheet this Sprite is on
				pMaskSheet = g_pGame->GetSSheetSet()->GetSpriteSheet(pMaskSprite);

				if (pMaskSheet)
					CopySpriteMask( m_Sprites[n].X,
									m_Sprites[n].Y,
									pMaskSprite);
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Name: DrawLayer()
// Desc: Called each frame to draw the background images
//
//-----------------------------------------------------------------------------
bool CScreen::DrawLayer(int wLayer)
{
	CSprite* pSprite        = NULL;
	RECT2D TempRect, r2;
	int tx, ty, o;

	// This ones for sprites
	RECT2D r1 = {g_pGame->GetWorldX() ,
			 g_pGame->GetWorldY() ,
			 g_pGame->GetWorldX() + VSCREEN_WIDTH ,
			 g_pGame->GetWorldY() + VSCREEN_HEIGHT };


	// Pre-calculate 1 colour value for every sprite
	SDL_Color dwBaseColor = { 255,255,255,255 };


	// ===========================
	// Draw the Fills
	// ===========================
	for (o = 0; o < m_lFills; o++)
	{
		// Get this fills sprite
		pSprite = m_Fills[o].pSprite;

		// Check its not an entity
		if (pSprite->m_fEntity)
			continue;

		// Check this sprite is on the correct layer
		if (wLayer == 1 && pSprite->m_fTopLayer)
			continue;
		if (wLayer == 2 && !pSprite->m_fTopLayer)
			continue;

		r2 = m_Fills[o].FillRect;
		r2.left += m_x;
		r2.right += m_x;
		r2.top += m_y;
		r2.bottom += m_y;

		// Check this sprite is on the screen - otherwise dont draw it
		if (!IntersectRect( &TempRect, &r1, &r2))
			continue;

		CompareAndCopyFill(pSprite, o, m_Fills[o].FillRect, dwBaseColor);
	}

	// ===========================
	// Draw the Sprites
	// ===========================
	for (o = 0; o < m_lNumSprites; o++)
	{
		// Return the Sprite object with a matching code
		pSprite = m_Sprites[o].pSprite;

		// Check its not an entity
		if (pSprite->m_fEntity)
			continue;

		// Check this sprite is on the correct layer
		if (wLayer == 1 && pSprite->m_fTopLayer)
			continue;
		if (wLayer == 2 && !pSprite->m_fTopLayer)
			continue;

		tx = m_x + (m_Sprites[o].X);
		ty = m_y + (m_Sprites[o].Y);

		// Create a rectangle with the coordinates of the sprite
		r2.left = tx;
		r2.top = ty;
		r2.right = tx + (pSprite->m_wWidth);
		r2.bottom = ty + (pSprite->m_wHeight);

		// Check this sprite is on the screen - otherwise dont draw it
		if (!IntersectRect( &TempRect, &r2, &r1))
			continue;

		CompareAndCopy(pSprite, o, dwBaseColor);
	}

	return true;
}


//-----------------------------------------------------------------------------
// Name: CompareAndCopy()
// Desc:
//
//-----------------------------------------------------------------------------
bool CScreen::CompareAndCopy(CSprite* pSprite, int wIndex, SDL_Color dwBaseCol)
{

	// Check if the Sprite is animated
	if (pSprite->m_fAnimated)
	{
		// If this sprite hasnt already been animated
		// this frame then animate it. Keep in mind that
		// 1 screen may refer to many of the same sprite.
		if (!pSprite->m_fBeenAnimated && !g_pGame->GetPauseLevel())
		{
			// Advance the animation count
			pSprite->m_rAnimCount += (pSprite->m_rAnimInc * g_rDelta);

			// Mark this sprite as having been animated.
			pSprite->m_fBeenAnimated = true;

			// is the animation count gets too large reset it to 0
			if (pSprite->m_rAnimCount >= pSprite->m_wFrames)
				pSprite->m_rAnimCount = 0;
		}
	}

	// Calculate screen coordinates for this sprite
	int tx = g_pGame->WorldToDispX(m_x + m_Sprites[wIndex].X);
	int ty = g_pGame->WorldToDispY(m_y + m_Sprites[wIndex].Y);


	// If this sprite has a different alpha value then add it into the base colour
	if ( pSprite->m_wAlpha != 255)
	{
		/*
		int red        = GetARValue(dwBaseCol);
		int green    = GetAGValue(dwBaseCol);
		int blue    = GetABValue(dwBaseCol);

		dwBaseCol = ARGB(pSprite->m_wAlpha, red, green, blue);
		*/
	}

	// Create the Correct color for the sprite
	SDL_Color dwColor = {255,255,255, 255};
	dwColor.a = pSprite->m_wAlpha;

	g_pGame->GetDisplay()->SimpleDraw(tx << ENLARGE_FACTOR,
									 ty << ENLARGE_FACTOR,
									 pSprite,
									 dwBaseCol,
									 1 << ENLARGE_FACTOR,
									 0,
									 2);
	return true;
}


//-----------------------------------------------------------------------------
// Name: CompareAndCopyFill()
// Desc:
//
//-----------------------------------------------------------------------------
bool CScreen::CompareAndCopyFill(CSprite* pSprite, int wIndex, RECT2D sFill, SDL_Color dwBaseCol)
{
	int tx, ty;

	// Make sure the fills are not bigger than the screen
	// this was a bug once and caused sprites to go missing
	if (sFill.left < 0)
		sFill.left = 0;

	if (sFill.top < 0)
		sFill.top = 0;

	if (sFill.right > 320)
		sFill.right = 320;

	if (sFill.bottom > 240)
		sFill.bottom = 240;


	int width = sFill.right - sFill.left;
	int height = sFill.bottom - sFill.top;


	// If this sprite has a different alpha value then add it into the base colour
	if ( pSprite->m_wAlpha != 255)
	{
		/*
		int red        = GetARValue(dwBaseCol);
		int green    = GetAGValue(dwBaseCol);
		int blue    = GetABValue(dwBaseCol);

		dwBaseCol = ARGB(pSprite->m_wAlpha, red, green, blue);
		*/
	}

	// Calculate screen coordinates for this sprite
	tx = g_pGame->WorldToDispX(m_x + sFill.left);
	ty = g_pGame->WorldToDispY(m_y + sFill.top);

	SDL_Color dwColor = {255,255,255, 255};
	dwColor.a = pSprite->m_wAlpha;

	g_pGame->GetDisplay()->SimpleTile(tx << ENLARGE_FACTOR,
									 ty << ENLARGE_FACTOR,
									 width * ENLARGE_FACTOR,
									 height * ENLARGE_FACTOR,
									 pSprite,
									 dwColor);
	return true;
}


//-----------------------------------------------------------------------------
// Name: CopySpriteMask()
// Desc:
//
//-----------------------------------------------------------------------------
void CScreen::CopySpriteMask(int x, int y, CSprite* pSprite)
{
	long y1;
	long x1;
	long val;
	unsigned char c;

	if (pSprite && pSprite->m_fisMask)
	{
		if (pSprite->m_pMask)
		{
			// Copy the Sprite's mask to the screens mask
			for ( y1 = 0; y1 < pSprite->m_wHeight; y1++)
			{
				for ( x1 = 0; x1 < pSprite->m_wWidth ; x1++)
				{
					val = (x + x1) + ((y + y1) * VSCREEN_WIDTH);

					if (val < MASKSIZE && val > 0 && (x + x1) >= 0 && (x + x1) < VSCREEN_WIDTH)
					{
						c = pSprite->m_pMask[x1 + (y1 * pSprite->m_wWidth)];

						// Dont copy 240 - its transparent
						if ( c != 240 )
							m_mask[val].setValue(c);
					}
				}
			}
		}
	}
}

void CScreen::FillMask(int x, int y, int w, int h, unsigned char c)
{
	if (c == 240)
		return;
	long y1;
	long x1;
	long val;

	for ( y1 = 0; y1 < h; y1++)
	{
		for ( x1 = 0; x1 < w ; x1++)
		{
			val = (x + x1) + ((y + y1) * VSCREEN_WIDTH);

			if (val < MASKSIZE && val > 0 && (x + x1) >= 0 && (x + x1) < VSCREEN_WIDTH)
			{
				m_mask[val].v8 = c;
			}
		}
	}
}
#include "Global.h"
extern SDL_Renderer * mask_renderer;

uint32_t mask_colors[33] = {
	0xFF000000, // 0
	0xFF087850, // 8
	0xFF102850, // leap 16
	0xFF183C78, // leap 24
	0xFF2050A0, // leap 32
	0xFF280AC8, // leap 40
	0xFF300000, // 48
	0xFF380000, // 56
	0xFF400000, // 64
	0xFF480000, // 72
	0xFF500000, // 80
	0xFF580000,
	0xFF600000,
	0xFF680000,
	0xFF700000,
	0xFF780000,
	0xFF800000,
	0xFF88A690, // Doors
	0xFF902C1E,	// Stairs
	0xFF980000,
	0xFFA00F64, // Deep Water
	0xFFA80000,
	0xFFB00000,
	0xFFB80000,
	0xFFC00000, // Water
	0xFFC80000,
	0xFFD00000,
	0xFFD80000,
	0xFFE00000,
	0xFFE80000, // Ice
	0xFFF00000,
	0xFFF80000, // Diggable
	0xFFFF0000
};

//-----------------------------------------------------------------------------
// Name: DisplayMask()
// Desc:
//
//-----------------------------------------------------------------------------
void CScreen::DisplayMask()
{
	if ( mask_renderer )
	{
		// Create Texture
		SDL_Texture * texture = SDL_CreateTexture( mask_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, VSCREEN_WIDTH, VSCREEN_HEIGHT );
		uint32_t c = 76800;
		uint32_t * pixels = new uint32_t[c]();

		if ( pixels )
		{
			for( uint32_t q = 0; q < c; q++ )
			{
				pixels[q] = mask_colors[m_mask[q].v5];
				//pixels[q] = 0xFF000000 + m_mask[q].value();
			}
			SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_NONE );
			SDL_UpdateTexture( texture, NULL, pixels, 1280 );
		}

		SDL_RenderClear( mask_renderer );
		SDL_SetTextureColorMod( texture, 255, 255, 255);
		SDL_SetTextureAlphaMod( texture, 255);
		SDL_RenderCopy( mask_renderer, texture, NULL, NULL );
		SDL_RenderPresent( mask_renderer );

		SDL_DestroyTexture( texture );
		delete [] pixels;

	}

}


//-----------------------------------------------------------------------------
// Name: AddHole()
// Desc:
//
//-----------------------------------------------------------------------------
void CScreen::AddHole(RECT2D rect, int Type)
{
	if (m_lNumHoles >= MAX_HOLES )
		return;

	// Add a new Hole to the list
	m_Holes[m_lNumHoles].holeRect = rect;
	m_Holes[m_lNumHoles].Type = Type;
	m_lNumHoles++;
}


//-----------------------------------------------------------------------------
// Name: CheckForHoleRECT()
// Desc: Checks if the supplied RECT2D is completly in a hole
//
//-----------------------------------------------------------------------------
int CScreen::CheckForHoleRECT(RECT2D rect)
{
	int n;

	for (n = 0; n < m_lNumHoles; n++)
	{
		// Check that supplied rectangle fits in the hole rectangle
		if (rect.left > m_Holes[n].holeRect.left && rect.right < m_Holes[n].holeRect.right)
		{
			if (rect.top > m_Holes[n].holeRect.top && rect.bottom < m_Holes[n].holeRect.bottom)
				return m_Holes[n].Type;
		}
	}
	return -1;
}


//-----------------------------------------------------------------------------
// Name: CheckForHoleXY()
// Desc:
//
//-----------------------------------------------------------------------------
int CScreen::CheckForHoleXY(long x, long y)
{
	int n;

	for (n = 0; n < m_lNumHoles; n++)
	{
		// Check that supplied rectangle fits in the hole rectangle
		if (x > m_Holes[n].holeRect.left && x < m_Holes[n].holeRect.right)
		{
			if (y > m_Holes[n].holeRect.top && y < m_Holes[n].holeRect.bottom)
				return m_Holes[n].Type;
		}
	}
	return -1;
}


//-----------------------------------------------------------------------------
// Name: ClearMask()
// Desc:
//-----------------------------------------------------------------------------
void CScreen::ClearMask()
{
	for (long l = 0; l < MASKSIZE; l++)
		m_mask[l].setValue(255);
}

//-----------------------------------------------------------------------------
// Name: CheckMask()
// Desc:
//-----------------------------------------------------------------------------
bool CScreen::CheckMask(int x, int y)
{
	if (x + (y * VSCREEN_WIDTH) >= 0 && x + ( y * VSCREEN_WIDTH) <= MASKSIZE)
		return true;

	return false;
}


//-----------------------------------------------------------------------------
// Name: GetMaskValue()
// Desc:
//-----------------------------------------------------------------------------
int CScreen::GetMaskValue(long lIndex)
{
	if (lIndex < 0 || lIndex > MASKSIZE)
		return -1;
	return m_mask[lIndex].value();
}
