/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// EntityList.cpp: implementation of the CEntityList class.
//
//////////////////////////////////////////////////////////////////////
#include "EntityList.h"
#include "GeneralData.h"
#include "SaveLoadFunctions.h"
#include "CGame.h"

extern CGame*         g_pGame;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEntityList::CEntityList()
{
    m_pNumericHash        = NULL;
    m_pThisEntity        = NULL;
    m_pCurrentEntity    = NULL;
    m_pBoundaryToSearch = NULL;
    m_lEntityCount        = 0;
    m_lNumDynamicEnt    = 0;
    m_lSearchX            = 0;
    m_lSearchY            = 0;
    m_lSearchDist        = 0;
    m_lEntityIndex        = 0;

    // Create a standard sized hash table for the entities until we get going
    m_pEntityList.CreateHash( 512 );
    m_pNumericHash = new CHash( 512 );
}

CEntityList::~CEntityList()
{
    delete m_pNumericHash;
    m_pEntityList.free();
}


//-----------------------------------------------------------------------------
// Name: ReHashEntities()
// Desc: Re-Hashes all the entities with the correct sized has table
//
//-----------------------------------------------------------------------------
void CEntityList::ReHashEntities( long lNumEnt )
{
    char szHashString[24];

    // Recreate the numeric Hash table
    delete m_pNumericHash;
    m_pNumericHash = new CHash( lNumEnt );

    if (m_pEntityList.isempty())
        return;

    // Move to the Start of the list
    m_pEntityList.tostart();
    do
    {
        // Add this entity to the Numeric Hash Table
        sprintf(szHashString, "%d", m_pEntityList.getcurrent()->GetCount());
        m_pNumericHash->AddString(szHashString, m_pEntityList.getcurrent());
    }while(m_pEntityList.advance());

    // Recreate the Text Hash Table
    m_pEntityList.ReHash( lNumEnt );
}


//-----------------------------------------------------------------------------
// Name: StartEntities()
// Desc: Runs each Entity Script once in header mode
//
//-----------------------------------------------------------------------------
void CEntityList::StartEntities()
{
    if (m_pEntityList.isempty())
        return;

    // Move to the Start of the list
    m_pEntityList.tostart();
    do
    {
        m_pThisEntity = m_pEntityList.getcurrent();
        m_pThisEntity->Initialise();
        
    }while(m_pEntityList.advance());

    // Now that all entities know which group they are in
    // we can add them to the correct group list.
    GroupEntities();
}

//-----------------------------------------------------------------------------
// Name: GroupEntities()
// Desc: Puts each entity into a group's Linked List
//
//-----------------------------------------------------------------------------
void CEntityList::GroupEntities()
{
    CBoundary* pBound;

    if (m_pEntityList.isempty())
        return;

    // Clear all the existing group entity Lists
    g_pGame->GetMap()->GetGroupSet()->ClearEntityLists();

    // Move to the Start of the list
    m_pEntityList.tostart();
    do
    {    
        // Get the boundary this entity is in
        pBound = m_pEntityList.getcurrent()->GetBoundary();

        if ( pBound )
        {
            // Add the current entity to the group's list
            pBound->AddEntity( m_pEntityList.getcurrent() );
        }
        
    }while(m_pEntityList.advance());
}


//-----------------------------------------------------------------------------
// Name: RunEntities()
// Desc: Runs each entities script if they are in range
//
//-----------------------------------------------------------------------------
void CEntityList::RunEntities( long lWorldX, long lWorldY)
{
	CEntity* pCurrent;
	long lActive;

	if (m_pEntityList.isempty())
		return;

	// Move to the Start of the list
	m_pEntityList.tostart();
	while(m_pEntityList.advance())
	{
		// Get the current entity
		pCurrent = m_pEntityList.getcurrent();

		// If this is an enemy, check its group is the same as the current
		if ( pCurrent->CheckForGroup())
		{
			lActive = pCurrent->GetActiveDist();

			// Check the entities Active Distance
			if ( lActive >= 0 || lActive == -2)
			{
				// If the active dist is -2 then always run - dont check it
				if (lActive != -2)
				{
					// If the Centre of the screen is not near enough to the entity
					// Then Dont run it's Script
					if (!isNearPoint(lWorldX + VSCREEN_WIDTH / 2, 
									 lWorldY + VSCREEN_HEIGHT / 2,
									 (long)pCurrent->GetXPos(), 
									 (long)pCurrent->GetYPos(),
									 pCurrent->GetActiveDist()))
						continue;
				}

				// Run the Current Entities Script
				m_pThisEntity = pCurrent;
				pCurrent->RunScript();
			}
		}
    }
    
    m_pEntityList.tostart();
    while(m_pEntityList.advance())
    {
        pCurrent = m_pEntityList.getcurrent();
        if(pCurrent->DeleteFlagSet())
          RemoveEntity(pCurrent);   
    }  	
}
//-----------------------------------------------------------------------------
// Name: AddEntity()
// Desc:
//
//-----------------------------------------------------------------------------
CEntity* CEntityList::AddEntity( bool bLibrary, long x, long y,
                                 char* szIdent, bool fDynamic, bool bParam, char* String )
{
    char szHashString[24];

    // Found a new entity, so create its object and fill it in
    CEntity* pNewEntity = new CEntity();
    
    // If this entity is a library then set its active dist to -1
    // So it is never run
    if ( bLibrary )
        pNewEntity->SetActiveDist( -1 );
    
    // Give this Entity a number
    pNewEntity->SetEntCount(m_lEntityCount);
    m_lEntityCount++;
    
    // Set its parameter
    if ( bParam )
    {
        pNewEntity->SetParam( String[ strlen( String ) - 1] );
        String[ strlen( String ) - 1] = '#';
    }
    
    // Load this Entitie's script and set up its Abstract Machine
    pNewEntity->LoadScript(String);
    
    // Fill in some supplied details
    pNewEntity->SetXPos((float)x);
    pNewEntity->SetYPos((float)y);
    pNewEntity->SetDynFlag(fDynamic);
    
    strcpy(pNewEntity->GetCode(), String);
    strcpy(pNewEntity->GetIdent(), String);
    
    // If this is a dynamically Created entity then initialise it here now.
    if (fDynamic)
    {
        CEntity* temp = GetThisEntity();
        
        // Set the entities parent
        if (temp)
            pNewEntity->SetParent(temp);
        
        SetThisEntity(pNewEntity);
        pNewEntity->Initialise();
        SetThisEntity(temp);
    }
    
    // Add this to the Main list
    m_pEntityList.additem(pNewEntity, szIdent);

    // Add this entity to the Numeric Hash Table
    sprintf(szHashString, "%d", m_lEntityCount - 1);
    m_pNumericHash->AddString(szHashString, pNewEntity);

    return pNewEntity;
}

//-----------------------------------------------------------------------------
// Name: StartEntity()
// Desc: Moves to the start of the entity list
//
//-----------------------------------------------------------------------------
void CEntityList::StartEntity( long lDist, long x, long y )
{
    CEntity* pTemp;

    // If the point supplied was -1 -1 then assume we want the player entity
    if ( x == -999 || y == -999)
    {
        pTemp = m_pEntityList.GetObjectFromID("player1");
        
        // Get Suitable coordinates for the Player Entity
        if (pTemp)
        {
            x = (long)pTemp->GetXPos();
            y = (long)pTemp->GetYPos();
        }
    }

    m_lSearchDist        = lDist;
    m_lSearchX            = x;
    m_lSearchY            = y;
    m_pBoundaryToSearch = NULL;
    m_pCurrentEntity    = NULL;

    // If the search dist is -1 then search all entities
    if ( lDist == -1 )
    {
        StartEntityWholeList( lDist, x, y);
        return;
    }

    // Now Get the Group this search point is in
    CBoundary* pBound = g_pGame->GetMap()->GetGroupSet()->ReturnBoundary( m_lSearchX, m_lSearchY );
    m_pBoundaryToSearch = pBound;
    
    // Point the current entity to the start of the groups list
    if (pBound)
    {
        // If the list contains no entities then end
        if (pBound->GetEntList()->isempty())
            return;

        pBound->GetEntList()->tostart();
        m_pCurrentEntity = pBound->GetEntList()->getstart();
    }
}


//-----------------------------------------------------------------------------
// Name: StartEntityWholeList()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntityList::StartEntityWholeList( long lDist, long x, long y )
{
    m_lEntityIndex = 0;
    m_pCurrentEntity = m_pEntityList.GetObjectFromNumericID(0);
}


//-----------------------------------------------------------------------------
// Name: NextEntity()
// Desc: Moves to the next entity in the list which meets the requirements
//
//-----------------------------------------------------------------------------
bool CEntityList::NextEntity()
{
    static bool endNextTime = false;
    bool bGetAnother = false;
    CEntity* pTemp;
    LList<CEntity>* pList;

    // If we are going to search the whole list then call a different function
    if ( m_lSearchDist == -1 )
        return NextEntityWholeList();

    // If we couldnt find a list or it was empty then just leave
    if (!m_pCurrentEntity)
        return false;

    // Select the correct list to use
    pList= m_pBoundaryToSearch->GetEntList();

another:

    // Get the Next Entity
    pTemp = pList->getcurrent();

    // If this entity isnt valid then this is the end of the list
    if (!pTemp)
        return false;

    // The next entity is valid, check if its close enough
    if (isNearPoint(m_lSearchX, m_lSearchY,
                 (long)pTemp->GetXPos(), (long)pTemp->GetYPos(),
                 m_lSearchDist))
    {
        m_pCurrentEntity = pTemp;
    }
    else
    {
        // This wasnt close enough, we want another
        bGetAnother = true;
    }

    // Move the list on
    if (!pList->advance())
    {
        if (endNextTime)
        {
            endNextTime = false;
            return false;
        }
        endNextTime = true;
    }
    
    // If the last entity we got wasnt close enough then
    // We have to go and get another one.
    if (bGetAnother)
    {
        bGetAnother = false;
        goto another;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Name: NextEntityWholeList()
// Desc:
//
//-----------------------------------------------------------------------------
bool CEntityList::NextEntityWholeList()
{
    CEntity* pTemp;

    // Get the Next Entity
    m_lEntityIndex++;
    pTemp = m_pEntityList.GetObjectFromNumericID(m_lEntityIndex);

    if (pTemp)
    {
        // Return Every entity we come across in the list
        m_pCurrentEntity = pTemp;
        return true;
    }

    // We reached the end of the list
    return false;
}

//-----------------------------------------------------------------------------
// Name: GetCurrentEntity()
// Desc: Gets the Count Index of the current entity
//
//-----------------------------------------------------------------------------
long CEntityList::GetCurrentEntity()
{
    if (m_pCurrentEntity)
        return m_pCurrentEntity->GetCount();

    return -1;
}

//-----------------------------------------------------------------------------
// Name: GetEntityWithCount()
// Desc: Returns an entity with a matching list count
//
//-----------------------------------------------------------------------------
CEntity* CEntityList::GetEntityWithCount( char* szCount )
{
    // Find the Entity
    return (CEntity*)m_pNumericHash->LookupString( szCount );
}

//-----------------------------------------------------------------------------
// Name: RemoveEntity()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntityList::RemoveEntity(CEntity* pTemp)
{
    char szHashString[24];
    
    //if (pTemp->GetDynamicFlag())
    //{
        if (pTemp->GetBoundary())
        {
            // Remove the Entity from the group list
            CBoundary* pBound = pTemp->GetBoundary();
            pBound->RemoveEntity(pTemp);
        }
        sprintf(szHashString, "*_ent%d", pTemp->GetCount() );
        m_pNumericHash->RemoveString( szHashString );
        m_pEntityList.removeitem(pTemp);
    //}
}

//-----------------------------------------------------------------------------
// Name: AddEntityRects()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntityList::AddEntityRects()
{
    CBoundary* pBound = g_pGame->GetMap()->GetGroupSet()->GetCurrentBoundary();
    if (!pBound)return;
    
    if (pBound->GetEntList()->isempty())
        return;

    // Move to the Start of the list
    pBound->GetEntList()->tostart();
    do
    {
        pBound->GetEntList()->getcurrent()->AddCollisionRects();
    }while(pBound->GetEntList()->advance());
}


//-----------------------------------------------------------------------------
// Name: GetPlayerEntity()
// Desc:
//
//-----------------------------------------------------------------------------
CEntity* CEntityList::GetPlayerEntity()
{
    // Get the Player entity from the hash table
    return m_pEntityList.GetObjectFromID("player1");
}


//-----------------------------------------------------------------------------
// Name: Save()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntityList::Save(FILE* sFile)
{
    if (m_pEntityList.isempty())
        return;

    WriteSlotHeader(sFile, "[ENTITIES]");

    // Put the number of Entities
    Writelong(sFile, "nument", m_pEntityList.GetCount());

    // Move to the Start of the list
    m_pEntityList.tostart();
    do
    {
        m_pEntityList.getcurrent()->Save(sFile, m_pEntityList.GetCurrentIdent());
    }while(m_pEntityList.advance());
}

//-----------------------------------------------------------------------------
// Name: Load()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntityList::Load(FILE* sFile)
{
    long n;
    long m;
    char szTemp[48];

    if (FindSlotHeader(sFile, "[ENTITIES]"))
    {
        // Delete the numeric Hash table
        delete m_pNumericHash;

        // Delete all entities
        m_pEntityList.free();
        
        // Get the number of Entities
        Readlong(sFile, "nument", n);

        // Recreate Hash tables
        m_pEntityList.CreateHash( n );
        m_pNumericHash = new CHash( n );

        // Create every Entity
        for ( m = 0; m < n; m++)
        {
            CEntity* pTemp = new CEntity();
            pTemp->Load(sFile, szTemp);
            
            // Load this Entitie's script and set up its Abstract Machine
            pTemp->LoadScript(pTemp->GetCode());
            strcpy(pTemp->GetIdent(), szTemp );
            pTemp->Load2(sFile);
            m_pEntityList.additem(pTemp, szTemp);

            // Add this entity to the Numeric Hash Table
            sprintf(szTemp, "%d", pTemp->GetCount());
            m_pNumericHash->AddString(szTemp, pTemp);
        }
    }
}
