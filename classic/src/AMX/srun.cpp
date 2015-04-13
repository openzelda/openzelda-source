/*  Command-line shell for the "Small" Abstract Machine, using the
 *  Microsoft Windows DLL interface.
 *
 *  Copyright (c) ITB CompuPhase, 1997-2001
 *  Copyright (c) Mark Peter, 1998-1999
 *
 *  This file may be freely used. No warranties of any kind.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "amx.h"


extern AMX_NATIVE_INFO core_Natives[];
extern AMX_NATIVE_INFO console_Natives[];

// *********************** NATIVE FUNCTION *****************
cell AMX_NATIVE_CALL print2(AMX *amx, cell *params);
static cell print2(AMX *amx, cell *params)
{
	char szString[512];
	cell div;

	cell *t = 0;
	amx_GetAddr(amx, params[1], &t);
	amx_GetString(szString, t);

	cell q = params[2];

	printf(szString);
	return 45;
}

AMX_NATIVE_INFO other_Natives[] = {
	{ "print2", print2 },
	{ 0, 0 }       /* terminator */
};
// *********************************************************

void *loadprogram(AMX *amx,char *filename)
{
	FILE *fp;
	AMX_HEADER hdr;
	void *program = NULL;
	
	if ((fp = fopen("..\\..\\hello.amx","rb")) != NULL) {
		fread(&hdr, sizeof hdr, 1, fp);
		if ((program = malloc((int)hdr.stp)) != NULL) {
			rewind(fp);
			fread(program, 1, (int)hdr.size, fp);
			fclose(fp);
			memset(amx,0,sizeof *amx);
			if (amx_Init(amx,program) == AMX_ERR_NONE)
				return program;
			free(program);
		} /* if */
	} /* if */
	return NULL;
}


class CEntity
{
public:
	CEntity(char* szFileName);
	CEntity();
	~CEntity();
	void RunScript();
	AMX amx;
	void *program;
};


CEntity::CEntity()
{
}

CEntity::CEntity(char* szFileName)
{
	program = loadprogram(&amx, szFileName);
	amx_Register(&amx, other_Natives, -1);
	//amx_Register(&amx, core_Natives, -1);
	//amx_Register(&amx, console_Natives, -1);
}

void CEntity::RunScript()
{
	cell ret;
	amx_Exec(&amx, &ret, AMX_EXEC_MAIN, 0);
}

CEntity::~CEntity()
{
	free(program);
}



int main(int argc,char *argv[])
{	
	CEntity* pEnt1 = new CEntity("..\\..\\hello.amx");
	pEnt1->RunScript();
	delete(pEnt1);
	
	return 0;
}