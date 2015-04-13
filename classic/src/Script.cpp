/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: Script.cpp
//
// Desc:
//
//
//-----------------------------------------------------------------------------
#include "Script.h"
#include "CGame.h"
#include "OpenZelda_private.h"
#include "SaveLoadFunctions.h"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame *			g_pGame;
extern float			g_rDelta;

extern AMX_NATIVE_INFO	core_Natives[];
extern AMX_NATIVE_INFO	console_Natives[];
extern AMX_NATIVE_INFO  entity_Natives[];
extern AMX_NATIVE_INFO  float_Natives[];
extern AMX_NATIVE_INFO  animation_Natives[];
extern AMX_NATIVE_INFO  counter_Natives[];
extern AMX_NATIVE_INFO  socket_Natives[];
extern AMX_NATIVE_INFO  time_Natives[];

//-----------------------------------------------------------------------------
// Name: loadprogram()
// Desc:
//
//-----------------------------------------------------------------------------
void * loadprogram(AMX *amx,char *filename)
{
	FILE *fp;
	AMX_HEADER hdr;
	void *program = NULL;

	if (amx)
		amx->firstrun = 1;


	std::string path(FS_Gamedir());
	path.append(filename);

	if ((fp = fopen(path.c_str(),"rb")) != NULL) {
		fread(&hdr, sizeof hdr, 1, fp);
		if ((program = malloc((int)hdr.stp)) != NULL) {
			rewind(fp);
			fread(program, 1, (int)hdr.size, fp);
			fclose(fp);
			memset(amx,0,sizeof *amx);
			if (amx_Init(amx,program) == AMX_ERR_NONE)
			{
				strcpy(amx->szFileName, filename);
				RegisterNatives(*amx);
				return program;
			}
			free(program);
		} /* if */
	} /* if */

	return NULL;
}

//-----------------------------------------------------------------------------
// Name: TestCollidePoint()
// Desc: Returns the mask value of the point at a world coordinate
//
//-----------------------------------------------------------------------------
long TestCollidePoint(long x, long y, bool bIncludeObj)
{
	// Convert the Given world Coordinates to screenOffset Coordinates
	long wTempX = g_pGame->GetMap()->WorldToScreenX(x, y);
	long wTempY = g_pGame->GetMap()->WorldToScreenY(x, y);

	// Not on a screen so treat it as solid 0
	if (wTempX < 0 || wTempY < 0)
		return -1;

	// Get the Actual Screen we are on
	CScreen* pScreenOn = g_pGame->GetMap()->GetScreenOn(x, y);

	if (!pScreenOn)
		return -1;

	if (bIncludeObj)
	{
		// Check for objects
		if (g_pGame->GetMap()->TestCollisionList( x, y ) == true)
			return 0;
	}

	if (pScreenOn->CheckMask(wTempX, wTempY))
		return pScreenOn->GetMaskValue(wTempX + (wTempY * VSCREEN_WIDTH));

	return -1;
}


//-----------------------------------------------------------------------------
// Name: CheckForHoleXY()
// Desc:
//
//-----------------------------------------------------------------------------
int CheckForHoleXY(long x, long y)
{
	// Convert the Given world Coordinates to screenOffset Coordinates
	long wTempX = g_pGame->GetMap()->WorldToScreenX(x, y);
	long wTempY = g_pGame->GetMap()->WorldToScreenY(x, y);

	if (wTempX < 0 || wTempY < 0)
		return -1;

	// Get the Actual Screen we are on
	CScreen* pScreenOn = g_pGame->GetMap()->GetScreenOn(x, y);

	if (!pScreenOn)
		return -1;

	return pScreenOn->CheckForHoleXY(wTempX, wTempY);
}

//-----------------------------------------------------------------------------
// Name: GetStringParam()
// Desc:
//
//-----------------------------------------------------------------------------
bool GetStringParam(AMX *amx, cell sParam, char* szString)
{
	cell *n;

	amx_GetAddr(amx, sParam, &n);
	amx_GetString(szString, n);

	return true;
}

/* Move to Pawn
void ConvertBit(int bin, char *str)
{
	unsigned int mask;

	mask = 0x80000000;
	while (mask)
	{
		if (bin & mask)
			  *str = '1';
		  else
			  *str = '0';
		str++;
		mask >>= 1;
	}
	*str = 0;
}
*/
//-----------------------------------------------------------------------------
// Name: PutSprite()
// Desc: PutSprite(ident[], valx, valy, vald, layer = 0, r = 255, g = 255, b = 255, a = 255, rot = 0, scale = 100, flip);
//
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL PutSprite(AMX *amx, cell *params)
{
	char szString[ MAX_CHARS ];

	// Check we have some parameters
	if (params[0] == 0)
		return 0;

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);
	int numparam = params[0]/sizeof(cell);
	g_pGame->PutSprite(params[2], params[3], params[4], szString,
					   params[6], params[7], params[8], params[9],
					   params[11], params[10], params[5], numparam == 12 ? params[12] : 0);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetWidth()
// Desc:
//
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetWidth(AMX *amx, cell *params)
{
	char szString[ MAX_CHARS ];

	// Check we have some parameters
	if (params[0] == 0)return 0;

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);
	return g_pGame->GetWidthHeight(szString, true);
}

//-----------------------------------------------------------------------------
// Name: GetHeight()
// Desc:
//
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetHeight(AMX *amx, cell *params)
{
	char szString[ MAX_CHARS ];

	// Check we have some parameters
	if (params[0] == 0)return 0;

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);
	return g_pGame->GetWidthHeight(szString, false);
}


//-----------------------------------------------------------------------------
// Name: QKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL QKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fQKey;
}

//-----------------------------------------------------------------------------
// Name: WKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL WKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fWKey;
}
static cell AMX_NATIVE_CALL EKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fEKey;
}
//-----------------------------------------------------------------------------
// Name: AKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL AKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fAKey;
}

//-----------------------------------------------------------------------------
// Name: SKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fSKey;
}

static cell AMX_NATIVE_CALL DKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fDKey;

}
//-----------------------------------------------------------------------------
// Name: AKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL ZKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fZKey;
}

//-----------------------------------------------------------------------------
// Name: SKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL XKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fXKey;

}

static cell AMX_NATIVE_CALL CKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fCKey;

}

static cell AMX_NATIVE_CALL RKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fRKey;
}

static cell AMX_NATIVE_CALL FKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fFKey;
}

static cell AMX_NATIVE_CALL VKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fVKey;
}

static cell AMX_NATIVE_CALL TKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fTKey;
}

static cell AMX_NATIVE_CALL GKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fGKey;
}


//-----------------------------------------------------------------------------
// Name: EnterKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL EnterKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fEnterKey;
}

//-----------------------------------------------------------------------------
// Name: UpKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL UpKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fUpKey;
}

//-----------------------------------------------------------------------------
// Name: DownKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DownKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fDownKey;
}

//-----------------------------------------------------------------------------
// Name: LeftKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL LeftKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fLeftKey;
}

//-----------------------------------------------------------------------------
// Name: RightKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL RightKey(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fRightKey;
}

//-----------------------------------------------------------------------------
// Name: SetQKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetQKey(AMX *amx, cell *params)
{
	// Override the Timeout
	g_pGame->m_rQKeyTimeOut = 0;
	g_pGame->m_fQKey = params[1];
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetWKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetWKey(AMX *amx, cell *params)
{
	g_pGame->m_fWKey = params[1];
	return 0;
}
static cell AMX_NATIVE_CALL SetEKey(AMX *amx, cell *params)
{
	g_pGame->m_fEKey = params[1];
	return 0;
}
//-----------------------------------------------------------------------------
// Name: SetAKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetAKey(AMX *amx, cell *params)
{
	g_pGame->m_fAKey = params[1];
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetSKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetSKey(AMX *amx, cell *params)
{
	g_pGame->m_fSKey = params[1];
	return 0;
}

static cell AMX_NATIVE_CALL SetDKey(AMX *amx, cell *params)
{
	g_pGame->m_fDKey = params[1];
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetAKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetZKey(AMX *amx, cell *params)
{
	g_pGame->m_fZKey = params[1];
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetSKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetXKey(AMX *amx, cell *params)
{
	g_pGame->m_fXKey = params[1];
	return 0;
}

static cell AMX_NATIVE_CALL SetCKey(AMX *amx, cell *params)
{
	g_pGame->m_fCKey = params[1];
	return 0;
}
//-----------------------------------------------------------------------------
// Name: SetEnterKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetEnterKey(AMX *amx, cell *params)
{
	g_pGame->m_fEnterKey = params[1];
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetUpKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetUpKey(AMX *amx, cell *params)
{
	g_pGame->m_fUpKey = params[1];
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetDownKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetDownKey(AMX *amx, cell *params)
{
	g_pGame->m_fDownKey = params[1];
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetLeftKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetLeftKey(AMX *amx, cell *params)
{
	g_pGame->m_fLeftKey = params[1];
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetRightKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetRightKey(AMX *amx, cell *params)
{
	g_pGame->m_fRightKey = params[1];
	return 0;
}


//-----------------------------------------------------------------------------
// Name: SetWorldX()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetWorldX(AMX *amx, cell *params)
{
	g_pGame->SetWorldX(params[1]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetWorldY()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetWorldY(AMX *amx, cell *params)
{
	g_pGame->SetWorldY(params[1]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetWorldX()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetWorldX(AMX *amx, cell *params)
{
	return g_pGame->GetWorldX();
}

//-----------------------------------------------------------------------------
// Name: GetWorldY()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetWorldY(AMX *amx, cell *params)
{
	return g_pGame->GetWorldY();
}

//-----------------------------------------------------------------------------
// Name: GetGroupMinX()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetGroupMinX(AMX *amx, cell *params)
{
	return g_pGame->GetMap()->GetBounds().left;
}

//-----------------------------------------------------------------------------
// Name: GetGroupMinY()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetGroupMinY(AMX *amx, cell *params)
{
	return g_pGame->GetMap()->GetBounds().top;
}

//-----------------------------------------------------------------------------
// Name: GetGroupMaxX()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetGroupMaxX(AMX *amx, cell *params)
{
	return g_pGame->GetMap()->GetBounds().right;
}

//-----------------------------------------------------------------------------
// Name: GetGroupMaxY()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetGroupMaxY(AMX *amx, cell *params)
{
	return g_pGame->GetMap()->GetBounds().bottom;
}

//-----------------------------------------------------------------------------
// Name: UpdateWorldCo()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL UpdateWorldCo(AMX *amx, cell *params)
{
	int numparam = params[0]/sizeof(cell);
	g_pGame->UpdateWorldCo(params[1], params[2], numparam == 3 ? params[3] : false);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: NearPoint(x1, y1, x2, y2, dist)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL NearPoint(AMX *amx, cell *params)
{
	return isNearPoint(params[1], params[2], params[3], params[4], params[5]);
}

//-----------------------------------------------------------------------------
// Name: InitTextBox("Text", width, true)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL InitTextBox(AMX *amx, cell *params)
{
	char szString[ENT_STRING_MAX];
	if (params[0] == 0)return 0;

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);

	// Initialise the Text Box
	g_pGame->GetTextBox()->InitText( szString, params[2], params[3]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetTextBoxColor(red, green, blue, alpha)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetTextBoxColor(AMX *amx, cell *params)
{
	g_pGame->GetTextBox()->SetTextColor(params[1], params[2], params[3], params[4]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetPauseLevel(n)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetPauseLevel(AMX *amx, cell *params)
{
	// Dont allow the pause level higher than 2
	// because then we would be saving/loading
	if ( params[1] < 3 )
		g_pGame->SetPauseLevel(params[1], amx->curline, amx->szFileName);

	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetPauseLevel()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetPauseLevel(AMX *amx, cell *params)
{
	return g_pGame->GetPauseLevel();
}

//-----------------------------------------------------------------------------
// Name: FirstRun()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL FirstRun(AMX *amx, cell *params)
{
	if (amx)
		return amx->firstrun;

	return 0;
}

//-----------------------------------------------------------------------------
// Name: InitialPlayerX()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL InitialPlayerX(AMX *amx, cell *params)
{
	return g_pGame->GetMap()->GetPlayerStartX();
}

//-----------------------------------------------------------------------------
// Name: InitialPlayerY()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL InitialPlayerY(AMX *amx, cell *params)
{
	return g_pGame->GetMap()->GetPlayerStartY();
}

//-----------------------------------------------------------------------------
// Name: DrawImage(code, x, y)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DrawImage(AMX *amx, cell *params)
{
	char szString[ MAX_CHARS ];

	// Check we have some parameters
	if (params[0] == 0)
		return 0;

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);
	int numparam = params[0]/sizeof(cell);
	g_pGame->PutImage(params[2], params[3], szString,
					  params[4], params[5], params[6],
					  params[7], params[9], params[8], numparam == 10 ? params[10] : 0);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: ToString(val, string)
// Desc: converts val to a string and places it in string
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL ToString(AMX *amx, cell *params)
{
	char szString[ MAX_CHARS ];
	cell *cptr;

	sprintf(szString, "%d", params[1]);

	// Place the converted number into the supplied buffer
	amx_GetAddr(amx,params[2],&cptr);
	amx_SetString(cptr, szString, 0);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: CheckForHoleXY(x,y)
// Desc: Checks if the point x,y is in a hole
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CheckForHoleXY(AMX *amx, cell *params)
{
	if (amx->firstrun)
		return -1;

	return CheckForHoleXY(params[1], params[2]);
}

//-----------------------------------------------------------------------------
// Name: ToFile(string)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL ToFile(AMX *amx, cell *params)
{
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%d\n", params[1]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: Text(string)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL Text(AMX *amx, cell *params)
{
	char szString[64];
	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n",szString);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: CheckMask(x,y)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CheckMask(AMX *amx, cell *params)
{
	return (long)TestCollidePoint(params[1], params[2], params[3]);
}

//-----------------------------------------------------------------------------
// Name: FadeTo(r,g,b, speed)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL FadeTo(AMX *amx, cell *params)
{
	return g_pGame->GetFade()->FadeTo(params[1], params[2], params[3], params[4]);
}

//-----------------------------------------------------------------------------
// Name: Wipe(dir, speed)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL Wipe(AMX *amx, cell *params)
{
	g_pGame->GetWipe()->BeginWipe(params[1], params[2]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetMinuteCount()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetMinuteCount(AMX *amx, cell *params)
{
	return g_pGame->GetNightDay()->GetMinuteCount();
}

//-----------------------------------------------------------------------------
// Name: GetHourCount()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetHourCount(AMX *amx, cell *params)
{
	return g_pGame->GetNightDay()->GetHourCount();
}

//-----------------------------------------------------------------------------
// Name: GetDayCount()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetDayCount(AMX *amx, cell *params)
{
	return g_pGame->GetNightDay()->GetDayCount();
}

//-----------------------------------------------------------------------------
// Name: SetMinuteCount(54)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetMinuteCount(AMX *amx, cell *params)
{
	g_pGame->GetNightDay()->SetMinuteCount(params[1]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetHourCount(23)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetHourCount(AMX *amx, cell *params)
{
	g_pGame->GetNightDay()->SetHourCount(params[1]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetDayCount(1)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetDayCount(AMX *amx, cell *params)
{
	g_pGame->GetNightDay()->SetDayCount(params[1]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetDayLength(1800)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetDayLength(AMX *amx, cell *params)
{
	g_pGame->GetNightDay()->SetDayLength(params[1]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: IsInside()
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL isInside(AMX *amx, cell *params)
{
	if (g_pGame->GetMap()->GetCurrentScreen())
	{
		if ( g_pGame->GetMap()->GetCurrentScreen()->inside())
		{
			return 1;
		}
	}
	return 0;
}


//-----------------------------------------------------------------------------
// Name: SetRestartTable(index, x, y, "desc");
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetRestartTable(AMX *amx, cell *params)
{
	char szString[256];
	if (params[0] == 0)return 0;

	// Get the String from the first Param

	GetStringParam(amx, params[4], szString);
	g_pGame->SetRestartTable(params[1], params[2], params[3], szString);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: ClearRestartTable();
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL ClearRestartTable(AMX *amx, cell *params)
{
	g_pGame->ClearRestartTable();
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetRestartPoint(x, y);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetRestartPoint(AMX *amx, cell *params)
{
	g_pGame->SetRestartPoint(params[1], params[2]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetGlobal(index, val);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetGlobal(AMX *amx, cell *params)
{
	g_pGame->GetMap()->SetGlobal(params[1], params[2]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetGlobal(index);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetGlobal(AMX *amx, cell *params)
{
	return g_pGame->GetMap()->GetGlobal(params[1]);
}

//-----------------------------------------------------------------------------
// Name: SetLocal(index, val);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetLocal(AMX *amx, cell *params)
{
	g_pGame->GetMap()->GetGroupSet()->SetLocal(params[1], params[2]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetLocal(index);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetLocal(AMX *amx, cell *params)
{
	return g_pGame->GetMap()->GetGroupSet()->GetLocal(params[1]);
}

//-----------------------------------------------------------------------------
// Name: PlaySound("code", volume);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL PlaySound(AMX *amx, cell *params)
{
	// Check the Sound object is active
	if (!g_pGame->GetSound())
		return -1;

	char szString[ MAX_CHARS ];

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);

	g_pGame->GetSound()->PlaySoundFX(szString, params[2]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: StopSound("name");
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL StopSound(AMX *amx, cell *params)
{
	// Check the Sound object is active
	if (!g_pGame->GetSound())
		return -1;

	char szString[64];

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);

	g_pGame->GetSound()->StopSoundFX(szString);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetMusicVolume(volume);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetMusicVolume(AMX *amx, cell *params)
{
	g_pGame->GetSound()->SetMusicVolume(params[1]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: PlayMusic("name", fade = true);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL PlayMusic(AMX *amx, cell *params)
{
	char szString[64];

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);

	g_pGame->GetSound()->SwitchMusic(szString, 0, params[2]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: GetMusic(String[]);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetMusic(AMX *amx, cell *params)
{
	if (params[0] == 0)return 0;
	char * music;
	cell *cptr;

	music = g_pGame->GetSound()->GetMusicString();
	amx_GetAddr(amx,params[1],&cptr);
	amx_SetString(cptr, music, 0);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetMusicFadeSpeed(speed);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetMusicFadeSpeed(AMX *amx, cell *params)
{
	g_pGame->GetSound()->SetMusicFadeSpeed(params[1]);
	return 0;
}


//-----------------------------------------------------------------------------
// Name: FinishedReading();
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL FinishedReading(AMX *amx, cell *params)
{
	return g_pGame->GetTextBox()->GetFinishedReading();
}

//-----------------------------------------------------------------------------
// Name: FinishedReading();
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetLastAnswer(AMX *amx, cell *params)
{
	return g_pGame->GetTextBox()->GetAnswer();
}


//-----------------------------------------------------------------------------
// Name: CalculateAngle(x1, y1, x2, y3);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL CalculateAngle(AMX *amx, cell *params)
{
	return RTOD * CalcAngle( params[1],
							 params[2],
							 params[3],
							 params[4] );
}

//-----------------------------------------------------------------------------
// Name: SetLowerLevel(x,y);
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetLowerLevel(AMX *amx, cell *params)
{
	g_pGame->GetMap()->SetLowerLevel(params[1], params[2]);
	return 0;
}


//-----------------------------------------------------------------------------
// Name: GetLowerLevelX();
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetLowerLevelX(AMX *amx, cell *params)
{
	SDL_Point point = g_pGame->GetMap()->GetLowerLevel();
	return point.x;
}

//-----------------------------------------------------------------------------
// Name: GetLowerLevelY();
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetLowerLevelY(AMX *amx, cell *params)
{
	SDL_Point point = g_pGame->GetMap()->GetLowerLevel();
	return point.y;
}

//-----------------------------------------------------------------------------
// Name: EnableControls( true );
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL EnableControls(AMX *amx, cell *params)
{
	g_pGame->SetEnableControls(params[1]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: DrawRectangle( x1,y1,x2,y2. Red, Green, Blue, Alpha );
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DrawRectangle(AMX *amx, cell *params)
{
	if (!g_pGame->GetDisplay())
		return 0;

	RECT2D sRect = { params[1],
				   params[2],
				   params[3],
				   params[4]};
	SDL_Color col = { params[5], params[6], params[7], params[8]};

	g_pGame->GetDisplay()->GetShapeBuffer()->AddShape( 0, sRect, params[9], col );
	return 1;
}

//-----------------------------------------------------------------------------
// Name: DrawLine( x1,y1,x2,y2. wid, Red, Green, Blue, Alpha );
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DrawLine(AMX *amx, cell *params)
{
	if (!g_pGame->GetDisplay())
		return 0;

	RECT2D sRect = { params[1],
				   params[2],
				   params[3],
				   params[4]};
	SDL_Color col = { params[6], params[7], params[8], params[9]};
	g_pGame->GetDisplay()->GetShapeBuffer()->AddShape( 1, sRect, params[5], col );
	return 1;
}

//-----------------------------------------------------------------------------
// Name: DrawText("Text", x, y, r, g, b, a)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DrawText(AMX *amx, cell *params)
{
	char szString[256];

	if (!g_pGame->GetDisplay())
		return 0;

	if (params[0] == 0)return 0;

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);
	SDL_Point point = { params[2], params[3] };

	SDL_Color col;
	SetARGB(col,params[7],params[4],params[5],params[6]);
	g_pGame->GetDisplay()->GetTextBuffer()->AddText( szString, point, col);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: DrawBitmapText("Text", x, y, style, r, g, b, a, scale)
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL DrawBitmapText(AMX *amx, cell *params)
{
	char szString[256];

	if (!g_pGame->GetDisplay())
		return 0;

	if (params[0] == 0)return 0;

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);
	SDL_Point point = { params[2], params[3] };
	SDL_Color col;
	SetARGB(col,params[8],params[5],params[6],params[7]);

	g_pGame->GetDisplay()->GetBitmapTextBuffer()->AddText( szString, point,
												   col, params[4], params[9]);
	return 0;
}


//-----------------------------------------------------------------------------
// Name: SetTextSize( size )
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetTextSize(AMX *amx, cell *params)
{
	if (g_pGame->GetFontList())
		g_pGame->GetFontList()->SetFontSize( params[1] );
	return 0;
}


//-----------------------------------------------------------------------------
// Name: GameOver( )
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GameOver(AMX *amx, cell *params)
{
	if (g_pGame->GetGameOver())
		g_pGame->GetGameOver()->PreGameOver();
	return 0;
}

//-----------------------------------------------------------------------------
// Name: SetSpriteAlpha( "sprite", newAlpha )
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SetSpriteAlpha(AMX *amx, cell *params)
{
	char szString[20];

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);

	if (!g_pGame->GetSSheetSet())
		return -1;

	CSprite* pSprite = g_pGame->GetSSheetSet()->GetSprite(szString, false);

	if (pSprite)
		pSprite->m_wAlpha = params[2];

	return 0;
}

//-----------------------------------------------------------------------------
// Name: SaveGame( Slot )
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL SaveGame(AMX *amx, cell *params)
{
	SaveTheGame(params[1]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: LoadGame( Slot )
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL LoadGame(AMX *amx, cell *params)
{
	LoadTheGame(params[1]);
	return 0;
}

static cell AMX_NATIVE_CALL _GetPlayerName(AMX *amx, cell *params)
{
	if (params[0] == 0)return 0;
	char * szPlayer;
	cell *cptr;
	szPlayer = g_pGame->GetPlayerName();

	amx_GetAddr(amx,params[1],&cptr);
	amx_SetString(cptr, szPlayer, 0);
	return 0;
}
static cell AMX_NATIVE_CALL _SetPlayerName(AMX *amx, cell *params)
{
	char szString[256];

	if (params[0] == 0)return 0;

	// Get the String from the first Param
	GetStringParam(amx, params[1], szString);

	g_pGame->SetPlayerName(szString);
	return 0;
}

static cell AMX_NATIVE_CALL _version(AMX *amx, cell *params)
{
	return VER_BUILD;
}

//extern int keyset[20];
//-----------------------------------------------------------------------------
// Name: Version()
// Desc: 3
// apr 2010: add by luke
// native SetKey(PLAYERKEY, NEWKEY);
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL _SetKey(AMX *amx, cell *params)
{
	unsigned int k = (unsigned int)params[1];
	if ( k < 20 )
	{
		//keyset[k] = params[2];
	}
	return 0;
}


//-----------------------------------------------------------------------------
// Name: Shift()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL _Shift(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fShift;
}

//---------------------------------------------------------------
// MOUSE: LClick
//---------------------------------------------------------------
static cell AMX_NATIVE_CALL _LClick(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fMouseKey1;
}

//---------------------------------------------------------------
// MOUSE: RClick
//---------------------------------------------------------------
static cell AMX_NATIVE_CALL _RClick(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->m_fMouseKey2;

}

//---------------------------------------------------------------
// MOUSE: MouseX
//---------------------------------------------------------------
static cell AMX_NATIVE_CALL _MouseX(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->MouseX;

}

//---------------------------------------------------------------
// MOUSE: MouseY
//---------------------------------------------------------------
static cell AMX_NATIVE_CALL _MouseY(AMX *amx, cell *params)
{
	// Return false if controls are not enabled
	if (!g_pGame->IsControlsEnabled())
		return 0;

	// If pauselevel is 3 we are saving/loading - so return false
	if ( g_pGame->GetPauseLevel() >= 3)
		return 0;

	return g_pGame->MouseY;

}

//---------------------------------------------------------------
// Name: SetDayNightColor
// SetDayNightColor(hour,r,g,b);
// Use -1 or number higher then 23 for instant color
//---------------------------------------------------------------
static cell AMX_NATIVE_CALL _SetDayNightColor(AMX *amx, cell *params)
{
	if(params[1] < 0 || params[1] > 24)
		g_pGame->GetNightDay()->SetColors(params[2],params[3],params[4]);
	else
	{
		g_pGame->GetNightDay()->m_DayNight.HourTarget[params[1]].r = (uint8_t)params[2];
		g_pGame->GetNightDay()->m_DayNight.HourTarget[params[1]].g = (uint8_t)params[3];
		g_pGame->GetNightDay()->m_DayNight.HourTarget[params[1]].b = (uint8_t)params[4];
	}
	return 0;
}

//---------------------------------------------------------------
// WindowMessage(message, param);
// native WindowMessage(message, value, type = 0x0111 );
// See resource.h for message numbers
//---------------------------------------------------------------
static cell AMX_NATIVE_CALL _WindowMessage(AMX *amx, cell *params)
{
	/*
	int numparam = params[0]/sizeof(cell);
	if (numparam >= 3)
		return PostMessage( g_hWnd, params[3], MAKEWPARAM(params[1],0), MAKELPARAM(params[2],0) );
	else if (numparam >= 2)
		return PostMessage( g_hWnd, WM_COMMAND, MAKEWPARAM(params[1],0), MAKELPARAM(params[2],0) );
	else if (numparam >= 1)
		return PostMessage( g_hWnd, WM_COMMAND, MAKEWPARAM(params[1],0), 0 );
*/

/*
uint32_t myEventType = SDL_RegisterEvents(1);
if (myEventType != ((uint32_t)-1)) {
	SDL_Event event;
	SDL_zero(event);
	event.type = myEventType;
	event.user.code = my_event_code;
	event.user.data1 = significant_data;
	event.user.data2 = 0;
	SDL_PushEvent(&event);
}
*/



	return 0;
}

int set_amxstring(AMX *amx,cell amx_addr,const char *source,int max)
{
	cell* dest = (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
	cell* start = dest;
	while (max--&&*source)
		*dest++=(cell)*source++;
	*dest = 0;
	return dest-start;
}

cell* get_amxaddr(AMX *amx,cell amx_addr)
{
	return (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
}

char* get_amxstring(AMX *amx,cell amx_addr,int id, int& len)
{
	static char buffor[4][3072];
	register cell* source = (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
	register char* dest = buffor[id];
	char* start = dest;
	while ((*dest++=(char)(*source++)))
		;
	len = --dest - start;
	return start;
}
#include <ctype.h>
char * format_amxstring(AMX *amx, cell *params, int parm, int &len)
{
	static char buffer[2][3072];
	static char format[16];
	char *ptr,*arg;
	char *dest = *buffer;
	cell *src = get_amxaddr(amx, params[parm++]);
	int numparam = *params/sizeof(cell);
	while(*src) {
		if (*src=='%'&&*(src+1))
		{
			ptr = format;
			*ptr++ = *src++;
			if (*src=='%')
			{
				*dest++=*src++;
				continue;
			}
			while (!isalpha(*ptr++=*src++))
			;
			*ptr=0;
			if (numparam < parm) continue;
			arg = buffer[1];
			switch(*(ptr-1))
			{
				case 's': sprintf(arg,format,get_amxstring(amx, params[parm++],2,len)); break;
				case 'f': sprintf(arg,format,(float)*get_amxaddr(amx, params[parm++])); break;
				case 'x': sprintf(arg,format,(int)*get_amxaddr(amx, params[parm++])); break;
				default: sprintf(arg,format,(int)*get_amxaddr(amx, params[parm++]));
			}
			while(*arg) *dest++=*arg++;
			continue;
		}
		*dest++=*src++;
	}
	*dest=0;
	len = dest - *buffer;
	return *buffer;
}
/*
* Fills string with given format and parameters.
* Function returns number of copied chars.
* Example: strformat(dest, sizeof(dest),"Hello %s. You are %d years old","Tom",17).
native strformat(output[] ,len ,const format[] , {Float,_}:...);
*/
static cell AMX_NATIVE_CALL strformat(AMX *amx, cell *params) /* 3 param */
{
	int len;
	return set_amxstring(amx, params[1], format_amxstring(amx,params,3,len), params[2]);
}

//-----------------------------------------------------------------------------
// Name: GetKey()
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL GetKey(AMX *amx, cell *params)
{
	return g_pGame->m_fkey;
}

//-----------------------------------------------------------------------------
// Name: EnableDayNight( Enable )
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL EnableDayNight(AMX *amx, cell *params)
{
	g_pGame->m_Inside = params[1];
	return 0;
}

// native DrawMask(name[], x, y);
static cell AMX_NATIVE_CALL _PutMask(AMX *amx, cell *params)
{
	char name[ MAX_CHARS ];
	GetStringParam(amx, params[1], name);
	CSprite* pSprite = g_pGame->GetSSheetSet()->GetSprite(name, false);
	g_pGame->GetMap()->GetCurrentScreen()->CopySpriteMask(params[2], params[3], pSprite);
	return 0;
}

// native FillMask(value, x, y, width,height);
static cell AMX_NATIVE_CALL _FillMask(AMX *amx, cell *params)
{
	g_pGame->GetMap()->GetCurrentScreen()->FillMask(params[1], params[2], params[3], params[4], params[5]);
	return 0;
}

//-----------------------------------------------------------------------------
// Name: EnableUpdates( Enable )
// Desc:
//-----------------------------------------------------------------------------
static cell AMX_NATIVE_CALL EnableUpdates(AMX *amx, cell *params)
{
	g_pGame->m_updates = params[1];
	return 0;
}

static cell AMX_NATIVE_CALL _Save(AMX *amx, cell *params)
{
	g_pGame->GetSaveLoad()->PreSave();
	return 0;
}
static cell AMX_NATIVE_CALL _Load(AMX *amx, cell *params)
{
	g_pGame->GetSaveLoad()->PreLoad();
	return 0;
}


static cell AMX_NATIVE_CALL ScriptFunctionUnsupported(AMX *amx, cell *params)
{
	return 0;
}

// =========================================
// Define a List of native General functions
// =========================================
extern AMX_NATIVE_INFO general_Natives[] = {
	{ "PutSprite", PutSprite },
	{ "GetWidth",  GetWidth },
	{ "GetHeight", GetHeight },
	{ "GetKey", GetKey },
	{ "EnableDayNight", EnableDayNight },
	{ "QKey", QKey },
	{ "WKey", WKey },
	{ "EKey", EKey },
	{ "AKey", AKey },
	{ "SKey", SKey },
	{ "DKey", DKey },
	{ "ZKey", ZKey },
	{ "XKey", XKey },
	{ "CKey", CKey },
	{ "RKey", RKey },
	{ "FKey", FKey },
	{ "VKey", VKey },
	{ "TKey", TKey },
	{ "GKey", GKey },
	{ "EnterKey", EnterKey },
	{ "UpKey", UpKey },
	{ "DownKey", DownKey },
	{ "LeftKey", LeftKey },
	{ "RightKey", RightKey },
	{ "SetQKey", SetQKey },
	{ "SetWKey", SetWKey },
	{ "SetEKey", SetEKey },
	{ "SetAKey", SetAKey },
	{ "SetSKey", SetSKey },
	{ "SetDKey", SetDKey },
	{ "SetZKey", SetZKey },
	{ "SetXKey", SetXKey },
	{ "SetCKey", SetCKey },
	{ "SetEnterKey", SetEnterKey },
	{ "SetUpKey", SetUpKey },
	{ "SetDownKey", SetDownKey },
	{ "SetLeftKey", SetLeftKey },
	{ "SetRightKey", SetRightKey },
	{ "SetWorldX", SetWorldX },
	{ "SetWorldY", SetWorldY },
	{ "GetWorldX", GetWorldX },
	{ "GetWorldY", GetWorldY },
	{ "GetGroupMinX", GetGroupMinX },
	{ "GetGroupMinY", GetGroupMinY },
	{ "GetGroupMaxX", GetGroupMaxX },
	{ "GetGroupMaxY", GetGroupMaxY },
	{ "UpdateWorldCo", UpdateWorldCo },
	{ "InitTextBox", InitTextBox },
	{ "SetTextBoxColor", SetTextBoxColor },
	{ "SetPauseLevel", SetPauseLevel },
	{ "GetPauseLevel", GetPauseLevel },
	{ "NearPoint", NearPoint },
	{ "FirstRun",  FirstRun},
	{ "InitialPlayerX",  InitialPlayerX},
	{ "InitialPlayerY",  InitialPlayerY},
	{ "DrawText",  DrawText},
	{ "DrawImage",  DrawImage},
	{ "ToString",   ToString},
	{ "ToFile",   ToFile},
	{ "CheckMask",   CheckMask},
	{ "CheckForHoleXY",   CheckForHoleXY},
	{ "FadeTo",   FadeTo},
	{ "Wipe",   Wipe},
	{ "GetMinuteCount",   GetMinuteCount},
	{ "GetHourCount",   GetHourCount},
	{ "GetDayCount",   GetDayCount},
	{ "SetMinuteCount",   SetMinuteCount},
	{ "SetHourCount",   SetHourCount},
	{ "SetDayCount",   SetDayCount},
	{ "SetDayLength",   SetDayLength},
	{ "isInside",   isInside},
	{ "SetRestartTable",   SetRestartTable},
	{ "ClearRestartTable",   ClearRestartTable},
	{ "SetRestartPoint",   SetRestartPoint},
	{ "SetGlobal",   SetGlobal},
	{ "GetGlobal",   GetGlobal},
	{ "SetLocal",   SetLocal},
	{ "GetLocal",   GetLocal},
	{ "SetWorldValue",   SetGlobal},
	{ "GetWorldValue",   GetGlobal},
	{ "SetGroupValue",   SetLocal},
	{ "GetGroupValue",   GetLocal},
	{ "PlaySound",  PlaySound},
	{ "TextFile",  Text},
	{ "SetMusicVolume",  SetMusicVolume},
	{ "PlayMusic",  PlayMusic},
	{ "GetMusic", GetMusic},
	{ "SetMusicFadeSpeed",  SetMusicFadeSpeed},
	{ "FinishedReading",  FinishedReading},
	{ "CalculateAngle",  CalculateAngle},
	{ "GetLastAnswer",  GetLastAnswer},
	{ "SetLowerLevel",  SetLowerLevel},
	{ "GetLowerLevelX",  GetLowerLevelX},
	{ "GetLowerLevelY",  GetLowerLevelY},
	{ "EnableControls",  EnableControls},
	{ "DrawRectangle",  DrawRectangle},
	{ "DrawLine",  DrawLine},
	{ "SetTextSize", SetTextSize },
	{ "GameOver", GameOver },
	{ "DrawBitmapText", DrawBitmapText },
	{ "SetSpriteAlpha", SetSpriteAlpha },
	{ "SaveGame", SaveGame },
	{ "LoadGame", LoadGame },
	{ "GetPlayerName", _GetPlayerName },
	{ "SetPlayerName", _SetPlayerName },
	{ "Version", _version },
	{ "SetKey", _SetKey },
	{ "Shift", _Shift },
	{ "LClick", _LClick },
	{ "RClick", _RClick },
	{ "MouseX", _MouseX },
	{ "MouseY", _MouseY },
	{ "SetDayNightColor", _SetDayNightColor },
	{ "strformat", strformat },
	{ "WindowMessage", _WindowMessage },
	{ "PutMask", _PutMask},
	{ "FillMask", _FillMask},
	{ "StopSound", StopSound},
	{ "ToBinary", ScriptFunctionUnsupported},
	{ "ToHex", ScriptFunctionUnsupported},
	{ "EnableUpdates", EnableUpdates},
	{ "Save", _Save },
	{ "Load", _Load },
	{ 0, 0 }
};


//-----------------------------------------------------------------------------
// Name: RegisterNatives()
// Desc:
//
//-----------------------------------------------------------------------------
bool RegisterNatives(AMX amx)
{
	// Register all the native functions
	amx_Register(&amx, socket_Natives, -1);
	amx_Register(&amx, time_Natives, -1);
	amx_Register(&amx, entity_Natives, -1);
	amx_Register(&amx, general_Natives, -1);
	amx_Register(&amx, core_Natives, -1);
	amx_Register(&amx, console_Natives, -1);
	amx_Register(&amx, float_Natives, -1);
	amx_Register(&amx, animation_Natives, -1);
	amx_Register(&amx, counter_Natives, -1);
	return true;
}

