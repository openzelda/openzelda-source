#if defined _network_included
	#endinput
#endif
#define _network_included


native EntityActive();
native EntityNetworkSync(bool:reliable = true);

///Network Connection Functions
native NetworkConnect(name{}, password{}, email{});
native NetworkDisconnect();
native NetworkMessage(reliable, serveronly, message[], length, reallength = sizeof(message));
native NetworkPlayerName(player, name{}, l = sizeof(name) );

///Online Functions
#pragma deprecated Future Feature
native OnlineUpdate();
#pragma deprecated Future Feature
native OnlineSubmitScore(data[4]);
#pragma deprecated Future Feature
native OnlineSave(data[64]);
native OnlineData(callback{32} = "ReceivedData", url{128}, post{} = "");



/***
<api title="Network Functions">
	<function name="EntityActive">
		<summary>Ables networking syncing for entity.</summary>
		<return>True if active</return>
	</function>
	<function name="EntityNetworkSync">
		<summary>Send network sync message.</summary>
		<param name="bool:reliable" default="true" type="n">Use an reliable message.</param>
	</function>
	<function name="NetworkConnect">
		<summary>Connect to Server.</summary>
		<param name="name[]" default="" type="a"></param>
		<param name="password[]" default="" type="a"></param>
		<param name="email[]" default="" type="a"></param>
	</function>
	<function name="NetworkDisconnect">
		<summary>Disconnect from Server.</summary>
	</function>
	<function name="NetworkMessage">
		<summary>Sends a message to entity with the same unique id.</summary>
		<param name="reliable" default="" type="n"></param>
		<param name="serveronly" default="" type="n"></param>
		<param name="message[]" default="" type="a"></param>
		<param name="length" default="" type="n"></param>
	</function>
	<function name="NetworkPlayerName">
		<summary>Gets network player name.</summary>
		<param name="player" default="" type="n"></param>
		<param name="name[]" default="" type="a"></param>
	</function>

	<function name="OnlineUpdate">
		<summary>"Future Function" Downloads latest game patches.</summary>
	</function>
	<function name="OnlineSubmitScore">
		<summary>Future Function" Sumbit score to online scoreboard.</summary>
		<param name="data[4]" default="" type="a"></param>
	</function>
	<function name="OnlineSave">
		<summary>"Future Function" Online save.</summary>
		<param name="data[64]" default="" type="a"></param>
	</function>
	<function name="OnlineData">
		<summary>Downloads URL to the callback function. If data is supported media, it will be loaded, and details pass to callback function.</summary>
		<param name="callback[32]" default="&quot;ReceivedData&quot;" type="a"></param>
		<param name="url[128]" default="" type="a"></param>
		<param name="post[]" default="&quot;&quot;" type="a"></param>
	</function>
</api>
***/