/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Hash.h"
#include <iostream>
#include <SDL2/SDL.h>
//------------------------------------------------------------------------
// Name: CHash()
// Desc: CHash Constructor
//
//------------------------------------------------------------------------
CHash::CHash(long lSize)
{

}


//------------------------------------------------------------------------
// Name: ~CHash()
// Desc: CHash Destructor
//
//------------------------------------------------------------------------
CHash::~CHash()
{
	table.clear();
}


//------------------------------------------------------------------------
// Name: AddString()
// Desc: Adds a string to the Hash Table
//
//------------------------------------------------------------------------
int CHash::AddString(char *szStr, void* pData)
{
	if ( szStr == NULL || szStr[0] == '\0' )
		return 2;
	// Retrieve the hash value for the string
	unsigned int uwHashVal = Hash(szStr);

	if ( table.find(uwHashVal) == table.end() )
	{
		table.insert( std::pair<unsigned int,void*>(uwHashVal,pData) );
		return 0;
	}
	else
	{
		SDL_Log("Failed to add '%s' to Hash Table.", szStr);

		return 2;
	}

}

//------------------------------------------------------------------------
// Name: RemoveString()
// Desc: Removes a string from the Hash Table
//
//------------------------------------------------------------------------
bool CHash::RemoveString(char *szStr)
{

	// Retrieve the hash value for the string
	unsigned int uwHashVal = Hash(szStr);
	std::map<unsigned int,void*>::iterator it = table.find(uwHashVal);


	if ( it == table.end() )
	{
		return false;
	}
	table.erase( it );
	return true;
}


//------------------------------------------------------------------------
// Name: LookupString()
// Desc: Check if a string exists and return it's 'bucket'
//
//------------------------------------------------------------------------
void* CHash::LookupString(char *szStr)
{
	// Retrieve the hash value for the string
	unsigned int uwHashVal = Hash(szStr);

	std::map<unsigned int,void*>::iterator it = table.find(uwHashVal);

	if ( it != table.end() )
	{
		return it->second;
	}
	//SDL_Log("Failed to find '%s' in Hash Table.", szStr);

	return NULL;
}


//------------------------------------------------------------------------
// Name: Hash()
// Desc: Hash Function - retrieves a hash value from a given string
//
//------------------------------------------------------------------------
unsigned int CHash::Hash(char *szStr)
{
	unsigned int uwHashVal = 0;



	/* for each character, we multiply the old hash by 31 and add the current
	 * character.  Remember that shifting a number left is equivalent to
	 * multiplying it by 2 raised to the number of places shifted.  So we
	 * are in effect multiplying hashval by 32 and then subtracting hashval.
	 * Why do we do this?  Because shifting and subtraction are much more
	 * efficient operations than multiplication.
	 */
	for(; *szStr != '\0'; szStr++) uwHashVal = *szStr + (uwHashVal << 5) - uwHashVal;

	/* we then return the hash value mod the hashtable size so that it will
	 * fit into the necessary range
	 */
	return uwHashVal;
}

