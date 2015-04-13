/*  Small debugger
 *
 *  Minimal debugger with a console interface.
 *
 *  Copyright (c) ITB CompuPhase, 1998-2001
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#if defined __WIN32 || defined _WIN32 || defined __MSDOS__ || defined __WATCOMC__
  #include <conio.h>
#else
  #include "linux/getch.h"
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "amx.h"


#define MAXFILES        32
#define MAXBREAKS       16
#define MAXSTACKTRACE   128
#define MAXLINELENGTH   128
#define NAMEMAX         32      /* length of variable/function names */
#define MAX_DIMS        2       /* number of array dimensions */
#define TABSIZE         8
#define EATLINE         5       /* the number of characters for the line number */
#define STD_COLUMNS     80      /* number of characters that fit on a line */
#define WIDE_COLUMNS    132
#if defined amx_Init
  #define MAXWATCHES    6
  #define SCREENLINES   29      /* number of lines that fit on a screen */
  #define DEF_LIST      13      /* default length of code list */
#else
  #define MAXWATCHES    4
  #define SCREENLINES   24      /* number of lines that fit on a screen */
  #define DEF_LIST      10      /* default length of code list */
#endif
#define CMDWINDOW       (SCREENLINES - MAXWATCHES - DEF_LIST - 2)

#if CMDWINDOW <= 1
  #error Command window too small (code list or watches window too big)
#endif

#if defined amx_Init
  #define CHR_PROMPT    '\xbb'
  #define CHR_HLINE     '\x97'
  #define CHR_VLINE     '|'
  #define DEF_TERMINAL  TERM_ANSI
#elif defined __WIN32 || defined _WIN32 || defined __MSDOS__ || defined __WATCOMC__
  #define CHR_PROMPT    '\xaf'
  #define CHR_HLINE     '\xc4'
  #define CHR_VLINE     '\xb3'
  #define DEF_TERMINAL  TERM_NONE
#else
  #define CHR_PROMPT    '>'
  #define CHR_HLINE     '-'
  #define CHR_VLINE     '|'
  #if defined LINUX
    #define DEF_TERMINAL  TERM_VT100
  #else
    #define DEF_TERMINAL  TERM_NONE
  #endif
#endif
#define CHR_CURLINE     '*'
#define CHR_HLINE_VT100 'q'
#define CHR_VLINE_VT100 'x'
#define CHR_DOWNT_VT100 'w'
#define CHR_UPT_VT100   'v'

enum {
  BP_NONE,
  BP_CODE,
  BP_DATA,
  /* --- */
  BP_TYPES
};

enum {
  TERM_NONE,
  TERM_ANSI,
  TERM_VT100,
  /* --- */
  TERM_TYPES
};

enum {
  DISP_DEFAULT,
  DISP_STRING,
  DISP_BIN,   /* ??? not implemented */
  DISP_HEX,   /* ??? not implemented */
  DISP_FIXED,
  DISP_FLOAT,
  /* --- */
  DISP_TYPES
};

typedef struct __symbol {
  char name[NAMEMAX];
  ucell addr;
  int file;             /* file number that a function appears in */
  int line;             /* line number that a function starts on */
  int vclass;           /* variable class (global, local, static) */
  int type;             /* symbol type (local var/global var/function */
  int calllevel;        /* nesting level (visibility) of the variable */
  int length[MAX_DIMS]; /* for arrays */
  int dims;             /* for arrays */
  int disptype;         /* display type */
  struct __symbol *next;
} SYMBOL;

typedef struct {
  int type;             /* one of the BP_xxx types */
  ucell addr;           /* line number, or previous value */
  int file;             /* file in which the breakpoint appears */
  int index;
  SYMBOL *sym;
} BREAKPOINT;

static char *filenames[MAXFILES];
static int curfileno=-1;
static char **cursource;
static int curline;
static int stopline;
static int autolist=1;
static int screencolumns=STD_COLUMNS;
static BREAKPOINT breakpoints[MAXBREAKS];
static char watches[MAXWATCHES][NAMEMAX];
static SYMBOL functab;
static SYMBOL vartab;
static int curtopline;  /* current line that is on top in the list */
static ucell callstack[MAXSTACKTRACE];
static int terminal=DEF_TERMINAL;
static char chr_hline=CHR_HLINE;
static char chr_vline=CHR_VLINE;

#if defined amx_Init
  int amx_printf(const char * strFmt, ...);
  int amx_fflush(void *stream);
  int amx_getch(void);
  char *amx_gets(char *str,int length);
  #define dprintf       amx_printf
  #define dflush()      amx_fflush(NULL)
  #define dgetch        amx_getch
  #define dgetstr       amx_gets
#else
  #define dprintf       printf
  #define dflush()      fflush(stdout)
  #define dgetstr(s,n)  fgets(s,n,stdin)
  #define dgetch        getch
#endif

static void term_clrscr(void)
{
  assert(terminal==TERM_ANSI || terminal==TERM_VT100);
  dprintf("\033[2J");
}

static void term_clreol(void)
{
  assert(terminal==TERM_ANSI || terminal==TERM_VT100);
  dprintf("\033[K");
}

static void term_setattrib(int bold)
{
  if (terminal==TERM_ANSI || terminal==TERM_VT100) {
    if (bold)
      dprintf("\033[1m");       /* set "bold" */
    else
      dprintf("\033[0m");       /* reset "bold" */
  } /* if */
}

static void term_setcharset(int graphics)
{
  if (terminal==TERM_VT100) {
    if (graphics)
      dprintf("\016");          /* SO code to select the graphic set */
    else
      dprintf("\017");          /* SI code to select the standard set */
  } /* if */
}

static void draw_hline(int forceinit)
{
static char hline_str[WIDE_COLUMNS+1] = "";

  /* initialize the string, if not yet done */
  if (forceinit || strlen(hline_str)==0) {
    memset(hline_str,chr_hline,sizeof hline_str);
    hline_str[screencolumns]='\0';
  } /* if */
  term_setcharset(1);
  dprintf(hline_str);
  term_setcharset(0);
}

static void term_csrset(int x,int y)
{
  assert(terminal==TERM_ANSI || terminal==TERM_VT100);
  dprintf("\033[%d;%dH",y,x);
}

static void term_csrget(int *x,int *y)
{
  int val,i;
  char str[10];

  assert(terminal==TERM_ANSI || terminal==TERM_VT100);
  dprintf("\033[6n");
  dflush();
  while (dgetch()!='\033')
    /* nothing */;
  val=dgetch();
  assert(val=='[');
  for (i=0; i<8 && (val=dgetch())!=';'; i++)
    str[i]=(char)val;
  str[i]='\0';
  *y=atoi(str);
  for (i=0; i<8 && (val=dgetch())!='R'; i++)
    str[i]=(char)val;
  str[i]='\0';
  *x=atoi(str);
  if (terminal==TERM_ANSI) {
    val=dgetch();
    assert(val=='\r');    /* ANSI driver adds CR to the end of the command */
  } /* if */
}

static void term_csrsave(void)
{
  assert(terminal==TERM_ANSI || terminal==TERM_VT100);
  if (terminal==TERM_ANSI)
    dprintf("\033[s");
  else
    dprintf("\0337");
}

static void term_csrrestore(void)
{
  assert(terminal==TERM_ANSI || terminal==TERM_VT100);
  if (terminal==TERM_ANSI)
    dprintf("\033[u");
  else
    dprintf("\0338");
}

static void term_open(void)
{
  assert(terminal==TERM_ANSI || terminal==TERM_VT100);
  term_clrscr();
  dprintf("\033[?7l");                  /* disable "auto-wrap" */
  if (terminal==TERM_VT100) {
    screencolumns=WIDE_COLUMNS;
    chr_hline=CHR_HLINE_VT100;
    chr_vline=CHR_VLINE_VT100;
    dprintf("\033[?3h");                /* set 132 column-mode */
    dprintf("\033[%d;%dr",MAXWATCHES+DEF_LIST+3,SCREENLINES); /* set window */
    dprintf("\033)0");                  /* select graphics codes for set G1 */
  } else {
    screencolumns=STD_COLUMNS;
    chr_hline=CHR_HLINE;
    chr_vline=CHR_VLINE;
  } /* if */
  term_csrset(1,MAXWATCHES+1);
  draw_hline(1);
  term_csrset(1,MAXWATCHES+DEF_LIST+2);
  draw_hline(0);
  term_csrset(1,MAXWATCHES+DEF_LIST+3);
}

static void term_close(void)
{
  assert(terminal==TERM_ANSI || terminal==TERM_VT100);
  term_clrscr();
  screencolumns=STD_COLUMNS;
}

static void term_restore(void)
{
  int x,y;

  if (terminal==TERM_ANSI) {
    term_csrget(&x,&y);
    if (y>=SCREENLINES)
      term_open();
  } /* if */
}

static void source_free(char **source)
{
  int i;

  assert(source!=NULL);
  for (i=0; source[i]!=NULL; i++)
    free(source[i]);
  free(source);
}

static char **source_load(char *filename)
{
  char **source;
  FILE *fp;
  char line[256];
  int lines,i;

  /* open the file, number of characters */
  if ((fp=fopen(filename,"rt"))==NULL)
    return NULL;
  lines=0;
  while (fgets(line,sizeof(line),fp)!=NULL)
    lines++;

  /* allocate memory, reload the file */
  if ((source=(char **)malloc((lines+1)*sizeof(char *)))==NULL) {
    fclose(fp);
    return NULL;
  } /* if */
  for (i=0; i<=lines; i++)      /* initialize, so that source_free() works */
    source[i]=NULL;
  rewind(fp);
  i=0;
  while (fgets(line,sizeof(line),fp)!=NULL) {
    assert(i<lines);
    source[i]=strdup(line);
    if (source[i]==NULL) {
      fclose(fp);
      source_free(source);      /* free everything allocated so far */
      return NULL;
    } /* if */
    i++;
  } /* if */

  fclose(fp);
  return source;
}

static void source_list(int startline, int numlines)
{
  /* cursource and curline should already have been set */
  int result,lastline;

  if (terminal==TERM_ANSI || terminal==TERM_VT100) {
    term_csrsave();
    term_csrset(1,MAXWATCHES+2);
    numlines=DEF_LIST;  /* override user setting */
  } /* if */

  if (startline<0)
    startline=0;
  lastline=startline+numlines;
  curtopline=startline; /* save line that is currently displayed at the top */

  /* seek to line number from the start (to avoid displaying something
   * beyond the file)
   */
  for (result=0; cursource[result]!=NULL && result<startline; result++)
    /* nothing */;
  if (cursource[result]!=NULL) {
    assert(result==startline);
    while (cursource[startline]!=NULL && startline<lastline) {
      char c1='\0',c2='\0';
      if (terminal==TERM_ANSI || terminal==TERM_VT100)
        term_clreol();
      if (strlen(cursource[startline])>screencolumns-EATLINE) {
        c1=cursource[startline][screencolumns-EATLINE-1];
        c2=cursource[startline][screencolumns-EATLINE];
        cursource[startline][screencolumns-EATLINE-1] = '\n';
        cursource[startline][screencolumns-EATLINE] = '\0';
      } /* if */
      term_setattrib(startline==curline);
      if (terminal==TERM_NONE) {
        if (startline==curline)
          dprintf("%3d%c %s",startline+1,CHR_CURLINE,cursource[startline]);
        else
          dprintf("%3d  %s",startline+1,cursource[startline]);
      } else {
        dprintf("%4d",startline+1);
        term_setcharset(1);
        dprintf("%c",chr_vline);
        term_setcharset(0);
        dprintf("%s",cursource[startline]);
      } /* if */
      if (c1!='\0') {
        cursource[startline][screencolumns-EATLINE-1] = c1;
        cursource[startline][screencolumns-EATLINE] = c2;
      } /* if */
      startline++;
    } /* while */
  } /* if */

  if (terminal==TERM_ANSI || terminal==TERM_VT100) {
    while (startline<lastline) {
      term_clreol();
      dprintf("\n");
      startline++;
    } /* while */
    term_csrrestore();
  } /* if */
}

static int gettopline(int line,int topline)
{
  if (terminal==TERM_NONE)
    return topline;
  if (line<curtopline || line>=curtopline+DEF_LIST)
    return topline;
  return curtopline;
}

static char *skipwhitespace(char *str)
{
  while (*str==' ' || *str=='\t')
    str++;
  return str;
}

static char *skipvalue(char *str)
{
  while (isdigit(*str))
    str++;
  str=skipwhitespace(str);
  return str;
}

static char *skippath(char *str)
{
  char *p1,*p2;

  /* DOS/Windows pathnames */
  if ((p1=strrchr(str,'\\'))!=NULL)
    p1++;
  else
    p1=str;
  /* Unix pathnames */
  if ((p2=strrchr(str,'/'))!=NULL)
    p2++;
  else
    p2=str;
  return p1>p2 ? p1 : p2;
}

static SYMBOL *add_symbol(SYMBOL *table,char *name,int type,ucell addr,
                          int vclass,int level)
{
  SYMBOL *sym;

  if ((sym=(SYMBOL *)malloc(sizeof(SYMBOL)))==NULL)
    return NULL;
  memset(sym,0,sizeof(SYMBOL));
  assert(strlen(name)<NAMEMAX);
  strcpy(sym->name,name);
  sym->type=type;
  sym->addr=addr;
  sym->vclass=vclass;
  sym->calllevel=level;
  sym->length[0]=0;     /* indeterminate */
  sym->length[1]=0;     /* indeterminate */
  if (type==3 || type==4)
    sym->dims=1;        /* for now, assume single dimension */
  else
    sym->dims=0;        /* not an array */
  sym->disptype=DISP_DEFAULT;
  sym->next=table->next;
  table->next=sym;
  return sym;
}

static SYMBOL *find_symbol(SYMBOL *table,char *name,int level)
{
  SYMBOL *sym = table->next;

  while (sym!=NULL) {
    if (strcmp(name,sym->name)==0 && sym->calllevel==level)
      return sym;
    sym=sym->next;
  } /* while */
  return NULL;
}

static SYMBOL *find_symbol_addr(SYMBOL *table,ucell addr,int level)
{
  SYMBOL *sym = table->next;

  while (sym!=NULL) {
    if (sym->addr==addr && sym->calllevel==level)
      return sym;
    sym=sym->next;
  } /* while */
  return NULL;
}

static void delete_symbol(SYMBOL *table,ucell addr,int level)
{
  SYMBOL *prev = table;
  SYMBOL *cur = prev->next;

  /* Delete all local symbols below a certain address (these are local
   * variables from a function/block that one stepped out of). Also
   * remove all symbols of deeper calllevels (removes the static variables
   * at the end of a function).
   */
  while (cur!=NULL) {
    if ((cur->calllevel > level)
        || ((1 == cur->vclass) && (cur->addr < addr)))
    {
      prev->next=cur->next;
      free(cur);
      cur=prev->next;
    } else {
      prev=cur;
      cur=cur->next;
    } /* if */
  } /* while */
}

static void delete_allsymbols(SYMBOL *table)
{
  SYMBOL *sym=table->next, *next;

  while (sym!=NULL) {
    next=sym->next;
    free(sym);
    sym=next;
  } /* while */
}

static cell get_symbolvalue(AMX *amx,SYMBOL *sym,int index)
{
  cell *value;
  cell base;

  if (sym->type==2 || sym->type==4) {   /* a reference */
    amx_GetAddr(amx,sym->addr,&value);
    base=*value;
  } else {
    base=sym->addr;
  } /* if */
  amx_GetAddr(amx,(cell)(base+index*sizeof(cell)),&value);
  return *value;
}

static char *get_string(AMX *amx,SYMBOL *sym,int maxlength)
{
static char string[MAXLINELENGTH];
  char *ptr;
  cell *addr;
  cell base;
  int length,num;

  assert(sym->type==3 || sym->type==4);
  assert(sym->dims==1);
  assert(maxlength<MAXLINELENGTH);
  string[0]='\0';

  /* get the staring address and the length of the string */
  base=sym->addr;
  if (sym->type==4) {   /* a reference */
    amx_GetAddr(amx,base,&addr);
    base=*addr;
  } /* if */
  amx_GetAddr(amx,base,&addr);
  amx_StrLen(addr,&length);

  /* allocate a temporary buffer */
  ptr=malloc(length+1);
  if (ptr!=NULL) {
    amx_GetString(ptr,addr);
    num=length;
    if (num>=maxlength) {
      num=maxlength-1;
      if (num>3)
        num-=3;         /* make space for the ... terminator */
    } /* if */
    assert(num>=0);
    strncpy(string,ptr,num);
    string[num]='\0';
    if (num<length && num==maxlength-3)
      strcat(string,"...");
    free(ptr);
  } /* if */
  return string;
}

static void watch_init(void)
{
  int i;

  for (i=0; i<MAXWATCHES && strlen(watches[i])>0; i++)
    watches[i][0]='\0';
}

static void watch_list(AMX *amx,int calllevel)
{
  int num,i,idx;
  SYMBOL *sym;
  char *indexptr;
  char name[NAMEMAX];

  if (terminal==TERM_ANSI || terminal==TERM_VT100) {
    term_csrsave();
    term_csrset(1,1);
    printf("\n");       /* clear "tab state" if the terminal driver forgot */
    term_csrset(1,1);   /* and set the cursor position again */
  } /* if */

  for (i=num=0; i<MAXWATCHES; i++) {
    if (strlen(watches[i])>0) {
      strcpy(name,watches[i]);
      indexptr=strchr(name,'[');
      if (indexptr!=NULL) {
        idx=atoi(indexptr+1);
        *indexptr='\0';
      } else {
        idx=0;
      } /* if */
      dprintf("%d   %s\t",i+1,watches[i]);  // ??? avoid tab characters in ANSI emulation
      /* search locals, then globals */
      sym=find_symbol(&vartab,name,calllevel);
      if (sym==NULL)
        sym=find_symbol(&vartab,name,-1);
      if (sym!=NULL) {
        if ((sym->type==3 || sym->type==4) && indexptr==NULL) {
          if (sym->disptype==DISP_STRING)
            dprintf("\"%s\"", get_string(amx,sym,40));
          else
            dprintf("(missing index)");
        } else if ((sym->type==3 || sym->type==4) && sym->length[0]>0 && idx>=sym->length[0]) {
          dprintf("(index out of range)");
        } else if (sym->type!=3 && sym->type!=4 && indexptr!=NULL) {
          dprintf("(invalid index)");
        } else {
          long symval=get_symbolvalue(amx,sym,idx);
          if (sym->disptype==DISP_FLOAT)
            dprintf("%f", symval);
          else if (sym->disptype==DISP_FIXED)
            dprintf("%ld.%03ld", symval/1000, (symval%1000+1000)%1000);
          else
            dprintf("%ld", symval);
        } /* if */
      } else {
        dprintf("(unknown symbol)");
      } /* if */
      if (terminal==TERM_ANSI || terminal==TERM_VT100)
        term_clreol();
      dprintf("\n");
      num++;
    } /* if */
  } /* for */

  if (terminal==TERM_ANSI || terminal==TERM_VT100) {
    if (num==0)
      dprintf("(no watches)");
    for ( ; num<MAXWATCHES; num++) {
      term_clreol();
      dprintf("\n");
    } /* for */
    term_csrrestore();
  } else {
    if (num>0)
      draw_hline(0);
  } /* if */
}

static int watch_set(int number, char *name)
{
  if (number<0 || number>MAXWATCHES)
    return 0;
  if (number==0) {
    /* find an empty spot */
    while (number<MAXWATCHES && strlen(watches[number])>0)
      number++;
    if (number>=MAXWATCHES)
      return 0;   /* watch table full */
  } else {
    number--;
  } /* if */
  /* add the watch */
  strcpy(watches[number],name);
  return 1;
}

static int watch_clear(int number)
{
  if (number>0 && number<=MAXWATCHES) {
    watches[number-1][0]='\0';
    return 1;
  } /* if */
  return 0;
}

static void break_init(void)
{
  int i;

  for (i=0; i<MAXBREAKS; i++) {
    breakpoints[i].type=BP_NONE;
    breakpoints[i].addr=0;
    breakpoints[i].sym=NULL;
    breakpoints[i].index=-1;
  } /* for */
}

static void break_clear(int index)
{
  if (index>=0 && index<=MAXBREAKS) {
    breakpoints[index].type=BP_NONE;
    breakpoints[index].addr=0;
    breakpoints[index].file=-1;
    breakpoints[index].sym=NULL;
    breakpoints[index].index=-1;
  } /* if */
}

static int break_set(AMX *amx,char *str,int calllevel)
{
  int index;
  SYMBOL *sym;

  /* find an empty spot */
  for (index=0; index<MAXBREAKS && breakpoints[index].type!=BP_NONE; index++)
    /* nothing */
  if (index>=MAXBREAKS)
    return -1;
  assert(breakpoints[index].sym==NULL);
  assert(breakpoints[index].addr==0);
  assert(breakpoints[index].index==-1);

  /* find type */
  str=skipwhitespace(str);
  if (isdigit(*str)) {
    breakpoints[index].type=BP_CODE;
    breakpoints[index].file=curfileno;
    breakpoints[index].addr=(ucell)atol(str);
  } else if ((sym=find_symbol(&functab,str,-1))!=NULL) {
    breakpoints[index].type=BP_CODE;
    breakpoints[index].addr=sym->line;
    breakpoints[index].file=sym->file;
    breakpoints[index].sym=sym;
  } else {
    char *idxptr=strchr(str,'[');
    int idx=-1;
    if (idxptr!=NULL) {
      idx=atoi(idxptr+1);
      *idxptr='\0';
    } /* if */
    if ((sym=find_symbol(&vartab,str,calllevel))!=NULL) {
      if (sym->type==3 || sym->type==4) {
        if (idxptr==NULL)
          return -1;            /* missing index on array */
        if (sym->length[0]>0 && idx>=sym->length[0])
          return -1;            /* index out of range */
      } /* if */
      if (sym->type!=3 && sym->type!=4 && idxptr!=NULL)
        return -1;
      breakpoints[index].type=BP_DATA;
      breakpoints[index].addr=get_symbolvalue(amx,sym,idx>=0 ? idx : 0);
      breakpoints[index].sym=sym;
      breakpoints[index].index=idx;
    } else {
      return -1;
    } /* if */
  } /* if */
  return index;
}

static void break_list(void)
{
  int index;

  for (index=0; index<MAXBREAKS; index++) {
    if (breakpoints[index].type==BP_NONE)
      continue;
    dprintf("%2d  ",index);
    if (breakpoints[index].type==BP_CODE) {
      int file = breakpoints[index].file;
      dprintf("line: %d",breakpoints[index].addr);
      if (breakpoints[index].sym!=NULL)
        dprintf("  func: %s",breakpoints[index].sym->name);
      else
        dprintf("  file: %s",skippath(filenames[file]));
      dprintf("\n");
    } else {
      /* must be variable */
      assert(breakpoints[index].sym!=NULL);
      dprintf("var: %s",breakpoints[index].sym->name);
      if (breakpoints[index].index>=0)
        dprintf("[%d]",breakpoints[index].index);
      dprintf("\n");
    } /* if */
  } /* for */
}

static int break_check(AMX *amx,int line,int file)
{
  int index;

  for (index=0; index<MAXBREAKS; index++) {
    if (breakpoints[index].type==BP_CODE && breakpoints[index].addr==line
        && breakpoints[index].file==file)
    {
      return index;
    } else if (breakpoints[index].type==BP_DATA) {
      int idx=breakpoints[index].index;
      SYMBOL *sym=breakpoints[index].sym;
      ucell value;
      assert(sym!=NULL);
      value=get_symbolvalue(amx,sym,idx>=0 ? idx : 0);
      if (breakpoints[index].addr!=value) {
        breakpoints[index].addr=value;
        return index;
      } /* if */
    } /* if */
  } /* for */
  return -1;
}

enum {
  GO,
  GO_RET,
  NEXT,
  STEP,
};

static void listcommands(char *command)
{
  if (command==NULL)
    command="";
  if (stricmp(command,"break")==0) {
    dprintf("\tBREAK\t\tlist all breakpoints\n"
            "\tBREAK n\t\tset a breakpoint at line \"n\"\n"
            "\tBREAK func\tset a breakpoint at function with name \"func\"\n"
            "\tBREAK var\tset a breakpoint at variable \"var\"\n"
            "\tBREAK var[i]\tset a breakpoint at array element \"var[i]\"\n");
  } else if (stricmp(command,"cbreak")==0) {
    dprintf("\tCBREAK n\tremove breakpoint number \"n\"\n"
            "\tCBREAK *\tremove all breakpoints\n");
  } else if (stricmp(command,"cw")==0 || stricmp(command,"cwatch")==0) {
    dprintf("\tCWATCH may be abbreviated to CW\n\n"
            "\tCWATCH n\tremove watch number \"n\"\n"
            "\tCWATCH *\tremove all watches\n");
  } else if (stricmp(command,"d")==0 || stricmp(command,"disp")==0) {
    dprintf("\tDISP may be abbreviated to D\n\n"
            "\tDISP\t\tdisplay all variables that are currently in scope\n"
            "\tDISP var\tdisplay the value of variable \"var\"\n"
            "\tDISP var[i]\tdisplay the value of array element \"var[i]\"\n");
  } else if (stricmp(command,"file")==0) {
    dprintf("\tFILE\t\tlist all files that this program is composed off\n"
            "\tFILE name\tset the current file to \"name\"\n");
  } else if (stricmp(command,"g")==0 || stricmp(command,"go")==0) {
    dprintf("\tGO may be abbreviated to G\n\n"
            "\tGO\t\trun until the next breakpoint or program termination\n"
            "\tGO RET\t\trun until the end of the current function\n"
            "\tGO n\t\trun until line number \"n\"\n");
  } else if (stricmp(command,"l")==0 || stricmp(command,"list")==0) {
    dprintf("\tLIST may be abbreviated to L\n\n"
            "\tLIST\t\tdisplay 10 lines around the current line\n"
            "\tLIST n\t\tdisplay 10 lines, starting from line \"n\"\n"
            "\tLIST n m\tdisplay \"m\" lines, starting from line \"n\" (not\n"
            "\t\t\tsupported in terminal emulation modes)\n"
            "\tLIST FUNCS\tdisplay all functions\n"
            "\tLIST ON\t\tautomatically list 10 lines after each step\n"
            "\tLIST OFF\tturn off automatic list (not supported in terminal\n"
            "\t\t\temulation modes)\n");
  } else if (stricmp(command,"calls")==0
             || stricmp(command,"n")==0 || stricmp(command,"next")==0
             || stricmp(command,"quit")==0
             || stricmp(command,"s")==0 || stricmp(command,"step")==0)
  {
    dprintf("\tno additional information\n");
  } else if (stricmp(command,"term")==0) {
    dprintf("\tTERM ANSI\tuse ANSI terminal display\n"
            "\tTERM VT100\tuse VT100 (compatible) terminal display\n"
            "\tTERM OFF\tno terminal support\n");
  } else if (stricmp(command,"type")==0) {
    dprintf("\tTYPE var STRING\tdisplay \"var\" as string\n"
            "\tTYPE var STD\tset default display format\n");
  } else if (stricmp(command,"watch")==0 || stricmp(command,"w")==0) {
    dprintf("\tWATCH may be abbreviated to W\n\n"
            "\tWATCH var\tset a new watch at variable \"var\"\n"
            "\tWATCH n var\tchange watch \"n\" to variable \"var\"\n");
  } else {
    dprintf("\tBREAK\t\tset breakpoint at line number or variable name\n"
            "\tCALLS\t\tshow call stack\n"
            "\tCBREAK\t\tremove breakpoint\n"
            "\tCW(atch)\tremove a \"watchpoint\"\n"
            "\tD(isp)\t\tdisplay the value of a variable, list variables\n"
            "\tFILE\t\tswitch to a file\n"
            "\tG(o)\t\trun program (until breakpoint)\n"
            "\tL(ist)\t\tdisplay source file and symbols\n"
            "\tN(ext)\t\tRun until next line, step over functions\n"
            "\tQUIT\t\texit debugger, terminate program\n"
            "\tS(tep)\t\tsingle step, step into functions\n"
            "\tTERM\t\tset terminal type\n"
            "\tTYPE\t\tset the \"display type\" of a symbol\n"
            "\tW(atch)\t\tset a \"watchpoint\" on a variable\n"
            "\n\tUse \"? <command name>\" to view more information on a command\n");
  } /* if */
}

static int docommand(AMX *amx,int calllevel)
{
static char lastcommand[10] = "";
  char line[MAXLINELENGTH], command[32];
  int result,i;
  SYMBOL *sym;
  char *params;

  for ( ;; ) {
    dprintf("%c ",CHR_PROMPT);
    dgetstr(line,sizeof(line));
    if (line[0]=='\033') {
      int x,y;
      assert(terminal==TERM_ANSI);
      term_csrget(&x,&y);
      term_csrset(1,y);
      if (strcmp(line+1,"F1")==0)
        strcpy(line,"?");       /* F1 == Help */
      else if (strcmp(line+1,"F3")==0)
        strcpy(line,"L");       /* F3 == List */
      else if (strcmp(line+1,"F5")==0)
        strcpy(line,"G");       /* F5 == Go */
      else if (strcmp(line+1,"F8")==0)
        strcpy(line,"S");       /* F8 == Step */
      else if (strcmp(line+1,"F10")==0)
        strcpy(line,"N");       /* F10 == Next */
      else
        continue;
    } /* if */
    if ((params=strchr(line,'\n'))!=NULL)
      *params='\0';     /* strip newline character */
    if (strlen(line)==0)
      strcpy(line,lastcommand);
    lastcommand[0]='\0';

    result=sscanf(line,"%8s",command);
    if (result<=0) {
      listcommands(NULL);
      continue;
    } /* if */
    params=strchr(line,' ');
    params=(params!=NULL) ? skipwhitespace(params) : "";

    if (stricmp(command,"?")==0) {
      result=sscanf(line,"%*s %30s",command);
      listcommands(result ? command : NULL);
    } else if (stricmp(command,"quit")==0) {
      exit(0);
    } else if (stricmp(command,"g")==0 || stricmp(command,"go")==0) {
      if (stricmp(params,"ret")==0)
        return GO_RET;
      stopline=atoi(params);
      return GO;
    } else if (stricmp(command,"s")==0 || stricmp(command,"step")==0) {
      strcpy(lastcommand,"s");
      return STEP;
    } else if (stricmp(command,"n")==0 || stricmp(command,"next")==0) {
      strcpy(lastcommand,"n");
      return NEXT;
    } else if (stricmp(command,"l")==0 || stricmp(command,"list")==0) {
      /* first check a few hard cases */
      if (stricmp(params,"funcs")==0) {
        for (sym=functab.next; sym!=NULL; sym=sym->next)
          dprintf("%s\t%s(%d)\n",sym->name,
                  skippath(filenames[sym->file]),sym->line);
      } else if (stricmp(params,"on")==0) {
        autolist=DEF_LIST;
        watch_list(amx,calllevel);
        source_list(curline-autolist/2,autolist);
      } else if (stricmp(params,"off")==0) {
        if (terminal==TERM_NONE)
          autolist=1;
        else
          dprintf("\tCommand not supported on terminals\n");
      } else {
        int lnum,numlines;
        lnum=curline-(DEF_LIST/2-1);    /* preset */
        numlines=DEF_LIST;
        sscanf(line,"%*s %d %d",&lnum,&numlines);
        lnum--;           /* if user filled in a line number, subtract 1 */
        term_restore();
        source_list(lnum,numlines);
      } /* if */
    } else if (stricmp(command,"break")==0) {
      if (*params=='\0') {
        break_list();
      } else {
        result=break_set(amx,params,calllevel);
        if (result<0)
          dprintf("Invalid breakpoint, or table full\n");
      } /* if */
    } else if (stricmp(command,"cbreak")==0) {
      if (*params=='*') {
        /* clear all breakpoints */
        for (i=0; i<MAXBREAKS; i++)
          break_clear(i);
      } else if (isdigit(*params)) {
        break_clear(atoi(params));
      } else {
        dprintf("\tInvalid command\n");
      } /* if */
    } else if (stricmp(command,"calls")==0) {
      assert(calllevel>=0);
      if (calllevel<MAXSTACKTRACE) {
        for (i=calllevel-1; i>=0; i--) {
          sym=find_symbol_addr(&functab,callstack[i],-1);
          assert(sym!=NULL);
          dprintf("\t%s\t%s(%d)\n",sym->name,
                 skippath(filenames[sym->file]),sym->line);
        } /* for */
      } else {
        dprintf("\tCall stack saturated\n");
      } /* if */
    } else if (stricmp(command,"disp")==0 || stricmp(command,"d")==0) {
      if (*params=='\0') {
        /* display all */
        for (sym=vartab.next; sym!=NULL; sym=sym->next) {
          if (sym->calllevel==-1 || sym->calllevel==calllevel) {
            dprintf("%s\t%s\t",sym->vclass>0 ? "loc" : "glb",sym->name);
            if (sym->type==3 || sym->type==4) {
              /* an array (try to print as string) */
              if (sym->disptype==DISP_STRING) {
                dprintf("\"%s\"", get_string(amx,sym,40));
              } else if (sym->dims==1) {
                int len=sym->length[0];
                if (len>5)
                  len=5;
                for (i=0; i<len; i++)
                  dprintf("%ld ",get_symbolvalue(amx,sym,i));
                if (len<sym->length[0])
                  dprintf("[...]");
              } else {
                dprintf("[...]");
              } /* if */
            } else {
              dprintf("%ld",get_symbolvalue(amx,sym,0));
            } /* if */
            dprintf("\n");
          } /* if */
        } /* for */
      } else {
        char *indexptr=strchr(params,'[');
        if (indexptr!=NULL) {
          i=atoi(indexptr+1);
          *indexptr='\0';
        } else {
          i=0;
        } /* if */
        if ((sym=find_symbol(&vartab,params,calllevel))!=NULL) {
          if (sym->dims>0 && indexptr==NULL) {
            if (sym->disptype==DISP_STRING)
              dprintf("%s\t%s\t\"%s\"\n",sym->vclass>0 ? "loc" : "glb",sym->name,
                      get_string(amx,sym,40));
            else
              dprintf("\tArray must be indexed\n");
          } else if (sym->dims==0 && indexptr!=NULL) {
            dprintf("\tInvalid index, not an array\n");
          } else if (sym->dims>0 && sym->length[0]>0 && i>=sym->length[0]) {
            dprintf("\tIndex out of range\n");
          } else {
            dprintf("%s\t%s\t%ld\n",sym->vclass>0 ? "loc" : "glb",sym->name,
                    get_symbolvalue(amx,sym,i));
          } /* if */
        } else {
          dprintf("\tSymbol not found, or not a variable\n");
        } /* if */
      } /* if */
    } else if (stricmp(command,"file")==0) {
      if (*params=='\0') {
        for (i=0; i<MAXFILES && filenames[i]!=NULL; i++)
          dprintf("%5d\t%s\n",i,filenames[i]);
      } else {
        /* find the file */
        int file;
        for (file=0; file<MAXFILES; file++) {
          if (filenames[file]!=NULL
              && (stricmp(params,filenames[file])==0
                  || stricmp(params,skippath(filenames[file]))==0))
            break;
        } /* for */
        if (file<MAXFILES) {
          if (curfileno!=file) {
            curfileno=file;
            curline=0;
          } /* if */
          if (cursource!=NULL)
            source_free(cursource);
          assert(filenames[curfileno]!=NULL);
          cursource=source_load(filenames[curfileno]);
          if (cursource==NULL) {
            dprintf("\tSource file not found or insufficient memory\n");
            continue;
          } /* if */
        } else {
          dprintf("\tunknown file\n");
        } /* if */
      } /* if */
    } else if (stricmp(command,"term")==0) {
      int new_term = terminal;
      if (stricmp(params,"off")==0)
        new_term=TERM_NONE;
      else if (stricmp(params,"ansi")==0)
        new_term=TERM_ANSI;
      else if (stricmp(params,"vt100")==0)
        new_term=TERM_VT100;
      else
        dprintf("\tUnknown terminal type \"%s\"\n",params);
      if (terminal!=new_term) {
        curtopline=0;
        if (terminal!=TERM_NONE)
          term_close();
        terminal=new_term;
        if (terminal!=TERM_NONE) {
          autolist=DEF_LIST;
          term_open();
        } /* if */
        watch_list(amx,calllevel);
        source_list(gettopline(curline,curline-autolist/2),autolist);
      } /* if */
    } else if (stricmp(command,"type")==0) {
      char symname[NAMEMAX],*ptr;
      int len;
      for (ptr=params; *ptr!='\0' && *ptr!=' ' && *ptr!='\t'; ptr++)
        /* nothing */;
      len=(int)(ptr-params);
      if (len==0 || len>=NAMEMAX) {
        dprintf("\tInvalid (or missing) symbol name\n");
      } else {
        strncpy(symname,params,len);
        symname[len]='\0';
        params=skipwhitespace(ptr);
        /* first look for a local symbol, if not found for a global symbol */
        sym=find_symbol(&vartab,symname,calllevel);
        if (sym==NULL)
          sym=find_symbol(&vartab,symname,-1);
        if (sym==NULL) {
          dprintf("\tUnknown symbol \"%s\"\n",symname);
        } else {
          if (stricmp(params,"std")==0) {
            sym->disptype=DISP_DEFAULT;
          } else if (stricmp(params,"string")==0) {
            /* check array with single dimension */
            if (!(sym->type==3 || sym->type==4) || sym->dims!=1)
              dprintf("\t\"string\" display type is only valid for arrays with one dimension\n");
            else
              sym->disptype=DISP_STRING;
          } else if (stricmp(params,"bin")==0) {
            sym->disptype=DISP_BIN;
          } else if (stricmp(params,"hex")==0) {
            sym->disptype=DISP_HEX;
          } else if (stricmp(params,"fixed")==0) {
            sym->disptype=DISP_FIXED;
          } else if (stricmp(params,"float")==0) {
            sym->disptype=DISP_FLOAT;
          } else {
            dprintf("\tUnknown (or missing) display type\n");
          } /* if */
          watch_list(amx,calllevel);
        } /* if */
      } /* if */
    } else if (stricmp(command,"w")==0 || stricmp(command,"watch")==0) {
      if (isdigit(*params)) {
        i=atoi(params);
        params=skipvalue(params);
      } else {
        i = 0;
      } /* if */
      result=watch_set(i,params);
      if (result>=0)
        watch_list(amx,calllevel);
      else
        dprintf("Invalid watch, or table full\n");
    } else if (stricmp(command,"cw")==0 || stricmp(command,"cwatch")==0) {
      if (*params=='*') {
        /* clear all breakpoints */
        for (i=0; i<MAXBREAKS; i++)
          watch_clear(i);
      } else if (isdigit(*params)) {
        watch_clear(atoi(params));
      } else {
        dprintf("\tInvalid command\n");
      } /* if */
      watch_list(amx,calllevel);
    } else {
      dprintf("\tInvalid command, use \"?\" to view all commands\n");
    } /* if */
  } /* for */
}

static int abortflagged = 0;
void sigabort(int sig)
{
  abortflagged=1;
  signal(sig,sigabort); /* re-install the signal handler */
}

int AMXAPI amx_InternalDebugProc(AMX *amx)
{
static int tracelevel;
static int calllevel;
static SYMBOL *curfunc;
static SYMBOL *curvar;
  int cmd,i,vclass,type,num,lvl;
  unsigned short flags;

  switch (amx->dbgcode) {
  case DBG_INIT:
    assert(amx->flags==AMX_FLAG_BROWSE);
    /* check whether we should run */
    amx_Flags(amx,&flags);
    if ((flags & AMX_FLAG_DEBUG)==0 || curfileno!=-1)
      return AMX_ERR_DEBUG;     /* the debugger cannot run */
    /* intialize the file table and other global variables */
    for (i=0; i<MAXFILES; i++)
      filenames[i]=NULL;
    cursource=NULL;
    curfileno=-1;
    stopline=0;
    break_init();
    watch_init();
    functab.next=NULL;
    vartab.next=NULL;
    if (terminal!=TERM_NONE) {
      curtopline=0;
      autolist=DEF_LIST;
      term_open();
    } /* if */
    /* initialize statics here */
    calllevel=0;
    curfunc=NULL;
    curvar=NULL;
    tracelevel=calllevel+1;     /* step INTO first function */
    break;
  case DBG_FILE:        /* file number in curfile, filename in dbgname */
    assert(amx->flags==(AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
    if (amx->curfile>=MAXFILES) {
      dprintf("\nCritical error: too many source files\n");
      exit(1);
    } /* if */
    /* file should not already be set */
    num=(int)amx->curfile;
    assert(filenames[num]==NULL);
    filenames[num]=(char *)malloc(strlen(amx->dbgname)+1);
    if (filenames[num]!=NULL) {
      strcpy(filenames[num],amx->dbgname);
    } else {
      dprintf("\nCritical error: insufficient memory\n");
      exit(1);
    } /* if */
    break;
  case DBG_LINE:        /* line number in curline, file number in curfile */
    if ((amx->flags & AMX_FLAG_BROWSE)!=0) {
      /* check whether there is a function symbol that needs to be adjusted */
      if (curfunc!=NULL) {
        curfunc->line=(int)amx->curline;
        curfunc->file=(int)amx->curfile;
      } /* if */
      curfunc=NULL;
      break;            /* ??? could build a list with valid breakpoints */
    } /* if */
    curline=(int)amx->curline-1;
    /* check breakpoints */
    if ((int)amx->curline==stopline) {
      dprintf("STOPPED at line %d\n",(int)amx->curline);
      tracelevel=calllevel;
      stopline=0;
    } else if ((i=break_check(amx,(int)amx->curline,(int)amx->curfile))>=0) {
      dprintf("STOPPED at breakpoint %d\n",i);
      tracelevel=calllevel;
    } else if (abortflagged) {
      dprintf("STOPPED because of break request\n");
      tracelevel=calllevel;
      abortflagged=0;
    } /* if */
    if (tracelevel<calllevel)
      break;
    assert(amx->curfile>=0 && amx->curfile<MAXFILES);
    if (curfileno!=(int)amx->curfile) {
      curfileno=(int)amx->curfile;
      if (cursource!=NULL)
        source_free(cursource);
      cursource=source_load(filenames[curfileno]);
      if (cursource==NULL) {
        dprintf("\nCritical error: source file not found or insufficient memory\n");
        exit(1);
      } /* if */
    } /* if */
    assert(cursource[curline]!=NULL);
    term_restore();
    watch_list(amx,calllevel);
    source_list(gettopline(curline,curline-autolist/2),autolist);
    cmd=docommand(amx,calllevel);
    switch (cmd) {
    case GO:
      tracelevel=-1;
      break;
    case GO_RET:
      tracelevel=calllevel-1;
      break;
    case NEXT:
      tracelevel= calllevel;    /* step OVER functions */
      break;
    case STEP:
      tracelevel= calllevel+1;  /* step INTO functions */
    } /* switch */
    break;
  case DBG_SYMBOL:      /* address in dbgaddr, class/type in dbgparam,
                         * symbol name in dbgname */
    vclass=(int)(amx->dbgparam>>8);
    type=(int)amx->dbgparam & 0xff;
    if (type==9) {
      /* function */
      assert(vclass==0);
      assert(amx->flags==(AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
      curfunc=add_symbol(&functab,amx->dbgname,type,amx->dbgaddr,vclass,-1);
    } else {
      assert((amx->flags & AMX_FLAG_DEBUG)!=0);
      /* for local variables, must modify address relative to frame */
      if (vclass==1)
        amx->dbgaddr += amx->frm;
      lvl= ((amx->flags & AMX_FLAG_BROWSE)!=0) ? -1 : calllevel;
      /* if a static variable is declared inside a loop, it was not removed
       * at the end of the loop, so we do not have to declared it again
       */
      if (vclass==2 && find_symbol(&vartab,amx->dbgname,lvl)!=NULL)
        break;
      curvar=add_symbol(&vartab,amx->dbgname,type,amx->dbgaddr,vclass,lvl);
    } /* if */
    break;
  case DBG_SRANGE:
    /* check whether there is a symbol that needs to be adjusted */
    if (curvar!=NULL) {
      curvar->length[(int)amx->dbgaddr]=(int)amx->dbgparam;
      if (curvar->dims<(int)amx->dbgaddr+1)
        curvar->dims=(int)amx->dbgaddr+1;
      if (amx->dbgaddr==0)
        curvar=NULL;    /* last dimension handled */
    } /* if */
    break;
  case DBG_CLRSYM:      /* stk = stack address below which locals should be removed */
    assert((amx->flags & (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE))==AMX_FLAG_DEBUG);
    delete_symbol(&vartab,amx->stk,calllevel);
    break;
  case DBG_CALL:        /* function call */
    assert((amx->flags & (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE))==AMX_FLAG_DEBUG);
    assert(calllevel>=0);
    if (calllevel<MAXSTACKTRACE)
      callstack[calllevel]=amx->dbgaddr;
    calllevel++;
    break;
  case DBG_RETURN:      /* function returns */
    assert((amx->flags & (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE))==AMX_FLAG_DEBUG);
    dprintf("Function returns: %ld\n",(long)amx->dbgparam);
    calllevel--;
    delete_symbol(&vartab,amx->stk,calllevel);
    if (tracelevel>calllevel)
      tracelevel=calllevel;
    break;
  case DBG_TERMINATE:   /* program ends, either because the "entry point"
                         * function returns, or because of a hard exit */
    assert((amx->flags & (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE))==AMX_FLAG_DEBUG);
    if (amx->dbgparam==AMX_ERR_SLEEP) {
      dprintf("AMX put to sleep: tag=%ld, value=%ld\n",(long)amx->alt,(long)amx->pri);
    } else {
      calllevel=0;
      tracelevel=0;
    } /* if */
    curfileno=-1;
    /* ??? save breakpoints on exit */
    /* ??? save watches */
    /* ??? save terminal type */
    break;
  } /* switch */
  return AMX_ERR_NONE;
}

#if !defined amx_Init

static void *loadprogram(AMX *amx,char *filename)
{
  FILE *fp;
  AMX_HEADER hdr;
  void *program = NULL;

  if ((fp = fopen(filename,"rb")) != NULL) {
    fread(&hdr, sizeof hdr, 1, fp);
    if ((program = malloc((int)hdr.stp)) != NULL) {
      rewind(fp);
      fread(program, 1, (int)hdr.size, fp);
      fclose(fp);
      memset(amx, 0, sizeof *amx);
      amx_SetDebugHook(amx, amx_InternalDebugProc);
      if (amx_Init(amx,program) == AMX_ERR_NONE)
        return program;
      free(program);
    } /* if */
  } /* if */
  return NULL;
}

#if !defined _MAX_PATH
  #if defined MAXPATH
    #define _MAX_PATH MAXPATH
  #elif defined MAX_PATH
    #define _MAX_PATH MAX_PATH
  #else
    #define _MAX_PATH 128
  #endif
#endif

int main(int argc,char *argv[])
{
extern AMX_NATIVE_INFO core_Natives[];
extern AMX_NATIVE_INFO console_Natives[];
extern void core_Init(void);
extern void core_Exit(void);
#if defined(FIXEDPOINT)
  extern AMX_NATIVE_INFO fixed_Natives[];
#endif
#if defined FLOATPOINT
  extern AMX_NATIVE_INFO float_Natives[];
#endif

  AMX amx;
  cell ret;
  int err,i;
  void *program;
  unsigned short flags;
  char filename[_MAX_PATH],*ptr;

  dprintf("Small command line debugger\n\n");
  /* get filename */
  if (argc == 2) {
    strcpy(filename,argv[1]);
  } else {
    dprintf("File: ");
    dgetstr(filename,sizeof filename);
    if ((ptr=strchr(filename,'\n'))!=NULL)
      *ptr='\0';                /* strip newline characters */
  } /* if */
  /* ??? add extension if needed */
  if (strlen(filename)==0)
    return 1;
  if ((program = loadprogram(&amx,filename)) == NULL) {
    dprintf("Cannot load the program file \"%s\"\n");
    return 1;
  } /* if */
  amx_Flags(&amx,&flags);
  if ((flags & AMX_FLAG_DEBUG)==0) {
    dprintf("This program has no debug information\n");
    return 1;
  } /* if */

  signal(SIGINT,sigabort);
  core_Init();

  amx_Register(&amx, core_Natives, -1);
  #if defined FIXEDPOINT
    amx_Register(&amx, fixed_Natives, -1);
  #endif
  #if defined FLOATPOINT
    err = amx_Register(&amx, float_Natives, -1);
  #endif
  err = amx_Register(&amx, console_Natives, -1);

  if (err == AMX_ERR_NONE) {
    err = amx_Exec(&amx, &ret, AMX_EXEC_MAIN, 0);
    while (err == AMX_ERR_SLEEP)
      err = amx_Exec(&amx, &ret, AMX_EXEC_CONT, 0);
  } /* if */

  if (err != AMX_ERR_NONE)
    dprintf("Run time error %d on line %ld\n", err, amx.curline);
  else
    dprintf("Normal termination, return value %ld\n", (long)ret);

  for (i=0; i<MAXFILES; i++)
    if (filenames[i]!=NULL)
      free(filenames[i]);
  if (cursource!=NULL)
    source_free(cursource);
  delete_allsymbols(&functab);
  delete_allsymbols(&vartab);
  free(program);
  core_Exit();

  return 0;
}

#endif /* !defined amx_Init */
