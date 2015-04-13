/* String functions
 *
 * (c) Copyright 2005-2007, ITB CompuPhase
 * This file is provided as is (no warranties).
 */
#if defined _string_included
  #endinput
#endif
#define _string_included

///String Functions
native strlen(const string[]);
native strpack(dest[], const source[], maxlength=sizeof dest);
native strunpack(dest[], const source[], maxlength=sizeof dest);
native strcopy(dest[], const source[], maxlength=sizeof dest);
native strcat(dest[], const source[], maxlength=sizeof dest);

native strmid(dest[], const source[], start=0, end=cellmax, maxlength=sizeof dest);
native bool:strins(string[], const substr[], index, maxlength=sizeof string);
native bool:strdel(string[], start, end);

native strcmp(const string1[], const string2[], bool:ignorecase=false, length=cellmax);
native strfind(const string[], const sub[], bool:ignorecase=false, index=0);

native strval(const string[], index=0);
native valstr(dest[], value, bool:pack=false);
native bool:ispacked(const string[]);

native strformat(dest[], size=sizeof dest, bool:pack=false, const format[], {Fixed,Float,_}:...);

native uudecode(dest[], const source[], maxlength=sizeof dest);
native uuencode(dest[], const source[], numbytes, maxlength=sizeof dest);
native memcpy(dest[], const source[], index=0, numbytes, maxlength=sizeof dest);
native urldecode(dest[], const source[], maxlength=sizeof dest, bool:pack=false);
native urlencode(dest[], const source[], maxlength=sizeof dest, bool:pack=false);

native StringLength(const string[]) = strlen;
native StringPack(dest[], const source[], maxlength=sizeof dest) = strpack;
native StringUnpack(dest[], const source[], maxlength=sizeof dest) = strunpack;
native StringCopy(dest[], const source[], maxlength=sizeof dest) = strcopy;
native StringConcatenate(dest[], const source[], maxlength=sizeof dest) = strcat;

native StringExtract(dest[], const source[], start=0, end=cellmax, maxlength=sizeof dest) = strmid;
native bool:StringInsert(string[], const substr[], index, maxlength=sizeof string) = strins;
native bool:StringRemove(string[], start, end) = strdel;

native StringFind(const string[], const sub[], bool:ignorecase=false, index=0)  = strfind;
native StringNumber(dest[], value, bool:pack=false)  = valstr;
native bool:StringIsPacked(const string[]) = ispacked;

native StringFormat(dest[], size=sizeof dest, const format[], {Fixed,Float,_}:...);

stock bool: StringEqual(const string1[], const string2[], bool:ignorecase=false, length=cellmax)
{
	return strcmp(string1, string2, ignorecase, length) == 0;
}

/***
<api title="String Function">
	<function name="StringLength">
		<summary>Returns the length of the the string</summary>
		<param name="string[]" default="" type="a"></param>
	</function>
	<function name="StringPack">
		<summary>Packs the source string into dest.</summary>
		<param name="dest[]" default="" type="a"></param>
		<param name="source[]" default="" type="a"></param>
	</function>
	<function name="StringUnpack">
		<summary>Unpacks the source string into dest.</summary>
		<param name="dest[]" default="" type="a"></param>
		<param name="source[]" default="" type="a"></param>
	</function>
	<function name="StringCopy">
		<summary>Copies the source string into dest.</summary>
		<param name="dest[]" default="" type="a"></param>
		<param name="source[]" default="" type="a"></param>
	</function>
	<function name="StringConcatenate">
		<summary>Appends the source string on to dest.</summary>
		<param name="dest[]" default="" type="a"></param>
		<param name="source[]" default="" type="a"></param>
	</function>
	<function name="StringExtract">
		<summary>Copy part of source string into dest.</summary>
		<param name="dest[]" default="" type="a"></param>
		<param name="source[]" default="" type="a"></param>
		<param name="start" default="" type="n"></param>
		<param name="end" default="cellmax" type="n"></param>
	</function>
	<function name="StringInsert">
		<summary>Insert substr into string at index.</summary>
		<param name="string[]" default="" type="a"></param>
		<param name="substr[]" default="" type="a"></param>
		<param name="index" default="" type="n"></param>
	</function>
	<function name="StringRemove">
		<summary>Removes part the string.</summary>
		<param name="string[]" default="" type="a"></param>
		<param name="start" default="" type="n"></param>
		<param name="end" default="cellmax" type="n"></param>
	</function>
	<function name="StringFind">
		<summary>Returns the starting position of the sub string or -1 if it's not found.</summary>
		<param name="string[]" default="" type="a"></param>
		<param name="sub[]" default="" type="a"></param>
	</function>
	<function name="StringNumber">
		<summary>Converts a whole number into string 'dest'.</summary>
		<param name="dest[]" default="" type="a"></param>
		<param name="value" default="" type="n"></param>
	</function>
	<function name="StringIsPacked">
		<summary>Check if string is packed.</summary>
		<param name="string[]" default="" type="a">String to check.</param>
		<return>True if string has been packed.</return>
	</function>
	<function name="StringFormat">
		<summary>Prints formatted text to string 'dest'.</summary>
		<param name="dest[]" default="" type="a">Destination string.</param>
		<param name="size" default="_" type="n">size of destination string.</param>
		<param name="pack" default="_" type="n">Pack string contents.</param>
		<param name="format" default="" type="a">Format String</param>
		<param name="..." default="" type="v"> List of Arguments to used to produce the formatted text.</param>
		<example>
		new string[128];
		strformat(string, _, true, "Text: %s, Color: %x, Number: %d, Fixed: %q\n", "Hello", 0xFFBBAAFF, 42, 50.254);
		ConsoleOutput(string);
		</example>
	</function>
	<function name="StringEqual">
		<summary>Check if the two string are the same.</summary>
		<param name="string1[]" default="" type="a"></param>
		<param name="string2[]" default="" type="a"></param>
		<param name="ignorecase" default="false" type="n"></param>
	</function>
</api>
***/
