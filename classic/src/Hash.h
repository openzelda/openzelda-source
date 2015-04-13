/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//////////////////////////////////////////////////////////////////////////
// File: DS_Hash.h
//
// Desc: Implementation of a hash table
//
// Date: 9th October 2001
//
// Author: Greg Denness
//
// Notes:  Referenced from:
//		   http://www.sparknotes.com/cs/searching/hashtables/
//
//////////////////////////////////////////////////////////////////////////

#ifndef CHASH_H
#define CHASH_H


/////////////////////////
// Linked List Node - 'Bucket'
//
typedef struct _list_t_ {
	char *szString;
	void *pData;
	struct _list_t_ *pNext;
} list_t;

#include <map>


/////////////////////////
// CHash Class
//
class CHash
{
public:
	CHash(long lSize);
	~CHash();

	bool RemoveString(char *szStr);
	int AddString(char *szStr, void* pData);// Add a string to the table
	void* LookupString(char *szStr);		// Check if a string exists and return it's 'bucket'


private:
	std::map<unsigned int, void*> table;
	
	unsigned int Hash(char *szStr);			// The Hash function
};



#endif


