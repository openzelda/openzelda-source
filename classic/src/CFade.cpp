/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CFade.cpp
//
// Desc: 
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include "GeneralData.h"
#include "CFade.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern float		 g_rDelta;



//-----------------------------------------------------------------------------
// Name: Constructor
// Desc:
// 
//-----------------------------------------------------------------------------
CFade::CFade()
{
	m_rCurrentRed	= 255;
	m_rCurrentGreen	= 255;
	m_rCurrentBlue	= 255;
	m_wRedTarg      = 255;
	m_wGreenTarg    = 255;
	m_wBlueTarg     = 255;
	m_rFadeRate		= 70;
}


//-----------------------------------------------------------------------------
// Name: Destructor
// Desc: 
//
//-----------------------------------------------------------------------------
CFade::~CFade()
{
}


//-----------------------------------------------------------------------------
// Name: FadeTo
// Desc: 
//
//-----------------------------------------------------------------------------
bool CFade::FadeTo(int r, int g, int b, int wSpeed)
{
   m_wRedTarg	= r;
   m_wGreenTarg	= g;
   m_wBlueTarg	= b;
   m_rFadeRate  = wSpeed;

   if ((int) m_rCurrentRed != m_wRedTarg || (int) m_rCurrentGreen != m_wGreenTarg || (int) m_rCurrentBlue != m_wBlueTarg)
	   return false;

   return true;
}


//-----------------------------------------------------------------------------
// Name: DoFade
// Desc: 
//
//-----------------------------------------------------------------------------
void CFade::DoFade()
{
	// Move all the Fade Counters
    if (m_rCurrentRed < m_wRedTarg)
		m_rCurrentRed += m_rFadeRate * g_rDelta;
	else if(m_rCurrentRed > m_wRedTarg)
        m_rCurrentRed -= m_rFadeRate * g_rDelta;

	if (m_rCurrentRed < 0)
		m_rCurrentRed = 0;

	if (m_rCurrentRed > 255)
		m_rCurrentRed = 255;

	if (m_rCurrentGreen < m_wGreenTarg)
		m_rCurrentGreen += m_rFadeRate * g_rDelta;
	else if(m_rCurrentGreen > m_wGreenTarg)
        m_rCurrentGreen -= m_rFadeRate * g_rDelta;

	if (m_rCurrentGreen < 0)
		m_rCurrentGreen = 0;

	if (m_rCurrentGreen > 255)
		m_rCurrentGreen = 255;

	if (m_rCurrentBlue < m_wBlueTarg)
		m_rCurrentBlue += m_rFadeRate * g_rDelta;
	else if(m_rCurrentBlue > m_wBlueTarg)
        m_rCurrentBlue -= m_rFadeRate * g_rDelta;

	if (m_rCurrentBlue < 0)
		m_rCurrentBlue = 0;

	if (m_rCurrentBlue > 255)
		m_rCurrentBlue = 255;
}


//-----------------------------------------------------------------------------
// Name: CombineColor()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CFade::CombineColor(int* wInputCol, int rgb)
{
	switch(rgb)
	{
	case 0:
		// Red
		*wInputCol -= (255 - m_rCurrentRed);
		break;
	case 1:
		// green
		*wInputCol -= (255 - m_rCurrentGreen);
		break;
	case 2:
		// blue
		*wInputCol -= (255 - m_rCurrentBlue);
		break;
	}

	if (*wInputCol < 0)  
		*wInputCol = 0;
}


