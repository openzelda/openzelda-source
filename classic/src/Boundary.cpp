/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// Boundary.cpp: implementation of the CBoundary class.
//
//////////////////////////////////////////////////////////////////////

#include "Boundary.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBoundary::CBoundary()
{

}

CBoundary::~CBoundary()
{
	m_pEntityList.free(true);
}


//-----------------------------------------------------------------------------
// Name: AddEntity()
// Desc: 
//
//-----------------------------------------------------------------------------
void CBoundary::AddEntity( CEntity* pNewEntity )
{
	// Add the entity supplied to the list
	m_pEntityList.additem(pNewEntity, pNewEntity->GetIdent());
}


//-----------------------------------------------------------------------------
// Name: RemoveEntity()
// Desc: 
//
//-----------------------------------------------------------------------------
void CBoundary::RemoveEntity(CEntity* pNewEntity )
{
	m_pEntityList.removeitem( pNewEntity, true );
}


//-----------------------------------------------------------------------------
// Name: ClearEntities()
// Desc: 
//
//-----------------------------------------------------------------------------
void CBoundary::ClearEntities()
{
	// Clear the Linked list and start again
	m_pEntityList.free(true);
}
