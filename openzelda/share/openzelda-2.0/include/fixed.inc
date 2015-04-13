/* Fixed point arithmetic
 *
 * (c) Copyright 1998-2007, ITB CompuPhase
 * This file is provided as is (no warranties).
 */
#if defined fixed_included
  #endinput
#endif
#define fixed_included


//title Rational Number Support
//description Fixed point numbers are the supported rational number system and allows for 3 decimal places. Floating point numbers are also support.

#include <real>

#pragma rational Fixed(3)

native Fixed:fixed(value);
native Fixed:strfixed(const string[]);
native Fixed:fmul(Fixed:oper1, Fixed:oper2);
native Fixed:fdiv(Fixed:dividend, Fixed:divisor);
native Fixed:ffract(Fixed:value);
native fround(Fixed:value, round_method:method=round_normal);
native Fixed:fpower(Fixed:value, exponent);
native Fixed:fsqroot(Fixed:value);
native Fixed:fabs(Fixed:value);

native Fixed:flog(Fixed:value, Fixed:base=10.0); /* Return the logarithm */

/**************************************************/
/* Return the sine, cosine or tangent. The input angle may be in radian, degrees or grades. */
native Fixed:fsin(Fixed:value, anglemode:mode=radian);
native Fixed:fcos(Fixed:value, anglemode:mode=radian);
native Fixed:ftan(Fixed:value, anglemode:mode=radian);
native Fixed:fatan2(Fixed:x, Fixed:y, anglemode:mode=radian );

/**************************************************/
/* user defined operators */
native Fixed:operator*(Fixed:oper1, Fixed:oper2) = fmul;
native Fixed:operator/(Fixed:oper1, Fixed:oper2) = fdiv;
native Fixed:operator=(oper) = fixed;

stock Fixed:operator++(Fixed:oper)
    return oper + fixed(1);

stock Fixed:operator--(Fixed:oper)
    return oper - fixed(1);

stock Fixed:operator*(Fixed:oper1, oper2)
    return Fixed: (_:oper1 * oper2);            /* "*" is commutative */

stock Fixed:operator/(Fixed:oper1, oper2)
    return oper1 / fixed(oper2);

stock Fixed:operator/(oper1, Fixed:oper2)
    return fdiv(fixed(oper1), oper2);

stock Fixed:operator+(Fixed:oper1, oper2)
    return oper1 + fixed(oper2);                /* "+" is commutative */

stock Fixed:operator-(Fixed:oper1, oper2)
    return oper1 - fixed(oper2);

stock Fixed:operator-(oper1, Fixed:oper2)
    return fixed(oper1) - oper2;

stock bool:operator>(Fixed:oper1, oper2)
    return oper1 > fixed(oper2);

stock bool:operator>(oper1, Fixed:oper2)
    return fixed(oper1) > oper2;

stock bool:operator>=(Fixed:oper1, oper2)
    return oper1 >= fixed(oper2);

stock bool:operator>=(oper1, Fixed:oper2)
    return fixed(oper1) >= oper2;

stock bool:operator<(Fixed:oper1, oper2)
    return oper1 < fixed(oper2);

stock bool:operator<(oper1, Fixed:oper2)
    return fixed(oper1) < oper2;

stock bool:operator<=(Fixed:oper1, oper2)
    return oper1 <= fixed(oper2);

stock bool:operator<=(oper1, Fixed:oper2)
    return fixed(oper1) <= oper2;

stock bool:operator==(Fixed:oper1, oper2)       /* "==" is commutative */
    return oper1 == fixed(oper2);

stock bool:operator!=(Fixed:oper1, oper2)       /* "!=" is commutative */
    return oper1 != fixed(oper2);

/* forbidden operations */
/*
forward operator%(Fixed:oper1, Fixed:oper2);
forward operator%(Fixed:oper1, oper2);
forward operator%(oper1, Fixed:oper2);
*/

stock operator%(Fixed:oper1, Fixed:oper2)
	return fround(oper1*1000)%fround(oper2*1000);
stock operator%(Fixed:oper1, oper2)
	return fround(oper1*1000)%(oper2*1000);
stock operator%(oper1, Fixed:oper2)
	return (oper1*1000)%fround(oper2*1000);



