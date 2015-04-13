/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CSpriteSheet.cpp
//
// Desc: Holds data on a particular sprite sheet (1 bitmap containing lots
//       of sprites). Each sheet has a config file to go with it.
//
//
//-----------------------------------------------------------------------------
#define STRICT

#include <stdio.h>
#include "CSpriteSheet.h"
#include "CGame.h"

extern CGame*				g_pGame;

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CSpriteSheet::CSpriteSheet(char* BitMapName, char *SPTname, char * realName)
{
	// Set Some Defaults
	m_lpSurface			= NULL;
	m_wNumSprites		= 0;
	m_fHasAnims			= false;
	m_fBackGround		= false;

	// Copy the Supplied Names
	strncpy(m_szBitmapName, BitMapName,128);
	strncpy(m_szSPTName, SPTname,128);
	strncpy(m_szRealName, realName,128);


	// Load the bitmap onto the CSurface
	if (LoadBitmap() != 1)
		return;

	// Load the sheet config file
	m_wNumSprites = LoadSPT();
}


//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CSpriteSheet::~CSpriteSheet()
{
	SDL_FreeSurface( m_lpSurface );

	// Delete all the CSprite objects
	m_pSprites.free();
}

//-----------------------------------------------------------------------------
// Name: LoadBitmap()
// Desc:
//
//-----------------------------------------------------------------------------
#include "BmpHeader.h"
		#define RMASK 0x000000ff
		#define GMASK 0x0000ff00
		#define BMASK 0x00ff0000
		#define AMASK 0xff000000
void PutPixel(SDL_Surface *surface, uint32_t x, uint32_t y, uint32_t pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16 *)p = pixel;
		break;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		} else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(uint32_t *)p = pixel;
		break;
	}
}
#include "Global.h"
#include <iostream>


int32_t CSpriteSheet::LoadBitmap()
{
	Uint32 new_color;
	std::string path(FS_Gamedir());
	path.append(m_szBitmapName);



	GetGameObject()->LoadingMessage("Extracting Sprites", 10, 10, 0);
	// Load the Surface
	m_lpSurface = SDL_LoadBMP(path.c_str());

	if (!m_lpSurface)
	{
		FILE * file = fopen(path.c_str(),"rb");
		BITMAPHEADER header;

		fseek( file , 14, SEEK_SET );
		fread(&header,sizeof(BITMAPHEADER),1,file);
		if ( header.Compression == 1 && header.Size == 40 )
		{
			m_lpSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, header.Width, header.Height, 32, RMASK, GMASK, BMASK, AMASK );
			if( !m_lpSurface) {
				SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n",SDL_GetError());
				return 0;
			}
			else
			{
				uint32_t * colours = new uint32_t[256];
				header.ColorsUsed = ( !header.ColorsUsed ? 256 : header.ColorsUsed);
				for  ( uint16_t c = 0; c < header.ColorsUsed ; c++  )
				{
					BITMAPPALETTE pal;
					fread(&pal,sizeof(BITMAPPALETTE),1,file);
					if ( pal.Red == 0 &&  pal.Green == 255 && pal.Blue == 0 )
						colours[c] = SDL_MapRGBA(m_lpSurface->format, pal.Red, pal.Green, pal.Blue, 0);
					else
						colours[c] = SDL_MapRGBA(m_lpSurface->format, pal.Red, pal.Green, pal.Blue, 255);
				}

				int32_t x = 0;
				int32_t y = header.Height - 1;

				for ( uint32_t s = 0; s < header.SizeOfBitmap; s++ )
				{
					uint8_t a, b;
					fread(&a,1,1,file);
					fread(&b,1,1,file);

					if ( a == 0 )
					{
						if (b == 0) // End of Line
						{
							x = 0;
							y--;
						}
						else if (b == 1) //End of Bitmap
							break;
						else if (b == 2) //Move
						{
							uint8_t mx = 0;
							uint8_t my = 0;
							fread( &mx, 1,1,file);
							fread( &my, 1,1,file);
							x += mx;
							y += my;
						}
						else // Pixel
						{
							int8_t d;
							for(uint16_t c = 0; c < b; c++)
							{
								fread(&d,1,1,file);
								PutPixel(m_lpSurface, x, y, colours[d]);
								x++;
							}
							if ( (b%2) )
								fread(&d,1,1,file);
						}
					}
					else
					{
						for(uint16_t c = 0; c < a; c++)
						{
							PutPixel(m_lpSurface, x, y, colours[b]);
							x++;
						}

					}
				}
			}
		}
		else if ( header.Compression == 2 && header.Size == 40 )
		{
			m_lpSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, header.Width, header.Height, 32, RMASK, GMASK, BMASK, AMASK );
			if( !m_lpSurface) {
				SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n",SDL_GetError());
				return 0;
			}
			else
			{
				BITMAPPALETTE palette[16];
				uint32_t * colours = new uint32_t[16];
				header.ColorsUsed = ( !header.ColorsUsed ? 16 : header.ColorsUsed);
				for ( uint16_t c = 0; c < header.ColorsUsed ; c++  )
				{
					BITMAPPALETTE pal;
					fread(&pal,sizeof(BITMAPPALETTE),1,file);
					if ( pal.Red == 0 &&  pal.Green == 255 && pal.Blue == 0 )
						colours[c] = SDL_MapRGBA(m_lpSurface->format, pal.Red, pal.Green, pal.Blue, 0);
					else
						colours[c] = SDL_MapRGBA(m_lpSurface->format, pal.Red, pal.Green, pal.Blue, 255);

					palette[c] = pal;
				}

				int32_t x = 0;
				int32_t y = header.Height - 1;

				for ( uint32_t s = 0; s < header.SizeOfBitmap; s++ )
				{
					uint8_t a, b;
					uint8_t high, low;

					fread(&a,1,1,file);
					fread(&b,1,1,file);



					if ( a == 0 ) //absolute mode
					{
						if (b == 0) // End of Line
						{
							x = 0;
							y--;
						}
						else if (b == 1) //End of Bitmap
							break;
						else if (b == 2) //Move
						{
							uint8_t mx = 0;
							uint8_t my = 0;
							fread( &mx, 1,1,file);
							fread( &my, 1,1,file);
							x += mx;
							y += my;
						}
						else // Pixel
						{
							uint8_t d;
							for(uint16_t c = 0; c < b; c += 2)
							{
								fread(&d,1,1,file);
								high = (d & 0xF0) >> 4;
								low = d & 0x0F;

								PutPixel(m_lpSurface, x++, y, colours[high]);
								PutPixel(m_lpSurface, x++, y, colours[low]);
							}
							if ( (b%2) )
								fread(&d,1,1,file);
						}
					}
					else
					{
						high = (b & 0xF0) >> 4;
						low = b & 0x0F;
						for(uint16_t c = 0; c < a; c++)
						{
							if ( c % 2 )
								PutPixel(m_lpSurface, x, y, colours[low]);
							else
								PutPixel(m_lpSurface, x, y, colours[high]);
							x++;
						}

					}
				}
			}
		}
		else
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s %s\n", "Can not load", m_szRealName);
			fclose(file);
			return 0;

		}
		fclose(file);
	}
	else
	{
		new_color = SDL_MapRGB(m_lpSurface->format, 0, 255, 0);
		SDL_SetColorKey(m_lpSurface, SDL_TRUE, new_color);
		SDL_Surface *q = SDL_ConvertSurfaceFormat(m_lpSurface, SDL_PIXELFORMAT_ABGR8888, 0);

		SDL_FreeSurface(m_lpSurface);
		m_lpSurface = q;
	}

	new_color = SDL_MapRGB(m_lpSurface->format, 0, 255, 0);
	SDL_SetColorKey(m_lpSurface, SDL_TRUE, new_color);


	// Record the Widht and height of the surface
	m_wWidth  = m_lpSurface->w;
	m_wHeight = m_lpSurface->h;
	return 1;
}


//-----------------------------------------------------------------------------
// Name: LoadSPT()
// Desc:
//
//-----------------------------------------------------------------------------
int CSpriteSheet::LoadSPT()
{
	int n				= 0;
	int NumFrames		= 0;
	bool ThisFrameAnim	= false;
	FILE *Conf;
	char NewLine[128];
	RECT2D sRect;

	// Attempt to Open the SPT file
	std::string path(FS_Gamedir());
	path.append(m_szSPTName);
	if ((Conf = fopen( path.c_str(), "r" )) == NULL)  // File cant be opened
	{
		return 0;
	}

	n = 0;

	// Disgard the First Line as this should be blank
	if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)
		return n;

	// Loop Through SPT file
	for (;;)
	{
		// Create a new CSprite Object, we will fill it's details in as we load
		// the SPT file and add it to the list at the end.
		CSprite* pTempSprite = new CSprite();
		pTempSprite->m_pParent = this;

label1:
		// Make sure NewLine is empty
		memset(NewLine ,0, sizeof(NewLine));

		// This line might be [ENTITY] or [BACKGROUND] or the Sprite Code
		if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL) return n;

		//strip \r

		char * nl = strpbrk(NewLine, "\r\n");
		if ( nl )
			*nl = '\0';

		if (strstr(NewLine, "[ENTITY]"))
		{
			// This Sprite is an Entity, meaning its not a background sprite but it
			// can be placed like one. Instead of this sprite appearing, it's matching
			// Entity will appear instead.
			pTempSprite->m_fEntity = true;

			// Go back and try again for the sprite code
			goto label1;
		}

		if (strstr(NewLine, "[BACKGROUND]"))
		{
			m_fBackGround = true;

			// This Sprite is a Background Tile
			pTempSprite->m_fBackground = true;

			// Go back and try again for the sprite code
			goto label1;
		}




		// If we reach this point then the line must be the sprite code, copy
		// it and remove the NULL byte at the end.
		memset(pTempSprite->m_szCode, 0, sizeof(pTempSprite->m_szCode));
		if ( strlen(NewLine) )
		{
			strncpy(pTempSprite->m_szCode, NewLine, strlen(NewLine) );
			RemoveNewline(pTempSprite->m_szCode, strlen(pTempSprite->m_szCode));
		}
		else
		{
			strncpy(pTempSprite->m_szCode, "__missing__", strlen("__missing__") );
		}

		if (strstr(pTempSprite->m_szCode, "door"))
				int g = 0;

		// Next Line will be ANIMATED, or the coordinates of the Rectangle
		if (!fgets( NewLine, sizeof(NewLine), Conf ))return n;

		ThisFrameAnim = false;

		// Check for the ANIMATED keyword. Sprites can either
		// have just the one frame (which is normal), or they
		// can have a few frames.
		if (strstr(NewLine, "ANIMATED"))
		{
			// Mark this sheet as having animations
			m_fHasAnims		= true;
			ThisFrameAnim	= true;

			// Retrieve the number of frames in animation
			if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
			NumFrames = atoi(NewLine);

			pTempSprite->m_wFrames = NumFrames;
			pTempSprite->m_fAnimated = true;

			// Retrieve animation speed
			if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
			pTempSprite->m_rAnimInc = (float)atof(NewLine);

			// Now start recording the RECT2D coordinates
			// for all frames
			for (int i=0;i<NumFrames;i++)
			{
				if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
				sRect.left = atoi(NewLine);
				if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
				sRect.top = atoi(NewLine);
				if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
				sRect.right = sRect.left + atoi(NewLine);
				if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
				sRect.bottom = sRect.top + atoi(NewLine);
				pTempSprite->m_sRect = sRect;

				// Create a Texture on this sprite from this rectangle
				pTempSprite->CreateTexture(m_lpSurface, &sRect);
			}
		}
		else
		{
			// If it's not animated then just get the one frame.
			sRect.left = atoi(NewLine);
			memset(NewLine ,0, sizeof(NewLine));
			if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
			sRect.top = atoi(NewLine);
			memset(NewLine ,0, sizeof(NewLine));
			if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
			sRect.right = sRect.left + atoi(NewLine);
			memset(NewLine ,0, sizeof(NewLine));
			if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
			sRect.bottom = sRect.top + atoi(NewLine);
			pTempSprite->m_sRect = sRect;

			// Create a Texture on this sprite from this rectangle
			pTempSprite->CreateTexture(m_lpSurface, &sRect);
		}

		// The next line Could be a FILL or not
		if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;

		// Assume this sprite isnt a fill unless proven otherwise
		pTempSprite->m_fFill = false;

		if (strstr(NewLine, "[FILL]"))
		{
			// The next line is [FILL], so record it and get the next line, which will
			// Be the Mask String
			pTempSprite->m_fFill = true;

			// Next line will be mask String
			memset(NewLine ,0, sizeof(NewLine));
			if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
		}

		// Copy the Mask String
		strncpy(pTempSprite->m_szMaskString, NewLine, MAX_CHARS);
		RemoveNewline(pTempSprite->m_szMaskString, strlen(pTempSprite->m_szMaskString));

		if ( strlen(pTempSprite->m_szMaskString) > 0)
			pTempSprite->m_fHasMask = true;

		// Now get the toplayer flag
		memset(NewLine ,0, sizeof(NewLine));
		if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;

		if (strstr(NewLine, "TRUE")!=NULL)
			pTempSprite->m_fTopLayer = true;

		if (strstr(NewLine, "FALSE")!=NULL)
			pTempSprite->m_fTopLayer = false;

		// Get the alpha Value
		memset(NewLine ,0, sizeof(NewLine));
		if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;
			pTempSprite->m_wAlpha = atoi(NewLine);

		// Get the [EE] marker
		memset(NewLine ,0, sizeof(NewLine));
		if (fgets( NewLine, sizeof(NewLine), Conf ) == NULL)return n;

		n++;

		// Now Addd This Sprite to the Linked List
		m_pSprites.additem(pTempSprite, pTempSprite->m_szCode);
	}

	fclose(Conf);
	return n;
}


//-----------------------------------------------------------------------------
// Name: BuildMasks()
// Desc:
//
//-----------------------------------------------------------------------------
void CSpriteSheet::BuildMasks()
{

	// Go through all the sprites so far and check if they should
	// Have a mask.
	if (m_pSprites.isempty())
		return;

	CSprite* temp;
	CSprite* temp2;
	CSpriteSheet* pSheet;

	m_pSprites.tostart();
	do
	{
		// Get the Current Sprite
		temp = m_pSprites.getcurrent();

		// Check if this sprite has a mask
		if ( temp->m_szMaskString[0] != 0 )
		{
			// Save our place in the list
			m_pSprites.SavePosition();



			int mv = -1;
			if ( sscanf(temp->m_szMaskString, "%*dx%*dx%d", &mv ) > 0 )
			{
				SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Default Mask: %s Value: %d\n",temp->m_szMaskString, mv);
			}


			// Try and find the sprite's mask on any sprite sheet
			temp2 = g_pGame->GetSSheetSet()->GetSprite( temp->m_szMaskString, false );
			if (temp2)
			{
				// Get the sheet this sprite is on
				pSheet = g_pGame->GetSSheetSet()->GetSpriteSheet(temp2);

				// If this sprite is a mask, but has not mask defined then define it
				if ( !temp2->m_fisMask)
				{
					temp2->m_fisMask = true;

					// Copy the correct image to this sprites mask surface
					temp2->CopyMask(pSheet->m_lpSurface, temp2->m_sRect);
				}
			}

			// Go back to where we were in the list
			m_pSprites.LoadPosition();
		}

	}while(m_pSprites.advance());

}


//-----------------------------------------------------------------------------
// Name: GetSprite()
// Desc:
//
//-----------------------------------------------------------------------------
CSprite* CSpriteSheet::GetSprite(char *szCode, bool fOnlyBackground)
{
	if (m_pSprites.isempty())
		return NULL;

	CSprite* pSprite = NULL;

	m_pSprites.tostart();

	do
	{
		if ( !strcmp(m_pSprites.getcurrent()->m_szCode, szCode) )
			return m_pSprites.getcurrent();

	}while(m_pSprites.advance());

	return NULL;
}


//-----------------------------------------------------------------------------
// Name: ResetAnim()
// Desc:
//
//-----------------------------------------------------------------------------
void CSpriteSheet::ResetAnim()
{
	if (m_pSprites.isempty())
		return;

	m_pSprites.tostart();
	do
	{
		// Reset the Animation Flag
		m_pSprites.getcurrent()->m_fBeenAnimated = false;

	}while(m_pSprites.advance());
}


//-----------------------------------------------------------------------------
// Name: CountSprites()
// Desc:
//
//-----------------------------------------------------------------------------
long CSpriteSheet::CountSprites()
{
	long lTotal = 0;

	if (m_pSprites.isempty())
		return 0;

	m_pSprites.tostart();
	do
	{
		lTotal++;
	}while(m_pSprites.advance());

	return lTotal;
}


//-----------------------------------------------------------------------------
// Name: HashSprites()
// Desc:
//
//-----------------------------------------------------------------------------
void CSpriteSheet::HashSprites( CHash* pHash )
{
	CSprite* pCurrent = NULL;

	if (m_pSprites.isempty())
		return;

	m_pSprites.tostart();
	do
	{
		pCurrent = m_pSprites.getcurrent();
		pHash->AddString( pCurrent->m_szCode, pCurrent);

	}while(m_pSprites.advance());

}















