/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CEntity.cpp
//
// Desc:
//    
//
//-----------------------------------------------------------------------------
#define STRICT

#include <math.h>
#include "CEntity.h"
#include "CGame.h"
#include "Script.h"
#include "AMX/amxcore.h"
#include "AMX/float.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame*        g_pGame;
extern float        g_rDelta;
#define MAX_PARAMS 64

CEntity* GetEntity(AMX *amx, cell* params);
CEntity* GetEntityFromString( char* szString );

extern float fConvertCellToFloat(cell cellValue);
extern cell ConvertFloatToCell(float fValue);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CEntity::CEntity()
{
    int n;
        
    // Set Default Values
    m_x                = 0;
    m_y                = 0;
    m_rMoveAngle    = 0;
    m_wDirection    = SOUTH;
    m_rSpeed        = 20;
    m_rSpeedMod        = 0;
    m_fDynamic        = false;
    m_fFirstRun        = true;
    m_pAmx            = NULL;
    m_fisOpen        = false;
    m_fisTaken        = false;
    m_fisDead        = false;
    m_fisVisible    = true;
    m_fisActive        = true;
    m_fisOwned        = false;
    m_wHealth        = 100;
    m_wMaxHealth    = 100;
    m_wType            = OTHER_TYPE;
    m_fDelete        = false;
    m_lActiveDist    = 190;
    m_wState        = 0;
    m_lInitialX        = 0;
    m_lInitialY     = 0;
    m_fisCuttable = false;
    m_wLiftLevel    = -1;
    m_wBounce        = -1;
    m_fisLarge        = false;
    m_wWeight        = 100;
    m_fisPickable = true;
    m_rRespawnCount = 0;
    m_lRespawnTarget= 0;
    m_fisPushed        = false;
    m_wStringLength    = 0;
    m_fisInteracting = false;
    m_wDamage        = 100;
    m_nParam        = -1;
    m_pParent        = NULL;
    m_pBoundary        = NULL;
    m_bActiveinAllGroups = false;

    for (n = 0; n < MAX_COLRECT; n++)
        ClearColRect(n);

    for (n = 0; n < MAX_VALUES; n++)
        m_wValue[n] = 0;

    for (n = 0; n < MAX_FLOATS; n++)
        m_fValue[n] = 0;

    strcpy(m_szItemString, " ");
    memset(m_szImageString, 0 , sizeof(m_szImageString));
}


//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CEntity::~CEntity()
{
    if (!strcmp("_itemlib", m_szCode ))
        int g = m_pStrings.GetCount();

    free(m_pProgram);
    SAFE_DELETE( m_pAmx );
    m_pStrings.free();
}


//-----------------------------------------------------------------------------
// Name: Initialise()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::Initialise()
{
    m_lInitialX = m_x;
    m_lInitialY = m_y;

    // Get the group pointer for this entity
    m_pBoundary = g_pGame->GetMap()->GetGroupSet()->ReturnBoundary( m_lInitialX, m_lInitialY );

    // Not so sure this is a good idea, this could run a script in the
    // middle of another script which causes problems, however, it is
    // essential for things like chests which need scripts to run.
    if (m_pAmx)
    {
        RunScript();
    }

    // Dont allow position change in the first script run
    // Player script returns wrong X coordinate if you do...?!?
    m_x = m_lInitialX;
    m_y = m_lInitialY;
}

//-----------------------------------------------------------------------------
// Name: ReCheckGroup()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::ReCheckGroup( bool bForceCheck )
{
    CBoundary* pTemp;

    // return if this cant be active in another group, the group cant change
    if (!bForceCheck)
    {
        if ( !m_bActiveinAllGroups )
            return;
    }

    // We could optimise this part a bit, becuase its quite rare that an entity
    // would cross a group anyway.

    // Get the group this entity is in, if its different from the current then change
    pTemp = g_pGame->GetMap()->GetGroupSet()->ReturnBoundary( m_x, m_y );

    if ( pTemp != NULL )
    {
        if ( pTemp != m_pBoundary || bForceCheck )
        {
            // Boundary Change, update the group lists.
            // Remove the entity from the last group list
            if (m_pBoundary)
                m_pBoundary->RemoveEntity( this );

            // Add the entity to the new group's list
            pTemp->AddEntity( this );

            // Swap the groups over
            m_pBoundary = pTemp;
        }
    }
}

//-----------------------------------------------------------------------------
// Name: CheckForGroup()
// Desc:
//
//-----------------------------------------------------------------------------
bool CEntity::CheckForGroup()
{
    // If this entity can be active in any group then return true
    if ( m_bActiveinAllGroups )
        return true;

    // If this entity has no group then return true
    if ( !m_pBoundary )
        return true;

    // Check if the current group matches the entities group
    if ( g_pGame->GetMap()->GetGroupSet()->GetCurrentBoundary() == m_pBoundary )
        return true;

    // If we are wiping to a new screen then check the last boundary as well, we
    // want to be able to see the enemies on the previus screen as we wipe away from it
    if ( g_pGame->GetWipe()->isWiping() )
    {
        if ( g_pGame->GetMap()->GetGroupSet()->GetLastBoundary() == m_pBoundary )
            return true;
    }

    return false;
}


//-----------------------------------------------------------------------------
// Name: BeginRespawn()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::BeginRespawn(long target)
{
    m_rRespawnCount = 0;
    m_lRespawnTarget = target;
}


//-----------------------------------------------------------------------------
// Name: Respawn()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::Respawn()
{
    if (m_lRespawnTarget == 0)
        return;

    m_rRespawnCount += g_rDelta;

    if (m_rRespawnCount > m_lRespawnTarget)
    {
        // Make sure this entity is not on screen when it reappears
        if (m_x < g_pGame->GetWorldX() - 50 || m_x > g_pGame->GetWorldX() + VSCREEN_WIDTH + 50)
            DoRespawn();

        if (m_y < g_pGame->GetWorldY() - 50 || m_y > g_pGame->GetWorldY() + VSCREEN_HEIGHT + 50)
            DoRespawn();
    }
}


//-----------------------------------------------------------------------------
// Name: DoRespawn()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::DoRespawn()
{
    // Make the entity live once more
    m_x                    = m_lInitialX;
    m_y                    = m_lInitialY;
    m_fisDead            = false;
    m_fisActive            = true;
    m_fisTaken            = false;
    m_wHealth            = m_wMaxHealth;
    m_lRespawnTarget    = 0;
}


//-----------------------------------------------------------------------------
// Name: LoadScript()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::LoadScript(char* szFileName)
{
    char szString[32];

    // Build the filename of the AMX file
	strcpy(szString, szFileName);
    strcat(szString, ".amx");
    
    m_pAmx = new AMX();

    m_pProgram = loadprogram(m_pAmx, szString);

    if (m_pProgram)
    {
	    //RegisterNatives(*m_pAmx);
	}
    else
    {
        // Loading program didnt work for some reason, will probably
        // crash now - make a note in the log.
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s %s\n","*** Loading AMX failed ***",szFileName );

    }
}


//-----------------------------------------------------------------------------
// Name: RunScript()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::RunScript()
{

    // Perform any respawning needed
    Respawn();

    cell ret;

    if (m_pAmx)
    {
        cell HeapBefore = m_pAmx->hea;
        amx_Exec(m_pAmx, &ret, AMX_EXEC_MAIN, 0);
        m_pAmx->hea = HeapBefore;
        m_pAmx->firstrun = 0;
    }
}


//-----------------------------------------------------------------------------
// Name: Copy()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::Copy(const CEntity &Copy)
{
    // Copy the Code
    strcpy(m_szCode, Copy.m_szCode);
}


//-----------------------------------------------------------------------------
// Name: CreateStrings()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::CreateStrings(int wNumString, int wLength)
{
    int n;
    CEntString* pTemp;
    char s[20];

    // Record the Length of the Entity Strings
    m_wStringLength = wLength;
    
    // Create the String Objects
    for (n = 0; n < wNumString; n++)
    {
        // Create the String Object and set it up
        pTemp = new CEntString(wLength);

        // Convert the number to a string
        sprintf(s, "%d", n);

        // Add it to the Linked List
        m_pStrings.additem(pTemp, s);
    }
}


//-----------------------------------------------------------------------------
// Name: GetEntString()
// Desc:
//
//-----------------------------------------------------------------------------
char* CEntity::GetEntString(int wIndex)
{
    if (g_pGame->IsLoadingEntities())
        return NULL;

    CEntString* pTemp;

    // Get the Obeject we are interested in
    pTemp = m_pStrings.GetObjectFromNumericID(wIndex);

    if (!pTemp)
        return NULL;

    if (!pTemp->GetMessage())
        return NULL;

    // return the correct string
    return pTemp->GetMessage();
}


//-----------------------------------------------------------------------------
// Name: SetEntString()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::SetEntString(int wIndex, char* szString)
{
    CEntString* pTemp;

    // Get the Obejct we are interested in
    pTemp = m_pStrings.GetObjectFromNumericID(wIndex);

    if (!pTemp)
        return;

    // Check the String Lengths
    //if (strlen(szString) < sizeof(pTemp->GetMessage()))
    {
        // Set the String
        strcpy(pTemp->GetMessage(), szString);
    }
}


//-----------------------------------------------------------------------------
// Name: MessageMap(index, string, gotonext, next[5])
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::MessageMap(int wIndex, char* szMessage, bool fGotoNext, int* params, int wNumParams )
{
    int n;
    cell *v;

    // Get the Entity String we are interested in
    CEntString* pTemp = m_pStrings.GetObjectFromNumericID(wIndex);

    if (!pTemp)
        return;

    // Set the Message
    strcpy(pTemp->GetMessage(), szMessage);

    // Set the GotoNext Param
    pTemp->SetGotoNext(fGotoNext);
    pTemp->SetNumNext(wNumParams);

    // Set the Next Messages
    for (n = 0; n < wNumParams; n++)
     pTemp->SetNextMessage(n, params[n]);

}


//-----------------------------------------------------------------------------
// Name: GetNextMessage(index, answer)
// Desc:
//
//-----------------------------------------------------------------------------
int CEntity::GetNextMessage(int wIndex, int wAnswer )
{
    // Get the Entity String we are interested in
    CEntString* pTemp = m_pStrings.GetObjectFromNumericID(wIndex);

    if (!pTemp)
        return 0;

    return pTemp->GetNextMessage(wAnswer);
}


//-----------------------------------------------------------------------------
// Name: GetGotoNextMessage(index)
// Desc:
//
//-----------------------------------------------------------------------------
bool CEntity::GetGotoNextMessage(int wIndex)
{
    // Get the Entity String we are interested in
    CEntString* pTemp = m_pStrings.GetObjectFromNumericID(wIndex);

    if (!pTemp)
        return 0;

    return pTemp->GetGotoNext();
}

//-----------------------------------------------------------------------------
// Name: GetNumNextMessages(index)
// Desc:
//
//-----------------------------------------------------------------------------
int CEntity::GetNumNextMessages(int wIndex)
{
    // Get the Entity String we are interested in
    CEntString* pTemp = m_pStrings.GetObjectFromNumericID(wIndex);

    if (!pTemp)
        return 0;

    return pTemp->GetNumNext();
}


//-----------------------------------------------------------------------------
// Name: SetValue()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::SetValue(int wIndex, long Param)
{
    if (wIndex < MAX_VALUES)
        m_wValue[wIndex] = Param;
}

//-----------------------------------------------------------------------------
// Name: GetValue()
// Desc:
//
//-----------------------------------------------------------------------------
long CEntity::GetValue(int wIndex)
{
    if (wIndex < MAX_VALUES)
        return m_wValue[wIndex];

    return 0;
}


//-----------------------------------------------------------------------------
// Name: SetFloat()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::SetFloat(int wIndex, float Param)
{
    if (wIndex < MAX_FLOATS)
        m_fValue[wIndex] = Param;
}

//-----------------------------------------------------------------------------
// Name: GetFloat()
// Desc:
//
//-----------------------------------------------------------------------------
float CEntity::GetFloat(int wIndex)
{
    if (wIndex < MAX_FLOATS)
        return m_fValue[wIndex];

    return 0;
}

//-----------------------------------------------------------------------------
// Name: ClearColRect()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::ClearColRect(int wIndex)
{
    if (wIndex >= MAX_COLRECT)
        wIndex = MAX_COLRECT;

    if (wIndex < 0)
        return;

    m_sRect[wIndex].fSolid = false;
    SetRect(&m_sRect[wIndex].sRect, -999,-999,-999,-999);
}


//-----------------------------------------------------------------------------
// Name: SetColRect()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::SetColRect(int wIndex, RECT2D sRect, bool fSolid)
{
    if (wIndex >= MAX_COLRECT)
        wIndex = MAX_COLRECT;

    m_sRect[wIndex].fSolid = fSolid;
    m_sRect[wIndex].sRect = sRect;
}


//-----------------------------------------------------------------------------
// Name: AddCollisionRects()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::AddCollisionRects()
{
    int n;
    for (n = 0; n < MAX_COLRECT; n++)
    {
        if (m_sRect[n].fSolid && m_sRect[n].sRect.top != -999)
        {
            g_pGame->GetMap()->AddColisionRect(m_sRect[n].sRect);
        //    ClearColRect(n);
        }
    }
}


//-----------------------------------------------------------------------------
// Name: ChangeDirection()
// Desc: Change to Direction to somthing else randomly
//
//-----------------------------------------------------------------------------
void CEntity::ChangeDirection()
{
    // Record the old direction
    int wOld = m_wDirection;

    // Pick another Random Direction
    m_wDirection = (rand() % 4);

    // Make sure we dont pick the same direction
    if (wOld == m_wDirection)
        ChangeDirection();
}


//-----------------------------------------------------------------------------
// Name: GetAngleFromDir()
// Desc: Changes the MoveAngle according to the Direction
//
//-----------------------------------------------------------------------------
void CEntity::GetAngleFromDir()
{
    // Retrieve the Move angle from the direction
    switch( m_wDirection )
    {
    case NORTH:
        m_rMoveAngle = PI / 2;
        break;
    case EAST:
        m_rMoveAngle = PI;
        break;
    case SOUTH:
        m_rMoveAngle = PI + PI / 2;
        break;
    case WEST:
        m_rMoveAngle = 0;
        break;
    case NORTHEAST:
        m_rMoveAngle = (PI / 2) + (PI / 4);
        break;
    case SOUTHEAST:
        m_rMoveAngle = PI + (PI / 4);
        break;
    case SOUTHWEST:
        m_rMoveAngle = (PI / 4) + (PI / 2) + PI;
        break;
    case NORTHWEST:
        m_rMoveAngle = PI / 4;
        break;
    }
}


//-----------------------------------------------------------------------------
// Name: SetDirFromAngle()
// Desc: Changes the Direction according to the moveangle
//
//-----------------------------------------------------------------------------
void CEntity::SetDirFromAngle()
{
    if (m_rMoveAngle > (PI / 4) && m_rMoveAngle < (PI / 2) + (PI / 4))
        m_wDirection = NORTH;
    else if (m_rMoveAngle >= (PI / 2) + (PI / 4) && m_rMoveAngle < PI + (PI / 4))
        m_wDirection = EAST;
    else if (m_rMoveAngle >= PI + (PI / 4) && m_rMoveAngle < (PI / 4) + (PI / 2) + PI)
        m_wDirection = SOUTH;
    else
        m_wDirection = WEST;
}

//-----------------------------------------------------------------------------
// Name: AngleMove()
// Desc: Move the Entity according to Its MoveAngle and Speed
//
//-----------------------------------------------------------------------------
void CEntity::AngleMove()
{    
    float fMoveAmt = (m_rSpeed + m_rSpeedMod) * g_rDelta;

    // Move the Entity
    m_x -= fMoveAmt * cos(m_rMoveAngle );
    m_y -= fMoveAmt * sin(m_rMoveAngle );

    ReCheckGroup();
}


//-----------------------------------------------------------------------------
// Name: AngleCollide()
// Desc: Checks for a collision between the Entity and the Mask Layer
//
//-----------------------------------------------------------------------------
bool CEntity::AngleCollide(int wDist, int wNumPoints, int wMaxDetect, bool fHoles, int wXOffSet, int wYOffSet)
{
    int n = 0;
    long lResult;
    float XSensor;
    float YSensor;
    bool hit = false;
    float TempAngle[5];
    RECT2D sCurrentBoundary;
    
    // Check the number of Points entered
    if (wNumPoints > 5)
        wNumPoints = 5;

    // Calculate the Angles of Each collision point based on
    // The angle of Movement
    TempAngle[3] = (float)(m_rMoveAngle + (PI / 2));
    TempAngle[1] = (float)(m_rMoveAngle + (PI / 4));
    TempAngle[0] = m_rMoveAngle;
    TempAngle[2] = (float)(m_rMoveAngle - (PI / 4));
    TempAngle[4] = (float)(m_rMoveAngle - (PI / 2));

    // Loop through the angles
    for (n = 0; n < wNumPoints; n++)
    {
        // Work out the points for collision detection, these
        // Will be in world Coordinates
        XSensor = (float)(m_x - (wDist * cos(TempAngle[n])) + wXOffSet);
        YSensor = (float)(m_y - (wDist * sin(TempAngle[n])) + wYOffSet);
        
        
        // If this is an enemy type dont let it go out of its boundary
        if ( !m_bActiveinAllGroups )
        {
            if ( m_pBoundary )
            {
                // Get this entities rectangle its allowed in
                sCurrentBoundary = m_pBoundary->bounds;

                // Check if the Entity is trying to go out of the group boundary
                // Check this collision point
                if (XSensor <= sCurrentBoundary.left || XSensor >= sCurrentBoundary.right ||
                    YSensor <= sCurrentBoundary.top || YSensor >= sCurrentBoundary.bottom )
                {
                    // Entity was trying to move out of its boundary
                    hit = true;
                    MoveBack(TempAngle[n], 40);
                }
            }
        }

        if (fHoles)
        {
            // Test For Holes
            if (TestHoleList(XSensor, YSensor, TempAngle[n]))
                hit = true;
        }

        
        // Test for Collision Rectangles
        if (TestCollisionList(XSensor, YSensor, TempAngle[n]))
            hit = true;
    
        // Test each of these collide points
        // To see if they have collided with somthing on the Mask Layer
        lResult = TestCollidePoint( (long)XSensor, (long)YSensor, false);
        
        if ( lResult == -1 )
        {
            if ( m_wType == PLAYER_TYPE )
            {
                // Probably the player going offscreen
            }
            else
            {
                // An enemy off the screen, if we moveback here
                // the we could move all over the place
                hit = true;
            }
        }
        else if (lResult < wMaxDetect)
        {
            MoveBack(TempAngle[n]);
            hit = true;
        }
    }
    
    return hit;
}

//-----------------------------------------------------------------------------
// Name: TestHoleList()
// Desc:
//
//-----------------------------------------------------------------------------
bool CEntity::TestHoleList(float x, float y, float rAngle)
{
    // Get the screen object this point is on
    CScreen* pScreen = g_pGame->GetMap()->GetScreenOn(x, y);

    if (!pScreen)
        return false;

    // adjust the x and y coordinates
    x -= pScreen->AccessXPos();
    y -= pScreen->AccessYPos();

    // Check for a hole
    if (pScreen->CheckForHoleXY(x, y) != -1)
    {
        MoveBack(rAngle);
        return true;
    }

    return false;
}


//-----------------------------------------------------------------------------
// Name: TestCollisionList()
// Desc:
//
//-----------------------------------------------------------------------------
bool CEntity::TestCollisionList(float x, float y, float rAngle)
{
    int n;
    RECT2D sRect;

    // Loop Through every Collision Rect
    for (n = 0; n < g_pGame->GetMap()->GetCollisionNum(); n++)
    {
        sRect = g_pGame->GetMap()->GetCollideRect(n);
        if (PointInRect(x, y, sRect))
        {
            MoveBack(rAngle);
            return true;
        }
    }

    return false;
}


//-----------------------------------------------------------------------------
// Name: MoveBack()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::MoveBack(float rAngle, float fMore)
{
    m_x += (((m_rSpeed + m_rSpeedMod + fMore) ) * g_rDelta) * cos(rAngle);
    m_y += (((m_rSpeed + m_rSpeedMod + fMore) ) * g_rDelta) * sin(rAngle);

    ReCheckGroup();
}


//-----------------------------------------------------------------------------
// Name: CheckForHole()
// Desc:
//
//-----------------------------------------------------------------------------
int CEntity::CheckForHole()
{
    RECT2D sRect = m_sRect[0].sRect;

    // Return if the entity has no collision rectangles
    if (sRect.top == -999)
        return -1;

    // Convert the Given world Coordinates to screenOffset Coordinates
    long wTempX = g_pGame->GetMap()->WorldToScreenX(sRect.left, sRect.top);
    long wTempY = g_pGame->GetMap()->WorldToScreenY(sRect.left, sRect.top);

    if (wTempX < 0 || wTempY < 0)
        return -1;

    // Get the Actual Screen we are on
    CScreen* pScreenOn = g_pGame->GetMap()->GetScreenOn(sRect.left, sRect.top);

    if (!pScreenOn)
        return -1;

    sRect.left -= pScreenOn->AccessXPos();
    sRect.right -= pScreenOn->AccessXPos();
    sRect.top    -= pScreenOn->AccessYPos();
    sRect.bottom -= pScreenOn->AccessYPos();
    
    return pScreenOn->CheckForHoleRECT(sRect);
}


//-----------------------------------------------------------------------------
// Name: SetItem()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::SetItem(char* szString)
{
    if (strlen(szString) < sizeof(m_szItemString))
        strcpy(m_szItemString, szString);
}


//-----------------------------------------------------------------------------
// Name: SetImage()
// Desc:
//
//-----------------------------------------------------------------------------
void CEntity::SetImage(char* szString)
{
    if (strlen(szString) < sizeof(m_szImageString))
        strcpy(m_szImageString, szString);
}


//-----------------------------------------------------------------------------
// Name: Collide()
// Desc:
//
//-----------------------------------------------------------------------------
bool CEntity::Collide(char* szEntity)
{
    CEntity* pTemp;
    int n, m;
    
    // Covert the String supplied to an Entity Pointer
    pTemp = GetEntityFromString(szEntity);
        
    if (!pTemp || !pTemp->GetisActiveFlag())
        return false;

    // Loop through all the collision rectangles of each entity
    // and see if any interact with each other. Only test a rectangle
    // if it is in use.
    for (n = 0; n < MAX_COLRECT; n++)
    {
        if (m_sRect[n].sRect.top != -999)
        {
            for (m = 0; m < MAX_COLRECT; m++)
            {
                if (pTemp->GetColRect(m).sRect.top != -999)
                {
                    RECT2D sr = pTemp->GetColRect(m).sRect;

                    if (Intersect(m_sRect[n].sRect, pTemp->GetColRect(m).sRect))
                        return true;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name: CollideAll()
// Desc:
//
//-----------------------------------------------------------------------------
bool CEntity::CollideAll(char* szEntity)
{
    CEntity* pTemp;
    int n, m;
    
    // Covert the String supplied to an Entity Pointer
    pTemp = GetEntityFromString(szEntity);

    if (!pTemp)
        return false;

    // Loop through all the collision rectangles of each entity
    // and see if all interact with each other. Only test a rectangle
    // if it is in use.
    for (n = 0; n < MAX_COLRECT; n++)
    {
        if (m_sRect[n].sRect.top != -999)
        {
            for (m = 0; m < MAX_COLRECT; m++)
            {
                if (pTemp->GetColRect(m).sRect.top != -999)
                {
                    RECT2D sr = pTemp->GetColRect(m).sRect;

                    if (!Intersect(m_sRect[n].sRect, pTemp->GetColRect(m).sRect))
                        return false;
                }
            }
        }
    }

    // Check if both entities are actually using at least 1 collision rectangle
    m = 0;
    for(n = 0; n < MAX_COLRECT; n++)
    {
        if (m_sRect[n].sRect.top != -999)
            break;
        m++;
    }

    if (m == MAX_COLRECT)
		return false;


    // Check the other entity
    m = 0;
    for(n = 0; n < MAX_COLRECT; n++)
    {
        if (pTemp->GetColRect(m).sRect.top != -999)
            break;
        m++;
    }

    if (m == MAX_COLRECT)
		return false;

    return true;
}

//-----------------------------------------------------------------------------
// Name: CollidePoint()
// Desc:
//
//-----------------------------------------------------------------------------
bool CEntity::CollidePoint(long x, long y)
{
    int n;

    // Loop through all the collision rectangles of this entity to see
    // if any interact with the point (x,y)
    for (n = 0; n < MAX_COLRECT; n++)
    {
        if (m_sRect[n].sRect.top != -999)
        {    
            if (PointInRect(x, y, m_sRect[n].sRect))
                return true;
        }
    }

    return false;
}


//-----------------------------------------------------------------------------
// Name: GetEntity()
// Desc:
//
//-----------------------------------------------------------------------------
CEntity* GetEntity(AMX *amx, cell* params)
{
    char szString[ MAX_CHARS ];
    
    // Check we have some parameters
    if (params[0] == 0)
        return NULL;

    // Get the String from the first Param
    GetStringParam(amx, params[1], szString);

    return GetEntityFromString(szString);
}

//-----------------------------------------------------------------------------
// Name: GetEntityFromString()
// Desc:
//
//-----------------------------------------------------------------------------
CEntity* GetEntityFromString( char* szString )
{
    // If the String returned is "this" then
    // Handle it in a different way
    if (!strcmp(szString, "this"))
        return g_pGame->GetEList()->GetThisEntity();
    else
    {
        // Check if its a numeric Ident
        if (szString[0] > 47 && szString[0] < 58)
        {
            // This is a number, so get an entity with a matching list count
            return g_pGame->GetEList()->GetEntityWithCount( szString );
        }
        else
            return g_pGame->GetEntityList()->GetObjectFromID(szString);
    }
    return NULL;
}


//-----------------------------------------------------------------------------
// Name: GetX()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetX(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        return pTemp->GetXPos();
    return -999;
}


//-----------------------------------------------------------------------------
// Name: GetY()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetY(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        return pTemp->GetYPos();
    return -999;
}


//-----------------------------------------------------------------------------
// Name: SetX()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetX(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->SetXPos(params[2]);
    return 0;
}


//-----------------------------------------------------------------------------
// Name: SetY()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetY(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->SetYPos(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetPosition("code", x,y)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetPosition(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
    {
        pTemp->SetPosition(params[2], params[3]);
        pTemp->ReCheckGroup();
    }
    return 0;
}



//-----------------------------------------------------------------------------
// Name: GetMoveAngle()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetMoveAngle(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        return (RTOD * pTemp->GetMoveAngle());
    return 0;
}


//-----------------------------------------------------------------------------
// Name: SetMoveAngle()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetMoveAngle(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->SetMoveAngle(DTOR * (float)params[2]);
    return 0;
}


//-----------------------------------------------------------------------------
// Name: GetDirection()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetDirection(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        return pTemp->GetDirection();
    return 0;
}


//-----------------------------------------------------------------------------
// Name: SetDirection()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetDirection(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->SetDirection(params[2]);
    return 0;
}


//-----------------------------------------------------------------------------
// Name: GetSpeed()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetSpeed(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        return pTemp->GetSpeed();
    return 0;
}


//-----------------------------------------------------------------------------
// Name: SetSpeed()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetSpeed(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->SetSpeed((float)params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetSpeedMod()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetSpeedMod(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        return pTemp->GetSpeedMod();
    return 0;
}


//-----------------------------------------------------------------------------
// Name: SetSpeed()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetSpeedMod(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->SetSpeedMod(params[2]);
    return 0;
}


//-----------------------------------------------------------------------------
// Name: AngleMove()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL AngleMove(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->AngleMove();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: AngleCollide()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL AngleCollide(AMX *amx, cell *params)
{
    cell ret;
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        ret = pTemp->AngleCollide(params[2], params[3], params[4],
                                 params[5], params[6], params[7]);
    return ret;
}

//-----------------------------------------------------------------------------
// Name: SetAngleFromDir()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetAngleFromDir(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->GetAngleFromDir();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: ChangeDirection()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL ChangeDirection(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->ChangeDirection();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: Delete()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DeleteEntity(AMX *amx, cell *params)
{
    char szString[ MAX_CHARS ];

    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    
    if (pTemp)
    {
        // Get the String from the first Param
        GetStringParam(amx, params[1], szString);
        // Mark the entity for deletion
        pTemp->SetDeletionFlag(true);
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Name: CreateEntity()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CreateEntity(AMX *amx, cell *params)
{
    char szString[ MAX_CHARS ];
    char szString2[16];
    cell *cptr;

    if (g_pGame->IsLoadingEntities())
        return 0;

    // Check we have some parameters
    if (params[0] == 0)
        return 0;

    // Increment the Create Entity Call Count
    g_pGame->GetEList()->IncrementDynEnt();

    // Get the String from the first Param
    GetStringParam(amx, params[1], szString);

    // Create an Ident Based on the Dynamic Entity Count
    sprintf(szString2, "__d%d", g_pGame->GetEList()->GetDynEntCount());

    CEntity* pNew = g_pGame->FindEntity(szString, params[2], params[3],
                                        szString2, true);

    // Check it was created ok
    if (!pNew)
        return 0;

    // Get the group pointer for this entity
    pNew->ReCheckGroup( true );
    
    // Now we want to place the ident of this Entity in the supplied
    // String Buffer
    amx_GetAddr(amx,params[4],&cptr);
    amx_SetString(cptr, szString2, 0);
    return 0;    
}

//-----------------------------------------------------------------------------
// Name: CreateEntityWithID("chest01", x, y, "myChest1")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CreateEntityWithID(AMX *amx, cell *params)
{
    char szString[ MAX_CHARS ];
    char szString2[ MAX_CHARS ];
    cell *cptr;

    if (g_pGame->IsLoadingEntities())
        return 0;

    // Check we have some parameters
    if (params[0] == 0)
        return 0;

    // Get the String from the first Param
    GetStringParam(amx, params[1], szString);

    // Get the String from the fourth Param
    GetStringParam(amx, params[4], szString2);

    // Create the Entity with the supplied ID
    CEntity* pNew = g_pGame->FindEntity(szString, params[2], params[3],
                                        szString2, true);

    // Check it was created ok
    if (!pNew)
        return 0;

    // Get the group pointer for this entity
    pNew->ReCheckGroup( true );

    return 0;    
}

//-----------------------------------------------------------------------------
// Name: SetCollisionRect("this", 0, true, 23, 34, 54, 56)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetCollisionRect(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    // Create a temp rectangle
    RECT2D sRect = {params[4], params[5], params[6], params[7]};

    if (pTemp)
        pTemp->SetColRect(params[2], sRect, params[3]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: ClearCollisionRect("this", 0)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL ClearCollisionRect(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->ClearColRect(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: isOpen()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isOpen(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisOpenFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetOpenFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetOpenFlag(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisOpenFlag(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: isTaken()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isTaken(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisTakenFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetTakenFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetTakenFlag(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisTakenFlag(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: isDead()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isDead(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisDeadFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetDeadFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetDeadFlag(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisDeadFlag(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: isVisible()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isVisible(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisVisibleFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetVisibleFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetVisibleFlag(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisVisibleFlag(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: isActive()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isActive(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisActiveFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetActiveFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetActiveFlag(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisActiveFlag(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: isOwned()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isOwned(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisOwnedFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetOwnedFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetOwnedFlag(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisOwnedFlag(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetItem("this", "pot0001")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetItem(AMX *amx, cell *params)
{
    char szString[ MAX_CHARS ];

    // Get the String from the Second Param
    GetStringParam(amx, params[2], szString);

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetItem(szString);
    return 0;    
}

//-----------------------------------------------------------------------------
// Name: GetItem("this", myString)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetItem(AMX *amx, cell *params)
{
    cell *cptr;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
    {
        // Place the Entities itemstring in the supplied Buffer
        amx_GetAddr(amx, params[2], &cptr);
        amx_SetString(cptr, pTemp->GetItemString(), 0);
    }
    return 0;    
}

//-----------------------------------------------------------------------------
// Name: SetImage("this", "bush001")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetImage(AMX *amx, cell *params)
{
    char szString[ MAX_CHARS ];

    // Get the String from the Second Param
    GetStringParam(amx, params[2], szString);

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetImage(szString);
    return 0;    
}


//-----------------------------------------------------------------------------
// Name: GetImage("this", myString)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetImage(AMX *amx, cell *params)
{
    cell *cptr;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
    {
        // Place the Entities itemstring in the supplied Buffer
        amx_GetAddr(amx, params[2], &cptr);
        amx_SetString(cptr, pTemp->GetImageString(), 0);
    }
    return 0;    
}

//-----------------------------------------------------------------------------
// Name: SetType("this", enemy)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetType(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
    {
        pTemp->SetType(params[2]);

        // If the type was set to enemy then make this entity
        // inactive whenever they are not in the current group
        if ( params[2] == ENEMY_TYPE )
            pTemp->SetActiveInAllGroups( false );
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetType()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetType(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetType();
    return OTHER_TYPE;
}

//-----------------------------------------------------------------------------
// Name: Collide("this", "player1")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL Collide(AMX *amx, cell *params)
{
    char szString[ MAX_CHARS ];

    // Get the String from the Second Param
    GetStringParam(amx, params[2], szString);

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->Collide(szString);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: CollideAll("this", "player1")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CollideAll(AMX *amx, cell *params)
{
    char szString[ MAX_CHARS ];

    // Get the String from the Second Param
    GetStringParam(amx, params[2], szString);

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->CollideAll(szString);
    return 0;
}


//-----------------------------------------------------------------------------
// Name: CollidePoint("this", x, y)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CollidePoint(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->CollidePoint(params[2], params[3]);
    return 0;
}


//-----------------------------------------------------------------------------
// Name: AllocateStrings("this", numstr, length)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL AllocateStrings(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->CreateStrings(params[2], params[3]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetString("this", 0, MyString)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetString(AMX *amx, cell *params)
{
    cell *cptr;

    if (g_pGame->IsLoadingEntities())
        return -1;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
    {
        // Check we have that string
        if (pTemp->GetEntString(params[2]) != NULL)
        {
            // Place the Entities String in the supplied Buffer
            amx_GetAddr(amx, params[3], &cptr);
            amx_SetString(cptr, pTemp->GetEntString(params[2]), 0);
            return 0;
        }
    }
    return -1;    
}


//-----------------------------------------------------------------------------
// Name: SetString("this", index, "Hello World")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetString(AMX *amx, cell *params)
{
    char szString[ENT_STRING_MAX];

    if (g_pGame->IsLoadingEntities())
        return 0;

    // Get the String from the third Param
    GetStringParam(amx, params[3], szString);

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetEntString(params[2], szString);
    return 0;    
}

//-----------------------------------------------------------------------------
// Name: SetActiveDist("this", 400)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetActiveDist(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetActiveDist(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetState("this", standing)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetState(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetState(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetState("this")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetState(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetState();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: CallFunction("entid", isLibrary, "funcname", "typestr", ... )
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CallFunction(AMX *amx, cell *params)
{
    char szFuncName[64];
    char szString[ENT_STRING_MAX];
    char szTypeStr[MAX_PARAMS];
    cell a[MAX_PARAMS];
    cell *b;
    int wIndex;
    cell ret;
    int c;
    int n = 0;
    int wNumParam = (int)(params[0]/sizeof(cell))-4;
    CEntity* pTempThis = NULL;

    // Get the Function name to call
    GetStringParam(amx, params[3], szFuncName);

    // Get the Parameter type string
    GetStringParam(amx, params[4], szTypeStr);
    
    // If this is not a Library style call then set the "this" pointer to
    // The new entity
    if (!params[2])
    {
        // Record the current "this" entity
        pTempThis = g_pGame->GetEList()->GetThisEntity();
    }

    bool Deallocate = false;
    cell allotAddress = NULL;
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
    {
        // Record the state of the heap in the script we are going to call
        cell heapBefore = pTemp->GetAMX()->hea;
        
        if (pTemp->GetAMX())
        {    
            if (wNumParam || (strlen(szTypeStr) > 0 && strcmp(szTypeStr, "NULL")))
            {
                // Go through the typestr, dealing with each character in turn
                for (;;)
                {
                    // Get the next character in the typestring
                    c = szTypeStr[n];
                    
                    // if this is the end then break
                    if (!c)
                        break;
                    
                    // Check if we find s = string param
                    if (c == 's')
                    {
                        // Get the next parameter as a string
                        GetStringParam(amx, *(params+5+n), szString);
                        
                        //if (strlen(szString) > 0) // Modified by luke - apr 2010
                        //{ // Modified by luke - apr 2010
                            //Allocate enough memory for it in the other AMX
                            if ( amx_Allot(pTemp->GetAMX(), strlen(szString)+1, &a[n], &b ) == AMX_ERR_MEMORY) // Modified by luke - apr 2010
                                return -1;

                            amx_SetString(b, szString, 1);

                            // If this is the first string then remember its address
                            if (!Deallocate)
                                allotAddress = a[n];

                            Deallocate = true;
                        //} // Modified by luke - apr 2010
                    }
                    
                    // Check if we find n = number param
                    if (c == 'n')
                    {
                        cell *v;
                        amx_GetAddr(amx, *(params+5+n), &v);
                        *(a+n) = *v;
                    }        
                    
                    n++;
                }    
            }
            
            // Set the "this" entity as the script we are calling
            if (!params[2])
                g_pGame->GetEList()->SetThisEntity(pTemp);

            amx_FindPublic(pTemp->GetAMX(), szFuncName, &wIndex);

            // 5 July 2002 - should wNumParam be the num params used in the func being called?
            //amx_Execv(pTemp->GetAMX(), &ret, wIndex, wNumParam, a);
            amx_Execv(pTemp->GetAMX(), &ret, wIndex, n, a);
            
            // Reset the "this" entity back to the previous entity
            if (!params[2])
                g_pGame->GetEList()->SetThisEntity(pTempThis);
            
            // Release used memory in the other AMX
            if (Deallocate)
                amx_Release(pTemp->GetAMX(), allotAddress);
            
            // HACK: We just called a function in another script, now reset the heap from that
            // script back to the state we found it in, all scripts tested do this anyway.
            // 5 July 2002 - not needed due to fixing the num of parameters passed
            pTemp->GetAMX()->hea = heapBefore;
            return ret;
        }    
    }

    
    // Call failed
    return -1;
}

//-----------------------------------------------------------------------------
// Name: GetInitialX()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetInitialX(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        return pTemp->GetInitialX();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetInitialY()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetInitialY(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        return pTemp->GetInitialY();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetEntityCount()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetEntityCount(AMX *amx, cell *params)
{
    return g_pGame->GetEntityList()->GetCount();
}

//-----------------------------------------------------------------------------
// Name: isCuttable()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isCuttable(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisCuttableFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetCuttableFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetCuttableFlag(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;
    
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisCuttableFlag(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetLiftLevel()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetLiftLevel(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetLiftLevel();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetLiftLevel()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetLiftLevel(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetLiftLevel(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetAnglePointX("this", dist, xoff)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetAnglePointX(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return (long)(pTemp->GetXPos() - (params[2] * cos(pTemp->GetMoveAngle())) + params[3]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetAnglePointY("this", dist, yoff)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetAnglePointY(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return (long)(pTemp->GetYPos() - (params[2] * sin(pTemp->GetMoveAngle())) + params[3]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: isLarge()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isLarge(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisLargeFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetLargeFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetLargeFlag(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisLargeFlag(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetWeight()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetWeight(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetWeight();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetWeight()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetWeight(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetWeight(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetBounceValue()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetBounceValue(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetBounce();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetBounceValue()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetBounceValue(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;
    
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetBounce(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: CheckForHole("player1")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CheckForHole(AMX *amx, cell *params)
{
    if (amx->firstrun)
        return -1;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->CheckForHole();

    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetValue("this", index, val)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetValue(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetValue(params[2], params[3]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetFloat("this", index, fval)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetFloat(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetFloat(params[2], fConvertCellToFloat(params[3]));
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetHealth("this", val)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetHealth(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetHealth(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetMaxHealth("this", val)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetMaxHealth(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetMaxHealth(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetValue("this", index)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetValue(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetValue(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetFloat("this", index)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetFloat(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return ConvertFloatToCell(pTemp->GetFloat(params[2]));
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetHealth("this", val)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetHealth(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetHealth();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetMaxHealth("this", val)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetMaxHealth(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetMaxHealth();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: isPickable()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isPickable(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisPickableFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetPickableFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetPickableFlag(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisPickableFlag(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: Respawn("ident", second)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL Respawn(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;
    
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->BeginRespawn(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: isPushed()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isPushed(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisPushedFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetPushedFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetPushedFlag(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisPushedFlag(params[2]);
    return 0;
}


//-----------------------------------------------------------------------------
// Name: isInteracting()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isInteracting(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetisInteractingFlag();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetPushedFlag()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetInteractingFlag(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetisInteractingFlag(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: StartEntity()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL StartEntity(AMX *amx, cell *params)
{
    g_pGame->GetEList()->StartEntity(params[1], params[2], params[3]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetCurrentEntity()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetCurrentEntity(AMX *amx, cell *params)
{
    return g_pGame->GetEList()->GetCurrentEntity();
}

//-----------------------------------------------------------------------------
// Name: NextEntity(100)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL NextEntity(AMX *amx, cell *params)
{
    return g_pGame->GetEList()->NextEntity();
}

//-----------------------------------------------------------------------------
// Name: MessageMap("this", 0, "this is me message", false, 1, 2, 4)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL MessageMap(AMX *amx, cell *params)
{
    char szString[ENT_STRING_MAX];
    int wNext[MAX_ANSWERS];
    int n;
    cell* v;

    if (g_pGame->IsLoadingEntities())
        return 0;

    // Get the String from the third Param
    GetStringParam(amx, params[3], szString);
    
    // Get the Number of Next Message Params
    int wNumParam = (int)(params[0]/sizeof(cell))-4;
    
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
    {
        for (n = 0; n < wNumParam; n++)
        {
         amx_GetAddr(amx, *(params+5+n), &v);
         wNext[n] = *v;
        }

        pTemp->MessageMap(params[2], szString, params[4], wNext, wNumParam);
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetNextMessage("this", 0, 2)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetNextMessage(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetNextMessage(params[2], params[3]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetGotoNextMessage("this", 0)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetGotoNextMessage(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetGotoNextMessage(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetNumNextMessages("this", 0)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetNumNextMessages(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetNumNextMessages(params[2]);
    return 0;
}


//-----------------------------------------------------------------------------
// Name: SetDirFromAngle()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetDirFromAngle(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);

    if (pTemp)
        pTemp->SetDirFromAngle();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetDamage("this")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetDamage(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        return pTemp->GetDamage();
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetDamage("this", val)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetDamage(AMX *amx, cell *params)
{
    if (g_pGame->IsLoadingEntities())
        return 0;

    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetDamage(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetParam("this")
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetParam(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
    {
        if (pTemp->GetParam() != -1)
        return pTemp->GetParam();
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetParent("this", szBuffer )
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetParent(AMX *amx, cell *params)
{
    char szBuffer[20];
    cell* cptr;
    
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
    {
        // Check this ent has a parent
        if ( !pTemp->GetParent() )
            return 0;

        // Make the parents count into a string
        sprintf(szBuffer, "%d", pTemp->GetParent()->GetCount());

        // copy the parent's ID into the string
        amx_GetAddr(amx, params[2], &cptr);
        amx_SetString(cptr, szBuffer, 0);
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Name: SetActiveInGroups("this", val)
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetActiveInGroups(AMX *amx, cell *params)
{
    CEntity* pTemp = GetEntity(amx, params);
    if (pTemp)
        pTemp->SetActiveInAllGroups(params[2]);
    return 0;
}

//-----------------------------------------------------------------------------
// Name: GetID("this", ID[])
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetID(AMX *amx, cell *params)
{
    char szBuffer[20];
    cell *cptr;
    CEntity* pTemp = GetEntity(amx, params);
    
    if(pTemp)
    {
        sprintf(szBuffer, "%d", pTemp->GetCount());
        
        // copy the ID into the string
        amx_GetAddr(amx, params[2], &cptr);
        amx_SetString(cptr, szBuffer, 0);
    }    
    return 0;
}


// Define a List of native Entity functions
extern AMX_NATIVE_INFO entity_Natives[] = {
    { "GetX", GetX },
    { "GetY", GetY },
    { "SetX", SetX },
    { "SetY", SetY },
    { "GetMoveAngle",    GetMoveAngle },
    { "SetMoveAngle",    SetMoveAngle },
    { "GetDirection",    GetDirection },
    { "SetDirection",    SetDirection },
    { "GetSpeed",        GetSpeed },
    { "SetSpeed",        SetSpeed },
    { "GetSpeedMod",    GetSpeedMod },
    { "SetSpeedMod",    SetSpeedMod },
    { "AngleMove",        AngleMove },
    { "AngleCollide",    AngleCollide },
    { "SetAngleFromDir", SetAngleFromDir },
    { "ChangeDirection", ChangeDirection },
    { "CreateEntity",    CreateEntity },
    { "DeleteEntity",    DeleteEntity },
    { "SetCollisionRect",    SetCollisionRect},
    { "ClearCollisionRect",    ClearCollisionRect},
    { "isOpen", isOpen },
    { "SetOpenFlag", SetOpenFlag },
    { "isTaken", isTaken },
    { "SetTakenFlag", SetTakenFlag },
    { "isDead", isDead },
    { "SetDeadFlag", SetDeadFlag },
    { "isVisible", isVisible },
    { "SetVisibleFlag", SetVisibleFlag },
    { "isOwned", isOwned },
    { "SetOwnedFlag", SetOwnedFlag },
    { "isActive", isActive },
    { "SetActiveFlag", SetActiveFlag },
    { "SetItem", SetItem },
    { "GetItem", GetItem },
    { "SetImage", SetImage },
    { "GetImage", GetImage },
    { "SetType", SetType },
    { "GetType", GetType },
    { "Collide", Collide },
    { "AllocateStrings", AllocateStrings },
    { "GetString", GetString },
    { "SetString", SetString },
    { "CreateEntityWithID", CreateEntityWithID },
    { "SetActiveDist", SetActiveDist },
    { "SetState", SetState },
    { "GetState", GetState },
    { "CallFunction", CallFunction },
    { "GetInitialX", GetInitialX },
    { "GetInitialY", GetInitialY },
    { "GetEntityCount", GetEntityCount },
    { "isCuttable", isCuttable },
    { "SetCuttableFlag", SetCuttableFlag },
    { "GetLiftLevel", GetLiftLevel },
    { "SetLiftLevel", SetLiftLevel },
    { "GetAnglePointX", GetAnglePointX },
    { "GetAnglePointY", GetAnglePointY },
    { "CollidePoint", CollidePoint },
    { "isLarge", isLarge },
    { "SetLargeFlag", SetLargeFlag },
    { "GetWeight", GetWeight },
    { "SetWeight", SetWeight },
    { "GetBounceValue", GetBounceValue },
    { "SetBounceValue", SetBounceValue },
    { "CheckForHole", CheckForHole },
    { "SetValue", SetValue },
    { "GetValue", GetValue },
    { "SetHealth", SetHealth },
    { "GetHealth", GetHealth },
    { "SetMaxHealth", SetMaxHealth },
    { "GetMaxHealth", GetMaxHealth },
    { "isPickable", isPickable },
    { "SetPickableFlag", SetPickableFlag },
    { "CollideAll", CollideAll },
    { "Respawn", Respawn },
    { "isPushed", isPushed },
    { "SetPushedFlag", SetPushedFlag },
    { "isInteracting", isInteracting },
    { "SetInteractingFlag", SetInteractingFlag },
    { "SetPosition", SetPosition },
    { "StartEntity", StartEntity },
    { "GetCurrentEntity", GetCurrentEntity },
    { "NextEntity", NextEntity },
    { "MessageMap", MessageMap },
    { "GetNextMessage", GetNextMessage },
    { "GetGotoNextMessage", GetGotoNextMessage },
    { "GetNumNextMessages", GetNumNextMessages },
    { "SetDirFromAngle", SetDirFromAngle },
    { "GetDamage", GetDamage },
    { "SetDamage", SetDamage },
    { "GetParam", GetParam },
    { "GetParent", GetParent },
    { "SetActiveInGroups", SetActiveInGroups },
    { "GetFloat", GetFloat },
    { "SetFloat", SetFloat },
    { "GetID", GetID},
    { 0, 0 }    

};
