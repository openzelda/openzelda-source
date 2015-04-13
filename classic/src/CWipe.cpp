/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CWipe.cpp
//
// Desc: 
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include "CWipe.h"
#include "CGame.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern float		 g_rDelta;
extern CGame*		 g_pGame;


//-----------------------------------------------------------------------------
// Name: Constructor
// Desc:
// 
//-----------------------------------------------------------------------------
CWipe::CWipe()
{
	m_fWiping		= false;
	m_bSecondWiping = false;
	m_rWipeCount	= 0;
	m_wWipeSpeed	= 80;
}


//-----------------------------------------------------------------------------
// Name: Destructor
// Desc: 
//
//-----------------------------------------------------------------------------
CWipe::~CWipe()
{
}


//-----------------------------------------------------------------------------
// Name: BeginWipe()
// Desc: 
//
//-----------------------------------------------------------------------------
void CWipe::BeginWipe(int wDir, int wSpeed)
{
	long x;
	long y;
	m_bSecondWiping = false;

	// Try and get a pointer to the player entitiy
	CEntity* pTemp = g_pGame->GetEntityList()->GetObjectFromID("player1");
	if (pTemp)
	{
		x = pTemp->GetXPos();
		y = pTemp->GetYPos();
	}
    else
	{
		x = g_pGame->GetWorldX();
		y = g_pGame->GetWorldY();	
	}

	// Check that there is another boundary on the other side
	switch(wDir)
	{
	case NORTH:
		if (!g_pGame->GetMap()->GetGroupSet()->GetBoundary(x,  y - 50))
			return;
		break;

	case EAST:
		if (!g_pGame->GetMap()->GetGroupSet()->GetBoundary(x + 50,  y))
			return;
		break;
	case SOUTH:
		if (!g_pGame->GetMap()->GetGroupSet()->GetBoundary(x,  y + 50))
			return;
		break;

	case WEST:
		if (!g_pGame->GetMap()->GetGroupSet()->GetBoundary(x - 50,  y))
			return;
		break;
	}

	// Init Wiping variables
	m_wWipeSpeed	= wSpeed;
	m_wWipeFacing	= wDir;
	m_fWiping		= true;
	m_rWipeCount	= 0;
	m_lWipeStartX	= g_pGame->GetWorldX();
	m_lWipeStartY	= g_pGame->GetWorldY();

	// Get the temp world coordintes before the wipe
	m_lTempWorldXBefore = g_pGame->GetTempWorldX();
	m_lTempWorldYBefore = g_pGame->GetTempWorldY();

	// Pause everything while we wipe
	g_pGame->SetPauseLevel(2, -1, NULL);
}

//-----------------------------------------------------------------------------
// Name: DoWipe()
// Desc: 
//
//-----------------------------------------------------------------------------
void CWipe::DoWipe()
{
	if (!m_fWiping)
		return;

	if (m_bSecondWiping)
	{
		DoSecondWipe();
		return;
	}

	int wipeLimit = 0;

	switch( m_wWipeFacing )
	{
	case NORTH:
		wipeLimit = VSCREEN_HEIGHT;
		g_pGame->SetWorldY(m_lWipeStartY - m_rWipeCount);
		break;

	case EAST:
		wipeLimit = VSCREEN_WIDTH;
		g_pGame->SetWorldX(m_lWipeStartX + m_rWipeCount);
		break;

	case SOUTH:
		wipeLimit = VSCREEN_HEIGHT;
		g_pGame->SetWorldY(m_lWipeStartY + m_rWipeCount);
		break;

	case WEST:
		wipeLimit = VSCREEN_WIDTH;
		g_pGame->SetWorldX(m_lWipeStartX - m_rWipeCount);
		break;
	};

	float WipeAmt = m_wWipeSpeed * g_rDelta;

	// Put this here to make sure we dont wipe too much and go over
	// this happened before and it loaded the wrong screens and caused
	// the blurry screen bug.
	if ( m_rWipeCount + WipeAmt > wipeLimit)
	{
		m_rWipeCount = wipeLimit;
		EndWipe();
	}
	else
		m_rWipeCount += m_wWipeSpeed * g_rDelta;

//	m_rWipeCount+= m_wWipeSpeed * g_rDelta;
//	if (m_rWipeCount > wipeLimit + 4)
//		EndWipe();
}


//-----------------------------------------------------------------------------
// Name: EndWipe()
// Desc: 
//
//-----------------------------------------------------------------------------
void CWipe::EndWipe()
{
	m_rWipeCount	= 0;
	m_fWiping		= false;

	// UnPause everything
	g_pGame->SetPauseLevel(0, -1, NULL);

	// Get the next Group Set based on the player entities position
	CEntity* pTemp = g_pGame->GetEntityList()->GetObjectFromID("player1");
	if (pTemp)
	{
       g_pGame->GetMap()->GetGroupSet()->GetBoundary(pTemp->GetXPos(), pTemp->GetYPos());
	   CheckForSecondWipe();
	}

	// We may have wondered into new screens so reload them all
	g_pGame->GetMap()->RestoreScreens();
}


//-----------------------------------------------------------------------------
// Name: CheckForSecondWipe()
// Desc: 
//
//-----------------------------------------------------------------------------
void CWipe::CheckForSecondWipe()
{
	long lDiff;

	// Get the next Group Set based on the player entities position
	CEntity* pTemp = g_pGame->GetEntityList()->GetObjectFromID("player1");

	
	if ( m_wWipeFacing == EAST || m_wWipeFacing == WEST)
	{
		// TempWorldY should not have changed since before wipe,
		// if it has it should have only change by half a screen at most
		lDiff = g_pGame->CheckTempYWorld(pTemp->GetYPos()) - m_lTempWorldYBefore;

		if ( abs(lDiff) > 0 && abs(lDiff) < VSCREEN_HEIGHT )
		{
			// Need to do a second wipe
			m_lSecondWipeLimit = abs(lDiff);
			
			if ( lDiff < 0 )
				m_wWipeFacing = NORTH;
			else
				m_wWipeFacing = SOUTH;

			BeginSecondWipe();
		}
		return;
	}
	
	if ( m_wWipeFacing == NORTH || m_wWipeFacing == SOUTH)
	{
		// TempWorldX should not have changed since before wipe,
		// if it has it should have only change by half a screen at most
		lDiff = g_pGame->CheckTempXWorld(pTemp->GetXPos()) - m_lTempWorldXBefore;

		if ( abs(lDiff) > 8 && abs(lDiff) < VSCREEN_WIDTH )
		{
			// Need to do a second wipe
			m_lSecondWipeLimit = abs(lDiff);
			
			if ( lDiff < 0 )
				m_wWipeFacing = WEST;
			else
				m_wWipeFacing = EAST;

			BeginSecondWipe();
		}
		return;
	}
}


//-----------------------------------------------------------------------------
// Name: BeginSecondWipe()
// Desc: 
//
//-----------------------------------------------------------------------------
void CWipe::BeginSecondWipe()
{
	m_bSecondWiping = true;
	m_fWiping		= true;
	m_lWipeStartX	= g_pGame->GetTempWorldX();
	m_lWipeStartY	= g_pGame->GetTempWorldY();
	g_pGame->SetPauseLevel(2, -1, NULL);
}

//-----------------------------------------------------------------------------
// Name: DoSecondWipe()
// Desc: 
//
//-----------------------------------------------------------------------------
void CWipe::DoSecondWipe()
{
	switch( m_wWipeFacing )
	{
	case NORTH:
		g_pGame->SetWorldY(m_lWipeStartY - m_rWipeCount);
		break;

	case EAST:
		g_pGame->SetWorldX(m_lWipeStartX + m_rWipeCount);
		break;

	case SOUTH:
		g_pGame->SetWorldY(m_lWipeStartY + m_rWipeCount);
		break;

	case WEST:
		g_pGame->SetWorldX(m_lWipeStartX - m_rWipeCount);
		break;
	};


	float WipeAmt = m_wWipeSpeed * g_rDelta;

	// Put this here to make sure we dont wipe too much and go over
	// this happened before and it loaded the wrong screens and caused
	// the blurry screen bug.
	if ( m_rWipeCount + WipeAmt > m_lSecondWipeLimit)
	{
		m_rWipeCount = m_lSecondWipeLimit;
		EndSecondWipe();
	}
	else
		m_rWipeCount += m_wWipeSpeed * g_rDelta;

/*
	m_rWipeCount+= m_wWipeSpeed * g_rDelta;
	
	if (m_rWipeCount > m_lSecondWipeLimit)
		EndSecondWipe();*/
}



//-----------------------------------------------------------------------------
// Name: EndSecondWipe()
// Desc: 
//
//-----------------------------------------------------------------------------
void CWipe::EndSecondWipe()
{
	m_rWipeCount	= 0;
	m_fWiping		= false;
	m_bSecondWiping = false;
	
	// UnPause everything
	g_pGame->SetPauseLevel(0, -1, NULL);

	CEntity* pTemp = g_pGame->GetEntityList()->GetObjectFromID("player1");
	g_pGame->UpdateWorldCo(pTemp->GetXPos(), pTemp->GetYPos());

	// We may have wondered into new screens so reload them all
	g_pGame->GetMap()->RestoreScreens();
}


