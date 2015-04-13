#if defined _core_included
  #endinput
#endif
#define _core_included



///Core Scripting Functions
native heapspace();

///Argument Functions
native numargs();
native getarg(arg, index=0);
native setarg(arg, index=0, value);
/***
<api title="Argument Functions">
	<function name="numargs">
		<summary>Get the number of arguments.</summary>
		<return>Number of arguments</return>
	</function>
	<function name="getarg">
		<summary>return the value of argument.</summary>
		<param name="arg" default="" type="n"></param>
		<param name="index" default="0" type="n"></param>
		<return>Value of argument.</return>
	</function>
	<function name="setarg">
		<summary>Get the number of argument</summary>
		<param name="arg" default="" type="n"></param>
		<param name="index" default="0" type="n"></param>
		<param name="value" default="" type="n"></param>
		<return></return>
	</function>
</api>
***/


///ASCII Character functions
native tolower(c);
native toupper(c);
native swapchars(c);
native CharLower(c) = tolower;
native CharUpper(c) = toupper;
native CharSwap(c) = swapchars;
/***
<api title="ASCII Character functions">
	<function name="CharLower">
		<summary>Returns lower case of c.</summary>
		<param name="c" default="" type="n"></param>
	</function>
	<function name="CharUpper">
		<summary>Returns upper case of c.</summary>
		<param name="c" default="" type="n"></param>
	</function>
	<function name="CharSwap">
		<summary>Returns the swapped case of c.</summary>
		<param name="c" default="" type="n"></param>
	</function>
</api>
***/


///Whole Number functions
native random(max);
native min(value1, value2);
native max(value1, value2);
native clamp(value, min=cellmin, max=cellmax);

native NumberRandom(max) = random;
native NumberMin(value1, value2) = min;
native NumberMax(value1, value2) = max;
native NumberClamp(value, min=cellmin, max=cellmax) = clamp;
native NumberString(const string[], index=0) = strval;
stock NumberAbs( value )
{
	return (value < 0 ? -value : value);
}
/***
<api title="Whole Number Functions">
	<function name="NumberRandom">
		<summary>Returns a random number between 0 and max - 1</summary>
		<param name="max" default="" type="n">max value.</param>
	</function>
	<function name="NumberMin">
		<summary>Returns the lower number between value1 &amp; value2</summary>
		<param name="value1" default="" type="n">Values to pick between.</param>
		<param name="value2" default="" type="n">Values to pick between.</param>
		<return>The smaller value.</return>
	</function>
	<function name="NumberMax">
		<summary>Returns the higher number between value1 &amp; value2</summary>
		<param name="value1" default="" type="n">Values to pick between.</param>
		<param name="value2" default="" type="n">Values to pick between.</param>
		<return>The larger value.</return>
	</function>
	<function name="NumberClamp">
		<summary>Return value thats has been limited between min and max.</summary>
		<param name="value" default="" type="n">The Number to check.</param>
		<param name="min" default="" type="n">Minimum limit.</param>
		<param name="max" default="" type="n">Maximum limit.</param>
		<return>The new value.</return>
	</function>
	<function name="NumberString">
		<summary>Converts a String into whole number.</summary>
		<param name="string[]" default="" type="a">String to convert.</param>
		<param name="index" default="0" type="n"></param>
	</function>
</api>
***/