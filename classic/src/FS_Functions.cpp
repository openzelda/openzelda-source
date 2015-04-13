/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
/* File-System Functions */

#include "FS_Functions.h"
#include "string.h"
#include <SDL2/SDL.h>

/* Create a Global String to store the base directory for the program */

char * g_szTempDir = NULL;
char * g_szSaveDir = NULL;
/*
============
FS_Gamedir

Called to find out the base Directory
============
*/
char * FS_Gamedir()
{
	if ( !g_szTempDir )
	{
		g_szTempDir = SDL_GetPrefPath("classic.openzelda.net", "temp");

	}

	return g_szTempDir;
}

char * FS_Savedir()
{
	if ( !g_szSaveDir )
	{
		g_szSaveDir = SDL_GetPrefPath("classic.openzelda.net", "saves");
	}

	return g_szSaveDir;
}

/*
============
FS_SetGamedir
============
*/
int FS_SetGamedir( char* szGameDir )
{

	return 1;
}
