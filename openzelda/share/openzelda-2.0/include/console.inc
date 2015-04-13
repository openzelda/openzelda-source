#if defined _console_included
  #endinput
#endif
#define _console_included


native ConsoleLog( text[], {Fixed,_}:... );
native ConsoleOutput( text[], {Fixed,_}:... );
native ConsoleInfomation();

/***
<api title="Console Functions">
	<function name="ConsoleLog">
		<summary>Prints formatted text to a text file.</summary>
		<param name="text[]" default="" type="a">Format string.%c print a character at this position. %d print a number at this position in decimal radix. %f print a floating point number at this position. %q print a fixed point number at this position. %s print a character string at this position. %x print a number at this position in hexadecimal radix</param>
		<param name="..." default="" type="v">List of Arguments to used to produce the formatted text</param>
		<example>
		ConsoleLog("Doing Something :)");
		ConsoleLog("Text: %s, Color: %x, Number: %d, Fixed: %q\n", "Hello", 0xFFBBAAFF, 42, 50.254); // Outputs Text: Hello, Color: FFBBAAFF, Number: 42, Fixed: 50.254
		ConsoleLog("Text: %s, Color: %d, Number: %05d, Fixed: %.1q\n", "Hello", 0xFFBBAAFF, 42, 50.254); // Outputs Text: Hello, Color: -4478209, Number: 00042, Fixed: 50.3
		</example>
	</function>
	<function name="ConsoleOutput">
		<summary>Prints formatted text to the debug console.</summary>
		<param name="text[]" default="" type="a">Format string. %c print a character at this position. %d print a number at this position in decimal radix. %f print a floating point number at this position. %q print a fixed point number at this position. %s print a character string at this position. %x print a number at this position in hexadecimal radix</param>
		<param name="..." default="" type="v">List of Arguments to used to produce the formatted text.</param>
		<example>
		ConsoleOutput("Doing Something :)");
		ConsoleOutput("Text: %s, Color: %x, Number: %d, Fixed: %q\n", "Hello", 0xFFBBAAFF, 42, 50.254); // Outputs Text: Hello, Color: FFBBAAFF, Number: 42, Fixed: 50.254
		ConsoleOutput("Text: %s, Color: %d, Number: %05d, Fixed: %.1q\n", "Hello", 0xFFBBAAFF, 42, 50.254); // Outputs Text: Hello, Color: -4478209, Number: 00042, Fixed: 50.3
		</example>
	</function>
</api>
***/