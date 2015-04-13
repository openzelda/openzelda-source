/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: GeneralData.h
//
// Desc: General Data for this program
//
//-----------------------------------------------------------------------------
#ifndef GENERALDATA_H
#define GENERALDATA_H
#define WIN32_LEAN_AND_MEAN
#define NOUSER
#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <string>
#include <iostream>
#include "FS_Functions.h"

#define MAX_CHARS 20
#define NUM_ENC_KEYS 3
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define VSCREEN_WIDTH 320
#define VSCREEN_HEIGHT 240
#define SCREEN_BPP 16
#define DEFAULT_QUEST "./quests/default.qst"
#define QUEST_FONT "Arial"
#define SCRIPTLINEMAX 1024
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
#define NORTHEAST 4
#define SOUTHEAST 5
#define SOUTHWEST 6
#define NORTHWEST 7
#define MAX_ANSWERS 10
#define PI 3.141592
#define DTOR 0.0174532
#define RTOD 57.2961
#define ENT_STRING_MAX 1024
#define ENLARGE_FACTOR 1
#define MAX_RESTART 6
#define MAX_OSPATH 512

#define RECVSOCK_EVENT (WM_APP + 100)
#define APPSOCK_EVENT (WM_APP + 101)

// This may just be for 16bit screen pixel formats
#define ARGB(a,r,g,b) (uint32_t)((uint8_t)(a << 24) | (uint8_t)(r << 16) | (uint8_t)(g << 8) | (uint8_t)b)
#define GetAAValue(rgb) (uint8_t)((uint32_t)(rgb) >> 24)
#define GetARValue(rgb) (uint8_t)(((uint32_t)(rgb) << 8) >> 24)
#define GetAGValue(rgb) (uint8_t)(((uint32_t)(rgb) << 16) >> 24)
#define GetABValue(rgb) (uint8_t)(((uint32_t)(rgb) << 24) >> 24)
#define RGB(r,g,b) {r,g,b,255}
#define SetRGB(color, rc,gc,bc) (color).r = rc; (color).b = bc; (color).g = gc;
#define SetARGB(color, ac, rc,gc,bc) (color).r = rc; (color).b = bc; (color).g = gc; (color).a = ac;


#define GetRValue(rgb) rgb.r
#define GetGValue(rgb) rgb.g
#define GetBValue(rgb) rgb.b

#define SAFE_DELETE(rgb) delete rgb; rgb = NULL;


typedef struct _RECT {
  long left;
  long top;
  long right;
  long bottom;
} RECT2D, *PRECT2;

// Structure to hold info on a collision Rectangle
struct ColRect
{
	RECT2D sRect;
	bool fSolid;    // is This rectangle solid?
};

struct Texture {

	SDL_Texture * buffer;
	int w, h;
};

inline SDL_Rect GetRect( RECT2D srcRect )
{
	SDL_Rect dstRect;

	dstRect.x = srcRect.left;
	dstRect.y = srcRect.top;
	dstRect.w = srcRect.right - srcRect.left;
	dstRect.h = srcRect.bottom - srcRect.top;

	return dstRect;
}

// General Non-Class Functions
void    RemoveChars(int wRemove, char* szString);
bool    strStart(char* szS1, char* szS2);
bool    PointInRect(long x, long y, RECT2D sRect);
bool    Intersect(RECT2D src, RECT2D dest);
bool    isNearPoint(long x, long y, long x2, long y2, long Extra);
float 	DegToRad(int wDegrees);
RECT2D    CalcDestRect(long left, long top, long right, long bottom, long x, long y);

bool IntersectRect(RECT2D * lprcDst,  RECT2D * lprcSrc1, RECT2D * lprcSrc2);
bool SetRect( RECT2D * lprcDst, int xLeft, int yTop, int xRight, int yBottom);

void InitCheck();


void StripExtension(char* szString);
void WriteCheckTextVal(const char * string, const long val);
float CalcAngle(long xdist, long ydist, long xdist1, long ydist1 );
void RemoveNewline(char* szString, size_t nLength);
void SetJoyKeys();
void CheckKeysDown(int32_t wParam);
void CheckKeysUp(int32_t wParam);
#endif
