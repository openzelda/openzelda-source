/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// EntityList.h: interface for the CEntityList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTITYLIST_H__5874FDEB_9B2E_4DD6_BDB4_69281D8E31C4__INCLUDED_)
#define AFX_ENTITYLIST_H__5874FDEB_9B2E_4DD6_BDB4_69281D8E31C4__INCLUDED_

#include "CEntity.h"
#include "LList.h"

class CEntityList  
{
	LList<CEntity>	m_pEntityList;			// The Big list of entities active
	CHash*			m_pNumericHash;			// Hash table to sort entities by their Counter Values
	CEntity*		m_pThisEntity;			// The "this" entity
	long			m_lEntityCount;
	long			m_lNumDynamicEnt;		// Number of a Dynamic entity to form the Identifier
	CEntity*		m_pCurrentEntity;		// What GetNextEntity sets and GetCurrentEnt gets
	long			m_lSearchX;				// Where GetNextEntity searches from
	long			m_lSearchY;
	long			m_lSearchDist;
	CBoundary*		m_pBoundaryToSearch;
	long			m_lEntityIndex;



public:
	CEntityList();
	virtual ~CEntityList();

	CEntity* AddEntity( bool bLibrary, long x, long y, 
		                char* szIdent, bool fDynamic, 
						bool bParam, char* String);
	
	void     GroupEntities();
	CEntity* GetEntityWithCount(  char* szCount );
	long     GetCurrentEntity();
	
	void	 StartEntity(long lDist, long x, long y);
	void     AddEntityRects();
	void     RemoveEntity(CEntity* pTemp);
	void     RunEntities( long lWorldX, long lWorldY);
	void     ReHashEntities( long lNumEnt );
	CEntity* GetPlayerEntity();
	void	 Save(FILE* sFile);
	void	 Load(FILE* sFile);

	void	 StartEntities();
	void     StartEntityWholeList( long lDist, long x, long y );
	bool	 NextEntityWholeList();
	bool     NextEntity();

	// Gets
	CEntity* GetThisEntity(){return m_pThisEntity;}
	LList<CEntity>* GetEntityList(){return &m_pEntityList;}
	long GetDynEntCount(){return m_lNumDynamicEnt;}

	// Sets
	void IncrementDynEnt(){m_lNumDynamicEnt++;}
	void SetThisEntity(CEntity* pEntity){if (pEntity){m_pThisEntity = pEntity;}}


};

#endif // !defined(AFX_ENTITYLIST_H__5874FDEB_9B2E_4DD6_BDB4_69281D8E31C4__INCLUDED_)
