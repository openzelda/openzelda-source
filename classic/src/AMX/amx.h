/*  Abstract Machine for the Small compiler
 *
 *  Copyright (c) ITB CompuPhase, 1997-2001
 *  This file may be freely used. No warranties of any kind.
 *
 *  Version: $Id: amx.h,v 1.31 2001-11-23 14:45:58+01 thiadmer Exp thiadmer $
 */

#include <stdio.h>
#include <string.h>
#if defined LINUX
  #include "sclinux.h"
#endif
#ifdef WIN32 
#define stricmp strcasecmp 
#define strnicmp strncasecmp 
#endif

#define FLOATPOINT
#ifndef __AMX_H
#define __AMX_H

#include <stdint.h>


#ifdef  __cplusplus
extern  "C" {
#endif

/* calling convention for native functions */
#if !defined AMX_NATIVE_CALL
  #define AMX_NATIVE_CALL
#endif
/* calling convention for all interface functions and callback functions */
#if !defined AMXAPI
  #define AMXAPI
#endif

/* File format version                          Required AMX version
 *   0 (original version)                       0
 *   1 (opcodes JUMP.pri, SWITCH and CASETBL)   1
 *   2 (compressed files)                       2
 *   3 (public variables)                       2
 *   4 (opcodes SWAP.pri/alt and PUSHADDR)      4
 *   5 (tagnames table)                         4
 */
#define MIN_FILE_VERSION  4     /* lowest file format version */
#define CUR_FILE_VERSION  5     /* current AMX version (parallel with file version) */

#if !defined CELL_TYPE
  #define CELL_TYPE
  #if defined(BIT16)
    typedef uint16_t  ucell;    /* only for type casting */
    typedef int16_t   cell;
  #else
    typedef uint32_t  ucell;
    typedef int32_t   cell;
  #endif
#endif

struct __amx;
typedef cell (AMX_NATIVE_CALL *AMX_NATIVE)(struct __amx *amx, cell *params);
typedef int (AMXAPI *AMX_CALLBACK)(struct __amx *amx, cell index,
                                   cell *result, cell *params);
typedef int (AMXAPI *AMX_DEBUG)(struct __amx *amx);
#if !defined FAR
  #define FAR
#endif

#if defined _MSC_VER
  #pragma warning(disable:4103)  /* disable warning message 4103 that complains
                                  * about pragma pack in a header file */
#endif

#if defined LINUX || defined __GNUWIN32__
  #pragma pack(1)         /* structures must be packed (byte-aligned) */
#else
  #pragma pack(push)
  #pragma pack(1)         /* structures must be packed (byte-aligned) */
  #if defined __TURBOC__
    #pragma option -a-    /* "pack" pragma for older Borland compilers */
  #endif
#endif

typedef struct {
  char FAR *name;
  AMX_NATIVE func;
} AMX_NATIVE_INFO;

#define AMX_USERNUM     4
#define sEXPMAX         19
typedef struct {
  uint32_t address;
  char name[sEXPMAX+1];
} AMX_FUNCSTUB;

/* The AMX structure is the internal structure for many functions. Not all
 * fields are valid at all times; many fields are cached in local variables.
 */
typedef struct __amx {
  unsigned char FAR *base;      /* points to the AMX header ("amxhdr") */
  AMX_CALLBACK callback;
  AMX_DEBUG debug;

  int firstrun;			// Is this the first time this script is being run?
  char szFileName[32];  // The actual filename for this script

  /* for external functions a few registers must be accessible from the outside */
  cell cip;                     /* relative to base + amxhdr->cod */
  cell frm;                     /* relative to base + amxhdr->dat */
  cell hea, hlw, stk, stp;      /* all four are relative to base + amxhdr->dat */
  int flags;                    /* current status, see amx_Flags() */
  /* for assertions and debug hook */
  cell curline, curfile;
  int dbgcode;
  cell dbgaddr, dbgparam;
  char FAR *dbgname;
  /* user data */
  long usertags[AMX_USERNUM];
  void FAR *userdata[AMX_USERNUM];
  /* native functions can raise an error */
  int error;
  /* the sleep opcode needs to store the full AMX status */
  cell pri, alt, reset_stk, reset_hea;
  #if defined JIT
    /* support variables for the JIT */
    int reloc_size;     /* required temporary buffer for relocations */
    long code_size;     /* estimated memory footprint of the native code */
  #endif
} AMX;

/* The AMX_HEADER structure is both the memory format as the file format. The
 * structure is used internaly.
 */
typedef struct __amx_header {
  int32_t size;         /* size of the "file" */
  uint16_t magic;       /* signature */
  char    file_version; /* file format version */
  char    amx_version;  /* required version of the AMX */
  int16_t flags;
  int16_t defsize;
  int32_t cod;          /* initial value of COD - code block */
  int32_t dat;          /* initial value of DAT - data block */
  int32_t hea;          /* initial value of HEA - start of the heap */
  int32_t stp;          /* initial value of STP - stack top */
  int32_t cip;          /* initial value of CIP - the instruction pointer */
  int16_t num_publics;  /* number of items in the "public functions" table */
  int32_t publics;      /* offset to the "public functions" table */
  int16_t num_natives;  /* number of items in the "native functions" table */
  int32_t natives;      /* offset to the "native functions" table */
  int16_t num_libraries;/* number of external libraries (dynamically loaded) */
  int32_t libraries;    /* offset to the table of libraries */
  int16_t num_pubvars;  /* number of items in the "public variables" table */
  int32_t pubvars;      /* the "public variables" table */
  int16_t num_tags;     /* number of items in the "public tagnames" table */
  int32_t tags;         /* the "public tagnames" table */
  int16_t reserved;     /* padding */
} AMX_HEADER;
#define AMX_MAGIC       0xf1e0

enum {
  AMX_ERR_NONE,
  /* reserve the first 15 error codes for exit codes of the abstract machine */
  AMX_ERR_EXIT,         /* forced exit */
  AMX_ERR_ASSERT,       /* assertion failed */
  AMX_ERR_STACKERR,     /* stack/heap collision */
  AMX_ERR_BOUNDS,       /* index out of bounds */
  AMX_ERR_MEMACCESS,    /* invalid memory access */
  AMX_ERR_INVINSTR,     /* invalid instruction */
  AMX_ERR_STACKLOW,     /* stack underflow */
  AMX_ERR_HEAPLOW,      /* heap underflow */
  AMX_ERR_CALLBACK,     /* no callback, or invalid callback */
  AMX_ERR_NATIVE,       /* native function failed */
  AMX_ERR_DIVIDE,       /* divide by zero */
  AMX_ERR_SLEEP,        /* go into sleepmode - code can be restarted */

  AMX_ERR_MEMORY = 16,  /* out of memory */
  AMX_ERR_FORMAT,       /* invalid file format */
  AMX_ERR_VERSION,      /* file is for a newer version of the AMX */
  AMX_ERR_NOTFOUND,     /* function not found */
  AMX_ERR_INDEX,        /* invalid index parameter (bad entry point) */
  AMX_ERR_DEBUG,        /* debugger cannot run */
  AMX_ERR_INIT,         /* AMX not initialized (or doubly initialized) */
  AMX_ERR_USERDATA,     /* unable to set user data field (table full) */
  AMX_ERR_INIT_JIT,     /* cannot initialize the JIT */
  AMX_ERR_PARAMS       /* parameter error */
};

enum {
  DBG_INIT,             /* query/initialize */
  DBG_FILE,             /* file number in curfile, filename in name */
  DBG_LINE,             /* line number in curline, file number in curfile */
  DBG_SYMBOL,           /* address in dbgaddr, class/type in dbgparam */
  DBG_CLRSYM,           /* stack address below which locals should be removed. stack address in stk */
  DBG_CALL,             /* function call, address jumped to in dbgaddr */
  DBG_RETURN,           /* function returns */
  DBG_TERMINATE,        /* program ends, code address in dbgaddr, reason in dbgparam */
  DBG_SRANGE           /* symbol size and dimensions (arrays); level in dbgaddr (!); length in dbgparam */
};

#define AMX_FLAG_CHAR16   0x01  /* characters are 16-bit */
#define AMX_FLAG_DEBUG    0x02  /* symbolic info. available */
#define AMX_FLAG_COMPACT  0x04  /* compact encoding */
#define AMX_FLAG_BIGENDIAN 0x08 /* big endian encoding */
#define AMX_FLAG_BROWSE 0x4000
#define AMX_FLAG_RELOC  0x8000  /* jump/call addresses relocated */

#define AMX_EXEC_MAIN   -1      /* start at program entry point */
#define AMX_EXEC_CONT   -2      /* continue from last address */

#define AMX_USERTAG(a,b,c,d)    ((a) | ((b)<<8) | ((long)(c)<<16) | ((long)(d)<<24))

uint16_t * AMXAPI amx_Align16(uint16_t *v);
uint32_t * AMXAPI amx_Align32(uint32_t *v);
int AMXAPI amx_Allot(AMX *amx, int cells, cell *amx_addr, cell **phys_addr);
int AMXAPI amx_Callback(AMX *amx, cell index, cell *result, cell *params);
int AMXAPI amx_Debug(AMX *amx); /* default debug procedure, does nothing */
int AMXAPI amx_Exec(AMX *amx, cell *retval, int index, int numparams, ...);
int AMXAPI amx_Execv(AMX *amx, cell *retval, int index, int numparams, cell params[]);
int AMXAPI amx_FindPublic(AMX *amx, char *funcname, int *index);
int AMXAPI amx_FindPubVar(AMX *amx, char *varname, cell *amx_addr);
int AMXAPI amx_FindTagId(AMX *amx, cell tag_id, char *tagname);
int AMXAPI amx_Flags(AMX *amx,uint16_t *flags);
int AMXAPI amx_GetAddr(AMX *amx,cell amx_addr,cell **phys_addr);
int AMXAPI amx_GetPublic(AMX *amx, int index, char *funcname);
int AMXAPI amx_GetPubVar(AMX *amx, int index, char *varname, cell *amx_addr);
int AMXAPI amx_GetString(char *dest,cell *source);
int AMXAPI amx_GetTag(AMX *amx, int index, char *tagname, cell *tag_id);
int AMXAPI amx_GetUserData(AMX *amx, long tag, void **ptr);
int AMXAPI amx_Init(AMX *amx, void *program);
int AMXAPI amx_InitJIT(AMX *amx, void *reloc_table, void *native_code);
int AMXAPI amx_NameLength(AMX *amx, int *length);
AMX_NATIVE_INFO * AMXAPI amx_NativeInfo(char *name,AMX_NATIVE func);
int AMXAPI amx_NumPublics(AMX *amx, int *number);
int AMXAPI amx_NumPubVars(AMX *amx, int *number);
int AMXAPI amx_NumTags(AMX *amx, int *number);
int AMXAPI amx_RaiseError(AMX *amx, int error);
int AMXAPI amx_Register(AMX *amx, AMX_NATIVE_INFO *list, int number);
int AMXAPI amx_Release(AMX *amx, cell amx_addr);
int AMXAPI amx_SetCallback(AMX *amx, AMX_CALLBACK callback);
int AMXAPI amx_SetDebugHook(AMX *amx, AMX_DEBUG debug);
int AMXAPI amx_SetString(cell *dest, char *source, int pack);
int AMXAPI amx_SetUserData(AMX *amx, long tag, void *ptr);
int AMXAPI amx_StrLen(cell *cstring, int *length);
int AMXAPI amx_SaveToFile(AMX *amx, FILE* pOut);

#if defined LINUX || defined __GNUWIN32__
  #pragma pack()    /* reset default packing */
#else
  #pragma pack(pop) /* reset previous packing */
#endif

#ifdef  __cplusplus
}
#endif

#endif /* __AMX_H */
