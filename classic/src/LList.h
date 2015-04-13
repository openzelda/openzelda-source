/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef LLIST
#define LLIST

#include <stdlib.h>
#include <cstring>

#include "Hash.h"


//------------------------------------------------------------------------
// Class Node
// 
//------------------------------------------------------------------------
template <class T> 
class Node
{
   // defines a node of the list
   Node*	next;				// pointer to next node
   T*		item;				// node contains a pointer to template parameter T
   char		m_szIdent[ 20 ];	// String Identifier for this Node

public:
   Node(T* newitem);
   ~Node();
   void setnext(Node* newnext);
   Node* getnext();
   T* getitem();
   void SetIdent(char* Ident);
   char* GetIdent(){return m_szIdent;};
   void deleteitem();

};


//------------------------------------------------------------------------
// Class LList
// 
//------------------------------------------------------------------------
template <class T> 
class LList
{
   // defines the linked list of nodes
   Node<T>* start;		// pointer to first node
   Node<T>* end;		// pointer to final node
   Node<T>* current;    // pointer to ‘current’ node
   Node<T>* tempn;		// pointer to a saved position
   CHash*   pHashTable;	// pointer to hash table
   long		lTableSize;	// Hash Table Size
   bool		bHasHashTable;	// Does this list have a hash table attached?

public:
   LList();
   void CreateHash( long size );
   void free(bool bDeleteNodeOnly = false); 
   void release();
   
   void additem(T* newitem, char* szIdent);
   void addtostart(T* newitem, char* szIdent);
   int  isempty(void);
   void tostart(void);
   int  advance(void);
   int  atend();
   void removeitem(T* newitem, bool bDeleteNodeOnly = false);
   void SavePosition();
   void LoadPosition();
   void ReHash( long lSize );

   // Data Broker Functions
   void SetHashSize( long lsize ){ lTableSize = lsize;}

   // Access Functions
   T*	getstart(void);
   T*   getend(void);
   T*   GetObjectFromID(char* Ident);
   T*   GetObjectFromHashTable(char* Ident);
   T*   GetObjectFromNumericID(int wIdent);
   T*   getcurrent(void);
   char* GetCurrentIdent();
   long	GetCount();
};


//-----------------------------------------------------------------------------
// Name: Node Constructor
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
Node<T>::Node(T* newitem)
{
   // constructor for Node
   next=NULL;
   item=newitem;
}

//-----------------------------------------------------------------------------
// Name: Node Destructor
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
Node<T>::~Node()
{
   // destructor
   //delete item;
}

//-----------------------------------------------------------------------------
// Name: getitem()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
T* Node<T>::getitem()
{
   // obtain contents
   return item;
}

//-----------------------------------------------------------------------------
// Name: setnext()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
void Node<T>::setnext(Node<T>* newnext)
{
   // set pointer to another node
   next=newnext;
}

//-----------------------------------------------------------------------------
// Name: getnext()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
Node<T>* Node<T>::getnext()
{
   // obtain location of next node
   return next;
}

//-----------------------------------------------------------------------------
// Name: SetIdent()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
void Node<T>::SetIdent(char* Ident)
{
	// Set the String Identifier for this node
	strncpy(m_szIdent, Ident, sizeof(m_szIdent));
}

//-----------------------------------------------------------------------------
// Name: deleteitem()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
void Node<T>::deleteitem()
{
	delete item;
	item = NULL;
}


//-----------------------------------------------------------------------------
// Name: LList Constructor
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
LList<T>::LList(void)
{
    // constructor
   start			= NULL;
   current			= NULL;
   end				= NULL;
   pHashTable		= NULL;
   bHasHashTable	= false;
}

//-----------------------------------------------------------------------------
// Name: CreateHash()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T>
void LList<T>::CreateHash( long size )
{
	// Create the hash table
   lTableSize = size;
   pHashTable = new CHash( lTableSize );
   bHasHashTable = true;
}

//-----------------------------------------------------------------------------
// Name: LList destructor
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
void LList<T>::free(bool bDeleteNodeOnly)
{
	Node<T>* pObject = NULL;

	if (!isempty())
	{
		// If this list has only 1 element in then delete it
		if (start == end)
		{
			if (!bDeleteNodeOnly)
				start->deleteitem();
			delete(start);

			start=NULL;
			current=NULL;
			end=NULL;
			return;
		}

		tostart();   				
		do
		{
			if (pObject)
			{
				if (!bDeleteNodeOnly)
					pObject->deleteitem();
				delete(pObject);
			}

			pObject = current;
		}
		while(advance());

		// Delete the last object
		if (!bDeleteNodeOnly)
			pObject->deleteitem();
		delete(pObject);
	}

	start=NULL;
    current=NULL;
    end=NULL;

	// Delete the Hash table
	if (pHashTable)
		delete pHashTable;
}

//-----------------------------------------------------------------------------
// Name: LList releaser
// Desc: Unsure if still needed
//
//-----------------------------------------------------------------------------
template <class T> 
void LList<T>::release(void)
{
	Node<T>* pObject = NULL;

	if (!isempty())
	{
		// If this list has only 1 element in then delete it
		if (start == end)
		{
			start->getitem()->Release();
			delete(start);
			return;
		}

		tostart();   				
		do
		{
			if (pObject)
			{
				pObject->getitem()->Release();
				delete(pObject);
			}

			pObject = current;
		}
		while(advance());

		pObject->getitem()->Release();
		delete(pObject);
	}
}

//-----------------------------------------------------------------------------
// Name: GetCount()
// Desc: Returns the Number of elements in the list
//
//-----------------------------------------------------------------------------
template <class T> 
long LList<T>::GetCount(void)
{
	long n = 0;

   // Record the Current Node
   Node<T>* pCurrent = current;

	if (!isempty())
	{
		tostart();   				
		do
		{
			n++;
		}
		while(advance());
	}

	// Put the Current pointer to the start of the list
    current = pCurrent;
	return n;
}

//-----------------------------------------------------------------------------
// Name: additem()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
void LList<T>::additem(T* newitem, char* szIdent)
{
	// add new item to list
	if (start==NULL)
	{
		start=new Node<T>(newitem);
		end=start;
		current=start;
		current->SetIdent(szIdent);
	}
	else
	{
		end->setnext(new Node<T>(newitem));
		end=end->getnext();
		end->SetIdent(szIdent);
	}
	
	// Add The item to the Hash Table
	if ( bHasHashTable )
		pHashTable->AddString(szIdent, (void *)newitem);
}

//-----------------------------------------------------------------------------
// Name: addtostart()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
void LList<T>::addtostart(T* newitem, char* szIdent)
{
	// add new item to the start of the list
	if (start==NULL)
	{
		start=new Node<T>(newitem);
		end=start;
		current=start;
		current->SetIdent(szIdent);
	}
	else
	{
		// Create a new node for this item
		Node<T>* pTemp = new Node<T>(newitem);
		
		// Set the new nodes next node to start
		pTemp->setnext(start);
		pTemp->SetIdent(szIdent);
		
		// Set the new start of the list
		start = pTemp;
	}
	
	// Add The item to the Hash Table
	if ( bHasHashTable )
		pHashTable->AddString(szIdent, (void *)newitem);
}

//-----------------------------------------------------------------------------
// Name: removeitem()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T>
void LList<T>::removeitem(T* newitem, bool bDeleteNodeOnly)
{
	Node<T>* pLastObj = NULL;
	
	// Check the list has somthing in it
	if (isempty())
		return;
	
	// If this list has only 1 element in then delete it and finish
	if (start == end)
	{
		if (start->getitem() == newitem)
		{
			// Remove this Item from The Hash Table
			if ( bHasHashTable )
				pHashTable->RemoveString( start->GetIdent() );

			if (!bDeleteNodeOnly)
				start->deleteitem();
			
			delete(start);
			start = NULL;
		}
	}
	else  // List has more than 1 item
	{
	
		pLastObj = start;
		tostart();
		do
		{
			// check if the current item is the same as the one supplied
			if (current->getitem() == newitem)
			{
				// Check if the object we want to remove is the first in the list
				if ( current == start )
				{
					start = current->getnext();
				}
				else
					// Disconnect the object we are removing from the list
					pLastObj->setnext( current->getnext() );
				
				if (current == end)
				{
					if ( end == start )
						start = NULL;
					
					end = pLastObj;
				}

				// Remove this Item from The Hash Table
				if ( bHasHashTable )
					pHashTable->RemoveString( current->GetIdent() );
				
				// Delete the object
				if (!bDeleteNodeOnly)
					current->deleteitem();
				
				delete( current );
				break;
			}
			pLastObj = current;
		}
		while(advance());
	}
	
	// Put the Current pointer to the start of the list
	current = start;
}

//-----------------------------------------------------------------------------
// Name: isempty()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
int  LList<T>::isempty(void)
{
    // is list empty?
   return start==NULL;
}

//-----------------------------------------------------------------------------
// Name: tostart()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
void LList<T>::tostart(void)
{
   // move ‘current node’ pointer to start of list
   current=start;
}

//-----------------------------------------------------------------------------
// Name: advance()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
int LList<T>::advance(void)
{
   // advance ‘current node’ pointer
   if (current->getnext()!=NULL)
   {
       current=current->getnext();
       return 1;
   }
   else
       return 0;
}


//-----------------------------------------------------------------------------
// Name: atend()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
int LList<T>::atend()
{
   // is ‘current pointer’ at end of list
  return current->getnext()==NULL;
}


//-----------------------------------------------------------------------------
// Name: getcurrent()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
T*   LList<T>::getcurrent(void)
{
    // find position of ‘current pointer’
    if (current==NULL)
       return NULL;
   else
       return current->getitem();
}


//-----------------------------------------------------------------------------
// Name: GetCurrentIdent()
// Desc: Returns the Ident of the current Node
//
//-----------------------------------------------------------------------------
template <class T> 
char*   LList<T>::GetCurrentIdent()
{
   // find position of ‘current pointer’
   if (current==NULL)
       return NULL;
   else
       return current->GetIdent();
}


//-----------------------------------------------------------------------------
// Name: getend()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
T*   LList<T>::getend(void)
{
    // find position of ‘end pointer’
    if (end==NULL)
       return NULL;
   else
       return end->getitem();
}

//-----------------------------------------------------------------------------
// Name: getstart()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
T*   LList<T>::getstart(void)
{
    // find position of ‘end pointer’
    if (start==NULL)
       return NULL;
   else
       return start->getitem();
}

//-----------------------------------------------------------------------------
// Name: GetObjectFromID()
// Desc: Returns an object which has a matching Ident to the one given
//
//-----------------------------------------------------------------------------
template <class T> 
T*   LList<T>::GetObjectFromID(char* Ident)
{
	int wId = 0;
	Node<T>* pCurrentObj = current;
	Node<T>* pTempObj	 = NULL;

	// The String Supplied can either be a Numeric or a String
	// Identifier. We can tell which is which by looking at the
	// ASCII value of the first character, if it's a number then
	// interpret this as a numeric ID.
	// This is why String ID's cant start with a number like '1hold'
	if (Ident[0] > 47 &&  Ident[0] < 58)
	{
		// This is a number, so assume a numeric ID
		// Convert it to an integer
		wId = atoi(Ident);

		return GetObjectFromNumericID(wId);
	}

	// The Ident isnt Numeric so treat it as a string
	// Search through all the Nodes until we find one
	// with a matching Ident

	// If we have a hash table then search it, otherwise
	// search the linked list
	if ( bHasHashTable )
	{
		return GetObjectFromHashTable(Ident);
	}
	else 
	{
		if (!isempty())
		{
			tostart();
			
			do
			{
				if (!strcmp(current->GetIdent(), Ident))
				{
					pTempObj = current;
					current = pCurrentObj;
					return pTempObj->getitem();
				}
			}
			while(advance());
		}

		current = pCurrentObj;
		return NULL;
	}

	return NULL;
}


//-----------------------------------------------------------------------------
// Name: GetObjectFromHashTable()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
T*   LList<T>::GetObjectFromHashTable(char* Ident)
{
	return (T*)pHashTable->LookupString(Ident);

}

//-----------------------------------------------------------------------------
// Name: GetObjectFromNumericID()
// Desc: Returns an object which has a matching index in the list to the one 
//       given.
//
//-----------------------------------------------------------------------------
template <class T> 
T*   LList<T>::GetObjectFromNumericID(int wIdent)
{
	int n = 0;
	Node<T>* pCurrentObj = current;
	Node<T>* pTempObj	 = NULL;

	// Search through all the Nodes until we find one
	// with a matching index
	if (!isempty())
	{
		tostart();
		
		do
		{
			if (n == wIdent)
			{
				pTempObj = current;
				current = pCurrentObj;
				return pTempObj->getitem();
			}

			n++;
		}
		while(advance());
	}

	current = pCurrentObj;
	return NULL;
}


//-----------------------------------------------------------------------------
// Name: ReHash()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
void  LList<T>::ReHash( long lSize )
{
	if (!pHashTable)
		return;

	// Delete the Hash table
	delete pHashTable;

	// Recreate the Hash table and fill it in
	pHashTable = new CHash( lSize );

	if (!isempty())
	{
		tostart();
		do
		{
			pHashTable->AddString(current->GetIdent(), current->getitem());
		}
		while(advance());
	}


}

//-----------------------------------------------------------------------------
// Name: SavePosition()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
void  LList<T>::SavePosition()
{
	tempn = current;
}

//-----------------------------------------------------------------------------
// Name: LoadPosition()
// Desc: 
//
//-----------------------------------------------------------------------------
template <class T> 
void   LList<T>::LoadPosition()
{
	current = tempn;
}

#endif


