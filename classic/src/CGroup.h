/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CGroup.h
//
// Desc: Holds a set of Groups
//       
//-----------------------------------------------------------------------------
#ifndef CGROUP_H
#define CGROUP_H
#define MAXLOCAL 1000

#include "Boundary.h"
#include "GeneralData.h"
#include "LList.h"
#include "AMX/amx.h"


//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CGroup;
class CBoundary;


//-----------------------------------------------------------------------------
// Name: class CGroup
// Desc:  
//-----------------------------------------------------------------------------
class CGroup
{
	LList<CBoundary>m_Boundary;				// List of Boundaries in this QST
	CBoundary*		m_pCurrentBoundary;		// pointer to the Current Boundary
	CBoundary*		m_pLastBoundary;		// pointer to the last boundary
	int				m_wNumBoundary;
	int				m_wLocal[MAXLOCAL];		// The local variables for this group

	// ===================
	// Script Variables
	// ===================
	AMX*	 m_pAmx;						// The Abstract Machine to run the script
	void*	 m_pProgram;					// Pointer to the location of the script


public:
    CGroup();
    ~CGroup();

	void Initialise();
	bool GetBoundary(long x, long y);
	CBoundary*  ReturnBoundary(long x, long y);
	void Save(FILE *sfile);
	void Load(FILE *sfile);
	void ChangeMusic();
	void RunScript();
	void ClearEntityLists();

	// Access Functions
	LList<CBoundary> GetBoundaryList(){return m_Boundary;}
	int GetBoundaryCount(){ return m_wNumBoundary; }
	AMX* GetScript(){return m_pAmx;}
	int GetLocal(int index){return m_wLocal[index];}
	CBoundary* GetCurrentBoundary(){ return m_pCurrentBoundary;}
	CBoundary* GetLastBoundary(){ return m_pLastBoundary;}

	// Data Broker Functions
	void SetBoundaryCount(int n){ m_wNumBoundary = n;}
	void AddBoundary(CBoundary* pGroup);
	void SetLocal(int index, int val){if (index < MAXLOCAL && index >= 0){m_wLocal[index] = val;}}


};


#endif

