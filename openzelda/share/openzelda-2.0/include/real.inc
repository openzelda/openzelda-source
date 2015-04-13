#include <realenums>

#if defined USE_FLOAT
	#tryinclude <float>
#endif


#if defined Float_included
	#define Real:			Float:

	native Real:real(value) = float;


	#define RealString(%1)		strfloat(%1)

	#define RealSqroot(%1)		floatsqroot(%1)
	#define RealPower(%1,%2)	floatpower(%1,%2)
	#define RealAbs(%1)		floatabs(%1)
	#define RealLog(%1,%2)		floatlog(%1,%2)
/*
	#define RealRound(%1,%2)	floatround(%1,%2)
	#define RealSin(%1,%2)		floatsin(%1,%2)
	#define RealCos(%1,%2)		floatcos(%1,%2)
	#define RealTan(%1,%2)		floattan(%1,%2)
	#define RealAtan2(%1,%2,%3)	floatatan2(%1,%2,%3)
*/
	native RealRound(Float:value, round_method:method=round_normal) = floatround;
	native Float:RealSin(Float:value, anglemode:mode=radian) = floatsin;
	native Float:RealCos(Float:value, anglemode:mode=radian) = floatcos;
	native Float:RealTan(Float:value, anglemode:mode=radian) = floattan;
	native Float:RealAtan2(Float:x, Float:y, anglemode:mode=radian ) = floatatan2;


#else
	#include <fixed>
	#define Real:			Fixed:

	native Real:real(value) = fixed;

	#define RealString(%1)		strfixed(%1)

	#define RealSqroot(%1)		fsqroot(%1)
	#define RealPower(%1,%2)	fpower(%1,%2)
	#define RealAbs(%1)		fabs(%1)
	#define RealLog(%1,%2)		flog(%1,%2)
/*
	#define RealRound(%1,%2)	fround(%1,%2)
	#define RealSin(%1,%2)		fsin(%1,%2)
	#define RealCos(%1,%2)		fcos(%1,%2)
	#define RealTan(%1,%2)		ftan(%1,%2)
	#define RealAtan2(%1,%2,%3)	fatan2(%1,%2,%3)
*/
	native RealRound(Fixed:value, round_method:method=round_normal) = fround;
	native Fixed:RealSin(Fixed:value, anglemode:mode=radian) = fsin;
	native Fixed:RealCos(Fixed:value, anglemode:mode=radian) = fcos;
	native Fixed:RealTan(Fixed:value, anglemode:mode=radian) = ftan;
	native Fixed:RealAtan2(Fixed:x, Fixed:y, anglemode:mode=radian ) = fatan2;


#endif

stock Real:RealClamp(Real:value, Real:min, Real:max)
{
	if ( value > max)
		return max;
	else if ( value < min)
		return min;
	else
		return value;
}

/***
<api title="">
	<function name="RealString">
		<summary>Converts a String into Rational number.</summary>
		<param name="string[]" default="" type="a"></param>
		<return>New Rational number</return>
	</function>
	<function name="RealRound">
		<summary>Converts a Rational number into a Whole number</summary>
		<param name="Real:value" default="" type="n"></param>
		<param name="round_method" default="round_normal" type="n"></param>
		<return>new whole number.</return>
	</function>
	<function name="RealSqroot">
		<summary>Returns the square root of a value.</summary>
		<param name="Real:value" default="" type="n"></param>
		<return>square root of a value.</return>
	</function>
	<function name="RealPower">
		<summary>Returns the value raised to the power of exponent.</summary>
		<param name="Real:value" default="" type="n"></param>
		<param name="exponent" default="" type="n"></param>
		<return>the value raised to the power of exponent</return>
	</function>
	<function name="RealAbs">
		<summary>Returns Absolute value of 'value'.</summary>
		<param name="Real:value" default="" type="n"></param>
	</function>
	<function name="RealLog">
		<summary>Return the logarithm </summary>
		<param name="Real:value" default="" type="n"></param>
		<param name="Real:base" default="10.0" type="n"></param>
	</function>
	<function name="RealSin">
		<summary>Return the sine. The input angle may be in radian, degrees or grades.</summary>
		<param name="Real:value" default="" type="n"></param>
		<param name="anglemode:mode" default="radian" type="n"></param>
		<return>sine of the angle.</return>
	</function>
	<function name="RealCos">
		<summary>Return the cosine. The input angle may be in radian, degrees or grades.</summary>
		<param name="Real:value" default="" type="n"></param>
		<param name="anglemode:mode" default="radian" type="n"></param>
		<return>cosine of the angle.</return>
	</function>
	<function name="RealTan">
		<summary>Return the tangent. The input angle may be in radian, degrees or grades.</summary>
		<param name="Real:value" default="" type="n"></param>
		<param name="anglemode:mode" default="radian" type="n"></param>
		<return>tangent of the angle.</return>
	</function>
	<function name="RealAtan2">
		<summary>Return the arctangent of x/y. The input angle may be in radian, degrees or grades.</summary>
		<param name="Real:value" default="" type="n"></param>
		<param name="anglemode:mode" default="radian" type="n"></param>
		<notes>Uses fatan2 for Fixed Point Number. Uses floatatan2 for Floating Point Number. </notes>
		<return>arc tangent of x/y.</return>
	</function>
	<function name="RealClamp">
		<summary>Return value thats has been limited between min and max.</summary>
		<param name="Real:value" default="" type="n"></param>
		<param name="Real:min" default="" type="n"></param>
		<param name="Real:max" default="" type="n"></param>
	</function>
</api>
***/