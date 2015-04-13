/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// Boundary.h: interface for the CBoundary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUNDARY_H__A9AAEA59_C01D_4649_AD81_9E2B9D78D3DC__INCLUDED_)
#define AFX_BOUNDARY_H__A9AAEA59_C01D_4649_AD81_9E2B9D78D3DC__INCLUDED_

#include "LList.h"
#include "CEntity.h"

class CEntity;

class CBoundary  
{
	LList<CEntity>	m_pEntityList;		// List of Entities in this group

public:

	RECT2D bounds;		// Bounding rectangle for the group
	long lScriptIndex;	// Unsure what this does really
	int	 wLoopBack;		// Not sure if used
	char szMusic[64];	// Name of the music file to play
	
public:
	CBoundary();
	virtual ~CBoundary();

	void AddEntity( CEntity* pNewEntity );
	void RemoveEntity(CEntity* pNewEntity );
	void ClearEntities();

	LList<CEntity>* GetEntList(){ return &m_pEntityList;}
};

#endif // !defined(AFX_BOUNDARY_H__A9AAEA59_C01D_4649_AD81_9E2B9D78D3DC__INCLUDED_)
