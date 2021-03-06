/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CEntity.h
//
// Desc: A base class used by everything in Open Zelda 
//       
//-----------------------------------------------------------------------------
#ifndef CENTITY_H
#define CENTITY_H

#include <SDL2/SDL.h>

#include "GeneralData.h"
#include "LList.h"
#include "AMX/amx.h"
#include "CEntString.h"
#include "CGroup.h"

#define MAX_FLOATS  10
#define MAX_VALUES  128
#define MAX_COLRECT 10
#define OTHER_TYPE  0
#define PLAYER_TYPE 1   // Set of standard types for an entity
#define ENEMY_TYPE  2
#define NPC_TYPE    3
#define WEAPON_TYPE 4
#define ITEM_TYPE   5
#define DOOR_TYPE   6


typedef unsigned char byte;

//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CEntity;
class CBoundary;

//-----------------------------------------------------------------------------
// Name: class CEntity
// Desc:  
//-----------------------------------------------------------------------------
class CEntity 
{
protected:
	
	char	 m_szIdent[ MAX_CHARS ];		// The Entities Code
	char	 m_szCode[ MAX_CHARS ];			// The Entites type
	long  	 m_lActiveDist;					// The Active Distance for this Entity

	// ===================
	// Script Variables
	// ===================
	AMX*	 m_pAmx;						// The Abstract Machine to run the script
	void*	 m_pProgram;					// Pointer to the location of the script
	bool	 m_fFirstRun;					// Is it the first time the script has been run?


	// ===================
	// Movement Variables
	// ===================
	float    m_rMoveAngle;					// Angle of movement
	int		 m_wDirection;					// Direction Entity is facing
	float	 m_rSpeed;						// Speed of movement
	float	 m_rSpeedMod;					// Speed modifier - Speed * SpeedMod
	float	 m_x;							// Coordinates of Entity
	float	 m_y;
	long	 m_lInitialX;					// Initial Coordinates of Entity
	long     m_lInitialY;		

	// ===================
	// Flags
	// ===================
	bool	 m_fisOpen;
	bool	 m_fisTaken;
	bool	 m_fisDead;
	bool	 m_fisVisible;
	bool	 m_fisActive;
	bool	 m_fisPickable;
	bool	 m_fisPushed;
	bool	 m_fisInteracting;
	bool	 m_fisCuttable;
	bool	 m_fisOwned;

	// ===================
	// Status Variables
	// ===================
	bool	 m_fDynamic;					// Was this Entity created with CreateEntity?
	int		 m_wHealth;
	int		 m_wMaxHealth;
	int      m_wType;
	bool	 m_fDelete;						// Should this Entity be deleted?
	int		 m_wState;						// The Current State the Entity is in
	int		 m_wLiftLevel;
	long	 m_wValue[MAX_VALUES];
	float	 m_fValue[MAX_FLOATS];
	float	 m_rRespawnCount;
	long	 m_lRespawnTarget;
	int		 m_wDamage;

	// ===================
	// Strings
	// ===================
	char	 m_szItemString[ MAX_CHARS ];
	char	 m_szImageString[ MAX_CHARS ];

	// ===================
	// Throwing Vars
	// ===================
    bool	 m_fisLarge;
	int		 m_wWeight;						// How far and high the object is thrown
	int		 m_wBounce;						// How much the object bouces when it hits the ground

	// ===================
	// Collision Vars
	// ===================
	ColRect	 m_sRect[MAX_COLRECT];			// Collision Rectangles	 

	// ===================
	// Message Strings
	// ===================
	LList<CEntString> m_pStrings;			// Linked List of String Objects
	int		 m_wStringLength;				// Length of the Entity Strings

	// Param
	char		m_nParam;

	// Parent
	CEntity*	m_pParent;					// Parent Entity

	// Index in list
	long	m_lCount;						// Generated by the FindEntity function

	// Boundary this entity is in
	CBoundary*  m_pBoundary;
	bool		m_bActiveinAllGroups;		// should the entity run while the player is in another group?


public:
    CEntity();
    ~CEntity();

	void Copy(const CEntity &Copy);				// Copy everything in the supplied object to this one
	void Initialise();

	void AngleMove();
	bool AngleCollide(int wDist, int wNumPoints, int wMaxDetect, bool fHoles, int wXOffSet, int wYOffSet);
	void ChangeDirection();
	void GetAngleFromDir();
	void LoadScript(char* szFileName);
	void RunScript();
	void AddCollisionRects();
	bool TestCollisionList(float x, float y, float rAngle);
	void MoveBack(float rAngle, float fMore = 0);
	bool Collide(char* szEntity);
	void CreateStrings(int wNumString, int wLength);
	char* GetEntString(int wIndex);
	void SetEntString(int wIndex, char* szString);
	bool CollidePoint(long x, long y);
	int CheckForHole();
	bool CollideAll(char* szEntity);
	void BeginRespawn(long target);
	void Respawn();
	void DoRespawn();
	void Save(FILE* sFile,char* Ident);
	void Load(FILE* sFile, char* Ident);
	void Load2(FILE* sFile);
	void MessageMap(int wIndex, char* szMessage, bool fGotoNext, int* params, int wNumParams );
	int  GetNextMessage(int wIndex, int wAnswer );
	bool GetGotoNextMessage(int wIndex);
	int GetNumNextMessages(int wIndex);
	void SetDirFromAngle();
	bool TestHoleList(float x, float y, float rAngle);
	bool CheckForGroup();
	void ReCheckGroup( bool bForceCheck = false );

	// Data Brokers
	void SetXPos(float x){m_x = x;}
	void SetYPos(float y){m_y = y;}
	void SetMoveAngle(float rAngle){m_rMoveAngle = rAngle;}
	void SetDirection(int wDir){m_wDirection = wDir;}
	void SetSpeed(float rSpeed){m_rSpeed = rSpeed;}
	void SetSpeedMod(float rSpeed){m_rSpeedMod = rSpeed;}
	void SetDynFlag(bool fDynamic){m_fDynamic = fDynamic;}
	void SetColRect(int wIndex, RECT2D sRect, bool fSolid);
	void ClearColRect(int wIndex);
	void SetisOpenFlag(bool Param){m_fisOpen = Param;}
	void SetisTakenFlag(bool Param){m_fisTaken = Param;}
	void SetisDeadFlag(bool Param){m_fisDead = Param;}
	void SetisVisibleFlag(bool Param){m_fisVisible = Param;}
	void SetisActiveFlag(bool Param){m_fisActive = Param;}
	void SetisOwnedFlag(bool Param){ m_fisOwned = Param;}
	void SetItem(char* szString);
	void SetImage(char* szString);
	void SetType(int wType){m_wType = wType;}
	void SetDeletionFlag(bool fDel){m_fDelete = fDel;}
	void SetActiveDist(long lDist){m_lActiveDist = lDist;}
	void SetState(int wState){m_wState = wState;}
	void SetisCuttableFlag(bool Param){m_fisCuttable = Param;}
	void SetLiftLevel(int Param){m_wLiftLevel = Param;}
	void SetisLargeFlag(bool Param){m_fisLarge = Param;}
	void SetWeight(int Param){m_wWeight = Param;}
	void SetBounce(int Param){m_wBounce = Param;}
	void SetValue(int wIndex, long Param);
	void SetHealth(int Param){m_wHealth = Param;}
	void SetMaxHealth(int Param){m_wMaxHealth = Param;}
	void SetisPickableFlag(bool Param){m_fisPickable = Param;}
	void SetisPushedFlag(bool Param){m_fisPushed = Param;}
	void SetisInteractingFlag(bool Param){m_fisInteracting = Param;}
	void SetPosition(long x, long y){m_x = (float)x; m_y = (float)y;}
	void SetDamage(int Param){m_wDamage = Param;}
	void SetParam( char cParam ){ m_nParam = cParam;}
	void SetParent( CEntity* parent ){ m_pParent = parent;}
	void SetEntCount( long lCount ){ m_lCount = lCount;}
	void SetBoundary( CBoundary* pVal){ m_pBoundary = pVal;}
	void SetActiveInAllGroups( bool bVal ){ m_bActiveinAllGroups = bVal;}
	void SetFloat(int wIndex, float Param);

	// Access Functions
	long  GetCount(){ return m_lCount;}
	char* GetIdent(){ return m_szIdent;}
	char* GetCode(){return m_szCode;}
	float GetXPos(){return m_x;}
	float GetYPos(){return m_y;}
	float GetMoveAngle(){return m_rMoveAngle;}
	int	 GetDirection(){return m_wDirection;}
	float GetSpeed(){return m_rSpeed;}
	float GetSpeedMod(){return m_rSpeedMod;}
	bool  GetDynamicFlag(){return m_fDynamic;}
	bool  GetFirstRun(){return m_fFirstRun;}
	ColRect GetColRect(int wIndex){return m_sRect[wIndex];}
	bool  GetisOpenFlag(){return m_fisOpen;}
	bool  GetisTakenFlag(){return m_fisTaken;}
	bool  GetisDeadFlag(){return m_fisDead;}
	bool  GetisVisibleFlag(){return m_fisVisible;}
	bool  GetisActiveFlag(){return m_fisActive;}
	bool  GetisOwnedFlag(){return m_fisOwned;}
	char* GetItemString(){return m_szItemString;}
	char* GetImageString(){return m_szImageString;}
	int	  GetType(){return m_wType;}
	bool  DeleteFlagSet(){return m_fDelete;}
	long  GetActiveDist(){return m_lActiveDist;}
	int   GetState(){return m_wState;}
	AMX*  GetAMX(){return m_pAmx;}
	long  GetInitialX(){return m_lInitialX;}
	long  GetInitialY(){return m_lInitialY;}
	int	  GetLiftLevel(){return m_wLiftLevel;}
	bool  GetisCuttableFlag(){return m_fisCuttable;}
	bool  GetisLargeFlag(){return m_fisLarge;}
	int	  GetWeight(){return m_wWeight;}
	int	  GetBounce(){return m_wBounce;}
	long   GetValue(int wIndex);
	int   GetHealth(){return m_wHealth;}
	int   GetMaxHealth(){return m_wMaxHealth;}
	bool  GetisPickableFlag(){return m_fisPickable;}
	bool  GetisPushedFlag(){return m_fisPushed;}
	bool  GetisInteractingFlag(){return m_fisInteracting;}
	int   GetDamage(){return m_wDamage;}
	char  GetParam(){ return m_nParam;}
	CEntity* GetParent(){ return m_pParent;}
	CBoundary*   GetBoundary(){ return m_pBoundary;}
	float GetFloat(int wIndex);
};

#endif

