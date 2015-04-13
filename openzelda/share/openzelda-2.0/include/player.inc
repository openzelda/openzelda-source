#if defined _player_included
	#endinput
#endif
#define _player_included

#include <controller>


/// Player Functions 
#pragma deprecated Future Feature
native PlayerAdd();
#pragma deprecated Future Feature
native PlayerRemove(num);
#pragma deprecated Future Feature
native PlayerCount();
#pragma deprecated Future Feature
native PlayerSetAI(player, entity);
#pragma deprecated Future Feature
native PlayerSetEntity(player, entity);


native PlayerUpdateControl(player, controlset);
native PlayerSetName( player, str{} );
native PlayerGetName( player, str{}, s = sizeof(str));
native PlayerSetColour(player, colour);

/***
<api title="Player Functions">
	<futurefunction name="PlayerAdd">
		<summary>Adds new player</summary>
		<return>0 on Error.</return>
	</futurefunction>
	<futurefunction name="PlayerRemove">
		<summary>Removes a players</summary>
		<param name="num" default="" type="n"></param>
		<return>0 on Error.</return>
	</futurefunction>
	<futurefunction name="PlayerCount">
		<summary>Returns the amount of active players.</summary>
		<return>Returns the amount of active players.</return>
	</futurefunction>
	<futurefunction name="PlayerUpdateControl">
		<summary></summary>
		<param name="player" default="" type="n"></param>
		<param name="controlset" default="1" type="n"></param>
	</futurefunction>
	<futurefunction name="PlayerSetAI">
		<summary></summary>
		<param name="player" default="" type="n"></param>
		<param name="entity" default="" type="n"></param>
	</futurefunction>
	<futurefunction name="PlayerSetEntity">
		<summary></summary>
		<param name="player" default="" type="n"></param>
		<param name="entity" default="" type="n"></param>
	</futurefunction>
	<function name="PlayerSetName">
		<summary>Set the name of player</summary>
		<param name="player" default="" type="n"></param>
		<param name="str[]" default="" type="a"></param>
	</function>
	<function name="PlayerGetName">
		<summary>Gets the name of player</summary>
		<param name="player" default="" type="n"></param>
		<param name="str[]" default="" type="a"></param>
	</function>
</api>
***/
