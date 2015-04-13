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
#include <player>

/* Variable */
public mqMaxHealth = 300;
public mqHealth = 300;

new mqHudEntity = 0;
new mqMenuEntity = 0;
new controller = 0;

new Fixed:push_delay = 0.00;


/* Public Functions */
forward public UpdatePosition();
forward public Hurt(type, damage, angle);
forward public GiveWeapon( weapon{} );
forward public SetWeapon( weapon_id, slot );
forward public SetPlayerSprites( normal{32}, push{32}, pull{32}, swim{32}, using{32} );
forward public SetRestartPosition( point, ndescription{}, nx, ny, nmapid );
forward public UsingCustomAnimation()

 /* Animation Code */ 
#define STATE_FLIP (mqDisplayDirection == 3 ? 16 : 0)
#define STATE_GRAPHIC animation[mqState][mqDisplayDirection]

stock animationUsingCustom = false;
stock animation[STATE_COUNT][4]{32} = [
	//SOUTH, WEST, NORTH, EAST, 
	/* ALIVE */			["", "", "", ""],
	/* STANDING */		["", "", "", ""],
	/* MOVING */		["", "", "", ""],
	/* HIT */			["", "", "", ""],
	/* KNOCKED */		["", "", "", ""],
	/* JUMPING */		["", "", "", ""],
	/* SWIMMING */		["", "", "", ""],
	/* FLOATING */		["", "", "", ""],
	/* CHASING */		["", "", "", ""],
	/* USING */			["", "", "", ""],
	/* LIFTING */		["", "", "", ""],
	/* STANDLIFT */		["", "", "", ""],
	/* WALKLIFT */		["", "", "", ""],
	/* SPECIALSTATE */	["", "", "", ""],
	/* FALLING */		["", "", "", ""],
	/* WAITING */		["", "", "", ""],
	/* LEAPING */		["", "", "", ""],
	/* PUSHING */		["", "", "", ""],
	/* PULLING */		["", "", "", ""],
	/* DYING */			["", "", "", ""],
	/* DEAD */			["", "", "", ""],
	/* GONE */			["", "", "", ""]
];

public UsingCustomAnimation()
{
	animationUsingCustom = true;
}

SetStateGraphic( _STATE:s, parent[], south_graphic[], west_graphic[], north_graphic[], east_graphic[] )
{
	s = _STATE:NumberClamp( _:s, _:ALIVE, _:DEAD );
	strformat( animation[s][0], _, true, "%s:%s", parent, south_graphic);
	strformat( animation[s][1], _, true, "%s:%s", parent, west_graphic);
	strformat( animation[s][2], _, true, "%s:%s", parent, north_graphic);
	strformat( animation[s][3], _, true, "%s:%s", parent, east_graphic);
}

public SetPlayerSprites( normal{32}, push{32}, pull{32}, swim{32}, using{32} )
{
	SheetReference(normal);
	SheetReference(push);
	SheetReference(pull);
	SheetReference(swim);
	SheetReference(using);

	SetStateGraphic( STANDING, normal, "front_0", "side_0", "back_0", "side_0" );
	SetStateGraphic( MOVING, normal, "front", "side", "back", "side" );
	SetStateGraphic( USING, using, "front", "side", "back", "side" );
	SetStateGraphic( KNOCKED, normal, "front_knock", "side_knock", "back_knock", "side_knock" );
	SetStateGraphic( PUSHING, push, "front", "side", "back", "side" );
	SetStateGraphic( PULLING, pull, "front", "side", "back", "side" );
	SetStateGraphic( DYING, normal, "death", "death", "death", "death" );
	SetStateGraphic( DEAD, normal, "death_1", "death_1", "death_1", "death_1" );

}

/* Restart Point Code */
#define RestartPoint[.description{32}, .section{64}, .x, .y, .dir, .mapid, .gridx, .gridy, .active]
new _restart[3][RestartPoint];

public SetRestartPosition( point, ndescription{}, nx, ny, nmapid )
{
	point %= 3;
	StringCopy(_restart[point].description, ndescription);

	if ( nx == -1 )
		_restart[point].x = _:mqEntityPosition.x;
	else
		_restart[point].x = nx;

	if ( ny == -1 )
		_restart[point].y = _:mqEntityPosition.y;
	else
		_restart[point].y = ny;

	
	if ( nmapid == -1 )
	{
		SectionGet(_restart[point].section, _restart[point].gridx, _restart[point].gridy);
		_restart[point].mapid = 0;
	}
	else
		_restart[point].mapid = nmapid;
	_restart[point].active = 1;
}

/* Hurting and Death */
new hit = 0;
new hidden = 1;
new DeathCount = 0;
HandleDeath()
{
	ObjectPosition(mqDisplayObject, mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex, 0, 0);
	if ( mqState == DEAD ) 
	{
		DeathCount = 0;
		ObjectReplace(mqDisplayObject, animation[DEAD][0], SPRITE);
		EntityPublicFunction( ENTITY_MAIN, "CustomText", ''snnnnnn'', "Press Enter to continue", -1, -1, 6000, 11, 2, 255 );
		if ( InputButton( 6 ) )
		{
			mqHealth = mqMaxHealth;
			mqState = STANDING;
			LayerColour(0, 0xFFFFFFFF);
			LayerColour(1, 0xFFFFFFFF);
			LayerColour(2, 0xFFFFFFFF);
			LayerColour(3, 0xFFFFFFFF);
			LayerColour(4, 0xFFFFFFFF);
			LayerColour(5, 0xFFFFFFFF);
		
			ObjectReplace(mqDisplayObject, animation[STANDING][0], SPRITE);

			if ( _restart[0].mapid == 0 )
			{
				TransitionPlayer( mqEntityId, 0, 0, _restart[0].section, _restart[0].gridx, _restart[0].gridy);
			}
			else
			{
				TransitionPlayer( mqEntityId, 0, _restart[0].mapid, "");
			}
			EntitySetPosition( _restart[0].x, _restart[0].y, _ );
		}
	}
	else if ( mqState == DYING ) 
	{
		LayerColour(0, 0xFF0000FF);
		LayerColour(1, 0xFF0000FF);
		LayerColour(2, 0xFF0000FF);
		LayerColour(3, 0xFF0000FF);
		LayerColour(4, 0xFF0000FF);
		LayerColour(5, 0xFF0000FF);
		
		if ( DeathCount == 0 )
		{
			DeathCount = AnimationGetLength( "p01n.png", "death" );
			ObjectReplace(mqDisplayObject, animation[DYING][0], SPRITE);
		}
		if ( TimerCountdown(DeathCount) )
			mqState = DEAD;
	}
}

/* Weapon */

new weapon_action[3] = [0,0,0];
new mqSelectedWeapons[4];
new mqUsingWeapon = -1;

public GiveWeapon( weapon{} )
{
	new hash;
	new entityName{64};
	StringCopy(entityName, weapon);

	hash = EntityCreate( entityName, "", 0, 0, mqEntityPosition.z, GLOBAL_MAP );

	EntityPublicFunction( hash, "SetOwner", ''n'', mqEntityId );
	EntityPublicFunction( mqMenuEntity, "AddWeapon", ''snn'', entityName, hash, mqEntityId );

	SetWeapon( hash, BUTTON_ACTION1 );
}

public SetWeapon( weapon_id, slot )
{
	if ( slot >= 0 && slot < 4 ) 
	{
		mqSelectedWeapons[slot] = weapon_id;
	}
}
