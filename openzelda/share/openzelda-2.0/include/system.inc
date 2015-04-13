#if defined _system_included
	#endinput
#endif

#define _system_included

#include <fixed>
#include <enums>

/// System Function
native SystemAudioVolume(AudioDevice:device, level = -1);
native SystemCommand( command, value = 0 );
native SystemListCount( list );
native SystemListItem( list, item, content{},  maxlength=sizeof content );

/// Config Functions
native ConfigSetString(key{}, value{}, maxlength=sizeof value);
native ConfigGetNumber(key{});
native ConfigGetString(key{}, value{});
native ConfigSave();

///Files
native FileGetList(strings[]{}, directory{} = "map", size=sizeof strings);

/***
<api title="System Function">
	<function name="SystemAudioVolume">
		<summary>Sets Audio device volume.</summary>
		<param name="AudioDevice:device" default="" type="n">The Audio device you wish to change.</param>
		<param name="level" default="-1" type="n">Requested Volume level.</param>
		<return>Current volume level</return>
	</function>
	<function name="SystemCommand">
		<summary>Runs a command such as exit, resize display and such.</summary>
		<param name="command" default="" type="n">Command to run</param>
		<param name="value" default="0" type="n"></param>
	</function>
	<function name="ConfigGetString">
		<summary>Get Engine config as a string.</summary>
		<param name="config[]" default="" type="a"></param>
		<param name="value[]" default="" type="a"></param>
	</function>
	<function name="ConfigGetNumber">
		<summary>Get Engine config as a number.</summary>
		<param name="config[]" default="" type="a"></param>
		<return>A number from game settings, 0 if no value is found or not is not a number.</return>
	</function>
	<function name="ConfigSetString">
		<summary>Update Engine config.</summary>
		<param name="config[]" default="" type="a"></param>
		<param name="value[]" default="" type="a"></param>
	</function>
	<function name="FileGetList">
		<summary>List File in a directory.</summary>
		<param name="strings[][]" default="" type="a">Stores file name in directory.</param>
		<param name="directory[]" default="&quot;map&quot;" type="a">Directory name.</param>
	</function>
</api>
***/

