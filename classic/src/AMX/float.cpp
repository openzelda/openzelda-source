/*  Float arithmetic for the Small AMX engine
 *
 *  Copyright (c) Artran, Inc. 1999
 *  Written by Greg Garner (gmg@artran.com)
 *  This file may be freely used. No warranties of any kind.
 *
 */
//#include "stdafx.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>      /* for NULL */
#include <stdlib.h>     /* for atof() */
#include <math.h>
#include "amx.h"
#include "float.h"


#define amx_ftoc(f)   ( * ((cell*)&f) )   /* float to cell */
#define amx_ctof(c)   ( * ((float*)&c) )  /* cell to float */

extern float		 g_rDelta;

/******************************************************************/
// Private function to
// allocate and fill a C style string from a small type string.
static char *pcCreateAndFillStringFromCell(AMX *amx,cell params)
{
    char *szDest;
    int nLen;
    cell *pString;

    // Get the real address of the string.
    amx_GetAddr(amx,params,&pString);

    // Find out how long the string is in characters.
    amx_StrLen(pString, &nLen);
    szDest = new char[nLen+1];

    // Now convert the Small String into a C type null terminated string
    amx_GetString(szDest, pString);

    return szDest;
}

/******************************************************************/
/*static */cell ConvertFloatToCell(float fValue)
{
    float *pFloat;
	cell fCell ;

    // Get a pointer to the cell that is a float pointer.
    pFloat = (float *)((void *)&fCell);

    // Now put the float value into the cell.
    *pFloat = fValue;

    // Return the cell that contains the float
    return fCell;
}

/******************************************************************/
/*static */float fConvertCellToFloat(cell cellValue)
{
    float *pFloat;

	//cellValue += 0.001;

    // Get a pointer to the cell that is a float pointer.
    pFloat = (float *)((void *)&cellValue);

    // Return the float
    return *pFloat;
}


/******************************************************************/
static cell _float(AMX *,cell *params)
{
    /*
    *   params[0] = number of bytes
    *   params[1] = long value to convert to a float
    */
    float fValue;

    // Convert to a float. Calls the compilers long to float conversion.
    fValue = (float) params[1];

    // Return the cell.
    return ConvertFloatToCell(fValue);
}


/******************************************************************/
static cell _floatstr(AMX *amx,cell *params)
{
    /*
    *   params[0] = number of bytes
    *   params[1] = virtual string address to convert to a float
    */
    char *szSource;
    float fNum;
    long lCells;

    lCells = params[0]/sizeof(cell);

    // They should have sent us 1 cell.
    assert(lCells==1);

    // Convert the Small string to a C style string.
    szSource = pcCreateAndFillStringFromCell(amx, params[1]);

    // Now convert this to a float.
    fNum = (float)atof(szSource);

    // Delete the string storage that was newed...
    delete[] szSource;

    return ConvertFloatToCell(fNum);
}

/******************************************************************/
static cell _floatmul(AMX *,cell *params)
{
    /*
    *   params[0] = number of bytes
    *   params[1] = float operand 1
    *   params[2] = float operand 2
    */
    float fA, fB, fRes;

    fA = fConvertCellToFloat(params[1]);
    fB = fConvertCellToFloat(params[2]);
    fRes = fA * fB;

    return ConvertFloatToCell(fRes);
}

/******************************************************************/
static cell _floatdiv(AMX *,cell *params)
{
    /*
    *   params[0] = number of bytes
    *   params[1] = float dividend (top)
    *   params[2] = float divisor (bottom)
    */
    float fA, fB, fRes;

    fA = fConvertCellToFloat(params[1]);
    fB = fConvertCellToFloat(params[2]);
    fRes = fA / fB;

    return ConvertFloatToCell(fRes);
}

/******************************************************************/
static cell _floatadd(AMX *,cell *params)
{
    /*
    *   params[0] = number of bytes
    *   params[1] = float operand 1
    *   params[2] = float operand 2
    */
    float fA, fB, fRes;

    fA = fConvertCellToFloat(params[1]);
    fB = fConvertCellToFloat(params[2]);
    fRes = fA + fB;

    return ConvertFloatToCell(fRes);
}

/******************************************************************/
static cell _floatsub(AMX *,cell *params)
{
    /*
    *   params[0] = number of bytes
    *   params[1] = float operand 1
    *   params[2] = float operand 2
    */
    float fA, fB, fRes;

    fA = fConvertCellToFloat(params[1]);
    fB = fConvertCellToFloat(params[2]);
    fRes = fA - fB;

    return ConvertFloatToCell(fRes);
}

/******************************************************************/
// Return fractional part of float
static cell _floatfract(AMX *,cell *params)
{
    /*
    *   params[0] = number of bytes
    *   params[1] = float operand
    */
    float fA;

    fA = fConvertCellToFloat(params[1]);
    fA = fA - (float)(floor((double)fA));

    return ConvertFloatToCell(fA);
}

/******************************************************************/
// Return integer part of float, rounded
static cell _floatround(AMX *,cell *params)
{
    /*
    *   params[0] = number of bytes
    *   params[1] = float operand
    *   params[2] = Type of rounding (long)
    */
    float fA;

    fA = fConvertCellToFloat(params[1]);

    switch (params[2])
    {
        case 1:       /* round downwards (truncate) */
            fA = (float)(floor((double)fA));
            break;
        case 2:       /* round upwards */
            float fValue;
            fValue = (float)(floor((double)fA));
            if ( (fA>=0) && ((fA-fValue)!=0) )
                fValue++;
            fA = fValue;
            break;
        default:      /* standard, round to nearest */
            fA = (float)(floor((double)fA+.5));
            break;
    }

    return (long)fA;
}

/******************************************************************/
static cell _floatcmp(AMX *,cell *params)
{
    /*
    *   params[0] = number of bytes
    *   params[1] = float operand 1
    *   params[2] = float operand 2
    */
    float fA, fB;

    fA = fConvertCellToFloat(params[1]);
    fB = fConvertCellToFloat(params[2]);
    if (fA == fB)
        return 0;
    else if (fA>fB)
        return 1;
    else
        return -1;
}


/******************************************************************/
static cell AMX_NATIVE_CALL GetTimeDelta(AMX *amx, cell *params)
{
	return amx_ftoc(g_rDelta);
}

/******************************************************************/
static cell AMX_NATIVE_CALL Sin(AMX *amx, cell *params)
{
	// Convert to radians
	float radians = 0.0174532 * (float)params[1];
	return ConvertFloatToCell( (float)sin(radians) );
}

/******************************************************************/
static cell AMX_NATIVE_CALL Cos(AMX *amx, cell *params)
{
	// Convert to radians
	float radians = 0.0174532 * (float)params[1];
	return ConvertFloatToCell( (float)cos(radians) );
}
/******************************************************************/
static cell AMX_NATIVE_CALL Sqrt(AMX *amx, cell *params)
{
	// Returns the square root of a floating-point number
	float a = fConvertCellToFloat( params[1] );
	return ConvertFloatToCell( (float)sqrt(a) );
}

/******************************************************************/
static cell AMX_NATIVE_CALL Pow(AMX *amx, cell *params)
{
	// Returns the square root of a floating-point number
	float a = fConvertCellToFloat( params[1] );
	float b = fConvertCellToFloat( params[2] );
	return ConvertFloatToCell( (float)pow(a,b) );
}

/******************************************************************
* april 2010 - add by lukex
*/
static cell AMX_NATIVE_CALL _Sin(AMX *amx, cell *params)
{
	// Convert to radians
	float radians = 0.0174532 * fConvertCellToFloat(params[1]);
	return ConvertFloatToCell( (float)sin(radians) );
}

/******************************************************************
* april 2010 - add by lukex
*/
static cell AMX_NATIVE_CALL _Cos(AMX *amx, cell *params)
{
	// Convert to radians
	float radians = 0.0174532 * fConvertCellToFloat(params[1]);
	return ConvertFloatToCell( (float)cos(radians) );
}

/******************************************************************
* april 2010 - add by lukex
*/
static cell AMX_NATIVE_CALL _atan2(AMX *amx, cell *params)
{
	float x, y, result;
	x = fConvertCellToFloat(params[1]);
	y = fConvertCellToFloat(params[2]);
	result = atan2 (y,x) * 180 / 3.1415926535897932384626433832795;

	if(result < 0)
	     result += 360;
 
	return ConvertFloatToCell(result);
}

extern AMX_NATIVE_INFO float_Natives[] = {
  { "float",         _float },
  { "floatstr",      _floatstr },
  { "floatmul",      _floatmul },
  { "floatdiv",      _floatdiv },
  { "floatadd",      _floatadd },
  { "floatsub",      _floatsub },
  { "floatfract",    _floatfract},
  { "floatround",    _floatround},
  { "floatcmp",      _floatcmp},
  { "GetTimeDelta",  GetTimeDelta },
  { "Sin",  Sin },
  { "Cos",  Cos },
  { "Sqrt", Sqrt },
  { "Pow", Pow},
  { "floatsin",  _Sin },
  { "floatcos",  _Cos },
  { "floatatan2",  _atan2 },
  { NULL, NULL }        /* terminator */
};
