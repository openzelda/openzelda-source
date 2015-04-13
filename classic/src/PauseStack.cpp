/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// PauseStack.cpp: implementation of the CPauseStack class.
//
//////////////////////////////////////////////////////////////////////

#include "PauseStack.h"
#include "GeneralData.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPauseStack::CPauseStack()
{
	m_nStackLevel = 0;

	// Set the bottom of the stack - unpaused
	m_nPauseStack[0].nPauseLevel = 0;
	m_nPauseStack[0].lLineNum	 = -1;
}


CPauseStack::~CPauseStack()
{
}



bool CPauseStack::SearchStack(long lLineNum, char* szScriptName, int nLevel)
{
	int n;

	// Search the stack
	for (n = 1; n < m_nStackLevel + 1; n++)
	{
		if ( !strcmp(szScriptName, m_nPauseStack[n].szScriptName))
		{
			// Check if the last call was from the same line
			if ( lLineNum == m_nPauseStack[n].lLineNum )
			{
				// Check that values set are not the same
				if ( m_nPauseStack[n].nPauseLevel == nLevel)
				{
					// This is the same call as last time repeating
					return true;
				}
			}
		}	
	}
	return false;
}



void CPauseStack::SetPauseLevel(int nLevel, long lLineNum, char* szScriptName )
{
	// Check this entry isnt the same as the last entry
	if (m_nStackLevel > 0 && lLineNum != -1)	// Dont check if we are at the bottom of the stack
	{
		// Try and find a matching entry in the stack
		if ( SearchStack(lLineNum, szScriptName, nLevel))
			return;
	}

	if (nLevel != 0)
	{
		// Check stack isnt too big
		if (m_nStackLevel >= MAX_STACK)
		{
			// Stack overflow!
			PurgeStack();
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","*** PauseLevel Stack Overflow! ***" );
		}

		m_nStackLevel++;

		// Recod this new info in a stack entry
		m_nPauseStack[m_nStackLevel].nPauseLevel = nLevel;

		if ( lLineNum != -1 )
		{
			m_nPauseStack[m_nStackLevel].lLineNum = lLineNum;
			strcpy(m_nPauseStack[m_nStackLevel].szScriptName, szScriptName);
		}

	}
	else
	{
		if (m_nStackLevel > 0)
			m_nStackLevel--;
	}
}


void CPauseStack::PurgeStack()
{
	m_nStackLevel = 0;
}
