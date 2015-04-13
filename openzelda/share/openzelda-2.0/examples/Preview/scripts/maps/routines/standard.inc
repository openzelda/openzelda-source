/***********************************************
 * Copyright © Luke Salisbury
 *
 * You are free to share, to copy, distribute and transmit this work
 * You are free to adapt this work
 * Under the following conditions:
 *  You must attribute the work in the manner specified by the author or licensor (but not in any way that suggests that they endorse you or your use of the work). 
 *  You may not use this work for commercial purposes.
 * Full terms of use: http://creativecommons.org/licenses/by-nc/3.0/
 * Changes:
 *     2013/04/01 [luke]: new file.
 ***********************************************/



new background_music{64} = ""; 
new screen_transition_mode{20} = "";
new map_id = 0;

InitMap()
{
	map_id = MapCurrent();
	EntityGetSetting("music", background_music, SELF); 
	EntityGetSetting("transition", screen_transition_mode, SELF); 
	AudioVolume(MUSIC, 64);
	if ( background_music{0} )
		AudioPlayMusic(background_music);
}
