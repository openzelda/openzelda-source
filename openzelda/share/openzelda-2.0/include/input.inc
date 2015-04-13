#if defined _input_included
	#endinput
#endif

#define _input_included

#include <fixed>
#include <enums>

/***
<api>
	<function name="KeyboardWatch">
		<summary>Send Keyboard input to KeyboardInput function in the current entity.</summary>
		<param name="able" default="" type="n"></param>
		<example>
		forward public KeyboardInput( unicode );
		public KeyboardInput( unicode )
		{
			ConsoleOutput("Input: %c %d", unicode, unicode)
		}

		public Init( ... )
		{
			KeyboardWatch(true);
		}
		</example>
	</function>
	<function name="InputButton">
		<summary>Returns Button value for Player.</summary>
		<param name="input" default="" type="n">A value between 0-15.</param>
		<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
		<return>0 for un-pressed button, 1 for pressed button, 2 for held down button</return>
		<example>
		if ( InputButton(0,0) == 1 )
			Shoot();
		</example>
	</function>
	<function name="InputAxis">
		<summary>Returns Axis value for Player.</summary>
		<param name="axis" default="" type="n">A value between 0-5, this representing x1,y1,z1,x2,y2,z2 axis.</param>
		<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
		<return>Value between -255 and 255.</return>
		<example>
		if ( InputAxis(1,0) == 255 )
			MoveUp();
		</example>
	</function>
	<function name="InputPointer">
		<summary>Returns Pointer value for Player.</summary>
		<param name="axis" default="" type="n">0 for x axis, 1 for y axis.</param>
		<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
		<return>Value the current value of axis of mouse or touchscreen.</return>
		<example>
		if ( InputButton(0,0) == 1 )
			ShootAt(InputPointer(0,0), InputPointer(1,0));
		</example>
	</function>
	<function name="InputButtonSet">
		<summary>Set Button value for the Player.</summary>
		<param name="input" default="" type="n">A value between 0-15.</param>
		<param name="value" default="" type="n">0 for un-pressed button, 1 for pressed button, 2 for held down button</param>
		<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
	</function>
	<function name="InputAxisSet">
		<summary>Set Axis value for the Player.</summary>
		<param name="axis" default="" type="n">A value between 0-5, this representing x1,y1,z1,x2,y2,z2 axis.</param>
		<param name="value" default="" type="n">Value between -255 and 255.</param>
		<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
	</function>
	<function name="InputPointerSet">
		<summary>Set Pointer value for the Player.</summary>
		<param name="axis" default="" type="n">0 for x axis, 1 for y axis.</param>
		<param name="value" default="" type="n"></param>
		<param name="player" default="0" type="n">0 for current player otherwise the selected player.</param>
	</function>
</api>
***/



///Input Functions
native KeyboardWatch( able );
native InputButton(input, player = 0);
native InputAxis(axis, player = 0);
native InputPointer(axis, player = 0);
native InputButtonSet(input, value, player = 0);
native InputAxisSet(axis, value, player = 0);
native InputPointerSet(axis, value, player = 0);
native InputSetDefault(player);

