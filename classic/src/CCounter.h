/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CCounter.h
//
// Desc: Counter Class
// 
//-----------------------------------------------------------------------------
#ifndef CCOUNTER_H
#define CCOUNTER_H

//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CCounter;

//-----------------------------------------------------------------------------
// Name: class CCounter
// Desc: 
//-----------------------------------------------------------------------------
class CCounter
{
	char	m_szIdent[64];			// Identifier for this object
	long	m_lMin;					// Min / Max Values
	long	m_lMax;		
	float	m_rValue;				// The actual Counter Value
	long	m_lTarget;				// The Target Value to reach
	float	m_lSpeed;				// Speed at which it moves from the current value to target
    
public:
	
    CCounter(char *szIdent, long lMin, long lMax);
    ~CCounter();
	void Update();


	// Data Broker Functions
	void SetMin(long lMin){m_lMin = lMin;}
	void SetMax(long lMax){m_lMax = lMax;}
	long GetMin(){return m_lMin;}
	long GetMax(){return m_lMax;}
	void SetValue(long lVal);
	void SetTarget(long lTarg);
	void SetSpeed(long lSpeed){m_lSpeed = (float)lSpeed;}

	// Access Functions
	long GetValue(){return (long)m_rValue;}
	long GetTarget(){return m_lTarget;}
	
};


#endif

