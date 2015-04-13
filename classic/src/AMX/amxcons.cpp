/* Since some of these routines go further than those of standard C,
 * they cannot always be implemented with portable C functions. In
 * other words, these routines must be ported to other environments.
 *
 *  Copyright (c) ITB CompuPhase, 1997-2001
 *  This file may be freely used. No warranties of any kind.
 *
 *  Version: $Id: Amxcons.c,v 1.22 2001-07-22 16:04:27+02 thiadmer Exp thiadmer $
 */
#if defined __WIN32__ || defined _WIN32 || defined __MSDOS__
  #include <conio.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "amx.h"


#if defined __MSDOS__
  #define EOL_CHAR       '\r'
#endif
#if defined __WIN32__ || defined _WIN32
  #define EOL_CHAR       '\r'
#endif
#if !defined EOL_CHAR
  /* if not a "known" operating system, assume Unix */
  #define EOL_CHAR     '\n'
#endif

#if defined NO_DEF_FUNCS
  int amx_printf(char *,...);
  int amx_putchar(int);
  int amx_fflush(void *);
  int amx_getch(void);
  char *amx_gets(char *,int);
#else
  #define amx_printf    printf
  #define amx_putchar   putchar
  #define amx_fflush    fflush
#endif

static int printstring(AMX *amx,cell *cstr,cell *params,int num);

static int dochar(AMX *amx,char ch,cell param)
{
  cell *cptr;

  switch (ch) {
  case '%':
    amx_putchar(ch);
    return 0;
  case 'c':
    amx_GetAddr(amx,param,&cptr);
    amx_putchar((int)*cptr);
    return 1;
  case 'd':
    amx_GetAddr(amx,param,&cptr);
    amx_printf("%ld",(long)*cptr);
    return 1;
#if defined FLOATPOINT
  case 'f': /* 32-bit floating point number */
    amx_GetAddr(amx,param,&cptr);
    amx_printf("%f",*(float*)cptr);
    return 1;
#endif
#if defined FIXEDPOINT
  #define FIXEDMULT 1000
  case 'r': /* 32-bit fixed point number */
    amx_GetAddr(amx,param,&cptr);
    amx_printf("%ld.%03d",(long)(*cptr/FIXEDMULT), (int)((*cptr%FIXEDMULT+FIXEDMULT)%FIXEDMULT));
    return 1;
#endif
  case 's':
    amx_GetAddr(amx,param,&cptr);
    printstring(amx,cptr,NULL,0);
    return 1;
  } /* switch */
  /* error in the string format, try to repair */
  amx_putchar(ch);
  return 0;
}

static int printstring(AMX *amx,cell *cstr,cell *params,int num)
{
  int i;
  int informat=0,paramidx=0;

  /* check whether this is a packed string */
  if (cstr[0] >= (1L<<8*sizeof(char))) {
    int j=sizeof(cell)-sizeof(char);
    char c;
    /* the string is packed */
    i=0;
    for ( ; ; ) {
      c=(char)((ucell)cstr[i] >> 8*j);
      if (c==0)
        break;
      if (informat) {
        assert(params!=NULL);
        paramidx+=dochar(amx,c,params[paramidx]);
        informat=0;
      } else if (params!=NULL && c=='%') {
        informat=1;
      } else {
        amx_putchar(c);
      } /* if */
      if (j==0)
        i++;
      j=(j+sizeof(cell)-sizeof(char)) % sizeof(cell);
    } /* for */
  } else {
    /* the string is unpacked */
    for (i=0; cstr[i]!=0; i++) {
      if (informat) {
        assert(params!=NULL);
        paramidx+=dochar(amx,(char)cstr[i],params[paramidx]);
        informat=0;
      } else if (params!=NULL && (int)cstr[i]=='%') {
        if (paramidx<num)
          informat=1;
        else
          amx_RaiseError(amx, AMX_ERR_NATIVE);
      } else {
        amx_putchar((int)cstr[i]);
      } /* if */
    } /* for */
  } /* if */
  return paramidx;
}

static cell AMX_NATIVE_CALL _print(AMX *amx, cell *params)
{
  cell *cstr;

  /* do the colour codes with ANSI strings */
  if (params[2]>=0)
    amx_printf("\x1b[%dm",(int)params[2]+30);
  if (params[3]>=0)
    amx_printf("\x1b[%dm",(int)params[3]+40);

  amx_GetAddr(amx,params[1],&cstr);
  printstring(amx,cstr,NULL,0);

  /* reset the colours */
  if (params[2]>=0 || params[3]>=0)
    amx_printf("\x1b[37;40m");

  amx_fflush(stdout);
  return 0;
}

static cell AMX_NATIVE_CALL _printf(AMX *amx,cell *params)
{
  cell *cstr;

  amx_GetAddr(amx,params[1],&cstr);
  printstring(amx,cstr,params+2,(int)(params[0]/sizeof(cell))-1);
  amx_fflush(stdout);
  return 0;
}



extern AMX_NATIVE_INFO console_Natives[] = {
/*
  { "getchar",   _getchar },
  { "getstring", _getstring },
  { "getvalue",  _getvalue },
  */
  { "print",     _print },
  { "printf",    _printf },

  { NULL, NULL }        // terminator
};
