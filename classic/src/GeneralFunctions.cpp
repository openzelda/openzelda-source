/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: GeneralFunctions.h
//
// Desc: General Functions for this program
//
//-----------------------------------------------------------------------------
#include "GeneralData.h"
#include "FS_Functions.h"
#include "OpenZelda_private.h"

#include <fcntl.h>
#include <math.h>

//-----------------------------------------------------------------------------
// Name: CalcDestRect()
// Desc:
//
//-----------------------------------------------------------------------------
RECT2D CalcDestRect(long left, long top, long right, long bottom, long x, long y)
{
	RECT2D sRect = {left, top, right, bottom};

	sRect.left  += x;
	sRect.right += x;
	sRect.top   += y;
	sRect.bottom+= y;

	return sRect;
}


//-----------------------------------------------------------------------------
// Name: DegToRad()
// Desc: Degrees to Radians conversion
//
//-----------------------------------------------------------------------------
float DegToRad(int wDegrees)
{
	return (DTOR * (float)wDegrees);
}


//-----------------------------------------------------------------------------
// Name: RemoveChars()
// Desc: Remove a number of chars from the beginning of the supplied string
//
//-----------------------------------------------------------------------------
void RemoveChars(int wRemove, char* szString)
{
	int n;
	char szBuffer[1024];

	for (n = wRemove; n < strlen(szString); n++ )
	{
		// Copy the Chars we want to the buffer
		szBuffer[n -  wRemove] = szString[n];
	}

	// Copy the Buffer back to the String
	memset(szString, 0, sizeof(szString));
	strcpy(szString, szBuffer);
}


//-----------------------------------------------------------------------------
// Name: strStart()
// Desc: Like strst, but a bit faster
//
//-----------------------------------------------------------------------------
bool strStart(char* szS1, char* szS2)
{
	int n;

	// Get the Length of both strings
	int wLen1 = strlen(szS1);
	int wLen2 = strlen(szS2);

	// If the string with the text we want to find is bigger
	// than the strig we are searching then exit
	if (wLen1 < wLen2)
		return false;

	for (n = 0; n < wLen2; n++)
	{
		if ( szS2[n] != szS1[n])
			return false;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Name: PointInRect()
// Desc: Returns true if the suppiled point is in the supplied RECT
//
//-----------------------------------------------------------------------------
bool PointInRect(long x, long y, RECT2D sRect)
{
	if (x > sRect.left && x < sRect.right)
	{
		if (y > sRect.top && y < sRect.bottom)
			return true;
	}

	return false;
}


//-----------------------------------------------------------------------------
// Name: NearPoint()
// Desc:
//
//-----------------------------------------------------------------------------
bool isNearPoint(long x, long y, long x2, long y2, long Extra)
{
	if (x < (x2 - Extra) || x > (x2 + Extra))
		return false;
	if (y < (y2 - Extra) || y > (y2 + Extra))
		return false;

	return true;
}


//-----------------------------------------------------------------------------
// Name: Intersect()
// Desc:
//
//-----------------------------------------------------------------------------
bool Intersect(RECT2D src, RECT2D dest)
{
	// BUG: some rectangles can interset and it still returns false
	// Using IntersectRect now
	if (src.left >= dest.left && src.left <= dest.right || src.right >= dest.left && src.right <= dest.right )
	{
		if (src.top >= dest.top && src.top <= dest.bottom || src.bottom >= dest.top && src.bottom <= dest.bottom )
			return true;
	}

	if (dest.left >= src.left && dest.left <= src.right || dest.right >= src.left && dest.right <= src.right )
	{
		if (dest.top >= src.top && dest.top <= src.bottom || dest.bottom >= src.top && dest.bottom <= src.bottom )
			return true;
	}
	return false;
}


// --------------------------------------------------------------------------------------
//		Initialise error checking file
// --------------------------------------------------------------------------------------
void InitCheck()
{
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s Log\n", PRODUCT_NAME);
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"********************", PRODUCT_NAME);
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Program directory: %s\n", FS_Gamedir());
}


//-----------------------------------------------------------------------------
// Name: TestExtraction()
// Desc:
//
//-----------------------------------------------------------------------------
bool TestExtraction()
{
	return true;
}


//-----------------------------------------------------------------------------
// Name: StripExtension()
// Desc:
//
//-----------------------------------------------------------------------------
void StripExtension(char* szString)
{
	int n;
	int m;
	char szTemp[128];

	// Clear Temp
	memset(szTemp, 0, sizeof(szTemp));

	// Work through the string backwards until we hit a . char
	for (n = strlen(szString); n > 0; n--)
	{
		if (szString[n] == '.')
			break;
	}

	// Copy the remaining string to Temp
	for (m = 0; m < n; m++)
		szTemp[m] = szString[m];

	// Set the Last char in Temp to \0
	szTemp[m] = '\0';

	strcpy(szString, szTemp);
}




//-----------------------------------------------------------------------------
// Name: CalcAngle()
// Desc:
//
//-----------------------------------------------------------------------------
float CalcAngle(long xdist, long ydist, long xdist1, long ydist1 )
{
	float xval = xdist-xdist1;
	float yval = ydist-ydist1;

	float hyp = sqrt( (xval * xval) + (yval * yval));
	float Angle = acos(xval / hyp);

	if (ydist1 > ydist)
		Angle += (PI - Angle) * 2;

	if (Angle == 0 || Angle < 0)
		Angle = 0.1;

	return Angle;
}


//-----------------------------------------------------------------------------
// Name: RemoveNewline()
// Desc: Removes newline byte from end of string
//-----------------------------------------------------------------------------
void RemoveNewline(char* szString, size_t nLength)
{
	// Remove newline byte
	if (szString[nLength-1] == '\n')
		szString[nLength-1] = 0;
}


bool IntersectRect( RECT2D * lprcDst, RECT2D *lprcSrc1, RECT2D *lprcSrc2)
{
	lprcDst->top = lprcDst->bottom = lprcDst->left = lprcDst->right = 0;

	return true;
}

bool SetRect( RECT2D * lprcDst, int xLeft, int yTop, int xRight, int yBottom)
{
	lprcDst->top = yTop;
	lprcDst->bottom = yBottom - yTop;
	lprcDst->left = xLeft;
	lprcDst->right = xRight - xLeft;

	return true;
}





