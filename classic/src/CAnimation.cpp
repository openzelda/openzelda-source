/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CAnimation.cpp
//
// Desc: 
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include "CAnimation.h"
#include "CGame.h"
#include "Script.h"
#include "SaveLoadFunctions.h"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame*		 g_pGame;
extern float		 g_rDelta;


//-----------------------------------------------------------------------------
// Name: Constructor
// Desc:
// 
//-----------------------------------------------------------------------------
CAnimation::CAnimation(char *szIdent, float rSpeed)
{
	// Copy the supplied Identifier
	strcpy(m_szIdent, szIdent);

	// Set some defaults
	m_rAnimIncrement	= rSpeed;
	m_rAnimCount		= 0.00;
	m_wAnimMax			= 0;
	m_wDirection		= 1;
	m_wEndAnim			= false;
	m_fLoopAnim			= true;
}


//-----------------------------------------------------------------------------
// Name: Destructor
// Desc: 
//
//-----------------------------------------------------------------------------
CAnimation::~CAnimation()
{
	m_pSprites.free();
}

//-----------------------------------------------------------------------------
// Name: AddFrame()
// Desc: Adds a Frame of Animation to the Animated Image
//
//-----------------------------------------------------------------------------
void CAnimation::AddFrame(char *szName, int x, int y)
{
	_Sprite* pSprite = new _Sprite();

	// Copy the code
	strcpy(pSprite->szSpriteCode, szName);

	// Copy the Offsets
	pSprite->wXOffset = x;
	pSprite->wYOffset = y;

	// Now add this new Surface to the list of Animation Frames.
	m_pSprites.additem(pSprite, "");

	// Increment the Frame counter;
	m_wAnimMax += 1;
}

//-----------------------------------------------------------------------------
// Name: SetAnimationSpeed()
// Desc: 
//
//-----------------------------------------------------------------------------
void CAnimation::SetAnimationSpeed(float rSpeed)
{
	// Perform some sanity checks on the entered data
	if (rSpeed < 0)
		return;

	if (rSpeed > 9999)
		return;

	m_rAnimIncrement = rSpeed;
}

//-----------------------------------------------------------------------------
// Name: SetAnimationCount()
// Desc: 
//
//-----------------------------------------------------------------------------
void CAnimation::SetAnimationCount(float rCount)
{
	// Perform some sanity checks on the entered data
	if (rCount < 0)
		return;

	if (rCount > m_wAnimMax)
		return;

	m_rAnimCount = rCount;
}


//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: Animates and Displays the Object
//
//-----------------------------------------------------------------------------
void CAnimation::Draw(long x, long y, int wDepth, int r, int g, int b, int a, float rot, int scale, int nLayer, int flip)
{
	_Sprite* pTemp = GetCurrentSprite();

	if (pTemp)
	{
		// Draw the Sprite
		g_pGame->PutSprite(x + (long)pTemp->wXOffset, 
						   y + (long)pTemp->wYOffset, 
						   wDepth, 
						   pTemp->szSpriteCode, r, g, b, a, scale, rot, nLayer, flip);
	}
	else
	{
		// Couldn't Get the current sprite some some reason, instead 
		// of not drawing this frame we can draw the first frame instead - this
		// prevents flicker
		pTemp = GetFirstSprite();

		if( pTemp )
		g_pGame->PutSprite(x + pTemp->wXOffset, 
		               y + pTemp->wYOffset, 
					   wDepth, 
					   pTemp->szSpriteCode, r, g, b, a, scale, rot, nLayer);

	}	
}


//-----------------------------------------------------------------------------
// Name: GetFirstSprite()
// Desc: Returns the First Sprite in the List
//
//-----------------------------------------------------------------------------
_Sprite* CAnimation::GetFirstSprite()
{
	return m_pSprites.GetObjectFromNumericID( 0 );
}

//-----------------------------------------------------------------------------
// Name: Increment()
// Desc: 
//
//-----------------------------------------------------------------------------
void CAnimation::Increment()
{
	m_wEndAnim = false;

	// Only animate when the game is fully unpaused
	if (g_pGame->GetPauseLevel())
		return;

	if (m_wDirection == 1)
	{
		// Increment the animation Counter
		m_rAnimCount += m_rAnimIncrement * g_rDelta;

		// Check it's not too high
		if ( m_rAnimCount >= m_wAnimMax)
		{
			if (m_fLoopAnim)
			   m_rAnimCount = 0;
			else
			   m_rAnimCount	= m_wAnimMax - 1;

			m_wEndAnim = true;
		}
	}
	else
	{
		// Decrement the animation Counter
		m_rAnimCount -= m_rAnimIncrement * g_rDelta;

		// Check it's not too high
		if ( (int)m_rAnimCount < 0)
		{
			if (m_fLoopAnim)
			   m_rAnimCount = m_wAnimMax - 1;
			else
			   m_rAnimCount	= 0;

			m_wEndAnim = true;
		}
	}
}


//-----------------------------------------------------------------------------
// Name: GetCurrentSprite()
// Desc: Returns the Current Sprite in the List
//
//-----------------------------------------------------------------------------
_Sprite* CAnimation::GetCurrentSprite()
{
	return m_pSprites.GetObjectFromNumericID(int(m_rAnimCount));
}


//-----------------------------------------------------------------------------
// Name: GetCurrentSpriteCode()
// Desc: 
//
//-----------------------------------------------------------------------------
char* CAnimation::GetCurrentSpriteCode()
{
	return m_pSprites.GetObjectFromNumericID(int(m_rAnimCount))->szSpriteCode;
}


//-----------------------------------------------------------------------------
// Name: GetAnim()
// Desc:
//  
//-----------------------------------------------------------------------------
CAnimation* GetAnim(AMX *amx, cell* params)
{
	char szString[ MAX_CHARS ];
	
	// Check we have some parameters
	if (params[0] == 0)
		return NULL;

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);

	// Return a pointer to the correct animtion
    return g_pGame->GetAnimationList()->GetObjectFromID(szString);
}


//-----------------------------------------------------------------------------
// Name: CreateAnimation(22, ident)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CreateAnimation(AMX *amx, cell *params)
{
	char szString[16];
	cell *cptr;

	// Check we have some parameters
	if (params[0] == 0)
		return 0;

	g_pGame->AddAnimation(params[1], szString);

	// Now we want to place the ident of this Animation in the supplied
	// String Buffer
	amx_GetAddr(amx,params[2],&cptr);
    amx_SetString(cptr, szString, 0);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: CreateAnimWithID(22, "MyAnim1")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CreateAnimationWithID(AMX *amx, cell *params)
{
	char szString[ MAX_CHARS ];

	// Check we have some parameters
	if (params[0] == 0)
		return 0;

	// Get the String from the first Param
	GetStringParam(amx, params[2], szString);

	// Add the animation
	g_pGame->AddAnimationID(params[1], szString);
	return 0;	
}

//-----------------------------------------------------------------------------
// Name: AddAnimframe()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL AddAnimframe(AMX *amx, cell *params)
{
	char szString[ MAX_CHARS ];

	// Check we have some parameters
	if (params[0] == 0)
		return 0;

	CAnimation* pTemp = GetAnim(amx, params);

	// Get the String from the first Param
	GetStringParam(amx, params[4], szString);

	if (pTemp)
		pTemp->AddFrame(szString, params[2], params[3]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: DrawAnim()
// DrawAnim(ident[], x, y, depth, layer = 0, r = 255, g = 255, b = 255, a = 255, rot = 0, scale = 100);
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DrawAnim(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
	{
	    int numparam = params[0]/sizeof(cell);
		pTemp->Draw(params[2], params[3], params[4],
			        params[6], params[7], params[8],
					params[9], params[10], params[11], params[5], numparam == 12 ? params[12] : 0);
		pTemp->Increment();
	}
	return 0;
}


//-----------------------------------------------------------------------------
// Name: DrawAnimNoInc()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DrawAnimNoInc(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
	{
	    int numparam = params[0]/sizeof(cell);
		pTemp->Draw(params[2], params[3], params[4],
			       params[6], params[7], params[8],
				   params[9], params[10], params[11], params[5], numparam == 12 ? params[12] : 0);
	}	   
	return 0;
}


//-----------------------------------------------------------------------------
// Name: SetAnimSpeed()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetAnimSpeed(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
		pTemp->SetAnimationSpeed(params[2]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetAnimDirection()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetAnimDirection(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
	{
		if (params[2] == 0)
			pTemp->SetAnimationDir(0);
		else
			pTemp->SetAnimationDir(1);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetAnimImage()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetAnimImage(AMX *amx, cell *params)
{
	cell *cptr;
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
	{
		amx_GetAddr(amx,params[2],&cptr);
        amx_SetString(cptr, pTemp->GetCurrentSpriteCode(), 0);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetAnimWidth()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetAnimWidth(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)	
		return g_pGame->GetWidthHeight(pTemp->GetCurrentSpriteCode(), true);

	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetAnimHeight()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetAnimHeight(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
		return g_pGame->GetWidthHeight(pTemp->GetCurrentSpriteCode(), false);

	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetAnimCount()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetAnimCount(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
		return pTemp->GetAnimCount();
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetAnimCount()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetAnimCount(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
		pTemp->SetAnimationCount(params[2]);

	return 0;
}


//-----------------------------------------------------------------------------
// Name: IncrementAnim()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL IncrementAnim(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
		pTemp->Increment();

	return 0;
}

//-----------------------------------------------------------------------------
// Name: DeleteAnimation()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DeleteAnimation(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
		g_pGame->GetAnimationList()->removeitem(pTemp);

	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetMaxAnim()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetMaxAnim(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
		return pTemp->GetMaxCount();
	return 0;
}

//-----------------------------------------------------------------------------
// Name: FinishedAnim()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL FinishedAnim(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
		return pTemp->FinishedAnim();
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetAnimLoop("sword1", false)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetAnimLoop(AMX *amx, cell *params)
{
	CAnimation* pTemp = GetAnim(amx, params);

	if (pTemp)
		pTemp->SetAnimLoops(params[2]);
	return 0;
}



// Define a List of native Animation functions
extern AMX_NATIVE_INFO animation_Natives[] = {
	{ "CreateAnim", CreateAnimation },
	{ "CreateAnimWithID", CreateAnimationWithID },
	{ "AddAnimframe",    AddAnimframe },
	{ "DrawAnim",        DrawAnim },
	{ "IncrementAnim",   IncrementAnim },
	{ "SetAnimSpeed",    SetAnimSpeed },
	{ "GetAnimImage",    GetAnimImage },
	{ "GetAnimCount",    GetAnimCount },
	{ "SetAnimCount",    SetAnimCount },
	{ "DeleteAnim", DeleteAnimation },
	{ "GetMaxAnim", GetMaxAnim },
	{ "GetAnimWidth", GetAnimWidth },
	{ "GetAnimHeight", GetAnimHeight },
	{ "DrawAnimNoInc", DrawAnimNoInc },
	{ "SetAnimDirection", SetAnimDirection },
	{ "FinishedAnim", FinishedAnim },
	{ "SetAnimLoop", SetAnimLoop },
	{ 0, 0 }      
};



