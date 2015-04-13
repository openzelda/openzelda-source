/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: NightDay.cpp
//
// Desc: 
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include "CNightDay.h"
#include "GeneralData.h"
#include "SaveLoadFunctions.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern float		 g_rDelta;


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CNightDay::CNightDay()
{
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","CNightDay::CNightDay() - Setting Default values");
	m_wRed			= 255;
	m_wGreen		= 255;
	m_wBlue			= 255;
	m_dayLength     = 900; 
	m_days			= 0;
	Init();
}


//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CNightDay::~CNightDay()
{
}


//-----------------------------------------------------------------------------
// Name: CreateTimeString()
// Desc: 
//
//-----------------------------------------------------------------------------
void CNightDay::Save(FILE* sFile)
{
	WriteSlotHeader(sFile, "[DAYNIGHT]");

	Writelong(sFile, "dl", m_dayLength);
	Writelong(sFile, "adl", m_actualDayLength);
	Writelong(sFile, "days", m_days);
	Writelong(sFile, "hc", m_DayNight.HourCount);
	Writelong(sFile, "mc", m_DayNight.MinCount);
}



//-----------------------------------------------------------------------------
// Name: CreateTimeString()
// Desc: 
//
//-----------------------------------------------------------------------------
void CNightDay::Load(FILE* sFile)
{
	FindSlotHeader(sFile, "[DAYNIGHT]");

	Readlong(sFile, "dl", m_dayLength);
	Readlong(sFile, "adl", m_actualDayLength);
	Readlong(sFile, "days", m_days);
	Readlong(sFile, "hc", m_DayNight.HourCount);
	Readlong(sFile, "mc", m_DayNight.MinCount);

	CalculateDayLength();
	UpdateMinutes(m_DayNight.MinCount);
	UpdateHours(m_DayNight.HourCount);
}


//-----------------------------------------------------------------------------
// Name: CreateTimeString()
// Desc: 
//
//-----------------------------------------------------------------------------
void CNightDay::CreateTimeString()
{
	// Create a string containing the current
	// game time in 24 hour format
	memset(m_szGameTimeString, 0 , sizeof(m_szGameTimeString));
	sprintf(m_szGameTimeString,"%d : %d", m_DayNight.HourCount, m_DayNight.MinCount);
}

//-----------------------------------------------------------------------------
// Name: CalculateDayLength()
// Desc:
//
//-----------------------------------------------------------------------------
void CNightDay::CalculateDayLength()
{
	m_actualDayLength = m_dayLength * 100;

	// fill in the DayNight Structure
	m_DayNight.DayLength  = m_actualDayLength;
	m_DayNight.HourLength = m_DayNight.DayLength / 24;
	m_DayNight.MinLength  = m_DayNight.HourLength / 60;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialise all the day/night data
//
//-----------------------------------------------------------------------------
void CNightDay::Init()
{
	CalculateDayLength();

	m_DayNight.HourCount = 12;
	m_DayNight.MinCount = 0;
	m_DayNight.rCounter = 0;

	// It would be nice to import these RGB vals from
	// an external file so they could be altered for different
	// worlds.
	SetRGB( m_DayNight.HourTarget[0], 78, 92, 175);
	SetRGB( m_DayNight.HourTarget[1], 89, 100, 169);
	SetRGB( m_DayNight.HourTarget[2], 96, 107, 174);
	SetRGB( m_DayNight.HourTarget[3], 106, 118, 186);
	SetRGB( m_DayNight.HourTarget[4], 110, 118, 186);
	SetRGB( m_DayNight.HourTarget[5], 120, 131, 198);
	SetRGB( m_DayNight.HourTarget[6], 152, 163, 232);
	SetRGB( m_DayNight.HourTarget[7], 182, 190, 238);
	SetRGB( m_DayNight.HourTarget[8], 214, 218, 243);
	SetRGB( m_DayNight.HourTarget[9], 244, 245, 251);
	SetRGB( m_DayNight.HourTarget[10], 255, 255, 255);
	SetRGB( m_DayNight.HourTarget[11], 255, 255, 255);
	SetRGB( m_DayNight.HourTarget[12], 247, 248, 236);
	SetRGB( m_DayNight.HourTarget[13], 247, 248, 231);
	SetRGB( m_DayNight.HourTarget[14], 239, 242, 225);
	SetRGB( m_DayNight.HourTarget[15], 237, 237, 210);
	SetRGB( m_DayNight.HourTarget[16], 232, 236, 200);
	SetRGB( m_DayNight.HourTarget[17], 232, 225, 190);
	SetRGB( m_DayNight.HourTarget[18], 228, 196, 126);
	SetRGB( m_DayNight.HourTarget[19], 229, 166, 126);
	SetRGB( m_DayNight.HourTarget[20], 229, 158, 150);
	SetRGB( m_DayNight.HourTarget[21], 125, 124, 200);
	SetRGB( m_DayNight.HourTarget[22], 118, 111, 216);
	SetRGB( m_DayNight.HourTarget[23], 100, 92, 190);
	SetRGB( m_DayNight.HourTarget[24], 82, 92, 175);

	m_DayNight.CurrentRed     = GetRValue( m_DayNight.HourTarget[m_DayNight.HourCount] ); 
	m_DayNight.CurrentGreen   = GetGValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
    m_DayNight.CurrentBlue    = GetBValue( m_DayNight.HourTarget[m_DayNight.HourCount] );

	m_wRed   = m_DayNight.CurrentRed;
	m_wGreen = m_DayNight.CurrentGreen;
	m_wBlue  = m_DayNight.CurrentBlue;

	// Set the next and last colors
    m_DayNight.LastRed     = GetRValue( m_DayNight.HourTarget[m_DayNight.HourCount] ); 
	m_DayNight.LastGreen   = GetGValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
	m_DayNight.LastBlue    = GetBValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
	m_DayNight.RedTarget   = GetRValue( m_DayNight.HourTarget[m_DayNight.HourCount+1] ); 
	m_DayNight.GreenTarget = GetGValue( m_DayNight.HourTarget[m_DayNight.HourCount+1] ); 
	m_DayNight.BlueTarget  = GetBValue( m_DayNight.HourTarget[m_DayNight.HourCount+1] ); 
}


//-----------------------------------------------------------------------------
// Name: UpdateMinutes()
// Desc: 
//
//-----------------------------------------------------------------------------
void CNightDay::UpdateMinutes(long minutes)
{
    // The minute count has changed so set daylight to what it should be
	m_DayNight.MinCount = minutes;
	m_DayNight.rCounter = 0;

	m_DayNight.CurrentRed     = GetRValue( m_DayNight.HourTarget[m_DayNight.HourCount] ); 
	m_DayNight.CurrentGreen   = GetGValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
	m_DayNight.CurrentBlue    = GetBValue( m_DayNight.HourTarget[m_DayNight.HourCount] );

	m_DayNight.CurrentRed	+= m_DayNight.ChangePerMinRed * minutes;
	m_DayNight.CurrentGreen += m_DayNight.ChangePerMinGreen * minutes;
	m_DayNight.CurrentBlue	+= m_DayNight.ChangePerMinBlue * minutes;
}


//-----------------------------------------------------------------------------
// Name: UpdateHours()
// Desc: 
//
//-----------------------------------------------------------------------------
void CNightDay::UpdateHours(long hours)
{
	m_DayNight.HourCount = hours;

	// Update the DayNight Structure
	m_DayNight.LastRed     = GetRValue( m_DayNight.HourTarget[m_DayNight.HourCount] ); 
	m_DayNight.LastGreen   = GetGValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
	m_DayNight.LastBlue    = GetBValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
	m_DayNight.RedTarget   = GetRValue( m_DayNight.HourTarget[m_DayNight.HourCount+1] ); 
	m_DayNight.GreenTarget = GetGValue( m_DayNight.HourTarget[m_DayNight.HourCount+1] ); 
	m_DayNight.BlueTarget  = GetBValue( m_DayNight.HourTarget[m_DayNight.HourCount+1] );
	m_DayNight.CurrentRed     = GetRValue( m_DayNight.HourTarget[m_DayNight.HourCount] ); 
	m_DayNight.CurrentGreen   = GetGValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
	m_DayNight.CurrentBlue    = GetBValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
	m_DayNight.ChangePerMinRed		= (m_DayNight.RedTarget		- m_DayNight.LastRed)   / 60.0;
	m_DayNight.ChangePerMinGreen	= (m_DayNight.GreenTarget	- m_DayNight.LastGreen) / 60.0;
	m_DayNight.ChangePerMinBlue		= (m_DayNight.BlueTarget	- m_DayNight.LastBlue)  / 60.0;

	m_DayNight.CurrentRed	+= m_DayNight.ChangePerMinRed * m_DayNight.MinCount;
	m_DayNight.CurrentGreen += m_DayNight.ChangePerMinGreen * m_DayNight.MinCount;
	m_DayNight.CurrentBlue	+= m_DayNight.ChangePerMinBlue * m_DayNight.MinCount;
}


//-----------------------------------------------------------------------------
// Name: UpdateDayNight()
// Desc: 
//
//-----------------------------------------------------------------------------
void CNightDay::UpdateDayNight()
{
	m_DayNight.ChangePerMinRed		= (m_DayNight.RedTarget		- m_DayNight.LastRed)   / 60.0;
	m_DayNight.ChangePerMinGreen	= (m_DayNight.GreenTarget	- m_DayNight.LastGreen) / 60.0;
	m_DayNight.ChangePerMinBlue		= (m_DayNight.BlueTarget	- m_DayNight.LastBlue)  / 60.0;


	// ===========================
	// Increment the minute count
	// ===========================
	if (m_dayLength !=0)
	{
		m_DayNight.rCounter += 100 * g_rDelta;

		// Check if the Counter has passed a minute
		if (m_DayNight.rCounter > m_DayNight.MinLength)
		{
			// Increment the Number of minutes passed
			m_DayNight.MinCount++;

			// Reset the counter to count another minute
			m_DayNight.rCounter = 0;

			// Every minute change the current daylight colours
			m_DayNight.CurrentRed	+= m_DayNight.ChangePerMinRed;
			m_DayNight.CurrentGreen += m_DayNight.ChangePerMinGreen;
			m_DayNight.CurrentBlue	+= m_DayNight.ChangePerMinBlue;
			
			
			// Check if we have 60 minutes - ie, passed an hour
			if (m_DayNight.MinCount >= 60)
			{
				// Reset the minutes
				m_DayNight.MinCount = 0;

				// Update the DayNight Structure
				m_DayNight.LastRed     = GetRValue( m_DayNight.HourTarget[m_DayNight.HourCount] ); 
				m_DayNight.LastGreen   = GetGValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
				m_DayNight.LastBlue    = GetBValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
				m_DayNight.RedTarget   = GetRValue( m_DayNight.HourTarget[m_DayNight.HourCount+1] ); 
				m_DayNight.GreenTarget = GetGValue( m_DayNight.HourTarget[m_DayNight.HourCount+1] ); 
				m_DayNight.BlueTarget  = GetBValue( m_DayNight.HourTarget[m_DayNight.HourCount+1] );
				
				m_DayNight.CurrentRed     = GetRValue( m_DayNight.HourTarget[m_DayNight.HourCount] ); 
				m_DayNight.CurrentGreen   = GetGValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
				m_DayNight.CurrentBlue    = GetBValue( m_DayNight.HourTarget[m_DayNight.HourCount] );
				

				// Increment the hour count
				m_DayNight.HourCount++;
				
				// Check if 24 hours, i.e - day has passed
				if (m_DayNight.HourCount >= 24)
				{
					m_DayNight.HourCount=0;
					m_days ++;
				}
			}
		}
	}
	
	// Record the current daylight
	m_wRed   = (int)m_DayNight.CurrentRed;
	m_wGreen = (int)m_DayNight.CurrentGreen;
	m_wBlue  = (int)m_DayNight.CurrentBlue;

	CreateTimeString();
}


//-----------------------------------------------------------------------------
// Name: CombineColor()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CNightDay::CombineColor(int* wInputCol, int rgb)
{
	switch(rgb)
	{
	case 0:
		// Red
		*wInputCol -= (255 - GetRed());
		break;
	case 1:
		// green
		*wInputCol -= (255 - GetGreen());
		break;
	case 2:
		// blue
		*wInputCol -= (255 - GetBlue());
		break;
	}

	if (*wInputCol < 0)  
		*wInputCol = 0;
}

