/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: SaveLoadFunctions.cpp
//
// Desc: Fuctions to save and load data
//       
//-----------------------------------------------------------------------------
#define STRICT
#include "CGame.h"
#include "SaveLoadFunctions.h"

extern CGame*	g_pGame;


//-----------------------------------------------------------------------------
// Name: LoadTheGame()
// Desc: Loads all game Data from a specific slot
//
//-----------------------------------------------------------------------------
void LoadTheGame(int Slot)
{
	int n = 0;
	char FileName[512];

	// Get the Filename of the save game
	GetSaveFileName(FileName);

	//======================
	// Open The Save File
	//======================
	FILE *sfile;
	char CurrentLine[32];

	// Attempt to open save file
	std::string path(FS_Savedir());
	path.append(FileName);
	// Attempt to open the save file
	if ( !(sfile = fopen(path.c_str(), "r+")))
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Couldn't open Save Game File!");
		return;
	}

	
	//=====================
	// Begin Loading Data
	//=====================
	// Each save file is divided into 4 slots, each begins with a [SLOT] line.
	// We will loop through until we come to a matching [SLOT] section and then
	// Load the Data within

	// Loop until we find our slot
	for (;;)
	{
		if (!fgets(CurrentLine, sizeof(CurrentLine), sfile))
			break;

		if (strstr(CurrentLine, "[SLOT]"))
		{
			// We found a slot now get it's number
			fgets(CurrentLine, sizeof(CurrentLine), sfile);

			if ( atoi(CurrentLine) == Slot)
			{
				LoadData( sfile, Slot);
				break;
			}
		}
	}
	fclose( sfile );
}


//-----------------------------------------------------------------------------
// Name: SaveTheGame()
// Desc: Saves all game Data to a specific Slot
//
//-----------------------------------------------------------------------------
void SaveTheGame(int Slot)
{
	int n=0;
	char FileName[512];

	// Get the FileName of the Saved Game for this Quest
	GetSaveFileName(FileName);

	//================================
	// Open The Save File + Check it
	//================================
	FILE *sfile;
	FILE *tempFile;
	char CurrentLine[ENT_STRING_MAX];

	// Open the Save file
	std::string path(FS_Savedir());
	path.append(FileName);
	// Attempt to open the save file
	if ( !(sfile = fopen(path.c_str(), "r")))
	{
		if (!CreateSaveFile(FileName))
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Couldn't open Save Game File!");
			return;
		}

		// Try opening the file again
		if ( !(sfile = fopen(FileName, "w+")))
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Couldn't open Save Game File!");
			return;
		}
	}

	path = FS_Savedir();
	path.append("temp");
	// Attempt to open the save file
	if ( !(tempFile = fopen(path.c_str(), "w+")))
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Couldn't open Temporary Save Game File!");
		return;
	}

	 //  Check that this file has the correct first line - otherwise we need to
	 //  Create a new file 
	fgets(CurrentLine, sizeof(CurrentLine), sfile);

	if (!strstr(CurrentLine, "[SLOT]"))
	{
		// This is a blank file, or not correctly created
		CreateSaveFile(FileName);
	}

	// Go back to the beginning of the file
	rewind(sfile);
	
	//=====================
	// Begin Saving Data
	//=====================
	// Each save file is divided into 4 slots, each begins with a [SLOT] line.
	// When we save we need to copy all of the slots up to our save slot to a
	// temporary file, Then copy all the slots after our save slot to the temp file.
	// Then we can erase the orignal file, write the save game and the other slots back.

	// Loop until we find our slot
	for (;;)
	{
		// Break if we hit EOF - shouldn't happen
		if (!fgets(CurrentLine, sizeof(CurrentLine), sfile))
			break;

		if (strstr(CurrentLine, "[SLOT]"))
		{
			// We found a slot now get it's number
			fgets(CurrentLine, sizeof(CurrentLine), sfile);

			if ( atoi(CurrentLine) == Slot)
			{
				// We have found the correct slot.
			}
			else
			{
				// This is not the correct slot, but we have to copy it's contents to
				// a temporary file to prevent it from being overwritten.
				fputs("[SLOT]\n", tempFile);
				fputs(CurrentLine, tempFile);

				// Now loop util we hit another [SLOT] recording this info
				// Into the Temp file
				for (;;)
				{
					if (!fgets(CurrentLine, sizeof(CurrentLine), sfile))
						break;

					if (strstr(CurrentLine, "[ENDSLOT]"))
					{
						fputs(CurrentLine, tempFile);
						break;
					}
					else
					{
						// record this line into the savefile
						fputs(CurrentLine, tempFile);
					}
				}
			}
		}
	}

	//=======================
	// Writing the new file
	//=======================
	// By this point we will have all the unused slots backup up to a temp file
	// and the slot we want will be left in the original file. so now we can wipe
	// the orginal file and start writing the data back.

	path = FS_Savedir();
	path.append(FileName);
	if ( !(sfile = fopen(path.c_str(), "w+")))
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Couldn't open Save Game File!");
		return;
	}

	rewind(tempFile);

	// Write all the Slots before our slot
	for (n = 0; n < Slot; n++)
		 WriteSlot( sfile, tempFile );

	SaveData( sfile, Slot);

	// Write all the Data after our slot
	for (n = Slot; n < 4; n++)
		 WriteSlot( sfile, tempFile );

	fclose( sfile );
	fclose( tempFile );
}


//-----------------------------------------------------------------------------
// Name: WriteSlot()
// Desc: 
//
//-----------------------------------------------------------------------------
void WriteSlot( FILE* sfile, FILE* temp )
{
	char CurrentLine[ENT_STRING_MAX];

	// We want to write a complete slot from the [SLOT] to the next [ENDSLOT]
	for (;;)
	{
		if (!fgets(CurrentLine, sizeof(CurrentLine), temp))
			break;

		// We found the start
		if (strstr(CurrentLine, "[SLOT]"))
		{
			fputs(CurrentLine, sfile);

			for (;;)
			{
				if (!fgets(CurrentLine, sizeof(CurrentLine), temp))
					break;

				if (strstr(CurrentLine, "[ENDSLOT]"))
				{
					// This is the end so return.
					fputs(CurrentLine, sfile);
					return;
				}
				else
					fputs(CurrentLine, sfile);
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Name: SaveData()
// Desc: Saves all Game Data to a file
//
//-----------------------------------------------------------------------------
void SaveData( FILE* sfile, int Slot)
{
	char String[50];
	sprintf(String, "%d\n", Slot);

	// Now we just need to save all of the games data
	// to the file and position supplied
	fputs("[SLOT]\n", sfile);
	fputs(String, sfile);

	//=====================================
	// Insert any Data you want saved here
	//=====================================
	g_pGame->Save(sfile);

	fputs("[ENDSLOT]\n", sfile);
}


//-----------------------------------------------------------------------------
// Name: LoadData()
// Desc: Loads all Game Data from a file
//
//-----------------------------------------------------------------------------
void LoadData( FILE* sfile, int Slot)
{
	// We are pointing to the correct slot int the game to load from
	// now we just need to load each object's data

	///////////////////////////////////////
	// Insert any Data you want Loaded here
	///////////////////////////////////////
	g_pGame->Load(sfile);
}


//-----------------------------------------------------------------------------
// Name: GetSaveFileName()
// Desc: 
//
//-----------------------------------------------------------------------------
#include <string>
void GetSaveFileName(char *buffer)
{	
	snprintf(buffer, 254, "%s/%s.ozs", FS_Savedir(), g_pGame->GetQuestName());

	/*
	int n;
	int c = 0;
	char fileName[512];
	char Temp[512];
	char *Temp2;

	//===================================
	// Get the FileName of the Save file
	//==================================
	// We need to get the Filename on its own with no extension
	
	memset(fileName,0,sizeof(fileName));
	memset(Temp,0,sizeof(Temp));

	// Remove the file extension
	strncpy(fileName, g_pGame->GetQuestFileName(), 
		    strlen(g_pGame->GetQuestFileName()) - 3);

	// Go through the String backwards and copy every char until we come to a backslash
	for (n = strlen(fileName) - 1; n >= 0; n--)
	{
		Temp[c] = fileName[n];
		
		if (fileName[n] == '\\' || fileName[n] == '/')
			break;
		c++;
	}
	
	// Reverse the String
	//Temp2 = strrev(Temp);
	
	memset(fileName,0,sizeof(fileName));
	strcpy(fileName, "savedata");
	strcat(fileName, Temp2);
	strcat(fileName, "ozs");

	// We now have the correct filename, copy it into buffer
	strcpy(buffer, fileName);
	*/
	
	
}


//-----------------------------------------------------------------------------
// Name: CreateSaveFile()
// Desc: Creates a new formatted save file
//
//-----------------------------------------------------------------------------
bool CreateSaveFile(char *FileName)
{
	int n;
	FILE *newfile;
	char temp[50];

	// Create a new file
	std::string path(FS_Savedir());
	path.append(FileName);
	if ( !(newfile = fopen(path.c_str(), "w+")))
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n","Couldn't Create Save Game File!");
		return false;
	}

	// Now format it with [SLOT] info
	for (n = 0; n < 4; n++)
	{
		sprintf(temp, "%d\n", n);
		fputs( "[SLOT]\n", newfile);
		fputs( temp, newfile);
		fputs( "\n", newfile);
		fputs( "[ENDSLOT]\n", newfile);
	}

	fclose( newfile );
	return true;
}


//-----------------------------------------------------------------------------
// Name: WriteSlotHeader()
// Desc: 
//
//-----------------------------------------------------------------------------
void WriteSlotHeader(FILE *sfile, char *Name )
{
	fputs(Name, sfile);
	fputs("\n", sfile);
}


//-----------------------------------------------------------------------------
// Name: Writeint()
// Desc:
// 
//-----------------------------------------------------------------------------
void Writeint(FILE *sfile, char *Name, int val )
{
	char temp[50];
	sprintf(temp, "%d", val);

	fputsEx(Name, sfile);
	fputs("\n", sfile);
	fputsEx(temp, sfile);
	fputs("\n", sfile);
}


//-----------------------------------------------------------------------------
// Name: WriteintNoHeader()
// Desc:
// 
//-----------------------------------------------------------------------------
void WriteintNoHeader(FILE *sfile, int val )
{
	char temp[50];
	sprintf(temp, "%d", val);

	fputsEx(temp, sfile);
	fputs("\n", sfile);
}

//-----------------------------------------------------------------------------
// Name: Writelong()
// Desc:
// 
//-----------------------------------------------------------------------------
void Writelong(FILE *sfile, char *Name, long val )
{
	char temp[50];
	sprintf(temp, "%d", val);

	fputsEx(Name, sfile);
	fputs("\n", sfile);
	fputsEx(temp, sfile);
	fputs("\n", sfile);
}


//-----------------------------------------------------------------------------
// Name: WritelongNoHeader()
// Desc:
// 
//-----------------------------------------------------------------------------
void WritelongNoHeader(FILE *sfile, long val )
{
	char temp[50];
	sprintf(temp, "%d", val);

	fputsEx(temp, sfile);
	fputs("\n", sfile);
}


//-----------------------------------------------------------------------------
// Name: Writefloat()
// Desc: 
//
//-----------------------------------------------------------------------------
void Writefloat(FILE *sfile, char *Name, float val )
{
	char temp[50];
	sprintf(temp, "%f", val);

	fputsEx(Name, sfile);
	fputs("\n", sfile);
	fputsEx(temp, sfile);
	fputs("\n", sfile);
}

//-----------------------------------------------------------------------------
// Name: WritefloatNoHeader()
// Desc: 
//
//-----------------------------------------------------------------------------
void WritefloatNoHeader(FILE *sfile, float val )
{
	char temp[50];
	sprintf(temp, "%f", val);

	fputsEx(temp, sfile);
	fputs("\n", sfile);
}

//-----------------------------------------------------------------------------
// Name: WriteSTRING()
// Desc: 
//
//-----------------------------------------------------------------------------
void WriteSTRING(FILE *sfile,char *Name, char *val )
{
	fputsEx(Name, sfile);
	fputs("\n", sfile);
	fputsEx(val, sfile);
	fputs("\n", sfile);
}

//-----------------------------------------------------------------------------
// Name: WriteSTRINGNoHeader()
// Desc: 
//
//-----------------------------------------------------------------------------
void WriteSTRINGNoHeader(FILE *sfile, char *val )
{
	fputsEx(val, sfile);
	fputs("\n", sfile);
}

//-----------------------------------------------------------------------------
// Name: fputsEx()
// Desc: fputs an encrypted String
//
//-----------------------------------------------------------------------------
void fputsEx(char *String, FILE *sfile)
{
	char string[512];
	strcpy(string, String);

	// Encrypt the String
	g_pGame->EncryptString(string);

	// Put the encrypted string in the file
	fputs(string, sfile);
}


//-----------------------------------------------------------------------------
// Name: FindSlotHeader()
// Desc: 
//
//-----------------------------------------------------------------------------
bool FindSlotHeader(FILE *sfile, char *Name )
{
	char CurrentLine[50];

	// We need to loop through until we match the specified section.
	for (;;)
	{
		if (!fgets(CurrentLine, sizeof(CurrentLine), sfile))
			return false;

		if (strstr(CurrentLine, Name))
			return true;
	}
}


//-----------------------------------------------------------------------------
// Name: Readint()
// Desc: Supply the variable name to load
//
//-----------------------------------------------------------------------------
void Readint(FILE *sfile, char *Name, int &val )
{
	char temp[50];

	if (fgetsEx(Name, temp, sfile, false))
		val = atoi(temp);
}

//-----------------------------------------------------------------------------
// Name: ReadintNoHeader()
// Desc:
// 
//-----------------------------------------------------------------------------
void ReadintNoHeader(FILE *sfile, int &val )
{
	char temp[50];

	if (fgetsEx(NULL, temp, sfile, false))
		val = atoi(temp);
}

//-----------------------------------------------------------------------------
// Name: Readlong()
// Desc: Supply the variable name to load
//
//-----------------------------------------------------------------------------
void Readlong(FILE *sfile, char *Name, long &val )
{
	char temp[50];

	if (fgetsEx(Name, temp, sfile, false))
		val = atol(temp);
}

//-----------------------------------------------------------------------------
// Name: ReadlongNoHeader()
// Desc:
// 
//-----------------------------------------------------------------------------
void ReadlongNoHeader(FILE *sfile, long &val )
{
	char temp[50];

	if (fgetsEx(NULL, temp, sfile, false))
		val = atol(temp);
}

//-----------------------------------------------------------------------------
// Name: Readfloat()
// Desc: 
//
//-----------------------------------------------------------------------------
void Readfloat(FILE *sfile, char *Name, float &val )
{
	char temp[50];

	if (fgetsEx(Name, temp, sfile, false))
		val = atof(temp);
}

//-----------------------------------------------------------------------------
// Name: ReadfloatNoHeader()
// Desc: 
//
//-----------------------------------------------------------------------------
void ReadfloatNoHeader(FILE *sfile, float &val )
{
	char temp[50];

	if (fgetsEx(NULL, temp, sfile, false))
		val = atof(temp);
}


//-----------------------------------------------------------------------------
// Name: ReadSTRING()
// Desc: 
//
//-----------------------------------------------------------------------------
void ReadSTRING(FILE *sfile, char *Name, char *val )
{
	char temp[ENT_STRING_MAX];

	if (fgetsEx(Name, temp, sfile, false))
	{
		// Clear the Null Byte from the end of temp
		if (temp[strlen(temp)-1] == '\n')
				temp[strlen(temp)-1] = 0;

		strcpy(val, temp);
	}
}

//-----------------------------------------------------------------------------
// Name: ReadSTRINGNoHeader()
// Desc: 
//
//-----------------------------------------------------------------------------
void ReadSTRINGNoHeader(FILE *sfile, char *val )
{
	char temp[ENT_STRING_MAX];

	if (fgetsEx(NULL, temp, sfile, false))
	{
		// Clear the Null Byte from the end of temp
		if (temp[strlen(temp)-1] == '\n')
				temp[strlen(temp)-1] = 0;

		strcpy(val, temp);
	}
}

//-----------------------------------------------------------------------------
// Name: fgetsEx()
// Desc: Searches through until it hits the Header string and then returns
//       the next line.
//
//-----------------------------------------------------------------------------
bool fgetsEx(char *Header, char *String, FILE *sfile, bool fSpanEntities)
{
	long lBackup = ftell(sfile);
	char string[512];

	// Clear the supplied buffer
	memset(string,0,sizeof(string));

	// If we have a header search for it, otherwise get next line
	if (Header)
	{
	    // Loop until we hit either an entry starting with [ or the Header String
		for (;;)
		{
			// Read the Next line
			if (!fgets(string, sizeof(string), sfile))
				return false;   // This shouldn't happen
			
			/* If the first character is a '[' then we have probably
			hit the start of a new section, shouldnt really happen. */
			if (strstr(string, "[ENDSLOT]"))
			{
				fseek(sfile, lBackup, SEEK_SET);
				return false;
			}
				

			// If this serach has to remain within one entity then enforce it here
			if (!fSpanEntities)
			{
				if (strstr(string, "[ENTITY]"))
				{
					fseek(sfile, lBackup, SEEK_SET);
					return false;
				}
			}
		
			// Decrypt the string
			g_pGame->EncryptString(string);

			// Remove Last Char
			string[strlen(string)-1] = 0;

			// The the string matches the header then get the next line
			if (!strcmp(string, Header))
				break;
		}
	}

	// Get the next line
	if (!fgets(string, sizeof(string), sfile))
		return false;

	// Decrypt the string
	g_pGame->EncryptString(string);

	// Remove Last Char
	string[strlen(string)-1] = 0;
	
	// Copy this decrypted string to the supplied buffer
	strcpy(String, string);
	return true;
}

