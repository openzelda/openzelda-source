/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: NightDay.h
//
// Desc: 
//       
//-----------------------------------------------------------------------------
#ifndef CNIGHTDAY_H
#define CNIGHTDAY_H

#include <stdio.h>
#include <SDL2/SDL.h>
// Structure to hold data on the 
// current day/night state.
typedef struct _DayNight{

	float DayLength;			// Length of a day in Ticks
	float HourLength;           // Length of an hour in Ticks
	float MinLength;            // Length of a minute in Ticks
	SDL_Color HourTarget[25];

	long HourCount;
	long MinCount;
	float rCounter;

	long RedTarget;
	long GreenTarget;
	long BlueTarget;

	long LastRed;
	long LastGreen;
	long LastBlue;

	float ChangePerMinRed;
	float ChangePerMinGreen;
	float ChangePerMinBlue;

	float CurrentRed;
	float CurrentGreen;
	float CurrentBlue;

}DayNight;


//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CNightDay;


//-----------------------------------------------------------------------------
// Name: class CNightDay
// Desc: 
// 
//-----------------------------------------------------------------------------
class CNightDay
{
	long		m_dayLength;				// The length of 1 day
	long		m_actualDayLength;
	long		m_days;

	// ======================
	// DayLight RGB values
	// ======================
	int m_wRed;
	int m_wGreen;
	int m_wBlue;
	char m_szGameTimeString[50];

public:
	DayNight	m_DayNight;
	CNightDay();
	~CNightDay();

	void Save(FILE* sFile);
	void Load(FILE* sFile);
	void CombineColor(int* wInputCol, int rgb);
	void Init();
	void CalculateDayLength();
	void UpdateDayNight();
	void CreateTimeString();
	void UpdateMinutes(long minutes);
	void UpdateHours(long hours);
	void SetColors(int r, int g, int b){m_wRed = r; m_wGreen = g; m_wBlue = b;}

	// Data Brokers
	void SetMinuteCount(long val){UpdateMinutes(val);}
	void SetHourCount(long val){UpdateHours(val);}
	void SetDayCount(long val){m_days = val;}
	void SetDayLength(long val){m_dayLength = val;CalculateDayLength(); }

	// Access functions
	int GetRed()  {return m_wRed;}
	int GetGreen(){return m_wGreen;}
	int GetBlue() {return m_wBlue;}
	char* GetTimeString(){return m_szGameTimeString;}
	long GetMinuteCount(){return m_DayNight.MinCount;}
	long GetHourCount(){return m_DayNight.HourCount;}
	long GetDayCount(){return m_days;}
	

}; 


#endif

