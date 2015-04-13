#if defined _game_included
	#endinput
#endif

#define _game_included

#include <fixed>
#include <enums>
#include <console>

///Engine Functions 
native GameState(newstate = -1);
native GameFrame();
native Fixed:GameFrameSeconds() = GameFrame;

///Save Functions
native GameLoad(slot);
native GameSave(slot, detail[64], maxlength=sizeof detail);
native GameSaveHibernate();
native GameHasSave(slot);
native GameRemoveSave(slot);

native GameGetDetails(slot, array[64], bool:pack_array = false, maxlength=sizeof array);
native GameDetails(gameid, slot, array[64], bool:pack_array = false, maxlength=sizeof array);

///Language Functions
native LanguageString(line, returnstring{}, maxlength=sizeof returnstring);
native LanguageSet(lang[], maxlength=sizeof lang);

///Dialog Functions
native DialogShow(line, callback{} = "dialogbox");
native DialogGetString(line, returnstring{}, maxlength=sizeof returnstring);

/// Audio Functions
native AudioPlayDialog(line);
native AudioPlayMusic(file{}, loop = 0, fadeLength = 0);
native AudioPlaySound(file{}, x = -1, y = -1);

// Misc Functions */
native FixedToFloat(value);
native FloatToFixed(value);
native StringHash(name{});


stock object:ObjectHash(name{}, global)
{
	new o = StringHash(name)
	if ( global )
	{
		o |= OBJECT_GLOBAL_VALUE;
	}
	else
	{
		o &= OBJECT_LOCAL_VALUE;
	}
	return object:o;

}


/***
<api title="Game Function">
	<function name="GameState">
		<summary>Get/Set Global State.</summary>
		<param name="newstate" default="-1" type="n">Set new engine state, -1 to skip.</param>
		<return>Current state of the engine.</return>
		<example>
		if ( GameState() == 0 )
		{
			if ( InputButton(0) )
			{
				GameState(1);
			}
		}
		else
		{
			MoveEntity();
		}
		</example>
	</function>
	<function name="GameFrame">
		<summary>Return the length of last frame in milliseconds.</summary>
		<return>Length of time of last frame.</return>
		<example>
		new time = GameFrame();
		ConsoleOutput("Last Frame took %d milliseconds", time);
		</example>
	</function>
	<function name="GameFrameSeconds">
		<summary>Return the length of last frame in seconds as a fixed point number.</summary>
		<return>Length of time of last frame.</return>
		<example>
		new Fixed:time = GameFrameSeconds();
		ConsoleOutput("Last Frame took %q seconds", time);
		</example>
	</function>
	<function name="GameLoad">
		<summary>Load Game from Slot.</summary>
		<param name="slot" default="" type="n">Save slot to use, 1-9 are available.</param>
	</function>
	<function name="GameSave">
		<summary>Save Current Game.</summary>
		<param name="slot" default="" type="n">Save slot to use, 1-9 are available.</param>
		<param name="detail[64]" default="" type="a">256 bytes of extra save data, for uses such as high scores, player name, time of save. This can be access via other games if feature is enable.</param>
	</function>
	<function name="GameSaveHibernate">
		<summary>Hibernate a game</summary>
	</function>
	<function name="GameHasSave">
		<summary>Check if file exist.</summary>
		<param name="slot" default="" type="n">Save slot to use, 1-9 are available.</param>
	</function>
	<function name="GameRemoveSave">
		<summary>Remove Save Game.</summary>
		<param name="slot" default="" type="n">Save slot to use, 1-9 are available.</param>
	</function>
	<function name="GameGetDetails">
		<summary>Get Save Game Details.</summary>
		<param name="slot" default="" type="n">Save slot to use, 0-9 are available.</param>
		<param name="array[64]" default="" type="a">Array to store the information in.</param>
		<example>
		new array[64];
		GameGetDetails(0, array);
		highscore = array[0];
		</example>
	</function>
	<function name="GameDetails">
		<summary>Get another game save details.</summary>
		<param name="gameid" default="" type="n">ID to the game you wish to access.</param>
		<param name="slot" default="" type="n">Save slot to use, 1-9 are available.</param>
		<param name="array[64]" default="" type="a">Array to store the information in.</param>
		<example>
		new array[64];
		GameDetails(prequel_id, 0, array);
		played_prequel = array[0];
		</example>
	</function>
	<function name="LanguageString">
		<summary>Get String from Language File.</summary>
		<param name="line" default="" type="n">line of language wanted.</param>
		<param name="returnstring[]" default="" type="a">String to store language string.</param>
	</function>
	<function name="LanguageSet">
		<summary>Loads a new language set. note: previous strings may have updated.</summary>
		<param name="lang[]" default="" type="a">Two letter language code </param>
	</function>
	<function name="DialogShow">
		<summary>Shows Dialog box.</summary>
		<param name="line" default="" type="n">the requested line.</param>
		<param name="callback[]" default="&quot;dialogbox&quot;" type="a">Function to call in the main script.</param>
	</function>
	<function name="DialogGetString">
		<summary>Get String from Dialog File.</summary>
		<param name="line" default="" type="n">the requested line.</param>
		<param name="returnstring[]" default="" type="a"></param>
	</function>
	<function name="AudioPlayDialog">
		<summary>Play Dialog Audio</summary>
		<param name="line" default="" type="n">The Audio device you wish to change.</param>
	</function>
	<function name="AudioPlaySound">
		<summary>Play Sound Effect. Also return length of file in seconds.</summary>
		<param name="file[]" default="" type="a">filename of the sound effect.</param>
		<param name="x" default="-1" type="n">X-axis screen position of the sound.</param>
		<param name="y" default="-1" type="n">y-axis screen position of the sound.</param>
		<return>Length of the audio file.</return>
	</function>
	<function name="AudioPlayMusic">
		<summary>Play Music.</summary>
		<param name="file[]" default="" type="a">Filename of the music you wish to play.</param>
		<param name="loop" default="1" type="n">Set to false to play music only once.</param>
		<param name="fadeLength" default="0" type="n">The length of fade in milliseconds</param>
	</function>
</api>
***/
