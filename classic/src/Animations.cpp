/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: Animations.cpp
//
// Desc: 
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include "CGame.h"
#include "SaveLoadFunctions.h"


//-----------------------------------------------------------------------------
// Name: CreateAnimations()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CGame::CreateAnimations()
{
	// Create a hash table for the animation list with a
	// Standard size
	m_pAnimList.CreateHash( 128 );

}

//-----------------------------------------------------------------------------
// Name: AddAnimation()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CGame::AddAnimation(float rSpeed, char* szString)
{
	// Increment the CreateAnimation Call Count
	IncrementDynAnim();

	// Create an Ident Based on the Dynamic Entity Count
	sprintf(szString, "__an%d", GetDynAnimCount());

	// Now Add the Animation Object to the game
	CAnimation *pNewAnim = new CAnimation(szString, rSpeed);

	m_pAnimList.additem(pNewAnim, szString);
}


//-----------------------------------------------------------------------------
// Name: AddAnimationID()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CGame::AddAnimationID(float rSpeed, char* szString)
{
	// Now Add the Animation Object to the game
	CAnimation *pNewAnim = new CAnimation(szString, rSpeed);
	m_pAnimList.additem(pNewAnim, szString);
}
