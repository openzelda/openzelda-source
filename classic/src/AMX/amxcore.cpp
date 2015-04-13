/*  Core module for the Small AMX
 *
 *  Copyright (c) ITB CompuPhase, 1997-2001
 *  This file may be freely used. No warranties of any kind.
 *
 *  Version: $Id: Amxcore.c,v 1.18 2001-11-14 11:47:47+01 thiadmer Exp thiadmer $
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include "amx.h"
#include "amxcore.h"
#define NOPROPLIST 1

#define CHARBITS        (8*sizeof(char))
typedef unsigned char   uchar;

#if !defined NOPROPLIST
typedef struct _property_list {
  struct _property_list *next;
  cell id;
  char *name;
  cell value;
} proplist;

static proplist proproot = { NULL };

static proplist *list_additem(proplist *root)
{
  proplist *item;

  assert(root!=NULL);
  if ((item=(proplist *)malloc(sizeof(proplist)))==NULL)
    return NULL;
  item->name=NULL;
  item->id=0;
  item->value=0;
  item->next=root->next;
  root->next=item;
  return item;
}
static void list_delete(proplist *pred,proplist *item)
{
  assert(pred!=NULL);
  assert(item!=NULL);
  pred->next=item->next;
  assert(item->name!=NULL);
  free(item->name);
  free(item);
}
static void list_setitem(proplist *item,cell id,char *name,cell value)
{
  char *ptr;

  assert(item!=NULL);
  if ((ptr=(char *)malloc(strlen(name)+1))==NULL)
    return;
  if (item->name!=NULL)
    free(item->name);
  strcpy(ptr,name);
  item->name=ptr;
  item->id=id;
  item->value=value;
}
static proplist *list_finditem(proplist *root,cell id,char *name,cell value,
                               proplist **pred)
{
  proplist *item=root->next;
  proplist *prev=root;

  /* check whether to find by name or by value */
  assert(name!=NULL);
  if (strlen(name)>0) {
    /* find by name */
    while (item!=NULL && (item->id!=id || stricmp(item->name,name)!=0)) {
      prev=item;
      item=item->next;
    } /* while */
  } else {
    /* find by value */
    while (item!=NULL && (item->id!=id || item->value!=value)) {
      prev=item;
      item=item->next;
    } /* while */
  } /* if */
  if (pred!=NULL)
    *pred=prev;
  return item;
}
#endif

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL numargs(AMX *amx, cell *params)
{
  AMX_HEADER *hdr;
  uchar *data;
  cell bytes;

  hdr=(AMX_HEADER *)amx->base;
  data=amx->base+(int)hdr->dat;
  /* the number of bytes is on the stack, at "frm + 2*cell" */
  bytes= * (cell *)(data+(int)amx->frm+2*sizeof(cell));
  /* the number of arguments is the number of bytes divided
   * by the size of a cell */
  return bytes/sizeof(cell);
}

static cell AMX_NATIVE_CALL getarg(AMX *amx, cell *params)
{
  AMX_HEADER *hdr;
  uchar *data;
  cell value;

  hdr=(AMX_HEADER *)amx->base;
  data=amx->base+(int)hdr->dat;
  /* get the base value */
  value= * (cell *)(data+(int)amx->frm+((int)params[1]+3)*sizeof(cell));
  /* adjust the address in "value" in case of an array access */
  value+=params[2]*sizeof(cell);
  /* get the value indirectly */
  value= * (cell *)(data+(int)value);
  return value;
}

static cell AMX_NATIVE_CALL setarg(AMX *amx, cell *params)
{
  AMX_HEADER *hdr;
  uchar *data;
  cell value;

  hdr=(AMX_HEADER *)amx->base;
  data=amx->base+(int)hdr->dat;
  /* get the base value */
  value= * (cell *)(data+(int)amx->frm+((int)params[1]+3)*sizeof(cell));
  /* adjust the address in "value" in case of an array access */
  value+=params[2]*sizeof(cell);
  /* verify the address */
  if (value<0 || value>=amx->hea && value<amx->stk)
    return 0;
  /* set the value indirectly */
  * (cell *)(data+(int)value) = params[3];
  return 1;
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL heapspace(AMX *amx,cell *params)
{
  return amx->stk - amx->hea;
}

static cell AMX_NATIVE_CALL funcidx(AMX *amx,cell *params)
{
  char name[64];
  cell *cstr;
  int index,err;

  amx_GetAddr(amx,params[1],&cstr);
  #if 0 /* if you are paranoia */
    amx_StrLen(cstr,&len);
    if (len>=64) {
      amx_RaiseError(amx,AMX_ERR_NATIVE);
      return 0;
    } /* if */
  #endif
  amx_GetString(name,cstr);
  err=amx_FindPublic(amx,name,&index);
  if (err!=AMX_ERR_NONE)
    index=-1;   /* this is not considered a fatal error */
  return index;
}

int amx_StrPack(cell *dest,cell *source)
{
  int len;

  amx_StrLen(source,&len);
  if (*source>UCHAR_MAX) {
    /* source string is already packed */
    while (len >= 0) {
      *dest++ = *source++;
      len-=sizeof(cell);
    } /* while */
  } else {
    /* pack string, from bottom up */
    cell c;
    int i;
    for (c=0,i=0; i<len; i++) {
      assert((*source & ~0xffL)==0);
      c=(c<<CHARBITS) | *source++;
      if (i%sizeof(cell) == sizeof(cell)-1) {
        *dest++=c;
        c=0;
      } /* if */
    } /* for */
    if (i%sizeof(cell) != 0)    /* store remaining packed characters */
      *dest=c << (sizeof(cell)-i%sizeof(cell))*CHARBITS;
    else
      *dest=0;                  /* store full cell of zeros */
  } /* if */
  return AMX_ERR_NONE;
}

int amx_StrUnpack(cell *dest,cell *source)
{
  if (*source>UCHAR_MAX) {
    /* unpack string, from top down (so string can be unpacked in place) */
    cell c;
    int i,len;
    amx_StrLen(source,&len);
    dest[len]=0;
    for (i=len-1; i>=0; i--) {
      c=source[i/sizeof(cell)] >> (sizeof(cell)-i%sizeof(cell)-1)*CHARBITS;
      dest[i]=c & UCHAR_MAX;
    } /* for */
  } else {
    /* source string is already unpacked */
    while ((*dest++ = *source++) != 0)
      /* nothing */;
  } /* if */
  return AMX_ERR_NONE;
}

static int verify_addr(AMX *amx,cell addr)
{
  int err;
  cell *cdest;

  err=amx_GetAddr(amx,addr,&cdest);
  if (err!=AMX_ERR_NONE)
    amx_RaiseError(amx,err);
  return err;
}

static cell AMX_NATIVE_CALL core_strlen(AMX *amx,cell *params)
{
  char szString1[1024];
  cell *cptr;
  int len = 0;

  if (amx_GetAddr(amx,params[1],&cptr)==AMX_ERR_NONE)
  {
    amx_StrLen(cptr,&len);
	amx_GetString(szString1, cptr);
  }
  return len;
}

static cell AMX_NATIVE_CALL core_strcmp(AMX *amx,cell *params)
{
  char szString1[1024];
  char szString2[1024];
  cell *n1;
  cell *n2;

  amx_GetAddr(amx, params[1], &n1);
  amx_GetString(szString1, n1);

  amx_GetAddr(amx, params[2], &n2);
  amx_GetString(szString2, n2);

  return strcmp(szString1, szString2);
}

static cell AMX_NATIVE_CALL core_strcpy(AMX *amx,cell *params)
{
  char szString1[1024];
  char szString2[1024];
  cell *n1;
  cell *n2;

  amx_GetAddr(amx, params[1], &n1);
  amx_GetString(szString1, n1);

  amx_GetAddr(amx, params[2], &n2);
  amx_GetString(szString2, n2);

  strcpy(szString1, szString2);

  amx_GetAddr(amx,params[1],&n1);
  amx_SetString(n1, szString1, 0);
  return 0;
}

static cell AMX_NATIVE_CALL core_strcat(AMX *amx,cell *params)
{
  char szString1[512];
  char szString2[512];
  cell *n1;
  cell *n2;

  amx_GetAddr(amx, params[1], &n1);
  amx_GetString(szString1, n1);

  amx_GetAddr(amx, params[2], &n2);
  amx_GetString(szString2, n2);

  strcat( szString1, szString2 );

  amx_GetAddr(amx, params[1], &n1);
  amx_SetString(n1, szString1, 0);
  return 0;
}


static cell AMX_NATIVE_CALL strpack(AMX *amx,cell *params)
{
  cell *cdest,*csrc;
  int len,needed,err;

  /* calculate number of cells needed for (packed) destination */
  amx_GetAddr(amx,params[2],&csrc);
  amx_StrLen(csrc,&len);
  needed=(len+sizeof(cell)-1)/sizeof(cell);     /* # of cells needed */
  if (verify_addr(amx,(cell)(params[1]+needed))!=AMX_ERR_NONE)
    return 0;

  amx_GetAddr(amx,params[1],&cdest);
  err=amx_StrPack(cdest,csrc);
  if (err!=AMX_ERR_NONE)
    return amx_RaiseError(amx,err);

  return len;
}

static cell AMX_NATIVE_CALL strunpack(AMX *amx,cell *params)
{
  cell *cdest,*csrc;
  int len,err;

  /* calculate number of cells needed for (packed) destination */
  amx_GetAddr(amx,params[2],&csrc);
  amx_StrLen(csrc,&len);
  if (verify_addr(amx,(cell)(params[1]+len+1))!=AMX_ERR_NONE)
    return 0;

  amx_GetAddr(amx,params[1],&cdest);
  err=amx_StrUnpack(cdest,csrc);
  if (err!=AMX_ERR_NONE)
    return amx_RaiseError(amx,err);

  return len;
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL swapchars(AMX *amx,cell *params)
{
  union {
    cell c;
    #if defined BIT16
      uchar b[2];
    #else
      uchar b[4];
    #endif
  } value;
  uchar t;

  assert((size_t)params[0]==sizeof(cell));
  value.c = params[1];
  #if defined BIT16
    t = value.b[0];
    value.b[0] = value.b[1];
    value.b[1] = t;
  #else
    t = value.b[0];
    value.b[0] = value.b[3];
    value.b[3] = t;
    t = value.b[1];
    value.b[1] = value.b[2];
    value.b[2] = t;
  #endif
  return value.c;
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL core_tolower(AMX *amx,cell *params)
{
  assert((size_t)params[0]==sizeof(cell));
  return tolower((int)params[1]);
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL core_toupper(AMX *amx,cell *params)
{
  assert((size_t)params[0]==sizeof(cell));
  return toupper((int)params[1]);
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL core_min(AMX *amx,cell *params)
{
  return params[1] <= params[2] ? params[1] : params[2];
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL core_max(AMX *amx,cell *params)
{
  return params[1] >= params[2] ? params[1] : params[2];
}

static cell AMX_NATIVE_CALL core_clamp(AMX *amx,cell *params)
{
  cell value = params[1];
  if (params[2] > params[3])  /* minimum value > maximum value ! */
    amx_RaiseError(amx,AMX_ERR_NATIVE);
  if (value < params[2])
    value = params[2];
  else if (value > params[3])
    value = params[3];
  return value;
}

#if !defined NOPROPLIST
static char *MakePackedString(cell *cptr)
{
  int len;
  char *dest;

  amx_StrLen(cptr,&len);
  dest=(char *)malloc(len+sizeof(cell));
  amx_GetString(dest,cptr);
  return dest;
}

static cell AMX_NATIVE_CALL getproperty(AMX *amx,cell *params)
{
  cell *cstr;
  char *name;
  proplist *item;

  amx_GetAddr(amx,params[2],&cstr);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],NULL);
  /* if list_finditem() found the value, store the name */
  if (item!=NULL && item->value==params[3] && strlen(name)==0) {
    int needed=(strlen(item->name)+sizeof(cell)-1)/sizeof(cell);     /* # of cells needed */
    if (verify_addr(amx,(cell)(params[4]+needed))!=AMX_ERR_NONE) {
      free(name);
      return 0;
    } /* if */
    amx_GetAddr(amx,params[4],&cstr);
    amx_SetString(cstr,item->name,1);
  } /* if */
  free(name);
  return (item!=NULL) ? item->value : 0;
}

static cell AMX_NATIVE_CALL setproperty(AMX *amx,cell *params)
{
  cell prev=0;
  cell *cstr;
  char *name;
  proplist *item;

  amx_GetAddr(amx,params[2],&cstr);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],NULL);
  if (item==NULL)
    item=list_additem(&proproot);
  if (item==NULL) {
    amx_RaiseError(amx,AMX_ERR_MEMORY);
  } else {
    prev=item->value;
    if (strlen(name)==0) {
      free(name);
      amx_GetAddr(amx,params[4],&cstr);
      name=MakePackedString(cstr);
    } /* if */
    list_setitem(item,params[1],name,params[3]);
  } /* if */
  free(name);
  return prev;
}

static cell AMX_NATIVE_CALL delproperty(AMX *amx,cell *params)
{
  cell prev=0;
  cell *cstr;
  char *name;
  proplist *item,*pred;

  amx_GetAddr(amx,params[2],&cstr);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],&pred);
  if (item!=NULL) {
    prev=item->value;
    list_delete(pred,item);
  } /* if */
  free(name);
  return prev;
}

static cell AMX_NATIVE_CALL existproperty(AMX *amx,cell *params)
{
  cell *cstr;
  char *name;
  proplist *item;

  amx_GetAddr(amx,params[2],&cstr);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],NULL);
  free(name);
  return (item!=NULL);
}
#endif

/* This routine comes from the book "Inner Loops" by Rick Booth, Addison-Wesley
 * (ISBN 0-201-47960-5). This is a "multiplicative congruential random number
 * generator" that has been extended to 31-bits (the standard C version returns
 * only 15-bits).
 */
static unsigned long IL_StandardRandom_seed = 0L;
#define IL_RMULT 1103515245L
#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL core_random(AMX *amx,cell *params)
{
    unsigned long lo, hi, ll, lh, hh, hl;
    unsigned long result;

    /* one-time initialization (or, mostly one-time*/
    if (IL_StandardRandom_seed == 0L)
        IL_StandardRandom_seed=(unsigned long)time(NULL);

    lo = IL_StandardRandom_seed & 0xffff;
    hi = IL_StandardRandom_seed >> 16;
    IL_StandardRandom_seed = IL_StandardRandom_seed * IL_RMULT + 12345;
    ll = lo * (IL_RMULT  & 0xffff);
    lh = lo * (IL_RMULT >> 16    );
    hl = hi * (IL_RMULT  & 0xffff);
    hh = hi * (IL_RMULT >> 16    );
    result = ((ll + 12345) >> 16) + lh + hl + (hh << 16);
    result &= ~LONG_MIN;        /* remove sign bit */
    if (params[1]!=0)
        result %= params[1];
    return (cell)result;
}

void core_Init(void)
{
  /* reduced to a do-nothing routine */
}

void core_Exit(void)
{
  #if !defined NOPROPLIST
    while (proproot.next!=NULL)
      list_delete(&proproot,proproot.next);
  #endif
}


AMX_NATIVE_INFO core_Natives[] = {
  { "numargs",       numargs },
  { "getarg",        getarg },
  { "setarg",        setarg },
  { "heapspace",     heapspace },
  { "funcidx",       funcidx },
  { "strlen",        core_strlen },
  { "strpack",       strpack },
  { "strunpack",     strunpack },
  { "swapchars",     swapchars },
  { "tolower",       core_tolower },
  { "toupper",       core_toupper },
  { "random",        core_random },
  { "min",           core_min },
  { "max",           core_max },
  { "clamp",         core_clamp },
  { "strcmp",        core_strcmp},
  { "strcpy",        core_strcpy},
  { "strcat",        core_strcat},
#if !defined NOPROPLIST
  { "getproperty",   getproperty },
  { "setproperty",   setproperty },
  { "deleteproperty",delproperty },
  { "existproperty", existproperty },
#endif
  { NULL, NULL }        /* terminator */
};


