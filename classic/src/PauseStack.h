/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// PauseStack.h: interface for the CPauseStack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAUSESTACK_H__44CBBBDB_08A0_4897_99FE_67C4C26B3C38__INCLUDED_)
#define AFX_PAUSESTACK_H__44CBBBDB_08A0_4897_99FE_67C4C26B3C38__INCLUDED_

#define MAX_STACK 20

struct _PauseEntry{
	int		nPauseLevel;			// The pause level being set
	char	szScriptName[32];		// Name of script which called it
	long	lLineNum;				// Line number in script call
};


class CPauseStack  
{
	_PauseEntry		m_nPauseStack[MAX_STACK];
	int				m_nStackLevel;

public:
	CPauseStack();
	virtual ~CPauseStack();

	void SetPauseLevel(int nLevel, long lLineNum, char* szScriptName );
	void PurgeStack();
	int  GetPauseLevel(){ return m_nPauseStack[m_nStackLevel].nPauseLevel;}
	bool SearchStack(long lLineNum, char* szScriptName, int nLevel);
};

#endif // !defined(AFX_PAUSESTACK_H__44CBBBDB_08A0_4897_99FE_67C4C26B3C38__INCLUDED_)
