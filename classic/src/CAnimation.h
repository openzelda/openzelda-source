/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CAnimation.h
//
// Desc: Animated Image Class
// 
//-----------------------------------------------------------------------------
#ifndef CANIMATION_H
#define CANIMATION_H

#include <stdio.h>
#include "LList.h"
#include "GeneralData.h"


// Structure for Holding info on a frame of animation
struct _Sprite
{
	char szSpriteCode[ MAX_CHARS ];
	int	 wXOffset;
	int	 wYOffset;
};


//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CAnimation;

//-----------------------------------------------------------------------------
// Name: class CImage
// Desc: 
//-----------------------------------------------------------------------------
class CAnimation
{
	LList<_Sprite> m_pSprites;  	// Linked List of Sprite Codes
	char	m_szIdent[64];			// Identifier for this object
	float	m_rAnimIncrement;		// Rate at which the animation increments
	int		m_wAnimMax;				// The number of frames in the animation
	float	m_rAnimCount;
	int		m_wDirection;			// Direction animation is going
	bool	m_wEndAnim;				// Are we at the end of the animation?
	bool	m_fLoopAnim;			// Does this animation loop?
	
    
public:
	
    CAnimation(char *szIdent, float rSpeed);
    ~CAnimation();

	void Draw(long x, long y, int wDepth, int r, int g, int b, int a, float rot, int scale, int nLayer = 0, int flip = 0);
	void AddFrame(char *szName, int x, int y);
	void SetAnimationSpeed(float rSpeed);
	void SetAnimationCount(float rCount);
	void Increment();
	_Sprite* GetFirstSprite();

	void SetAnimationDir(int wVal){m_wDirection = wVal;}
	void SetAnimLoops(bool fVal){m_fLoopAnim = fVal;}
	
	// Access Functions
	_Sprite* GetCurrentSprite();
	char* GetCurrentSpriteCode();
	char* GetIdent(){return m_szIdent;}
	float GetAnimCount(){return m_rAnimCount;}
	int GetMaxCount(){return m_wAnimMax - 1;}
	bool FinishedAnim(){return m_wEndAnim;}
	bool GetLoopAnim(){return m_fLoopAnim;}
};


#endif

