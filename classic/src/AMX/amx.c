/*  Abstract Machine for the Small compiler
 *
 *  Copyright (c) ITB CompuPhase, 1997-2001
 *  This file may be freely used. No warranties of any kind.
 *
 *  Version: $Id: amx.c,v 1.38 2001-11-23 14:47:01+01 thiadmer Exp thiadmer $
 */
#define WIN32_LEAN_AND_MEAN
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#if defined LINUX
  #include <sclinux.h>
#endif
#include "osdefs.h"
#include "amx.h"

#if !defined LINUX  /* Linux GCC already has these types */
  typedef unsigned char  u_char;
#endif

typedef enum {
  OP_NONE,              /* invalid opcode */
  OP_LOAD_PRI,
  OP_LOAD_ALT,
  OP_LOAD_S_PRI,
  OP_LOAD_S_ALT,
  OP_LREF_PRI,
  OP_LREF_ALT,
  OP_LREF_S_PRI,
  OP_LREF_S_ALT,
  OP_LOAD_I,
  OP_LODB_I,
  OP_CONST_PRI,
  OP_CONST_ALT,
  OP_ADDR_PRI,
  OP_ADDR_ALT,
  OP_STOR_PRI,
  OP_STOR_ALT,
  OP_STOR_S_PRI,
  OP_STOR_S_ALT,
  OP_SREF_PRI,
  OP_SREF_ALT,
  OP_SREF_S_PRI,
  OP_SREF_S_ALT,
  OP_STOR_I,
  OP_STRB_I,
  OP_LIDX,
  OP_LIDX_B,
  OP_IDXADDR,
  OP_IDXADDR_B,
  OP_ALIGN_PRI,
  OP_ALIGN_ALT,
  OP_LCTRL,
  OP_SCTRL,
  OP_MOVE_PRI,
  OP_MOVE_ALT,
  OP_XCHG,
  OP_PUSH_PRI,
  OP_PUSH_ALT,
  OP_PUSH_R,
  OP_PUSH_C,
  OP_PUSH,
  OP_PUSH_S,
  OP_POP_PRI,
  OP_POP_ALT,
  OP_STACK,
  OP_HEAP,
  OP_PROC,
  OP_RET,
  OP_RETN,
  OP_CALL,
  OP_CALL_PRI,
  OP_JUMP,
  OP_JREL,
  OP_JZER,
  OP_JNZ,
  OP_JEQ,
  OP_JNEQ,
  OP_JLESS,
  OP_JLEQ,
  OP_JGRTR,
  OP_JGEQ,
  OP_JSLESS,
  OP_JSLEQ,
  OP_JSGRTR,
  OP_JSGEQ,
  OP_SHL,
  OP_SHR,
  OP_SSHR,
  OP_SHL_C_PRI,
  OP_SHL_C_ALT,
  OP_SHR_C_PRI,
  OP_SHR_C_ALT,
  OP_SMUL,
  OP_SDIV,
  OP_SDIV_ALT,
  OP_UMUL,
  OP_UDIV,
  OP_UDIV_ALT,
  OP_ADD,
  OP_SUB,
  OP_SUB_ALT,
  OP_AND,
  OP_OR,
  OP_XOR,
  OP_NOT,
  OP_NEG,
  OP_INVERT,
  OP_ADD_C,
  OP_SMUL_C,
  OP_ZERO_PRI,
  OP_ZERO_ALT,
  OP_ZERO,
  OP_ZERO_S,
  OP_SIGN_PRI,
  OP_SIGN_ALT,
  OP_EQ,
  OP_NEQ,
  OP_LESS,
  OP_LEQ,
  OP_GRTR,
  OP_GEQ,
  OP_SLESS,
  OP_SLEQ,
  OP_SGRTR,
  OP_SGEQ,
  OP_EQ_C_PRI,
  OP_EQ_C_ALT,
  OP_INC_PRI,
  OP_INC_ALT,
  OP_INC,
  OP_INC_S,
  OP_INC_I,
  OP_DEC_PRI,
  OP_DEC_ALT,
  OP_DEC,
  OP_DEC_S,
  OP_DEC_I,
  OP_MOVS,
  OP_CMPS,
  OP_FILL,
  OP_HALT,
  OP_BOUNDS,
  OP_SYSREQ_PRI,
  OP_SYSREQ_C,
  OP_FILE,
  OP_LINE,
  OP_SYMBOL,
  OP_SRANGE,
  OP_JUMP_PRI,
  OP_SWITCH,
  OP_CASETBL,
  OP_SWAP_PRI,
  OP_SWAP_ALT,
  OP_PUSHADDR,
  /* ----- */
  OP_NUM_OPCODES
} OPCODE;

static int amx_LittleEndian = -1;   /* set to TRUE for Little Endian, and
                                     * to FALSE for Big Endian */

static void init_little_endian(void)
{
  if (amx_LittleEndian < 0) {       /* initialize this variable only once */
    uint16_t val=0x00ff;
    u_char *ptr=(u_char *)&val;

    /* "ptr" points to the starting address of "val". If that address
     * holds the byte "0xff", the computer stored the low byte of "val"
     * at the lower address, and so the memory lay out is Little Endian.
     */
    assert(*ptr==0xff || *ptr==0x00);
    amx_LittleEndian= *ptr==0xff;
  } /* if */
}

static void swap16(uint16_t *v)
{
  unsigned char *s = (unsigned char *)v;
  unsigned char t;

  assert(sizeof(*v)==2);
  /* swap two bytes */
  t=s[0];
  s[0]=s[1];
  s[1]=t;
}

static void swap32(uint32_t *v)
{
  unsigned char *s = (unsigned char *)v;
  unsigned char t;

  assert(sizeof(*v)==4);
  /* swap outer two bytes */
  t=s[0];
  s[0]=s[3];
  s[3]=t;
  /* swap inner two bytes */
  t=s[1];
  s[1]=s[2];
  s[2]=t;
}

uint16_t *amx_Align16(uint16_t *v)
{
  assert(sizeof(*v)==2);
  init_little_endian();
  if (!amx_LittleEndian)
    swap16(v);
  return v;
}

uint32_t *amx_Align32(uint32_t *v)
{
  assert(sizeof(cell)==4);
  init_little_endian();
  if (!amx_LittleEndian)
    swap32(v);
  return v;
}

#if defined BIT16
  #define swapcell  swap16
#else
  #define swapcell  swap32
#endif

int AMXAPI amx_Flags(AMX *amx,uint16_t *flags)
{
  AMX_HEADER *hdr;

  *flags=0;
  if (amx==NULL)
    return AMX_ERR_FORMAT;
  hdr=(AMX_HEADER *)amx->base;
  if (hdr->magic!=AMX_MAGIC)
    return AMX_ERR_FORMAT;
  *flags=hdr->flags;
  return AMX_ERR_NONE;
}

int AMXAPI amx_Callback(AMX *amx, cell index, cell *result, cell *params)
{
  AMX_HEADER *hdr=(AMX_HEADER *)amx->base;
  AMX_FUNCSTUB *func=(AMX_FUNCSTUB *)(amx->base+(int)hdr->natives+(int)index*sizeof(AMX_FUNCSTUB));
  AMX_NATIVE f=(AMX_NATIVE)func->address;
  if ( f == NULL )
  {
	printf( "Function not found: %s\n", func->name );
    return AMX_ERR_NOTFOUND;
  }
  assert(f!=NULL);
  assert(index<hdr->num_natives);

  /* Note:
   *   params[0] == number of parameters passed to the native function
   *   params[1] == first argument
   *   etc.
   */

  amx->error=AMX_ERR_NONE;
  *result = f(amx,params);
  return amx->error;
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
int AMXAPI amx_Debug(AMX *amx)
{
  return AMX_ERR_DEBUG;
}

#if defined JIT
  extern int AMXAPI getMaxCodeSize(void);
  extern int AMXAPI asm_runJIT(void *sourceAMXbase, void *jumparray, void *compiledAMXbase);
#endif

#if defined BIT16
  #define JUMPABS(base,ip)      ((cell *)(base+*ip))
  #define RELOC_ABS(base, off)
#else
  #define JUMPABS(base, ip)     ((cell *)*ip)
  #define RELOC_ABS(base, off)  *(ucell *)(base+(int)off) += (ucell)base
#endif

#define DBGPARAM(v)     ( (v)=*(cell *)(code+(int)cip), cip+=sizeof(cell) )

static int amx_BrowseRelocate(AMX *amx)
{
  AMX_HEADER *hdr;
  u_char *code;
  cell cip;
  long codesize;
  OPCODE op;
  int debug;
  int last_sym_global = 0;
  #if defined __GNUC__ || defined ASM32 || defined JIT
    ucell **opcode_list;
  #endif
  #if defined JIT
    int opcode_count = 0;
    int reloc_count = 0;
  #endif


  hdr=(AMX_HEADER *)amx->base;
  code=amx->base+(int)hdr->cod;
  codesize=hdr->dat - hdr->cod;

  /* sanity checks */
  assert(OP_PUSH_PRI==36);
  assert(OP_PROC==46);
  assert(OP_SHL==65);
  assert(OP_SMUL==72);
  assert(OP_EQ==95);
  assert(OP_INC_PRI==107);
  assert(OP_MOVS==117);
  assert(OP_SYMBOL==126);

  /* check the debug hook */
  amx->dbgcode=DBG_INIT;
  assert(amx->flags==0);
  amx->flags=AMX_FLAG_BROWSE;
  debug= amx->debug(amx)==AMX_ERR_NONE;
  if (debug)
    amx->flags=AMX_FLAG_DEBUG | AMX_FLAG_BROWSE;

  #if defined __GNUC__ || defined ASM32 || defined JIT
    amx_Exec(NULL, (cell *)&opcode_list, 0, 0);
  #endif

  /* start browsing code */
  for (cip=0; cip<codesize; ) {
    op=(OPCODE) *(ucell *)(code+(int)cip);
    assert(op>0 && op<OP_NUM_OPCODES);
    if ((int)op>=256)
      return AMX_ERR_INVINSTR;
    #if defined __GNUC__ || defined ASM32 || defined JIT
      /* relocate symbol */
      *(ucell **)(code+(int)cip) = opcode_list[op];
    #endif
    #if defined JIT
      opcode_count++;
    #endif
    cip+=sizeof(cell);
    switch (op) {
    case OP_LOAD_PRI:   /* instructions with 1 parameter */
    case OP_LOAD_ALT:
    case OP_LOAD_S_PRI:
    case OP_LOAD_S_ALT:
    case OP_LREF_PRI:
    case OP_LREF_ALT:
    case OP_LREF_S_PRI:
    case OP_LREF_S_ALT:
    case OP_LODB_I:
    case OP_CONST_PRI:
    case OP_CONST_ALT:
    case OP_ADDR_PRI:
    case OP_ADDR_ALT:
    case OP_STOR_PRI:
    case OP_STOR_ALT:
    case OP_STOR_S_PRI:
    case OP_STOR_S_ALT:
    case OP_SREF_PRI:
    case OP_SREF_ALT:
    case OP_SREF_S_PRI:
    case OP_SREF_S_ALT:
    case OP_STRB_I:
    case OP_LIDX_B:
    case OP_IDXADDR_B:
    case OP_ALIGN_PRI:
    case OP_ALIGN_ALT:
    case OP_LCTRL:
    case OP_SCTRL:
    case OP_PUSH_R:
    case OP_PUSH_C:
    case OP_PUSH:
    case OP_PUSH_S:
    case OP_STACK:
    case OP_HEAP:
    case OP_JREL:
    case OP_SHL_C_PRI:
    case OP_SHL_C_ALT:
    case OP_SHR_C_PRI:
    case OP_SHR_C_ALT:
    case OP_ADD_C:
    case OP_SMUL_C:
    case OP_ZERO:
    case OP_ZERO_S:
    case OP_EQ_C_PRI:
    case OP_EQ_C_ALT:
    case OP_INC:
    case OP_INC_S:
    case OP_DEC:
    case OP_DEC_S:
    case OP_MOVS:
    case OP_CMPS:
    case OP_FILL:
    case OP_HALT:
    case OP_BOUNDS:
    case OP_SYSREQ_C:
    case OP_PUSHADDR:
      cip+=sizeof(cell);
      break;

    case OP_LOAD_I:     /* instructions without parameters */
    case OP_STOR_I:
    case OP_LIDX:
    case OP_IDXADDR:
    case OP_MOVE_PRI:
    case OP_MOVE_ALT:
    case OP_XCHG:
    case OP_PUSH_PRI:
    case OP_PUSH_ALT:
    case OP_POP_PRI:
    case OP_POP_ALT:
    case OP_PROC:
    case OP_RET:
    case OP_RETN:
    case OP_CALL_PRI:
    case OP_SHL:
    case OP_SHR:
    case OP_SSHR:
    case OP_SMUL:
    case OP_SDIV:
    case OP_SDIV_ALT:
    case OP_UMUL:
    case OP_UDIV:
    case OP_UDIV_ALT:
    case OP_ADD:
    case OP_SUB:
    case OP_SUB_ALT:
    case OP_AND:
    case OP_OR:
    case OP_XOR:
    case OP_NOT:
    case OP_NEG:
    case OP_INVERT:
    case OP_ZERO_PRI:
    case OP_ZERO_ALT:
    case OP_SIGN_PRI:
    case OP_SIGN_ALT:
    case OP_EQ:
    case OP_NEQ:
    case OP_LESS:
    case OP_LEQ:
    case OP_GRTR:
    case OP_GEQ:
    case OP_SLESS:
    case OP_SLEQ:
    case OP_SGRTR:
    case OP_SGEQ:
    case OP_INC_PRI:
    case OP_INC_ALT:
    case OP_INC_I:
    case OP_DEC_PRI:
    case OP_DEC_ALT:
    case OP_DEC_I:
    case OP_SYSREQ_PRI:
    case OP_JUMP_PRI:
    case OP_SWAP_PRI:
    case OP_SWAP_ALT:
      break;

    case OP_CALL:       /* opcodes that need relocation */
    case OP_JUMP:
    case OP_JZER:
    case OP_JNZ:
    case OP_JEQ:
    case OP_JNEQ:
    case OP_JLESS:
    case OP_JLEQ:
    case OP_JGRTR:
    case OP_JGEQ:
    case OP_JSLESS:
    case OP_JSLEQ:
    case OP_JSGRTR:
    case OP_JSGEQ:
    case OP_SWITCH:
      #if defined JIT
        reloc_count++;
      #endif
	  RELOC_ABS(code, cip);
      cip+=sizeof(cell);
      break;

    case OP_FILE: {
      cell num;
      DBGPARAM(num);
      DBGPARAM(amx->curfile);
      amx->dbgname=(char *)(code+(int)cip);
      cip+=num - sizeof(cell);
      if (debug) {
        assert(amx->flags==(AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
        amx->dbgcode=DBG_FILE;
        amx->debug(amx);
      } /* if */
      break;
    } /* case */
    case OP_LINE:
      DBGPARAM(amx->curline);
      DBGPARAM(amx->curfile);
      if (debug) {
        assert(amx->flags==(AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
        amx->dbgcode=DBG_LINE;
        amx->debug(amx);
      } /* if */
      break;
    case OP_SYMBOL: {
      cell num;
      DBGPARAM(num);
      DBGPARAM(amx->dbgaddr);
      DBGPARAM(amx->dbgparam);
      amx->dbgname=(char *)(code+(int)cip);
      cip+=num - 2*sizeof(cell);
      last_sym_global = (amx->dbgparam >> 8)==0;
      if (debug && last_sym_global) { /* do global symbols only */
        assert(amx->flags==(AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
        amx->dbgcode=DBG_SYMBOL;
        amx->debug(amx);
      } /* if */
      break;
    } /* case */
    case OP_SRANGE:
      DBGPARAM(amx->dbgaddr);   /* dimension level */
      DBGPARAM(amx->dbgparam);  /* length */
      if (debug && last_sym_global) { /* do global symbols only */
        assert(amx->flags==(AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
        amx->dbgcode=DBG_SRANGE;
        amx->debug(amx);
      } /* if */
      break;
    case OP_CASETBL: {
      cell num;
      int i;
      DBGPARAM(num);    /* number of records follows the opcode */
      for (i=0; i<=num; i++) {
		RELOC_ABS(code, cip+2*i*sizeof(cell));
        #if defined JIT
          reloc_count++;
        #endif
      } /* for */
      cip+=(2*num + 1)*sizeof(cell);
      break;
    } /* case */
    default:
      return AMX_ERR_INVINSTR;
    } /* switch */
  } /* for */

  #if defined JIT
    amx->code_size = getMaxCodeSize()*opcode_count + hdr->cod
                     + (hdr->stp - hdr->dat) + 4;
    amx->reloc_size = 2*sizeof(cell)*reloc_count;
  #endif

  amx->flags |= AMX_FLAG_RELOC;
  return AMX_ERR_NONE;
}

static void expand(unsigned char *code, long codesize, long memsize)
{
  ucell c;
  int shift;

  /* for in-place expansion, move from the end backward */
  assert(memsize % sizeof(cell) == 0);
  while (codesize>0) {
    c=0;
    shift=0;
    do {
      codesize--;
      assert(shift>0 || (code[(int)codesize] & 0x80)==0);
      c|=(ucell)(code[(int)codesize] & 0x7f) << shift;
      shift+=7;
    } while (codesize>0 && (code[(int)codesize-1] & 0x80)!=0);
    /* sign expand */
    if ((code[(int)codesize] & 0x40)!=0) {
      while (shift < 8*sizeof(cell)) {
        c|=(ucell)0xff << shift;
        shift+=8;
      } /* while */
    } /* if */
    /* store */
    memsize -= sizeof(cell);
    *(ucell *)(code+(int)memsize)=c;
  } /* while */
  /* when all bytes have been expaned, the complete memory block should be done */
  assert(memsize==0);
}

int AMXAPI amx_Init(AMX *amx,void *program)
{
  AMX_HEADER *hdr;

  amx->firstrun = 1;
  hdr=(AMX_HEADER *)program;
  /* the header is in Little Endian, on a Big Endian machine, swap all
   * multi-byte words
   */
  init_little_endian();
  if (!amx_LittleEndian) {
    amx_Align32((uint32_t*)&hdr->size);
    amx_Align16(&hdr->magic);
    amx_Align16((uint16_t*)&hdr->flags);
    amx_Align16((uint16_t*)&hdr->defsize);
    amx_Align32((uint32_t*)&hdr->cod);
    amx_Align32((uint32_t*)&hdr->dat);
    amx_Align32((uint32_t*)&hdr->hea);
    amx_Align32((uint32_t*)&hdr->stp);
    amx_Align32((uint32_t*)&hdr->cip);
    amx_Align16((uint16_t*)&hdr->num_publics);
    amx_Align32((uint32_t*)&hdr->publics);
    amx_Align16((uint16_t*)&hdr->num_natives);
    amx_Align32((uint32_t*)&hdr->natives);
    amx_Align16((uint16_t*)&hdr->num_libraries);
    amx_Align32((uint32_t*)&hdr->libraries);
    amx_Align16((uint16_t*)&hdr->num_pubvars);
    amx_Align32((uint32_t*)&hdr->pubvars);
    amx_Align16((uint16_t*)&hdr->num_tags);
    amx_Align32((uint32_t*)&hdr->tags);
  } /* if */

  if (hdr->magic!=AMX_MAGIC || hdr->defsize!=sizeof(AMX_FUNCSTUB))
    return AMX_ERR_FORMAT;
  if (hdr->amx_version<MIN_FILE_VERSION)
    return AMX_ERR_VERSION;
  if (hdr->stp<=0)
    return AMX_ERR_FORMAT;
  if ((hdr->flags & AMX_FLAG_CHAR16)!=0)
    return AMX_ERR_FORMAT;      /* 16-bit characters currently not supported */
  if (!amx_LittleEndian && (hdr->flags & AMX_FLAG_COMPACT)==0)
    return AMX_ERR_FORMAT;      /* On Big Endian machines, use compact encoding */
  assert((hdr->flags & AMX_FLAG_COMPACT)!=0 || hdr->hea == hdr->size);
  if ((hdr->flags & AMX_FLAG_COMPACT)!=0)
    expand((unsigned char *)program+(int)hdr->cod,
           hdr->size - hdr->cod, hdr->hea - hdr->cod);

  amx->base=(unsigned char *)program;

  /* Set a zero cell at the top of the stack, which functions
   * as a sentinel for strings.
   */
  hdr->stp -= sizeof(cell);
  * (cell *)(amx->base+(int)hdr->stp) = 0;

  /* set initial values */
  amx->hlw=hdr->hea - hdr->dat; /* stack and heap relative to data segment */
  amx->stp=hdr->stp - hdr->dat;
  amx->hea=amx->hlw;
  amx->stk=amx->stp;
  if (amx->callback==NULL)
    amx->callback=amx_Callback;
  if (amx->debug==NULL)
    amx->debug=amx_Debug;
  amx->curline=0;
  amx->curfile=0;

  /* also align all addresses in the public function, public variable and
   * public tag tables
   */
  if (!amx_LittleEndian) {
    AMX_FUNCSTUB *fs;
    int i;

    fs=(AMX_FUNCSTUB *)(amx->base + (int)hdr->publics);
    for (i=0; i<hdr->num_publics; i++) {
      amx_Align32(&fs->address);
      fs++;
    } /* for */
    fs=(AMX_FUNCSTUB *)(amx->base + (int)hdr->pubvars);
    for (i=0; i<hdr->num_pubvars; i++) {
      amx_Align32(&fs->address);
      fs++;
    } /* for */
    fs=(AMX_FUNCSTUB *)(amx->base + (int)hdr->tags);
    for (i=0; i<hdr->num_tags; i++) {
      amx_Align32(&fs->address);
      fs++;
    } /* for */
  } /* if */

  /* relocate call and jump instructions, optionally gather debug information */
  amx_BrowseRelocate(amx);

  return AMX_ERR_NONE;
}

#if defined JIT

#if defined __WIN32__   /* this also applies to Win32 "console" applications */

int memoryFullAccess( void* addr, int len )
{
  uint32_t op;

  if ( VirtualProtect( addr, len, PAGE_EXECUTE_READWRITE, &op ) )
    return op;
  return 0;
}

int memorySetAccess( void* addr, int len, int access )
{
  uint32_t op;

  if ( access == 0 )
    return 0;
  return VirtualProtect( addr, len, access, &op );
}

#else /* #if defined __WIN32 __ */

// TODO: Add cases for Linux, Unix, OS/2, ...

// DOS32 has no imposed limits on its segments.
#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
int memoryFullAccess( void* addr, int len ) { return 1; }

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
int memorySetAccess( void* addr, int len, int access ) { return 1; }

#endif /* #if defined __WIN32 __ */

int AMXAPI amx_InitJIT(AMX *amx, void *reloc_table, void *native_code)
{
  int mac, res;
  AMX_HEADER *hdr;

  mac = memoryFullAccess( asm_runJIT, 20000 );
  if ( ! mac )
    return AMX_ERR_INIT_JIT;

  /* copy the prefix */
  memcpy( native_code, amx->base, ((AMX_HEADER *)(amx->base))->cod );
  hdr = native_code;

  /* JIT rulz! (TM) */
  /* MP: added check for correct compilation */
  res = asm_runJIT( amx->base, reloc_table, native_code );
  if ( res != 0 )
  {
    memorySetAccess( asm_runJIT, 20000, mac );
    return AMX_ERR_INIT_JIT;
  }

  /* update the required memory size (the previous value was a
   * conservative estimate, now we know the exact size)
   */
  amx->code_size = (4 + hdr->dat + hdr->stp) & ~3;
  /* The compiled code is relocatable, since only relative jumps are
   * used for destinations within the generated code and absoulute
   * addresses for jumps into the runtime, which is fixed in memory.
   */
  amx->base = (unsigned char*) native_code;
  amx->cip = hdr->cip;
  amx->hea = hdr->hea;
  amx->stp = hdr->stp;
  /* also put a sentinel for strings at the top the stack */
  * (cell *) ( (char*)native_code + hdr->dat + hdr->stp ) = 0;
  amx->stk = amx->stp;

  memorySetAccess( asm_runJIT, 20000, mac );
  return AMX_ERR_NONE;
}

#else /* #if defined JIT */

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
int AMXAPI amx_InitJIT(AMX *amx,void *compiled_program,void *reloc_table)
{
  return AMX_ERR_INIT_JIT;
}

#endif  /* #if defined JIT */

int AMXAPI amx_NameLength(AMX *amx, int *length)
{
  AMX_HEADER *hdr=(AMX_HEADER *)amx->base;
  assert(hdr!=NULL);
  *length=hdr->defsize - sizeof(uint32_t);
  return AMX_ERR_NONE;
}

int AMXAPI amx_NumPublics(AMX *amx, int *number)
{
  AMX_HEADER *hdr=(AMX_HEADER *)amx->base;
  assert(hdr!=NULL);
  *number=hdr->num_publics;
  return AMX_ERR_NONE;
}

int AMXAPI amx_GetPublic(AMX *amx, int index, char *funcname)
{
  AMX_HEADER *hdr;
  AMX_FUNCSTUB *func;

  hdr=(AMX_HEADER *)amx->base;
  assert(hdr!=NULL);
  if (index>=hdr->num_publics)
    return AMX_ERR_INDEX;

  func=(AMX_FUNCSTUB *)(amx->base+(int)hdr->publics+index*sizeof(AMX_FUNCSTUB));
  strcpy(funcname,func->name);
  return AMX_ERR_NONE;
}

int AMXAPI amx_FindPublic(AMX *amx, char *name, int *index)
{
  int first,last,mid,result;
  char pname[sEXPMAX+1];

  amx_NumPublics(amx, &last);
  last--;       /* last valid index is 1 less than the number of functions */
  first=0;
  /* binary search */
  while (first<=last) {
    mid=(first+last)/2;
    amx_GetPublic(amx, mid, pname);
    result=strcmp(pname,name);
    if (result>0) {
      last=mid-1;
    } else if (result<0) {
      first=mid+1;
    } else {
      *index=mid;
      return AMX_ERR_NONE;
    } /* if */
  } /* while */
  /* not found, set to an invalid index, so amx_Exec() will fail */
  *index=INT_MAX;
  return AMX_ERR_NOTFOUND;
}

int AMXAPI amx_NumPubVars(AMX *amx, int *number)
{
  AMX_HEADER *hdr=(AMX_HEADER *)amx->base;
  assert(hdr!=NULL);
  *number=hdr->num_pubvars;
  return AMX_ERR_NONE;
}

int AMXAPI amx_GetPubVar(AMX *amx, int index, char *varname, cell *amx_addr)
{
  AMX_HEADER *hdr;
  AMX_FUNCSTUB *var;

  hdr=(AMX_HEADER *)amx->base;
  assert(hdr!=NULL);
  if (index>=hdr->num_pubvars)
    return AMX_ERR_INDEX;

  var=(AMX_FUNCSTUB *)(amx->base+(int)hdr->pubvars+index*sizeof(AMX_FUNCSTUB));
  strcpy(varname,var->name);
  *amx_addr=var->address;
  return AMX_ERR_NONE;
}

int AMXAPI amx_FindPubVar(AMX *amx, char *varname, cell *amx_addr)
{
  int first,last,mid,result;
  char pname[sEXPMAX+1];
  cell paddr;

  amx_NumPubVars(amx, &last);
  last--;       /* last valid index is 1 less than the number of functions */
  first=0;
  /* binary search */
  while (first<=last) {
    mid=(first+last)/2;
    amx_GetPubVar(amx, mid, pname, &paddr);
    result=strcmp(pname,varname);
    if (result>0) {
      last=mid-1;
    } else if (result<0) {
      first=mid+1;
    } else {
      *amx_addr=paddr;
      return AMX_ERR_NONE;
    } /* if */
  } /* while */
  /* not found */
  *amx_addr=0;
  return AMX_ERR_NOTFOUND;
}

int AMXAPI amx_NumTags(AMX *amx, int *number)
{
  AMX_HEADER *hdr=(AMX_HEADER *)amx->base;
  assert(hdr!=NULL);
  if (hdr->file_version<5) {    /* the tagname table appeared in file format 5 */
    *number=0;
    return AMX_ERR_VERSION;
  } /* if */
  *number=hdr->num_tags;
  return AMX_ERR_NONE;
}

int AMXAPI amx_GetTag(AMX *amx, int index, char *tagname, cell *tag_id)
{
  AMX_HEADER *hdr;
  AMX_FUNCSTUB *tag;

  hdr=(AMX_HEADER *)amx->base;
  assert(hdr!=NULL);
  if (hdr->file_version<5) {    /* the tagname table appeared in file format 5 */
    *tagname='\0';
    *tag_id=0;
    return AMX_ERR_VERSION;
  } /* if */

  if (index>=hdr->num_tags)
    return AMX_ERR_INDEX;

  tag=(AMX_FUNCSTUB *)(amx->base+(int)hdr->tags+index*sizeof(AMX_FUNCSTUB));
  strcpy(tagname,tag->name);
  *tag_id=tag->address;
  return AMX_ERR_NONE;
}

int AMXAPI amx_FindTagId(AMX *amx, cell tag_id, char *tagname)
{
  int first,last,mid;
  cell mid_id;

  #if !defined NDEBUG
    /* verify that the tagname table is sorted on the tag_id */
    amx_NumTags(amx, &last);
    if (last>0) {
      cell cur_id;
      amx_GetTag(amx,0,tagname,&cur_id);
      for (first=1; first<last; first++) {
        amx_GetTag(amx,first,tagname,&mid_id);
        assert(cur_id<mid_id);
        cur_id=mid_id;
      } /* for */
    } /* if */
  #endif

  amx_NumTags(amx, &last);
  last--;       /* last valid index is 1 less than the number of functions */
  first=0;
  /* binary search */
  while (first<=last) {
    mid=(first+last)/2;
    amx_GetTag(amx,mid,tagname,&mid_id);
    if (mid_id>tag_id)
      last=mid-1;
    else if (mid_id<tag_id)
      first=mid+1;
    else
      return AMX_ERR_NONE;
  } /* while */
  /* not found */
  *tagname='\0';
  return AMX_ERR_NOTFOUND;
}

int AMXAPI amx_GetUserData(AMX *amx, long tag, void **ptr)
{
  int index;

  assert(amx!=NULL);
  assert(tag!=0);
  for (index=0; index<AMX_USERNUM && amx->usertags[index]!=tag; index++)
    /* nothing */;
  if (index>=AMX_USERNUM)
    return AMX_ERR_USERDATA;
  *ptr=amx->userdata[index];
  return AMX_ERR_NONE;
}

int AMXAPI amx_SetUserData(AMX *amx, long tag, void *ptr)
{
  int index;

  assert(amx!=NULL);
  assert(tag!=0);
  /* try to find existing tag */
  for (index=0; index<AMX_USERNUM && amx->usertags[index]!=tag; index++)
    /* nothing */;
  /* if not found, try to find empty tag */
  if (index>=AMX_USERNUM)
    for (index=0; index<AMX_USERNUM && amx->usertags[index]!=0; index++)
      /* nothing */;
  /* if still not found, quit with error */
  if (index>=AMX_USERNUM)
    return AMX_ERR_INDEX;
  /* set the tag and the value */
  amx->usertags[index]=tag;
  amx->userdata[index]=ptr;
  return AMX_ERR_NONE;
}

static AMX_NATIVE findfunction(char *name, AMX_NATIVE_INFO *list, int number)
{
  int i;

  for (i=0; list[i].name!=NULL && (i<number || number==-1); i++)
    if (strcmp(name,list[i].name)==0)
      return list[i].func;
  return NULL;
}

int AMXAPI amx_Register(AMX *amx, AMX_NATIVE_INFO *list, int number)
{
  AMX_FUNCSTUB *func;
  AMX_HEADER *hdr;
  int i,err;
  AMX_NATIVE funcptr;

  hdr=(AMX_HEADER *)amx->base;
  assert(hdr!=NULL);

  err=AMX_ERR_NONE;
  func=(AMX_FUNCSTUB *)(amx->base+(int)hdr->natives);
  for (i=0; i<hdr->num_natives; i++) {
    if (func->address==0) {
      /* this function is not yet located */
      funcptr=findfunction(func->name,list,number);
      if (funcptr!=NULL)
        func->address=(uint32_t)funcptr;
      else
        err=AMX_ERR_NOTFOUND;
    } /* if */
    func++;
  } /* for */
  return err;
}

AMX_NATIVE_INFO * AMXAPI amx_NativeInfo(char *name,AMX_NATIVE func)
{
  static AMX_NATIVE_INFO n;
  n.name=name;
  n.func=func;
  return &n;
}

#define GETPARAM(v)     ( v=*(cell *)cip++ )
#define PUSH(v)         ( stk-=sizeof(cell), *(cell *)(data+(int)stk)=v )
#define POP(v)          ( v=*(cell *)(data+(int)stk), stk+=sizeof(cell) )
#define ABORT(amx,v)    { (amx)->stk=reset_stk; (amx)->hea=reset_hea; return v; }

#define STKMARGIN       ((cell)(16*sizeof(cell)))
#define CHKMARGIN()     if (hea+STKMARGIN>stk) return AMX_ERR_STACKERR
#define CHKSTACK()      if (stk>amx->stp) return AMX_ERR_STACKLOW
#define CHKHEAP()       if (hea<amx->hlw) return AMX_ERR_HEAPLOW

#if defined __GNUC__
    /* GNU C version uses the "labels as values" extension to create
     * fast "indirect threaded" interpreter.
     */

#define NEXT(cip)       goto **cip++


int AMXAPI amx_Exec(AMX *amx, cell *retval, int index, int numparams, ...)
{
static void *labels[] = {
        &&op_none,      &&op_load_pri,  &&op_load_alt,  &&op_load_s_pri,
        &&op_load_s_alt,&&op_lref_pri,  &&op_lref_alt,  &&op_lref_s_pri,
        &&op_lref_s_alt,&&op_load_i,    &&op_lodb_i,    &&op_const_pri,
        &&op_const_alt, &&op_addr_pri,  &&op_addr_alt,  &&op_stor_pri,
        &&op_stor_alt,  &&op_stor_s_pri,&&op_stor_s_alt,&&op_sref_pri,
        &&op_sref_alt,  &&op_sref_s_pri,&&op_sref_s_alt,&&op_stor_i,
        &&op_strb_i,    &&op_lidx,      &&op_lidx_b,    &&op_idxaddr,
        &&op_idxaddr_b, &&op_align_pri, &&op_align_alt, &&op_lctrl,
        &&op_sctrl,     &&op_move_pri,  &&op_move_alt,  &&op_xchg,
        &&op_push_pri,  &&op_push_alt,  &&op_push_r,    &&op_push_c,
        &&op_push,      &&op_push_s,    &&op_pop_pri,   &&op_pop_alt,
        &&op_stack,     &&op_heap,      &&op_proc,      &&op_ret,
        &&op_retn,      &&op_call,      &&op_call_pri,  &&op_jump,
        &&op_jrel,      &&op_jzer,      &&op_jnz,       &&op_jeq,
        &&op_jneq,      &&op_jless,     &&op_jleq,      &&op_jgrtr,
        &&op_jgeq,      &&op_jsless,    &&op_jsleq,     &&op_jsgrtr,
        &&op_jsgeq,     &&op_shl,       &&op_shr,       &&op_sshr,
        &&op_shl_c_pri, &&op_shl_c_alt, &&op_shr_c_pri, &&op_shr_c_alt,
        &&op_smul,      &&op_sdiv,      &&op_sdiv_alt,  &&op_umul,
        &&op_udiv,      &&op_udiv_alt,  &&op_add,       &&op_sub,
        &&op_sub_alt,   &&op_and,       &&op_or,        &&op_xor,
        &&op_not,       &&op_neg,       &&op_invert,    &&op_add_c,
        &&op_smul_c,    &&op_zero_pri,  &&op_zero_alt,  &&op_zero,
        &&op_zero_s,    &&op_sign_pri,  &&op_sign_alt,  &&op_eq,
        &&op_neq,       &&op_less,      &&op_leq,       &&op_grtr,
        &&op_geq,       &&op_sless,     &&op_sleq,      &&op_sgrtr,
        &&op_sgeq,      &&op_eq_c_pri,  &&op_eq_c_alt,  &&op_inc_pri,
        &&op_inc_alt,   &&op_inc,       &&op_inc_s,     &&op_inc_i,
        &&op_dec_pri,   &&op_dec_alt,   &&op_dec,       &&op_dec_s,
        &&op_dec_i,     &&op_movs,      &&op_cmps,      &&op_fill,
        &&op_halt,      &&op_bounds,    &&op_sysreq_pri,&&op_sysreq_c,
        &&op_file,      &&op_line,      &&op_symbol,    &&op_srange,
        &&op_jump_pri,  &&op_switch,    &&op_casetbl,   &&op_swap_pri,
        &&op_swap_alt,  &&op_pushaddr };
  AMX_HEADER *hdr;
  AMX_FUNCSTUB *func;
  u_char *code, *data;
  cell pri,alt,stk,frm,hea;
  cell reset_stk, reset_hea, *cip;
  cell offs;
  int num,i;
  va_list ap;
  int debug;

  /* HACK: return label table (for amx_BrowseRelocate) if amx structure
   * is not passed.
   */
  if (amx==NULL) {
    assert(sizeof(cell)==sizeof(void *));
    assert(retval!=NULL);
    *retval=(cell)labels;
    return 0;
  } /* if */

  if (amx->callback==NULL)
    return AMX_ERR_CALLBACK;

  amx->flags &= ~AMX_FLAG_BROWSE;
  if ((amx->flags & AMX_FLAG_RELOC)==0)
    return AMX_ERR_INIT;
  debug= (amx->flags & AMX_FLAG_DEBUG)!=0;

  /* set up the registers */
  hdr=(AMX_HEADER *)amx->base;
  code=amx->base+(int)hdr->cod;
  data=amx->base+(int)hdr->dat;
  hea=amx->hea;
  stk=amx->stk;
  reset_stk=stk;
  reset_hea=hea;
  frm=0;        /* just to avoid compiler warnings */

  /* get the start address */
  if (index==AMX_EXEC_MAIN) {
    if (hdr->cip<0)
      return AMX_ERR_INDEX;
    cip=(cell *)(code + (int)hdr->cip);
  } else if (index==AMX_EXEC_CONT) {
    /* all registers: pri, alt, frm, cip, hea, stk, reset_stk, reset_hea */
    frm=amx->frm;
    stk=amx->stk;
    hea=amx->hea;
    pri=amx->pri;
    alt=amx->alt;
    reset_stk=amx->reset_stk;
    reset_hea=amx->reset_hea;
    cip=(cell *)(code + (int)amx->cip);
  } else if (index<0) {
    return AMX_ERR_INDEX;
  } else {
    if (index>=hdr->num_publics)
      return AMX_ERR_INDEX;
    func=(AMX_FUNCSTUB *)(amx->base + (int)hdr->publics + index*sizeof(AMX_FUNCSTUB));
    cip=(cell *)(code + (int)func->address);
  } /* if */
  /* check values just copied */
  CHKSTACK();
  CHKHEAP();

  if (debug && index!=AMX_EXEC_CONT) {
    /* set the entry point in the debugger by marking a "call" to the
     * exported function
     */
    amx->dbgcode=DBG_CALL;
    amx->dbgaddr=(ucell)((u_char*)cip-code);
    amx->debug(amx);
  } /* if */

  /* sanity checks */
  assert(OP_PUSH_PRI==36);
  assert(OP_PROC==46);
  assert(OP_SHL==65);
  assert(OP_SMUL==72);
  assert(OP_EQ==95);
  assert(OP_INC_PRI==107);
  assert(OP_MOVS==117);
  assert(OP_SYMBOL==126);
  assert(sizeof(cell)==4);

  if (index!=AMX_EXEC_CONT) {
    /* push the parameters to the stack (in reverse order) */
    if( numparams & 0xFFFF0000 ) {
      cell *params;
      stk-=(numparams>>=16)*sizeof(cell);
	  va_start(ap,numparams);
	  params = va_arg(ap,cell*);
	  va_end(ap);
	  for (i=0; i<numparams; i++)
	    *(cell *)(data+(int)stk+i*sizeof(cell))=params[i];
    } else {
      stk-=numparams*sizeof(cell);
      va_start(ap,numparams);
      for (i=0; i<numparams; i++)
        *(cell *)(data+(int)stk+i*sizeof(cell))=va_arg(ap,cell);
      va_end(ap);
    } /* if */
    PUSH(numparams*sizeof(cell));
    PUSH(0);      /* zero return address */
  } /* if */
  /* check stack/heap before starting to run */
  CHKMARGIN();

  /* start running */
  NEXT(cip);

  op_none:
    ABORT(amx,AMX_ERR_INVINSTR);
  op_load_pri:
    GETPARAM(offs);
    pri= * (cell *)(data+(int)offs);
    NEXT(cip);
  op_load_alt:
    GETPARAM(offs);
    alt= * (cell *)(data+(int)offs);
    NEXT(cip);
  op_load_s_pri:
    GETPARAM(offs);
    pri= * (cell *)(data+(int)frm+(int)offs);
    NEXT(cip);
  op_load_s_alt:
    GETPARAM(offs);
    alt= * (cell *)(data+(int)frm+(int)offs);
    NEXT(cip);
  op_lref_pri:
    GETPARAM(offs);
    offs= * (cell *)(data+(int)offs);
    pri= * (cell *)(data+(int)offs);
    NEXT(cip);
  op_lref_alt:
    GETPARAM(offs);
    offs= * (cell *)(data+(int)offs);
    alt= * (cell *)(data+(int)offs);
    NEXT(cip);
  op_lref_s_pri:
    GETPARAM(offs);
    offs= * (cell *)(data+(int)frm+(int)offs);
    pri= * (cell *)(data+(int)offs);
    NEXT(cip);
  op_lref_s_alt:
    GETPARAM(offs);
    offs= * (cell *)(data+(int)frm+(int)offs);
    alt= * (cell *)(data+(int)offs);
    NEXT(cip);
  op_load_i:
    /* verify address */
    if (pri>=hea && pri<stk || pri>=amx->stp)
      ABORT(amx,AMX_ERR_MEMACCESS);
    pri= * (cell *)(data+(int)pri);
    NEXT(cip);
  op_lodb_i:
    GETPARAM(offs);
    /* verify address */
    if (pri>=hea && pri<stk || pri>=amx->stp)
      ABORT(amx,AMX_ERR_MEMACCESS);
    switch (offs) {
    case 1:
      pri= * (data+(int)pri);
      break;
    case 2:
      pri= * (uint16_t *)(data+(int)pri);
      break;
    case 4:
      pri= * (uint32_t *)(data+(int)pri);
      break;
    } /* switch */
    NEXT(cip);
  op_const_pri:
    GETPARAM(pri);
    NEXT(cip);
  op_const_alt:
    GETPARAM(alt);
    NEXT(cip);
  op_addr_pri:
    GETPARAM(pri);
    pri+=frm;
    NEXT(cip);
  op_addr_alt:
    GETPARAM(alt);
    alt+=frm;
    NEXT(cip);
  op_stor_pri:
    GETPARAM(offs);
    *(cell *)(data+(int)offs)=pri;
    NEXT(cip);
  op_stor_alt:
    GETPARAM(offs);
    *(cell *)(data+(int)offs)=alt;
    NEXT(cip);
  op_stor_s_pri:
    GETPARAM(offs);
    *(cell *)(data+(int)frm+(int)offs)=pri;
    NEXT(cip);
  op_stor_s_alt:
    GETPARAM(offs);
    *(cell *)(data+(int)frm+(int)offs)=alt;
    NEXT(cip);
  op_sref_pri:
    GETPARAM(offs);
    offs= * (cell *)(data+(int)offs);
    *(cell *)(data+(int)offs)=pri;
    NEXT(cip);
  op_sref_alt:
    GETPARAM(offs);
    offs= * (cell *)(data+(int)offs);
    *(cell *)(data+(int)offs)=alt;
    NEXT(cip);
  op_sref_s_pri:
    GETPARAM(offs);
    offs= * (cell *)(data+(int)frm+(int)offs);
    *(cell *)(data+(int)offs)=pri;
    NEXT(cip);
  op_sref_s_alt:
    GETPARAM(offs);
    offs= * (cell *)(data+(int)frm+(int)offs);
    *(cell *)(data+(int)offs)=alt;
    NEXT(cip);
  op_stor_i:
    /* verify address */
    if (alt>=hea && alt<stk || alt>=amx->stp)
      ABORT(amx,AMX_ERR_MEMACCESS);
    *(cell *)(data+(int)alt)=pri;
    NEXT(cip);
  op_strb_i:
    GETPARAM(offs);
    /* verify address */
    if (alt>=hea && alt<stk || alt>=amx->stp)
      ABORT(amx,AMX_ERR_MEMACCESS);
    switch (offs) {
    case 1:
      *(data+(int)alt)=(u_char)pri;
      break;
    case 2:
      *(uint16_t *)(data+(int)alt)=(uint16_t)pri;
      break;
    case 4:
      *(uint32_t *)(data+(int)alt)=(uint32_t)pri;
      break;
    } /* switch */
    NEXT(cip);
  op_lidx:
    offs=pri*sizeof(cell)+alt;
    /* verify address */
    if (offs>=hea && offs<stk || offs>=amx->stp)
      ABORT(amx,AMX_ERR_MEMACCESS);
    pri= * (cell *)(data+(int)offs);
    NEXT(cip);
  op_lidx_b:
    GETPARAM(offs);
    offs=(pri << (int)offs)+alt;
    /* verify address */
    if (offs>=hea && offs<stk || offs>=amx->stp)
      ABORT(amx,AMX_ERR_MEMACCESS);
    pri= * (cell *)(data+(int)offs);
    NEXT(cip);
  op_idxaddr:
    pri=pri*sizeof(cell)+alt;
    NEXT(cip);
  op_idxaddr_b:
    GETPARAM(offs);
    pri=(pri << (int)offs)+alt;
    NEXT(cip);
  op_align_pri:
    GETPARAM(offs);
    if (amx_LittleEndian && offs<sizeof(cell))
      pri ^= sizeof(cell)-offs;
    NEXT(cip);
  op_align_alt:
    GETPARAM(offs);
    if (amx_LittleEndian && offs<sizeof(cell))
      alt ^= sizeof(cell)-offs;
    NEXT(cip);
  op_lctrl:
    GETPARAM(offs);
    switch (offs) {
    case 0:
      pri=hdr->cod;
      break;
    case 1:
      pri=hdr->dat;
      break;
    case 2:
      pri=hea;
      break;
    case 3:
      pri=amx->stp;
      break;
    case 4:
      pri=stk;
      break;
    case 5:
      pri=frm;
      break;
    case 6:
      pri=(cell)((u_char *)cip - code);
      break;
    } /* switch */
    NEXT(cip);
  op_sctrl:
    GETPARAM(offs);
    switch (offs) {
    case 0:
    case 1:
    case 3:
      /* cannot change these parameters */
      break;
    case 2:
      hea=pri;
      break;
    case 4:
      stk=pri;
      break;
    case 5:
      frm=pri;
      break;
    case 6:
      cip=(cell *)(code + (int)pri);
      break;
    } /* switch */
    NEXT(cip);
  op_move_pri:
    pri=alt;
    NEXT(cip);
  op_move_alt:
    alt=pri;
    NEXT(cip);
  op_xchg:
    offs=pri;         /* offs is a temporary variable */
    pri=alt;
    alt=offs;
    NEXT(cip);
  op_push_pri:
    PUSH(pri);
    NEXT(cip);
  op_push_alt:
    PUSH(alt);
    NEXT(cip);
  op_push_c:
    GETPARAM(offs);
    PUSH(offs);
    NEXT(cip);
  op_push_r:
    GETPARAM(offs);
    while (offs--)
      PUSH(pri);
    NEXT(cip);
  op_push:
    GETPARAM(offs);
    PUSH(* (cell *)(data+(int)offs));
    NEXT(cip);
  op_push_s:
    GETPARAM(offs);
    PUSH(* (cell *)(data+(int)frm+(int)offs));
    NEXT(cip);
  op_pop_pri:
    POP(pri);
    NEXT(cip);
  op_pop_alt:
    POP(alt);
    NEXT(cip);
  op_stack:
    GETPARAM(offs);
    alt=stk;
    stk+=offs;
    CHKMARGIN();
    CHKSTACK();
    if (debug && offs>0) {
      amx->dbgcode=DBG_CLRSYM;
      amx->stk=stk;
      amx->debug(amx);
    } /* if */
    NEXT(cip);
  op_heap:
    GETPARAM(offs);
    alt=hea;
    hea+=offs;
    CHKMARGIN();
    CHKHEAP();
    NEXT(cip);
  op_proc:
    PUSH(frm);
    frm=stk;
    CHKMARGIN();
    NEXT(cip);
  op_ret:
    POP(frm);
    POP(offs);
    cip=(cell *)(code+(int)offs);
    if (offs==0) {
      /* entry function returns -> end of program */
      if (retval!=NULL)
        *retval=pri;
      if (debug) {
        amx->dbgcode=DBG_TERMINATE;
        amx->dbgparam=0;
        amx->debug(amx);
      } /* if */
      /* The caller must remove the parameters from the stack, but... there
       * is no caller. So reset it the hard way. */
      amx->stk=reset_stk;
      return AMX_ERR_NONE;
    } else {
      if (debug) {
        amx->dbgcode=DBG_RETURN;
        amx->dbgparam=pri;    /* store "return value" */
        amx->debug(amx);
      } /* if */
    } /* if */
    NEXT(cip);
  op_retn:
    POP(frm);
    POP(offs);
    cip=(cell *)(code+(int)offs);
    stk+= *(cell *)(data+(int)stk) + sizeof(cell); /* remove parameters from the stack */
    amx->stk=stk;
    if (offs==0) {
      /* entry function returns -> end of program */
      if (retval!=NULL)
        *retval=pri;
      if (debug) {
        amx->dbgcode=DBG_TERMINATE;
        amx->dbgparam=0;
        amx->debug(amx);
      } /* if */
      assert(amx->stk==reset_stk);
      return AMX_ERR_NONE;
    } else {
      if (debug) {
        amx->dbgcode=DBG_RETURN;
        amx->dbgparam=pri;    /* store "return value" */
        amx->debug(amx);
      } /* if */
    } /* if */
    if (debug) {
      amx->dbgcode=DBG_CLRSYM;
      amx->debug(amx);
    } /* if */
    NEXT(cip);
  op_call:
    PUSH(((u_char *)cip-code)+sizeof(cell));    /* skip address */
    cip=JUMPABS(code, cip);                     /* jump to the address */
    if (debug) {
      amx->dbgcode=DBG_CALL;
      amx->dbgaddr=(ucell)((u_char*)cip-code);
      amx->debug(amx);
    } /* if */
    NEXT(cip);
  op_call_pri:
    PUSH((u_char *)cip-code);
    cip=(cell *)(code+(int)pri);
    if (debug) {
      amx->dbgcode=DBG_CALL;
      amx->dbgaddr=pri;
      amx->debug(amx);
    } /* if */
    NEXT(cip);
  op_jump:
    /* since the GETPARAM() macro modifies cip, you cannot
     * do GETPARAM(cip) directly */
    cip=JUMPABS(code, cip);
    NEXT(cip);
  op_jrel:
    offs=*cip;
    cip=(cell *)((u_char *)cip + (int)offs + sizeof(cell));
    NEXT(cip);
  op_jzer:
    if (pri==0)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jnz:
    if (pri!=0)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jeq:
    if (pri==alt)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jneq:
    if (pri!=alt)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jless:
    if ((ucell)pri < (ucell)alt)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jleq:
    if ((ucell)pri <= (ucell)alt)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jgrtr:
    if ((ucell)pri > (ucell)alt)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jgeq:
    if ((ucell)pri >= (ucell)alt)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jsless:
    if (pri<alt)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jsleq:
    if (pri<=alt)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jsgrtr:
    if (pri>alt)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_jsgeq:
    if (pri>=alt)
      cip=JUMPABS(code, cip);
    else
      cip=(cell *)((u_char *)cip+sizeof(cell));
    NEXT(cip);
  op_shl:
    pri<<=alt;
    NEXT(cip);
  op_shr:
    pri=(ucell)pri >> (ucell)alt;
    NEXT(cip);
  op_sshr:
    pri>>=alt;
    NEXT(cip);
  op_shl_c_pri:
    GETPARAM(offs);
    pri<<=offs;
    NEXT(cip);
  op_shl_c_alt:
    GETPARAM(offs);
    alt<<=offs;
    NEXT(cip);
  op_shr_c_pri:
    GETPARAM(offs);
    pri=(ucell)pri >> (ucell)offs;
    NEXT(cip);
  op_shr_c_alt:
    GETPARAM(offs);
    alt=(ucell)alt >> (ucell)offs;
    NEXT(cip);
  op_smul:
    pri*=alt;
    NEXT(cip);
  op_sdiv:
    if (alt==0)
      ABORT(amx,AMX_ERR_DIVIDE);
    /* divide must always round down; this is a bit
     * involved to do in a machine-independent way.
     */
    offs=(pri % alt + alt) % alt;     /* true modulus */
    pri=(pri - offs) / alt;           /* division result */
    alt=offs;
    NEXT(cip);
  op_sdiv_alt:
    if (pri==0)
      ABORT(amx,AMX_ERR_DIVIDE);
    /* divide must always round down; this is a bit
     * involved to do in a machine-independent way.
     */
    offs=(alt % pri + pri) % pri;     /* true modulus */
    pri=(alt - offs) / pri;           /* division result */
    alt=offs;
    NEXT(cip);
  op_umul:
    pri=(ucell)pri * (ucell)alt;
    NEXT(cip);
  op_udiv:
    if (alt==0)
      ABORT(amx,AMX_ERR_DIVIDE);
    offs=(ucell)pri % (ucell)alt;     /* temporary storage */
    pri=(ucell)pri / (ucell)alt;
    alt=offs;
    NEXT(cip);
  op_udiv_alt:
    if (pri==0)
      ABORT(amx,AMX_ERR_DIVIDE);
    offs=(ucell)alt % (ucell)pri;     /* temporary storage */
    pri=(ucell)alt / (ucell)pri;
    alt=offs;
    NEXT(cip);
  op_add:
    pri+=alt;
    NEXT(cip);
  op_sub:
    pri-=alt;
    NEXT(cip);
  op_sub_alt:
    pri=alt-pri;
    NEXT(cip);
  op_and:
    pri&=alt;
    NEXT(cip);
  op_or:
    pri|=alt;
    NEXT(cip);
  op_xor:
    pri^=alt;
    NEXT(cip);
  op_not:
    pri=!pri;
    NEXT(cip);
  op_neg:
    pri=-pri;
    NEXT(cip);
  op_invert:
    pri=~pri;
    NEXT(cip);
  op_add_c:
    GETPARAM(offs);
    pri+=offs;
    NEXT(cip);
  op_smul_c:
    GETPARAM(offs);
    pri*=offs;
    NEXT(cip);
  op_zero_pri:
    pri=0;
    NEXT(cip);
  op_zero_alt:
    alt=0;
    NEXT(cip);
  op_zero:
    GETPARAM(offs);
    *(cell *)(data+(int)offs)=0;
    NEXT(cip);
  op_zero_s:
    GETPARAM(offs);
    *(cell *)(data+(int)frm+(int)offs)=0;
    NEXT(cip);
  op_sign_pri:
    if ((pri & 0xff)>=0x80)
      pri|= ~ (ucell)0xff;
    NEXT(cip);
  op_sign_alt:
    if ((alt & 0xff)>=0x80)
      alt|= ~ (ucell)0xff;
    NEXT(cip);
  op_eq:
    pri= pri==alt ? 1 : 0;
    NEXT(cip);
  op_neq:
    pri= pri!=alt ? 1 : 0;
    NEXT(cip);
  op_less:
    pri= (ucell)pri < (ucell)alt ? 1 : 0;
    NEXT(cip);
  op_leq:
    pri= (ucell)pri <= (ucell)alt ? 1 : 0;
    NEXT(cip);
  op_grtr:
    pri= (ucell)pri > (ucell)alt ? 1 : 0;
    NEXT(cip);
  op_geq:
    pri= (ucell)pri >= (ucell)alt ? 1 : 0;
    NEXT(cip);
  op_sless:
    pri= pri<alt ? 1 : 0;
    NEXT(cip);
  op_sleq:
    pri= pri<=alt ? 1 : 0;
    NEXT(cip);
  op_sgrtr:
    pri= pri>alt ? 1 : 0;
    NEXT(cip);
  op_sgeq:
    pri= pri>=alt ? 1 : 0;
    NEXT(cip);
  op_eq_c_pri:
    GETPARAM(offs);
    pri= pri==offs ? 1 : 0;
    NEXT(cip);
  op_eq_c_alt:
    GETPARAM(offs);
    pri= alt==offs ? 1 : 0;
    NEXT(cip);
  op_inc_pri:
    pri++;
    NEXT(cip);
  op_inc_alt:
    alt++;
    NEXT(cip);
  op_inc:
    GETPARAM(offs);
    *(cell *)(data+(int)offs) += 1;
    NEXT(cip);
  op_inc_s:
    GETPARAM(offs);
    *(cell *)(data+(int)frm+(int)offs) += 1;
    NEXT(cip);
  op_inc_i:
    *(cell *)(data+(int)pri) += 1;
    NEXT(cip);
  op_dec_pri:
    pri--;
    NEXT(cip);
  op_dec_alt:
    alt--;
    NEXT(cip);
  op_dec:
    GETPARAM(offs);
    *(cell *)(data+(int)offs) -= 1;
    NEXT(cip);
  op_dec_s:
    GETPARAM(offs);
    *(cell *)(data+(int)frm+(int)offs) -= 1;
    NEXT(cip);
  op_dec_i:
    *(cell *)(data+(int)pri) -= 1;
    NEXT(cip);
  op_movs:
    GETPARAM(offs);
    memcpy(data+(int)alt, data+(int)pri, (int)offs);
    NEXT(cip);
  op_cmps:
    GETPARAM(offs);
    pri=memcmp(data+(int)alt, data+(int)pri, (int)offs);
    NEXT(cip);
  op_fill:
    GETPARAM(offs);
    for (i=(int)alt; offs>=sizeof(cell); i+=sizeof(cell), offs-=sizeof(cell))
      *(cell *)(data+i) = pri;
    NEXT(cip);
  op_halt:
    GETPARAM(offs);
    if (retval!=NULL)
      *retval=pri;
    /* store complete status */
    amx->frm=frm;
    amx->stk=stk;
    amx->hea=hea;
    amx->pri=pri;
    amx->alt=alt;
    amx->cip=(cell)((u_char*)cip-code);
    if (debug) {
      amx->dbgcode=DBG_TERMINATE;
      amx->dbgaddr=(cell)((u_char *)cip-code);
      amx->dbgparam=offs;
      amx->debug(amx);
    } /* if */
    if (offs==AMX_ERR_SLEEP) {
      amx->reset_stk=reset_stk;
      amx->reset_hea=reset_hea;
      return (int)offs;
    } /* if */
    ABORT(amx,(int)offs);
  op_bounds:
    GETPARAM(offs);
    if (pri>offs || pri<0)
      ABORT(amx,AMX_ERR_BOUNDS);
    NEXT(cip);
  op_sysreq_pri:
    /* save a few registers */
    amx->cip=(cell)((u_char *)cip-code);
    amx->hea=hea;
    amx->frm=frm;
    amx->stk=stk;
    num=amx->callback(amx,pri,&pri,(cell *)(data+(int)stk));
    if (num!=AMX_ERR_NONE)
      ABORT(amx,num);
    NEXT(cip);
  op_sysreq_c:
    GETPARAM(offs);
    /* save a few registers */
    amx->cip=(cell)((u_char *)cip-code);
    amx->hea=hea;
    amx->frm=frm;
    amx->stk=stk;
    num=amx->callback(amx,offs,&pri,(cell *)(data+(int)stk));
    if (num!=AMX_ERR_NONE)
      ABORT(amx,num);
    NEXT(cip);
  op_file:
    GETPARAM(offs);
    cip=(cell *)((u_char *)cip + (int)offs);
    assert(0);        /* this code should not occur during execution */
    NEXT(cip);
  op_line:
    assert((amx->flags & AMX_FLAG_BROWSE)==0);
    GETPARAM(amx->curline);
    GETPARAM(amx->curfile);
    if (debug) {
      amx->frm=frm;
      amx->stk=stk;
      amx->dbgcode=DBG_LINE;
      num=amx->debug(amx);
      if (num!=AMX_ERR_NONE)
        ABORT(amx,num);
    } /* if */
    NEXT(cip);
  op_symbol:
    assert((amx->flags & AMX_FLAG_BROWSE)==0);
    GETPARAM(offs);
    GETPARAM(amx->dbgaddr);
    GETPARAM(amx->dbgparam);
    amx->dbgname=(char *)cip;
    cip=(cell *)((u_char *)cip + (int)offs - 2*sizeof(cell));
    amx->dbgcode=DBG_SYMBOL;
    assert((amx->dbgparam >> 8)>0);         /* local symbols only */
    if (debug) {
      amx->frm=frm;   /* debugger needs this to relocate the symbols */
      amx->debug(amx);
    } /* if */
    NEXT(cip);
  op_srange:
    assert((amx->flags & AMX_FLAG_BROWSE)==0);
    GETPARAM(amx->dbgaddr);     /* dimension level */
    GETPARAM(amx->dbgparam);    /* length */
    amx->dbgcode=DBG_SRANGE;
    if (debug) {
      amx->frm=frm;   /* debugger needs this to relocate the symbols */
      amx->debug(amx);
    } /* if */
    NEXT(cip);
  op_jump_pri:
    cip=(cell *)(code+(int)pri);
    NEXT(cip);
  op_switch: {
    cell *cptr;

    cptr=(cell *)*cip + 1;      /* +1, to skip the "casetbl" opcode */
    cip=(cell *)*(cptr+1);      /* preset to "none-matched" case */
    num=(int)*cptr;             /* number of records in the case table */
    for (cptr+=2; num>0 && *cptr!=pri; num--,cptr+=2)
      /* nothing */;
    if (num>0)
      cip=(cell *)*(cptr+1);    /* case found */
    NEXT(cip);
    }
  op_casetbl:
    assert(0);          /* this should not occur during execution */
    NEXT(cip);
  op_swap_pri:
    offs=*(cell *)(data+(int)stk);
    *(cell *)(data+(int)stk)=pri;
    pri=offs;
    NEXT(cip);
  op_swap_alt:
    offs=*(cell *)(data+(int)stk);
    *(cell *)(data+(int)stk)=alt;
    alt=offs;
    NEXT(cip);
  op_pushaddr:
    GETPARAM(offs);
    PUSH(frm+offs);
    NEXT(cip);
}

#else
    /* ANSI C & assembler versions */

#if defined ASM32 || defined JIT
  /* For Watcom C/C++ use register calling convention (faster); for
   * Microsoft C/C++ (and possibly others) use __cdecl.
   * The important point is that you assemble AMXEXEC.ASM with the matching
   * calling convention, or the right jit, respectively.
   * jitr.asm is for Watcom's register calling convention, jits.asm for __cdecl.
   */
  #if defined __WATCOMC__
    #if defined amx_Init  /* for AMX32.DLL */
      extern cell amx_exec_asm(cell *regs,cell *retval,cell stp,cell hea);
            /* The following pragma tells the compiler into which registers
             * the parameters have to go. */
		        #pragma aux amx_exec_asm parm [eax] [edx] [ebx] [ecx];
    #else
      extern cell AMXAPI amx_exec_asm(cell *regs,cell *retval,cell stp,cell hea);
	  #endif
  #else
    extern cell __cdecl amx_exec_asm(cell *regs,cell *retval,cell stp,cell hea);
  #endif
#endif

int AMXAPI amx_Exec(AMX *amx, cell *retval, int index, int numparams, ...)
{
  AMX_HEADER *hdr;
  AMX_FUNCSTUB *func;
  u_char *code, *data;
  cell pri,alt,stk,frm,hea;
  cell reset_stk, reset_hea, *cip;
  int i;
  va_list ap;
  int debug;
  #if defined ASM32 || defined JIT
    extern void *amx_opcodelist[];
      #ifdef __WATCOMC__
        #pragma aux amx_opcodelist "_*"
      #endif
    cell  parms[9];     /* MP: registers for assembler AMX */
  #else
    OPCODE op;
    cell offs;
    int num;
  #endif

  #if defined ASM32 || defined JIT
    /* HACK: return label table (for amx_BrowseRelocate) if amx structure
     * is not passed.
     */
    if (amx==NULL) {
      assert(sizeof(cell)==sizeof(void *));
      assert(retval!=NULL);
      *retval=(cell)amx_opcodelist;
      return 0;
    } /* if */
  #endif

  if (amx->callback==NULL)
    return AMX_ERR_CALLBACK;

  amx->flags &= ~AMX_FLAG_BROWSE;
  if ((amx->flags & AMX_FLAG_RELOC)==0)
    return AMX_ERR_INIT;
  debug= (amx->flags & AMX_FLAG_DEBUG)!=0;

  /* set up the registers */
  hdr=(AMX_HEADER *)amx->base;
  code=amx->base+(int)hdr->cod;
  data=amx->base+(int)hdr->dat;
  hea=amx->hea;
  stk=amx->stk;
  reset_stk=stk;
  reset_hea=hea;
  frm=alt=pri=0;        /* silence up compiler */

  /* get the start address */
  if (index==AMX_EXEC_MAIN) {
    if (hdr->cip<0)
      return AMX_ERR_INDEX;
    cip=(cell *)(code + (int)hdr->cip);
  } else if (index==AMX_EXEC_CONT) {
    /* all registers: pri, alt, frm, cip, hea, stk, reset_stk, reset_hea */
    frm=amx->frm;
    stk=amx->stk;
    hea=amx->hea;
    pri=amx->pri;
    alt=amx->alt;
    reset_stk=amx->reset_stk;
    reset_hea=amx->reset_hea;
    cip=(cell *)(code + (int)amx->cip);
  } else if (index<0) {
    return AMX_ERR_INDEX;
  } else {
    if (index>=hdr->num_publics)
      return AMX_ERR_INDEX;
    func=(AMX_FUNCSTUB *)(amx->base + (int)hdr->publics + index*sizeof(AMX_FUNCSTUB));
    cip=(cell *)(code + (int)func->address);
  } /* if */
  /* check values just copied */
  CHKSTACK();
  CHKHEAP();

  if (debug && index!=AMX_EXEC_CONT) {
    /* set the entry point in the debugger by marking a "call" to the
     * exported function
     */
    amx->dbgcode=DBG_CALL;
    amx->dbgaddr=(ucell)((u_char *)cip-code);
    amx->debug(amx);
  } /* if */

  /* sanity checks */
  assert(OP_PUSH_PRI==36);
  assert(OP_PROC==46);
  assert(OP_SHL==65);
  assert(OP_SMUL==72);
  assert(OP_EQ==95);
  assert(OP_INC_PRI==107);
  assert(OP_MOVS==117);
  assert(OP_SYMBOL==126);
  #if defined(BIT16)
    assert(sizeof(cell)==2);
  #else
    assert(sizeof(cell)==4);
  #endif
	
  if (index!=AMX_EXEC_CONT) {
    /* push the parameters to the stack (in reverse order) */
    if (numparams & 0xffff0000L) {
      cell *params;
      stk-=(numparams>>=16)*sizeof(cell);
	  va_start(ap,numparams);
	  params = va_arg(ap,cell*);
	  va_end(ap);
	  for (i=0; i<numparams; i++)
	    *(cell *)(data+(int)stk+i*sizeof(cell))=params[i];
    } else {
      stk-=numparams*sizeof(cell);
      va_start(ap,numparams);
      for (i=0; i<numparams; i++)
        *(cell *)(data+(int)stk+i*sizeof(cell))=va_arg(ap,cell);
      va_end(ap);
    } /* if */
    PUSH(numparams*sizeof(cell));
    PUSH(0);              /* zero return address */
  } /* if */
  /* check stack/heap before starting to run */
  //CHKMARGIN();
  if (hea+STKMARGIN>stk)
	  return AMX_ERR_STACKERR;


  // start running 
#if defined ASM32 || defined JIT
  // either the assembler abstract machine or the JIT; both by Marc Peter 

  parms[0] = pri;
  parms[1] = alt;
  parms[2] = (cell)cip;
  parms[3] = (cell)data;
  parms[4] = stk;
  parms[5] = frm;
  parms[6] = (cell)amx;
  parms[7] = (cell)code;
  parms[8] = (cell)debug;

  i = amx_exec_asm(parms,retval,hdr->stp,hea);
  if (i == AMX_ERR_SLEEP) {
    amx->reset_stk=reset_stk;
    amx->reset_hea=reset_hea;
  } else {
    // remove parameters from the stack; do this the "hard" way, because
    // the assembler version has no internal knowledge of the local
    // variables, so any "clean" way would be a kludge anyway.
    //
    amx->stk=reset_stk;
    amx->hea=reset_hea;
  } 
  return i;

#else

  for ( ;; ) {
    op=(OPCODE) *cip++;
    switch (op) {
    case OP_LOAD_PRI:
      GETPARAM(offs);
      pri= * (cell *)(data+(int)offs);
      break;
    case OP_LOAD_ALT:
      GETPARAM(offs);
      alt= * (cell *)(data+(int)offs);
      break;
    case OP_LOAD_S_PRI:
      GETPARAM(offs);
      pri= * (cell *)(data+(int)frm+(int)offs);
      break;
    case OP_LOAD_S_ALT:
      GETPARAM(offs);
      alt= * (cell *)(data+(int)frm+(int)offs);
      break;
    case OP_LREF_PRI:
      GETPARAM(offs);
      offs= * (cell *)(data+(int)offs);
      pri= * (cell *)(data+(int)offs);
      break;
    case OP_LREF_ALT:
      GETPARAM(offs);
      offs= * (cell *)(data+(int)offs);
      alt= * (cell *)(data+(int)offs);
      break;
    case OP_LREF_S_PRI:
      GETPARAM(offs);
      offs= * (cell *)(data+(int)frm+(int)offs);
      pri= * (cell *)(data+(int)offs);
      break;
    case OP_LREF_S_ALT:
      GETPARAM(offs);
      offs= * (cell *)(data+(int)frm+(int)offs);
      alt= * (cell *)(data+(int)offs);
      break;
    case OP_LOAD_I:
      /* verify address */
      if (pri>=hea && pri<stk || pri>=amx->stp)
        ABORT(amx,AMX_ERR_MEMACCESS);
      pri= * (cell *)(data+(int)pri);
      break;
    case OP_LODB_I:
      GETPARAM(offs);
      /* verify address */
      if (pri>=hea && pri<stk || pri>=amx->stp)
        ABORT(amx,AMX_ERR_MEMACCESS);
      switch (offs) {
      case 1:
        pri= * (data+(int)pri);
        break;
      case 2:
        pri= * (uint16_t *)(data+(int)pri);
        break;
      case 4:
        pri= * (uint32_t *)(data+(int)pri);
        break;
      } /* switch */
      break;
    case OP_CONST_PRI:
      GETPARAM(pri);
      break;
    case OP_CONST_ALT:
      GETPARAM(alt);
      break;
    case OP_ADDR_PRI:
      GETPARAM(pri);
      pri+=frm;
      break;
    case OP_ADDR_ALT:
      GETPARAM(alt);
      alt+=frm;
      break;
    case OP_STOR_PRI:
      GETPARAM(offs);
      *(cell *)(data+(int)offs)=pri;
      break;
    case OP_STOR_ALT:
      GETPARAM(offs);
      *(cell *)(data+(int)offs)=alt;
      break;
    case OP_STOR_S_PRI:
      GETPARAM(offs);
      *(cell *)(data+(int)frm+(int)offs)=pri;
      break;
    case OP_STOR_S_ALT:
      GETPARAM(offs);
      *(cell *)(data+(int)frm+(int)offs)=alt;
      break;
    case OP_SREF_PRI:
      GETPARAM(offs);
      offs= * (cell *)(data+(int)offs);
      *(cell *)(data+(int)offs)=pri;
      break;
    case OP_SREF_ALT:
      GETPARAM(offs);
      offs= * (cell *)(data+(int)offs);
      *(cell *)(data+(int)offs)=alt;
      break;
    case OP_SREF_S_PRI:
      GETPARAM(offs);
      offs= * (cell *)(data+(int)frm+(int)offs);
      *(cell *)(data+(int)offs)=pri;
      break;
    case OP_SREF_S_ALT:
      GETPARAM(offs);
      offs= * (cell *)(data+(int)frm+(int)offs);
      *(cell *)(data+(int)offs)=alt;
      break;
    case OP_STOR_I:
      /* verify address */
      if (alt>=hea && alt<stk || alt>=amx->stp)
        ABORT(amx,AMX_ERR_MEMACCESS);
      *(cell *)(data+(int)alt)=pri;
      break;
    case OP_STRB_I:
      GETPARAM(offs);
      /* verify address */
      if (alt>=hea && alt<stk || alt>=amx->stp)
        ABORT(amx,AMX_ERR_MEMACCESS);
      switch (offs) {
      case 1:
        *(data+(int)alt)=(u_char)pri;
        break;
      case 2:
        *(uint16_t *)(data+(int)alt)=(uint16_t)pri;
        break;
      case 4:
        *(uint32_t *)(data+(int)alt)=(uint32_t)pri;
        break;
      } /* switch */
      break;
    case OP_LIDX:
      offs=pri*sizeof(cell)+alt;
      /* verify address */
      if (offs>=hea && offs<stk || offs>=amx->stp)
        ABORT(amx,AMX_ERR_MEMACCESS);
      pri= * (cell *)(data+(int)offs);
      break;
    case OP_LIDX_B:
      GETPARAM(offs);
      offs=(pri << (int)offs)+alt;
      /* verify address */
      if (offs>=hea && offs<stk || offs>=amx->stp)
        ABORT(amx,AMX_ERR_MEMACCESS);
      pri= * (cell *)(data+(int)offs);
      break;
    case OP_IDXADDR:
      pri=pri*sizeof(cell)+alt;
      break;
    case OP_IDXADDR_B:
      GETPARAM(offs);
      pri=(pri << (int)offs)+alt;
      break;
    case OP_ALIGN_PRI:
      GETPARAM(offs);
      if (amx_LittleEndian && (size_t)offs<sizeof(cell))
        pri ^= sizeof(cell)-offs;
      break;
    case OP_ALIGN_ALT:
      GETPARAM(offs);
      if (amx_LittleEndian && (size_t)offs<sizeof(cell))
        alt ^= sizeof(cell)-offs;
      break;
    case OP_LCTRL:
      GETPARAM(offs);
      switch (offs) {
      case 0:
        pri=hdr->cod;
        break;
      case 1:
        pri=hdr->dat;
        break;
      case 2:
        pri=hea;
        break;
      case 3:
        pri=amx->stp;
        break;
      case 4:
        pri=stk;
        break;
      case 5:
        pri=frm;
        break;
      case 6:
        pri=(cell)((u_char *)cip - code);
        break;
      } /* switch */
      break;
    case OP_SCTRL:
      GETPARAM(offs);
      switch (offs) {
      case 0:
      case 1:
      case 3:
        /* cannot change these parameters */
        break;
      case 2:
        hea=pri;
        break;
      case 4:
        stk=pri;
        break;
      case 5:
        frm=pri;
        break;
      case 6:
        cip=(cell *)(code + (int)pri);
        break;
      } /* switch */
      break;
    case OP_MOVE_PRI:
      pri=alt;
      break;
    case OP_MOVE_ALT:
      alt=pri;
      break;
    case OP_XCHG:
      offs=pri;         /* offs is a temporary variable */
      pri=alt;
      alt=offs;
      break;
    case OP_PUSH_PRI:
      PUSH(pri);
      break;
    case OP_PUSH_ALT:
      PUSH(alt);
      break;
    case OP_PUSH_C:
      GETPARAM(offs);
      PUSH(offs);
      break;
    case OP_PUSH_R:
      GETPARAM(offs);
      while (offs--)
        PUSH(pri);
      break;
    case OP_PUSH:
      GETPARAM(offs);
      PUSH(* (cell *)(data+(int)offs));
      break;
    case OP_PUSH_S:
      GETPARAM(offs);
      PUSH(* (cell *)(data+(int)frm+(int)offs));
      break;
    case OP_POP_PRI:
      POP(pri);
      break;
    case OP_POP_ALT:
      POP(alt);
      break;
    case OP_STACK:
      GETPARAM(offs);
      alt=stk;
      stk+=offs;
      CHKMARGIN();
      CHKSTACK();
      if (debug && offs>0) {
        amx->dbgcode=DBG_CLRSYM;
        amx->stk=stk;
        amx->debug(amx);
      } /* if */
      break;
    case OP_HEAP:
      GETPARAM(offs);
      alt=hea;
      hea+=offs;
      CHKMARGIN();
      CHKHEAP();
      break;
    case OP_PROC:
      PUSH(frm);
      frm=stk;
      CHKMARGIN();
      break;
    case OP_RET:
      POP(frm);
      POP(offs);
      cip=(cell *)(code+(int)offs);
      if (offs==0) {
        /* entry function returns -> end of program */
        if (retval!=NULL)
          *retval=pri;
        if (debug) {
          amx->dbgcode=DBG_TERMINATE;
          amx->dbgparam=0;
          amx->debug(amx);
        } /* if */
        /* The caller must remove the parameters from the stack, but... there
         * is no caller. So reset it the hard way. */
        amx->stk=reset_stk;
        return AMX_ERR_NONE;
      } else {
        if (debug) {
          amx->dbgcode=DBG_RETURN;
          amx->dbgparam=pri;  /* store "return value" */
          amx->debug(amx);
        } /* if */
      } /* if */
      break;
    case OP_RETN:
      POP(frm);
      POP(offs);
      cip=(cell *)(code+(int)offs);
      stk+= *(cell *)(data+(int)stk) + sizeof(cell); /* remove parameters from the stack */
      amx->stk=stk;
      if (offs==0) {
        /* entry function returns -> end of program */
        if (retval!=NULL)
          *retval=pri;
        if (debug) {
          amx->dbgcode=DBG_TERMINATE;
          amx->dbgparam=0;
          amx->debug(amx);
        } /* if */
        assert(amx->stk==reset_stk);
        return AMX_ERR_NONE;
      } else {
        if (debug) {
          amx->dbgcode=DBG_RETURN;
          amx->dbgparam=pri;  /* store "return value" */
          amx->debug(amx);
        } /* if */
      } /* if */
      if (debug) {
        amx->dbgcode=DBG_CLRSYM;
        amx->debug(amx);
      } /* if */
      break;
    case OP_CALL:
      PUSH(((u_char *)cip-code)+sizeof(cell));  /* skip address */
      cip=JUMPABS(code, cip);                   /* jump to the address */
      if (debug) {
        amx->dbgcode=DBG_CALL;
        amx->dbgaddr=(ucell)((u_char *)cip-code);
        amx->debug(amx);
      } /* if */
      break;
    case OP_CALL_PRI:
      PUSH((u_char *)cip-code);
      cip=(cell *)(code+(int)pri);
      if (debug) {
        amx->dbgcode=DBG_CALL;
        amx->dbgaddr=pri;
        amx->debug(amx);
      } /* if */
      break;
    case OP_JUMP:
      /* since the GETPARAM() macro modifies cip, you cannot
       * do GETPARAM(cip) directly */
      cip=JUMPABS(code, cip);
      break;
    case OP_JREL:
      offs=*cip;
      cip=(cell *)((u_char *)cip + (int)offs + sizeof(cell));
      break;
    case OP_JZER:
      if (pri==0)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JNZ:
      if (pri!=0)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JEQ:
      if (pri==alt)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JNEQ:
      if (pri!=alt)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JLESS:
      if ((ucell)pri < (ucell)alt)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JLEQ:
      if ((ucell)pri <= (ucell)alt)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JGRTR:
      if ((ucell)pri > (ucell)alt)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JGEQ:
      if ((ucell)pri >= (ucell)alt)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JSLESS:
      if (pri<alt)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JSLEQ:
      if (pri<=alt)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JSGRTR:
      if (pri>alt)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_JSGEQ:
      if (pri>=alt)
        cip=JUMPABS(code, cip);
      else
        cip=(cell *)((u_char *)cip+sizeof(cell));
      break;
    case OP_SHL:
      pri<<=alt;
      break;
    case OP_SHR:
      pri=(ucell)pri >> (int)alt;
      break;
    case OP_SSHR:
      pri>>=alt;
      break;
    case OP_SHL_C_PRI:
      GETPARAM(offs);
      pri<<=offs;
      break;
    case OP_SHL_C_ALT:
      GETPARAM(offs);
      alt<<=offs;
      break;
    case OP_SHR_C_PRI:
      GETPARAM(offs);
      pri=(ucell)pri >> (int)offs;
      break;
    case OP_SHR_C_ALT:
      GETPARAM(offs);
      alt=(ucell)alt >> (int)offs;
      break;
    case OP_SMUL:
      pri*=alt;
      break;
    case OP_SDIV:
      if (alt==0)
        ABORT(amx,AMX_ERR_DIVIDE);
      /* divide must always round down; this is a bit
       * involved to do in a machine-independent way.
       */
      offs=(pri % alt + alt) % alt;     /* true modulus */
      pri=(pri - offs) / alt;           /* division result */
      alt=offs;
      break;
    case OP_SDIV_ALT:
      if (pri==0)
        ABORT(amx,AMX_ERR_DIVIDE);
      /* divide must always round down; this is a bit
       * involved to do in a machine-independent way.
       */
      offs=(alt % pri + pri) % pri;     /* true modulus */
      pri=(alt - offs) / pri;           /* division result */
      alt=offs;
      break;
    case OP_UMUL:
      pri=(ucell)pri * (ucell)alt;
      break;
    case OP_UDIV:
      if (alt==0)
        ABORT(amx,AMX_ERR_DIVIDE);
      offs=(ucell)pri % (ucell)alt;     /* temporary storage */
      pri=(ucell)pri / (ucell)alt;
      alt=offs;
      break;
    case OP_UDIV_ALT:
      if (pri==0)
        ABORT(amx,AMX_ERR_DIVIDE);
      offs=(ucell)alt % (ucell)pri;     /* temporary storage */
      pri=(ucell)alt / (ucell)pri;
      alt=offs;
      break;
    case OP_ADD:
      pri+=alt;
      break;
    case OP_SUB:
      pri-=alt;
      break;
    case OP_SUB_ALT:
      pri=alt-pri;
      break;
    case OP_AND:
      pri&=alt;
      break;
    case OP_OR:
      pri|=alt;
      break;
    case OP_XOR:
      pri^=alt;
      break;
    case OP_NOT:
      pri=!pri;
      break;
    case OP_NEG:
      pri=-pri;
      break;
    case OP_INVERT:
      pri=~pri;
      break;
    case OP_ADD_C:
      GETPARAM(offs);
      pri+=offs;
      break;
    case OP_SMUL_C:
      GETPARAM(offs);
      pri*=offs;
      break;
    case OP_ZERO_PRI:
      pri=0;
      break;
    case OP_ZERO_ALT:
      alt=0;
      break;
    case OP_ZERO:
      GETPARAM(offs);
      *(cell *)(data+(int)offs)=0;
      break;
    case OP_ZERO_S:
      GETPARAM(offs);
      *(cell *)(data+(int)frm+(int)offs)=0;
      break;
    case OP_SIGN_PRI:
      if ((pri & 0xff)>=0x80)
        pri|= ~ (ucell)0xff;
      break;
    case OP_SIGN_ALT:
      if ((alt & 0xff)>=0x80)
        alt|= ~ (ucell)0xff;
      break;
    case OP_EQ:
      pri= pri==alt ? 1 : 0;
      break;
    case OP_NEQ:
      pri= pri!=alt ? 1 : 0;
      break;
    case OP_LESS:
      pri= (ucell)pri < (ucell)alt ? 1 : 0;
      break;
    case OP_LEQ:
      pri= (ucell)pri <= (ucell)alt ? 1 : 0;
      break;
    case OP_GRTR:
      pri= (ucell)pri > (ucell)alt ? 1 : 0;
      break;
    case OP_GEQ:
      pri= (ucell)pri >= (ucell)alt ? 1 : 0;
      break;
    case OP_SLESS:
      pri= pri<alt ? 1 : 0;
      break;
    case OP_SLEQ:
      pri= pri<=alt ? 1 : 0;
      break;
    case OP_SGRTR:
      pri= pri>alt ? 1 : 0;
      break;
    case OP_SGEQ:
      pri= pri>=alt ? 1 : 0;
      break;
    case OP_EQ_C_PRI:
      GETPARAM(offs);
      pri= pri==offs ? 1 : 0;
      break;
    case OP_EQ_C_ALT:
      GETPARAM(offs);
      pri= alt==offs ? 1 : 0;
      break;
    case OP_INC_PRI:
      pri++;
      break;
    case OP_INC_ALT:
      alt++;
      break;
    case OP_INC:
      GETPARAM(offs);
      *(cell *)(data+(int)offs) += 1;
      break;
    case OP_INC_S:
      GETPARAM(offs);
      *(cell *)(data+(int)frm+(int)offs) += 1;
      break;
    case OP_INC_I:
      *(cell *)(data+(int)pri) += 1;
      break;
    case OP_DEC_PRI:
      pri--;
      break;
    case OP_DEC_ALT:
      alt--;
      break;
    case OP_DEC:
      GETPARAM(offs);
      *(cell *)(data+(int)offs) -= 1;
      break;
    case OP_DEC_S:
      GETPARAM(offs);
      *(cell *)(data+(int)frm+(int)offs) -= 1;
      break;
    case OP_DEC_I:
      *(cell *)(data+(int)pri) -= 1;
      break;
    case OP_MOVS:
      GETPARAM(offs);
      memcpy(data+(int)alt, data+(int)pri, (int)offs);
      break;
    case OP_CMPS:
      GETPARAM(offs);
      pri=memcmp(data+(int)alt, data+(int)pri, (int)offs);
      break;
    case OP_FILL:
      GETPARAM(offs);
      for (i=(int)alt; (size_t)offs>=sizeof(cell); i+=sizeof(cell), offs-=sizeof(cell))
        *(cell *)(data+i) = pri;
      break;
    case OP_HALT:
      GETPARAM(offs);
      if (retval!=NULL)
        *retval=pri;
      /* store complete status */
      amx->frm=frm;
      amx->stk=stk;
      amx->hea=hea;
      amx->pri=pri;
      amx->alt=alt;
      amx->cip=(cell)((u_char*)cip-code);
      if (debug) {
        amx->dbgcode=DBG_TERMINATE;
        amx->dbgaddr=(cell)((u_char *)cip-code);
        amx->dbgparam=offs;
        amx->debug(amx);
      } /* if */
      if (offs==AMX_ERR_SLEEP) {
        amx->reset_stk=reset_stk;
        amx->reset_hea=reset_hea;
        return (int)offs;
      } /* if */
      ABORT(amx,(int)offs);
    case OP_BOUNDS:
      GETPARAM(offs);
      if (pri>offs || pri<0)
        ABORT(amx,AMX_ERR_BOUNDS);
      break;
    case OP_SYSREQ_PRI:
      /* save a few registers */
      amx->cip=(cell)((u_char *)cip-code);
      amx->hea=hea;
      amx->frm=frm;
      amx->stk=stk;
      num=amx->callback(amx,pri,&pri,(cell *)(data+(int)stk));
      if (num!=AMX_ERR_NONE)
        ABORT(amx,num);
      break;
    case OP_SYSREQ_C:
      GETPARAM(offs);
      /* save a few registers */
      amx->cip=(cell)((u_char *)cip-code);
      amx->hea=hea;
      amx->frm=frm;
      amx->stk=stk;
      num=amx->callback(amx,offs,&pri,(cell *)(data+(int)stk));
      if (num!=AMX_ERR_NONE)
        ABORT(amx,num);
      break;
    case OP_FILE:
      GETPARAM(offs);
      cip=(cell *)((u_char *)cip+(int)offs);
      assert(0);        /* this code should not occur during execution */
      break;
    case OP_LINE:
      assert((amx->flags & AMX_FLAG_BROWSE)==0);
      GETPARAM(amx->curline);
      GETPARAM(amx->curfile);
      if (debug) {
        amx->frm=frm;
        amx->stk=stk;
        amx->dbgcode=DBG_LINE;
        num=amx->debug(amx);
        if (num!=AMX_ERR_NONE)
          ABORT(amx,num);
      } /* if */
      break;
    case OP_SYMBOL:
      assert((amx->flags & AMX_FLAG_BROWSE)==0);
      GETPARAM(offs);
      GETPARAM(amx->dbgaddr);
      GETPARAM(amx->dbgparam);
      amx->dbgname=(char *)cip;
      cip=(cell *)((u_char *)cip + (int)offs - 2*sizeof(cell));
      assert((amx->dbgparam >> 8)>0);         /* local symbols only */
      if (debug) {
        amx->frm=frm;   /* debugger needs this to relocate the symbols */
        amx->dbgcode=DBG_SYMBOL;
        amx->debug(amx);
      } /* if */
      break;
    case OP_SRANGE:
      assert((amx->flags & AMX_FLAG_BROWSE)==0);
      GETPARAM(amx->dbgaddr);   /* dimension level */
      GETPARAM(amx->dbgparam);  /* length */
      if (debug) {
        amx->frm=frm;   /* debugger needs this to relocate the symbols */
        amx->dbgcode=DBG_SRANGE;
        amx->debug(amx);
      } /* if */
      break;
    case OP_JUMP_PRI:
      cip=(cell *)(code+(int)pri);
      break;
    case OP_SWITCH: {
      cell *cptr;

      cptr=(cell *)*cip + 1;    /* +1, to skip the "casetbl" opcode */
      cip=(cell *)*(cptr+1);    /* preset to "none-matched" case */
      num=(int)*cptr;           /* number of records in the case table */
      for (cptr+=2; num>0 && *cptr!=pri; num--,cptr+=2)
        /* nothing */;
      if (num>0)
        cip=(cell *)*(cptr+1);  /* case found */
      break;
    } /* case */
    case OP_SWAP_PRI:
      offs=*(cell *)(data+(int)stk);
      *(cell *)(data+(int)stk)=pri;
      pri=offs;
      break;
    case OP_SWAP_ALT:
      offs=*(cell *)(data+(int)stk);
      *(cell *)(data+(int)stk)=alt;
      alt=offs;
      break;
    case OP_PUSHADDR:
      GETPARAM(offs);
      PUSH(frm+offs);
      break;
    case OP_CASETBL:
      assert(0);                /* should not occur during execution */
      /* drop through to "invalid instruction" */
    default:
      ABORT(amx,AMX_ERR_INVINSTR);
    } /* switch */
  } /* for */
#endif
}

#endif  /* __GNUC__ */

/* For interfacing applications not written in C/C++, amx_Execv() works like
 * amx_Exec(), but has all parameters passed via an array.
 */
int AMXAPI amx_Execv(AMX *amx, cell *retval, int index, int numparams, cell params[])
{
  return amx_Exec(amx, retval, index, numparams<<16, params);
}

int AMXAPI amx_SetCallback(AMX *amx,AMX_CALLBACK callback)
{
  assert(amx!=NULL);
  assert(callback!=NULL);
  amx->callback=callback;
  return AMX_ERR_NONE;
}

int AMXAPI amx_SetDebugHook(AMX *amx,AMX_DEBUG debug)
{
  assert(amx!=NULL);
  assert(debug!=NULL);
  amx->debug=debug;
  return AMX_ERR_NONE;
}

int AMXAPI amx_RaiseError(AMX *amx, int error)
{
  assert(error>0);
  amx->error=error;
  return AMX_ERR_NONE;
}

int AMXAPI amx_GetAddr(AMX *amx,cell amx_addr,cell **phys_addr)
{
  AMX_HEADER *hdr=(AMX_HEADER *)amx->base;

  if (amx_addr>=amx->hea && amx_addr<amx->stk || amx_addr>=amx->stp)
    return AMX_ERR_MEMACCESS;
  *phys_addr=(cell *)(amx->base + (int)(hdr->dat + amx_addr));
  return AMX_ERR_NONE;
}

int AMXAPI amx_Allot(AMX *amx,int cells,cell *amx_addr,cell **phys_addr)
{
  AMX_HEADER *hdr=(AMX_HEADER *)amx->base;

  if (amx->stk - amx->hea - cells*sizeof(cell) < STKMARGIN)
    return AMX_ERR_MEMORY;
  assert(amx_addr!=NULL);
  assert(phys_addr!=NULL);
  *amx_addr=amx->hea;
  *phys_addr=(cell *)(amx->base + (int)(hdr->dat + amx->hea));
  amx->hea += cells*sizeof(cell);
  return AMX_ERR_NONE;
}

int AMXAPI amx_Release(AMX *amx,cell amx_addr)
{
  if (amx->hea > amx_addr)
    amx->hea=amx_addr;
  return AMX_ERR_NONE;
}

#define CHARBITS        (8*sizeof(char))
#if defined BIT16
  #define CHARMASK      (0xffffu << 8*(2-sizeof(char)))
#else
  #define CHARMASK      (0xffffffffuL << 8*(4-sizeof(char)))
#endif

int AMXAPI amx_StrLen(cell *cstr, int *length)
{
  int len = 0;

  if (*cstr>UCHAR_MAX) {
    /* packed string */
    assert(sizeof(char)==1);    /* Unicode needs different functions */
    len=strlen((char *)cstr);   /* find '\0' */
    if (amx_LittleEndian) {
      /* on Little Endian machines, toggle the last bytes */
      cell c=cstr[len/sizeof(cell)];    /* get last cell */
      len=len - len % sizeof(cell);     /* len = multiple of "cell" bytes */
      while ((c & CHARMASK)!=0) {
        len++;
        c <<= 8*sizeof(char);
      } /* if */
    } /* if */
  } else {
    for (len=0; cstr[len]!=0; len++)
      /* nothing */;
  } /* if */
  *length = len;
  return AMX_ERR_NONE;
}

int AMXAPI amx_SetString(cell *dest, char *source, int pack)
{
	/* the memory blocks should not overlap */
   int len=strlen(source);

  // Check Dest isnt NULL
  if (!dest)
		return -1;

  if (pack) {
    /* create a packed string */
    dest[len/sizeof(cell)]=0;   /* clear last bytes of last (semi-filled) cell*/
    memcpy(dest,source,len);
    /* On Big Endian machines, the characters are well aligned in the
     * cells; on Little Endian machines, we must swap all cells.
     */
    if (amx_LittleEndian) {
      len /= sizeof(cell);
      while (len>=0)
        swapcell((ucell *)&dest[len--]);
    } /* if */
  } else {
    /* create an unpacked string */
    int i;
    for (i=0; i<len; i++)
      dest[i]=(cell)source[i];
    dest[len]=0;
  } /* if */
  return AMX_ERR_NONE;
}

int AMXAPI amx_GetString(char *dest,cell *source)
{
  if (*source>UCHAR_MAX) {
    /* source string is packed */
    cell c = 0;         /* to avoid a compiler warning */
    int i=sizeof(cell)-1;
    do {
      if (i==sizeof(cell)-1)
        c=*source++;
      *dest=(char)(c >> i*CHARBITS);
      i=(i+sizeof(cell)-1) % sizeof(cell);
    } while (*dest++!='\0');
  } else {
    /* source string is unpacked */
    while (*source!=0)
      *dest++=(char)*source++;
    *dest='\0';         /* store terminator */
  } /* if */
  return AMX_ERR_NONE;
}


int AMXAPI amx_SaveToFile(AMX *amx, FILE* pOut)
{
	AMX_HEADER *hdr = (AMX_HEADER *)amx->base;
	unsigned char *datasegment = amx->base + (int)hdr->dat;
	long datalength = hdr->hea - (int)hdr->dat;

	/* dump this to file ... */
	fwrite(datasegment, 1, datalength, pOut);
	return 1;
}
