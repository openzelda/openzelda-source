/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: SaveLoadFunctions.h
//
// Desc: Fuctions to save and load data
//       
//-----------------------------------------------------------------------------

#include "GeneralData.h"

void SaveTheGame(int Slot);
void SaveData( FILE* sfile, int Slot);
void GetSaveFileName(char *buffer);
bool CreateSaveFile(char *FileName);
void WriteSlotHeader(FILE *sfile, char *Name );
void Writeint(FILE *sfile, char *Name, int val );
void WriteintNoHeader(FILE *sfile, int val );
void Writelong(FILE *sfile, char *Name, long val );
void WritelongNoHeader(FILE *sfile, long val );
void Writefloat(FILE *sfile, char *Name, float val );
void WritefloatNoHeader(FILE *sfile, float val );
void WriteSTRING(FILE *sfile,char *Name, char *val );
void WriteSTRINGNoHeader(FILE *sfile, char *val );
void fputsEx(char *String, FILE *sfile);
bool FindSlotHeader(FILE *sfile, char *Name );
void Readint(FILE *sfile, char *Name, int &val );
void ReadintNoHeader(FILE *sfile, int &val );
void Readlong(FILE *sfile, char *Name, long &val );
void ReadlongNoHeader(FILE *sfile, long &val );
void Readfloat(FILE *sfile, char *Name, float &val );
void ReadfloatNoHeader(FILE *sfile, float &val );
void ReadSTRING(FILE *sfile, char *Name, char *val );
void ReadSTRINGNoHeader(FILE *sfile, char *val );
bool fgetsEx(char *Header, char *String, FILE *sfile, bool fSpanEntities = true);
void WriteSlot( FILE* sfile, FILE* temp );
void LoadData( FILE* sfile, int Slot);
void LoadTheGame(int Slot);
