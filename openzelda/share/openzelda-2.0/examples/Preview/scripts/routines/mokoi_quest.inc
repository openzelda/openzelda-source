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
 *     2010/01/11 [luke]: new file.
 ***********************************************/
#if defined _mokoi_quest_included
  #endinput
#endif
#define _mokoi_quest_included

/* Mokoi Quest
 *  Routines that are part of most entities
 */

#include <quest_types>
#include <helper>


/* public functions */
forward public SetState(q);
forward public GetState();
forward public GetType();
forward public SetType(q);
forward public GetDir();
forward public SetDir(q);

/* Public Variables */
public _STATE:mqState = ALIVE;
public mqType = 0;
public mqDirection = SOUTH;
public object:mqDisplayObject = OBJECT_NONE;


/* Local Variables */

stock entityId:mqEntityId;
stock mqEntityPosition[POSITIONF];
stock mqDisplayArea[RECT] = [ 0, 0, 16, 16];
stock mqDisplayZIndex, mqDisplayLayer;
stock mqDisplayOffset[POSITION];
stock mqDisplayDirection = 0;
stock mqUpdate = false;
new mqDirectionPrev = -1;
new _STATE:mqStatePrev =  _STATE:-1;

/* Functions */
stock SetupEntity( _STATE:init_state, type, &object:display_object, width, height, offset_x = 0, offset_y = 0 )
{
	mqState = init_state;
	mqType = type;
	mqDisplayArea.h = height;
	mqDisplayArea.w = width;
	mqDisplayOffset.x = offset_x
	mqDisplayOffset.y = offset_y;
	mqDisplayObject = EntityGetObject();
	mqEntityId = entityId:EntityGetSettingHash("id");
	GetEntityPosition(mqEntityPosition.x, mqEntityPosition.y, mqEntityPosition.z, mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex, mqDisplayLayer);

}

stock SetEntityDimension( width, height, offset_x = 0, offset_y = 0 )
{
	mqDisplayArea.h = height;
	mqDisplayArea.w = width;
	mqDisplayOffset.x = offset_x
	mqDisplayOffset.y = offset_y;

	GetEntityPosition(mqEntityPosition.x, mqEntityPosition.y, mqEntityPosition.z, mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex, mqDisplayLayer);
}

stock GetEntityPosition( &Fixed:fx, &Fixed:fy, &Fixed:fz, &ix, &iy, &iz, &il)
{
	EntityGetPosition(fx, fy, fz); // Fixed Position
	ix = fround(fx, round_unbiased); // Display Positions
	iy = fround(fy, round_unbiased);
	il = fround(fz, round_floor);
	iz = (il*1000) + NumberAbs( iy/8 );
}

stock UpdateDisplayPosition()
{
	mqDisplayArea.x = fround(mqEntityPosition.x, round_unbiased);
	mqDisplayArea.y = fround(mqEntityPosition.y, round_unbiased);
	mqDisplayLayer = fround(mqEntityPosition.z, round_floor);
	mqDisplayZIndex = (mqDisplayLayer*1000) + NumberAbs(mqDisplayArea.y/8);
}

stock HasStateChanged()
{
	new changed = false;
	if ( mqState != mqStatePrev || mqDirection != mqDirectionPrev )
		changed = mqUpdate = true;
	else
		mqUpdate = false;
	mqStatePrev = mqState;
	mqDirectionPrev = mqDirection;
	return changed;
}


/* public functions */
public GetDir()
{
	return mqDirection;
}

public SetDir(q)
{
	q %= 8; // Make sure q is between 0-7.
	mqDirection = q;
	mqDisplayDirection = q/2;
	return mqDirection;
}

public GetState()
{
	return mqState;
}

public SetState(q)
{
	mqState = _STATE:q;
	return mqState;
}

public GetType()
{
	return mqType;
}

public SetType(q)
{
	mqType = q;
	return mqType;
}




/***
<variables name="mqDisplayObject"/>
<function name="SetupEntity">
	<summary></summary>
</function>
***/
