/*  Fixed-point arithmetic for the Small AMX
 *
 *  Fixed point numbers compromise range versus number of decimals. This
 *  library decimal fixed point numbers with an configurable number of
 *  decimals. The current setting is 3 decimals.
 *
 *  Copyright (c) ITB CompuPhase, 1998-2000
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>      /* for NULL */
#include "amx.h"

#define MULTIPLIER      1000L   /* 10^decimals */


#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL fixed(AMX *amx,cell *params)
{
  return params[1] * MULTIPLIER;
}

static cell AMX_NATIVE_CALL fixedstr(AMX *amx,cell *params)
{
  char str[50],*ptr;
  cell *cstr,intpart,decimals,mult;
  int len;

  amx_GetAddr(amx,params[1],&cstr);
  amx_StrLen(cstr,&len);
  if (len>=50) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } /* if */
  amx_GetString(str,cstr);
  ptr=str;
  intpart=0;
  decimals=0;
  mult=1;
  while (isdigit(*ptr)) {
    intpart=intpart*10 + (*ptr-'0');
    ptr++;
  } /* while */
  if (*ptr=='.') {
    ptr++;
    len=0;
    while (isdigit(*ptr) && len<4) {
      decimals=decimals*10 + (*ptr-'0');
      mult*=10;
      ptr++;
      len++;
    } /* while */
  } /* if */
  return (intpart*MULTIPLIER) + (decimals*MULTIPLIER)/mult;
                                /* ??? ^^^^ round this */
}

/* Some C/C++ compilers have problems with long lists of definitions, so
 * I create another macro to fix this.
 */
#if defined __WATCOMC__  && defined __386__             /* Watcom C/C++ */
  /* ANSI 64-bit division routine not needed for Watcom C/C++ because
   * it uses inline assembler.
   */
  #define USE_ANSI_C    0

#elif defined _MSC_VER && _MSC_VER>=9 && defined _WIN32 /* Microsoft C/C++ */
  /* ANSI 64-bit division routine not needed for Microsoft Visual C/C++
   * because it supports 64-bit integers
   */
  #define USE_ANSI_C    0

#elif defined __BORLANDC__  && __BORLANDC__ >= 0x500 && defined __32BIT__ /* Borland C++ v.5 */
  /* ANSI 64-bit division routine not needed for Borland C++ because it
   * supports 64-bit integers
   */
  #define USE_ANSI_C    0

#elif defined __GNUC__                                  /* GNU GCC */
  /* ANSI 64-bit division routine not needed for GNU GCC because it
   * supports 64-bit integers
   */
  #define USE_ANSI_C    0

#else

  #define USE_ANSI_C    1

#endif

#if USE_ANSI_C
  static ucell div64_32(ucell t[2], ucell divisor)
  {
    /* This function was adapted from source code that appeared in
     * Dr. Dobb's Journal, August 1992, page 117.
     */
    typedef unsigned short ushort;
    #if !defined LOWORD
      #define LOWORD(v)   (ushort)((v) & 0xffff)
      #define HIWORD(v)   (ushort)(((v) >> 16) & 0xffff)
    #endif

    ucell u, v;
    ushort rHigh, rLow, dHigh, dLow;

    assert(divisor!=0);
    /* if the divisor is smaller than t[1], the result will not fit in a cell */
    assert(divisor>=t[1]);

    dHigh=HIWORD(divisor);
    dLow=LOWORD(divisor);

    /* Underestimate high half of quotient and subtract product
     * of estimate and divisor from dividend.
     */
    rHigh = (ushort)(t[1] / (dHigh + 1));
    u = (ucell)rHigh * (ucell)dLow;
    v = (ucell)rHigh * (ucell)dHigh;
    if ((t[0] -= (u << 16)) > (0xffffffffL - (u << 16)))
      t[1]--;
    t[1] -= HIWORD(u);
    t[1] -= v;

    /* Correct estimate. */
    while ((t[1] > dHigh) || ((t[1] == dHigh) && (t[0] >= ((ucell)dLow << 16)))) {
      if ((t[0] -= ((ucell)dLow << 16)) > 0xffffffffL - ((ucell)dLow << 16))
        t[1]--;
      t[1] -= dHigh;
      rHigh++;
    } /* while */
    /* Underestimate low half of quotient and subtract product of
     * estimate and divisor from what remains of dividend.
     */
    rLow = (ushort) ((ucell)((t[1] << 16) + HIWORD(t[0])) / (dHigh + 1));
    u = (ucell)rLow * (ucell)dLow;
    v = (ucell)rLow * (ucell)dHigh;
    if ((t[0] -= u) > (0xffffffffL - u))
      t[1]--;
    if ((t[0] -= (v << 16)) > (0xffffffffL - (v << 16)))
      t[1]--;
    t[1] -= HIWORD(v);

    /* Correct estimate. */
    while ((t[1] > 0) || ((t[1] == 0) && t[0] >= divisor)) {
      if ((t[0] -= divisor) > (0xffffffffL - divisor))
        t[1]--;
      rLow++;
    } /* while */

    return ((ucell)rHigh << 16) + rLow;
  }
#endif

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL fmul(AMX *amx,cell *params)
{
#if defined __WATCOMC__ && defined __386__

  cell __fmul(void);
  cell a=params[1];
  cell b=params[2];
  #if MULTIPLIER != 1000
    #error Assembler chunks must be modified for a different base
  #endif
  #pragma aux __fmul =    \
    "mov    eax, [a]"     \
    "mov    ebx, 1000"    \
    "imul   [b]"          \
    "idiv   ebx"          \
    "mov    [a], eax"     \
    modify [eax ebx edx];
  __fmul();
  return a;

#elif _MSC_VER>=9 && defined _WIN32

  __int64 a=(__int64)params[1] * (__int64)params[2];
  a /= MULTIPLIER;
  return (cell)a;

#elif defined __BORLANDC__  && __BORLANDC__ >= 0x500 && defined __32BIT__

  __int64 a=(__int64)params[1] * (__int64)params[2];
  a /= MULTIPLIER;
  return (cell)a;

#elif defined __GNUC__

  long long a=(long long)params[1] * (long long)params[2];
  a /= MULTIPLIER;
  return (cell)a;

#else

  /* (Xs * Ys) == (X*Y)ss, where "s" stands for scaled.
   * The desired result is (X*Y)s, so we must unscale once.
   * but we cannot do this before multiplication, because of loss
   * of precision, and we cannot do it after the multiplication
   * because of the possible overflow.
   * The technique used here is to cut the multiplicands into
   * components and to multiply these components separately:
   *
   * Assume Xs == (A << 16) + B and Ys == (C << 16) + D, where A, B,
   * C and D are 16 bit numbers.
   *
   *    A B
   *    C D
   *    --- *
   *    D*B + (D*A << 16) + (C*B << 16) + (C*A << (16+16))
   *
   * Thus we have built a 64-bit number, which can now be scaled back
   * to 32-bit by dividing by the scale factor.
   */
  cell a,b,c,d;
  ucell v[2];
  cell sign=1;

  assert(sizeof(cell)==4);
  assert(MULTIPLIER<=(1L<<16));

  /* make both operands positive values, but keep the sign of the result */
  if (params[1]<0) {
    params[1]=-params[1];
    sign=-sign;     /* negate result */
  } /* if */
  if (params[2]<0) {
    params[2]=-params[2];
    sign=-sign;     /* negate result */
  } /* if */

  a = (params[1] >> 16) & 0xffff;
  b = params[1] & 0xffff;
  c = (params[2] >> 16) & 0xffff;
  d = params[2] & 0xffff;

  /* store the intermediate into a 64-bit number */
  v[0]=d*b + (d*a << 16) + (c*b << 16);
  v[1]=c*a;
  return (cell)div64_32(v,MULTIPLIER) * sign;
#endif
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL fdiv(AMX *amx,cell *params)
{
#if defined __WATCOMC__ && defined __386__

  cell __fdiv(void);
  cell a=params[1];
  cell b=params[2];
  #if MULTIPLIER != 1000
    #error Assembler chunks must be modified for a different base
  #endif
  #pragma aux __fdiv =    \
    "mov    eax, [a]"     \
    "cdq"                 \
    "mov    ebx, 1000"    \
    "imul   ebx"          \
    "idiv   [b]"          \
    "mov    [a], eax"     \
    modify [eax ebx edx];
  __fdiv();
  return a;

#elif _MSC_VER>=9 && defined _WIN32

  __int64 a=(__int64)params[1] * (__int64)MULTIPLIER;
  a /= (__int64)params[2];
  return (cell)a;

#elif defined __BORLANDC__  && __BORLANDC__ >= 0x500  && defined __32BIT__

  __int64 a=(__int64)params[1] * (__int64)MULTIPLIER;
  a /= (__int64)params[2];
  return (cell)a;

#elif defined __GNUC__

  long long a=(long long)params[1] * (long long)MULTIPLIER;
  a /= (long long)params[2];
  return (cell)a;

#else

  /* The dividend must be scaled prior to division. The dividend
   * is a 32-bit number, however, so when shifted, it will become
   * a value that no longer fits in a 32-bit variable. This routine
   * does the division by using only 16-bit and 32-bit values, but
   * with considerable effort.
   * If your compiler supports 64-bit integers, modify this routine
   * to use them. If your processor can do a simple 64-bit by 32-bit
   * division in assembler, write assembler chunks.
   * In other words: the straight C routine that follows is correct
   * and portable, but use it only as a last resort.
   *
   * This function was adapted from source code that appeared in
   * Dr. Dobb's Journal, August 1992, page 117.
   */
  typedef unsigned short ushort;
  #define LOWORD(v)     (ushort)((v) & 0xffff)
  #define HIWORD(v)     (ushort)(((v) >> 16) & 0xffff)

  cell dividend=params[1];
  cell divisor=params[2];
  cell sign=1;
  ucell b[2];

  if (divisor==0) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } /* if */

  /* make both operands positive values, but keep the sign of the result */
  if (dividend<0) {
    dividend=-dividend;
    sign=-sign;     /* negate result */
  } /* if */
  if (divisor<0) {
    divisor=-divisor;
    sign=-sign;     /* negate result */
  } /* if */

  /* pre-scale the dividend into a 64-bit number */
  b[0]=dividend*MULTIPLIER;
  b[1]=(HIWORD(dividend)*MULTIPLIER) >> 16;

  /* If the divisor is now smaller than b[1], the result
   * will not fit in a cell.
   */
  if ((ucell)divisor<b[1]) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } /* if */

  return (cell)div64_32(b,(ucell)divisor) * sign;
#endif
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL ffract(AMX *amx,cell *params)
{
  return params[1] % MULTIPLIER;
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell AMX_NATIVE_CALL fround(AMX *amx,cell *params)
{
  cell value;

  switch (params[2]) {
  case 1:       /* round downwards (truncate) */
    value=params[1] / MULTIPLIER;
    if (params[1]<0 && (params[1] % MULTIPLIER)!=0)
      value--;
    break;
  case 2:       /* round upwards */
    value=params[1] / MULTIPLIER;
    if (params[1]>=0 && (params[1] % MULTIPLIER)!=0)
      value++;
    break;
  case 3:       /* round to even number when fractional part is exactly 0.5 */
    value=(params[1] + MULTIPLIER/2) / MULTIPLIER;
    if ((params[1] % MULTIPLIER)==MULTIPLIER/2 && (value & 1)==1)
      value--;
    break;
  default:      /* standard (fractional pert of 0.5 is rounded up */
    value=(params[1] + MULTIPLIER/2) / MULTIPLIER;
  } /* switch */
  return value;
}

AMX_NATIVE_INFO fixed_Natives[] = {
  { "fixed",         fixed },
  { "fixedstr",      fixedstr },
  { "fmul",          fmul },
  { "fdiv",          fdiv },
  { "ffract",        ffract },
  { "fround",        fround },
  { NULL, NULL }        /* terminator */
};
