/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CGame.h
//
// Desc: 
// 
//-----------------------------------------------------------------------------
#ifndef CGAME_H
#define CGAME_H
#include <string>

#include "CGraphics.h"
#include "CQuestLoader.h"
#include "GeneralData.h"
#include "CDisplay.h"
#include "CEntity.h"
#include <list>
#include "CAnimation.h"
#include "CTextBox.h"
#include "CCounter.h"
#include "CNightDay.h"
#include "CFade.h"
#include "CWipe.h"
#include "CSaveLoad.h"
#include "CSound.h"
#include "PauseStack.h"
#include "FontList.h"
#include "GameOver.h"
#include "EntityList.h"



// Structure to hold information on an entity
// used to create a list of all entities when
// QST file is loaded
struct _Entity{
	char szCode[ MAX_CHARS ];
	bool fLibrary;
};


// Structure to hold an entry in a restart table
// which describes locations to restart the game
// from when loading.
struct _RestartTable{
	long lStartX;
	long lStartY;
	char szName[256];
};


//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CGame;

//-----------------------------------------------------------------------------
// Name: Class CGame
// Desc: 
//-----------------------------------------------------------------------------
class CGame
{
	CDisplay*		m_pDisplay;				// Main Display Interface
	CQuestLoader*	m_pMap;					// The Map Object
	CGraphics*		m_pGraphics;			// The Sprite Sheet set
	LList<_Entity>  m_pAvailableEntities;   // List of all entity codes
	CEntityList*	m_pEntityList;			// The list of entities
	
	LList<CAnimation> m_pAnimList;			// List of Animations
	CTextBox*		m_pTextBox;				// The Text Box
	LList<CCounter> m_pCounterList;			// List of Counters
	CFade*			m_pFade;				// CFade Object
	CWipe*			m_pWipe;				// CWipe Object
	CSaveLoad*		m_pSaveLoad;			// CSaveLoad Object
	CGameOver*		m_pGameOver;			
	CSound*			m_pSound;				// CSound Object
	CPauseStack*	m_pPauseStack;
	CFontList*		m_pFontList;

	char			m_cKeys[3];				// Encryption Keys used in save/load
	char			m_szPlayerName[32];		// Name of Player1
	char            m_szFileName[512];		// filename of QST file
	char 			m_szName[256];
	float			m_lWorldX;				// The game's World coordinates
	float			m_lWorldY;
	float			m_lTempWorldX;			// The game's Temp World coordinates
	float			m_lTempWorldY;
	long			m_lNumDynamicAnim;		// Number of CreateAnimation calls
	long			m_lNumDynamicCounter;	// Number of CreateCounter calls
	_RestartTable   m_sRestartTable[MAX_RESTART]; // The Restart table
	bool			m_fLoadingEntities;
	bool			m_fFirstTimeLoad;
	bool			m_bControlsEnabled;		// Are the controls enabled?

	// =======================
	// Font Objects
	// =======================
	TextFont*		m_pTextBoxFont;	
	TextFont*		m_pScreenTextFont;	
	CNightDay* 		m_pDayNight;			// Day/Night object

		
public:

	// =======================
	// Control Keys
	// =======================
	bool			m_fEnterKey;
	bool			m_fLeftKey;
	bool			m_fRightKey;
	bool			m_fDownKey;
	bool			m_fUpKey;
	bool			m_fQKey;
	bool			m_fWKey;
	bool			m_fEKey;
	bool			m_fAKey;
	bool			m_fSKey;
	bool			m_fDKey;
	bool			m_fZKey;
	bool			m_fXKey;
	bool			m_fCKey;
	bool			m_fRKey;
	bool			m_fFKey;
	bool			m_fVKey;
	bool			m_fTKey;
	bool			m_fGKey;
	bool			m_fShift;
	unsigned char	m_fkey;
	bool			m_fMouseKey1;
	bool			m_fMouseKey2;
	
	unsigned int    MouseX;
	unsigned int    MouseY;	
	
	bool			fatalError;
	float			m_rQKeyTimeOut;
    bool			m_Inside;
    bool            m_updates;
    CGame();
    ~CGame();

	void InitialiseLists();
	void Save(FILE* sFile);
	void Load(FILE* sFile);
	bool Initialise(const char * szCommandLine);
	bool RestoreSurfaces();			// Creates Display and Loads all surfaces
	void Update();
	void ResetKeys();
	void DrawFPS();
	void EncryptString(char *String);
	void UpdateWorldCo(float x, float y, bool forceUpdate = false);
	void ChangeWorldCo(float x, float y);
	void PutSprite(long x, long y, long wDepth, char* szCode, 
		           int r, int g, int b, int a, int wScale, int wRot,int nLayer = 0, int flip = 0);
	void PutImage(long x, long y, char* szCode, 
					  int r, int g, int b, int a, int wScale, int wRot, int flip = 0);

	float WorldToDispX(float x);
	float WorldToDispY(float y);
	int GetWidthHeight(char* szCode, bool fWidth);
	void SwapTempWorldCo();
	void CreateFonts();
	void Render();
	void RestoreFonts();
	void SimpleDrawCode(long x, long y, char* szCode);
	void SimpleDrawCode(long x, long y, char* szCode, 
		                int r, int g, int b, int a);
	void SimpleDrawCodeLarge(long x, long y, char* szCode,
		                int r, int g, int b, int a, int scale = 1);

	void SimpleTileCode(long x, long y, long width, long height, char* szCode, 
		                int r, int g, int b, int a);
	void RestartFrom(int wIndex);


	uint32_t CombineColors(uint32_t dwInputColor);
	void InvalidateFonts();
	void SetRestartTable(int index, long x, long y, char* szDesc);
	void ClearRestartTable();
	void SetRestartPoint(long x, long y);
	long CheckTempXWorld( long x );
	long CheckTempYWorld( long y );

	// Entity Functions
	CEntity* FindEntity(char *String, long x, long y, char *szIdent, bool fDynamic);
	void AddAvailableEntity(char* szCode, bool fLibrary);
	
	/*
	void RunEntities();
	void StartEntities();
	void RemoveEntity(CEntity* pTemp);
	
	void AddEntityRects();
	char* AddEntity(char* szCode, float x, float y, float rAngle, char* szIdent);
	CEntity* GetEntityWithCount( long lCount );
	*/

	// Animation Functions
	void CreateAnimations();
	void AddAnimation(float rSpeed, char* szString);
	void AddAnimationID(float rSpeed, char* szString);

	// Counter Functions
	void CreateCounters();
	void AddCounter(long lMin, long lMax, char* szString);
	void AddCounterID(long lMin, long lMax, char* szString);
	void UpdateAllCounters();
	
	// Access Functions
	CDisplay* GetDisplay(){return m_pDisplay;}
	CQuestLoader* GetMap(){return m_pMap;}
	CGraphics* GetSSheetSet(){return m_pGraphics;}
	char GetEncryptionKey(int wIndex){return m_cKeys[wIndex];}
	char* GetEncryptionKeys(){return m_cKeys;}
	long GetWorldX(){return (long)m_lWorldX;}
	long GetWorldY(){return (long)m_lWorldY;}
	long GetTempWorldX(){return (long)m_lTempWorldX;}
	long GetTempWorldY(){return (long)m_lTempWorldY;}
	LList<CEntity>* GetEntityList(){return m_pEntityList->GetEntityList();}
	CEntityList* GetEList(){return m_pEntityList;}
	LList<_Entity>* GetAvailableEntities(){return &m_pAvailableEntities;}
	LList<CAnimation>* GetAnimationList(){return &m_pAnimList;}
	LList<CCounter>* GetCounterList(){return &m_pCounterList;}
	long GetDynAnimCount(){return m_lNumDynamicAnim;}
	long GetDynCounterCount(){return m_lNumDynamicCounter;}
	int	  GetPauseLevel(){return m_pPauseStack->GetPauseLevel();}
	CTextBox* GetTextBox(){return m_pTextBox;}
	CNightDay* GetNightDay(){return m_pDayNight;}
	CFade* GetFade(){return m_pFade;}
	CWipe* GetWipe(){return m_pWipe;}
	TextFont* GetTextBoxFont(){return m_pTextBoxFont;}
	char* GetQuestFileName(){return m_szFileName;}
	char* GetQuestName(){return m_szName;}
	CSaveLoad* GetSaveLoad(){return m_pSaveLoad;}
	bool GetUpKey(){return m_fUpKey;}
	bool GetDownKey(){return m_fDownKey;}
	bool GetLeftKey(){return m_fLeftKey;}
	bool GetRightKey(){return m_fRightKey;}
	bool GetAKey(){return m_fAKey;}
	bool GetSKey(){return m_fSKey;}	
	bool GetDKey(){return m_fDKey;}
	bool GetQKey(){return m_fQKey;}
	bool GetWKey(){return m_fWKey;}	
	bool GetEKey(){return m_fDKey;}
	bool GetZKey(){return m_fZKey;}
	bool GetXKey(){return m_fXKey;}	
	bool GetCKey(){return m_fCKey;}
	bool GetRKey(){return m_fRKey;}
	bool GetFKey(){return m_fFKey;}
	bool GetVKey(){return m_fVKey;}
	bool GetTKey(){return m_fTKey;}
	bool GetGKey(){return m_fGKey;}
	bool GetEnterKey(){return m_fEnterKey;}
	char* GetPlayerName(){return m_szPlayerName;}
	void SetPlayerName(char* m_tPlayerName){strcpy(m_szPlayerName, m_tPlayerName);}
	_RestartTable GetRestartTable(int index){return m_sRestartTable[index];}
	bool IsLoadingEntities(){return m_fLoadingEntities;}
	bool IsFirstTime(){return m_fFirstTimeLoad;}
	CSound* GetSound(){return m_pSound;}
	bool IsControlsEnabled(){ return m_bControlsEnabled;}
	TextFont* GetTextFont(){ return	m_pScreenTextFont;}
	CFontList* GetFontList(){ return m_pFontList;}
	CGameOver* GetGameOver(){ return m_pGameOver;}

	// Data Brokers
	void IncrementDynAnim(){m_lNumDynamicAnim++;}
	void IncrementDynCounter(){m_lNumDynamicCounter++;}
	void SetWorldX(float lWorldX){m_lTempWorldX = lWorldX;}
	void SetWorldY(float lWorldY){m_lTempWorldY = lWorldY;}
	void SetRealWorldX(float lWorldX){m_lWorldX = lWorldX;}
	void SetRealWorldY(float lWorldY){m_lWorldY = lWorldY;}
	void SetPauseLevel(int wPauseLevel, long LineNum, char* szName){ m_pPauseStack->SetPauseLevel(wPauseLevel, LineNum, szName); }
	void SetQuestFileName(const char * name);
	void SetUpKey(bool val){m_fUpKey = val;}
	void SetDownKey(bool val){m_fDownKey = val;}
	void SetLeftKey(bool val){m_fLeftKey = val;}
	void SetRightKey(bool val){m_fRightKey = val;}
	void SetAKey(bool val){m_fAKey = val;}
	void SetSKey(bool val){m_fSKey = val;}
	void SetEKey(bool val){m_fEKey = val;}
	void SetQKey(bool val){m_fQKey = val;}
	void SetWKey(bool val){m_fWKey = val;}
	void SetDKey(bool val){m_fDKey = val;}
	void SetZKey(bool val){m_fZKey = val;}
	void SetXKey(bool val){m_fXKey = val;}
	void SetCKey(bool val){m_fCKey = val;}
	void SetRKey(bool val){m_fRKey = val;}
	void SetFKey(bool val){m_fFKey = val;}
	void SetVKey(bool val){m_fVKey = val;}
	void SetTKey(bool val){m_fTKey = val;}
	void SetGKey(bool val){m_fGKey = val;}
	void SetEnterKey(bool val){m_fEnterKey = val;}
	void SetFirstTime(bool val){m_fFirstTimeLoad = val;}
	void SetEnableControls(bool val);
	void LoadingMessage(char * message, float x, float y, uint32_t waitTime);
	void InitDeviceObjects();
	void DeleteDeviceObjects();
};


#endif

