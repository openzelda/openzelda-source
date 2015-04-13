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
 *     2013/01/31 [luke]: switch to Pawn 4
 ***********************************************/

#include <entities>
#include <public_events>

#define STATEFUNCTION  

#define MENU_ENTITY_ID "menuent"  
#define HUD_ENTITY_ID "hudent"  

const _STATE: {
	ALIVE = 0,
	STANDING,
	MOVING,
	HIT,
	KNOCKED,
	JUMPING,
	SWIMMING,
	FLOATING,
	CHASING,
	USING,
	LIFTING,
	STANDLIFT,
	WALKLIFT,
	SPECIALSTATE,
	FALLING,
	WAITING,
	LEAPING,
	PUSHING,
	PULLING,
	DYING,
	DEAD,
	GONE,
	STATE_COUNT
}

const _STATEEFFECTS: {
	NOEFFECT = 0,
	STUNNED,
	FROZEN,
	SHOCKED,
	BURNING,
}

stock type_name[11]{} = [ "UNKNOWN","ENEMY","DOOR","WEAPON","AWAKING","TRANSPORT","PUSHABLE","SWITCH","ITEM","PLAYER","OUTOFRANGE" ];

#define TYPE_UNKNOWN 0
#define TYPE_ENEMY 1
#define TYPE_DOOR 2
#define TYPE_WEAPON 3
#define TYPE_AWAKING 4
#define TYPE_TRANSPORT 5
#define TYPE_PUSHABLE 6
#define TYPE_SWITCH 7
#define TYPE_ITEM 8
#define TYPE_PLAYER 9




#define RECT[.x, .y, .w, .h]
#define POSITION[.x, .y, .z]
#define POSITIONF[Fixed:.x, Fixed:.y, Fixed:.z]

#define SwitchEvent[.active, .entity, .function{24}]

#define EntityInfo[.hash, .id{32}]
#define EntityGraphic[object:.obj, .x, .y, .z, .width, .height]


stock entityId:private_map_id = 0;
stock entityId:private_main_id = 0;

stock entityId:get_private_map_id()
{
	if ( !private_map_id )
	{
		private_map_id = entityId:EntityHash("__map__");
	}
	return private_map_id;
}

stock entityId:get_private_main_id()
{
	if ( !private_main_id )
	{
		private_main_id = entityId:EntityHash("main");
	}
	return private_main_id;
}


#define ENTITY_MAP get_private_map_id() 
#define ENTITY_MAIN get_private_main_id()


/* Item Types */
#define ITEM_BOMBS 1
#define ITEM_ARROW 2
#define ITEM_RUPEE 3
#define ITEM_KEY 4
#define ITEM_MASTERKEY 5


/* Mask Values */
#define MASK_CLEAR 0
#define MASK_DIRT 4
#define MASK_GRASS 8
#define MASK_PUDDLE 16
#define MASK_ICE 24

#define MASK_NORMALGROUND 64

#define MASK_ENEMYSOLID1 120 // Air Based

#define MASK_HOLE1 140
#define MASK_HOLE2 142

#define MASK_ENEMYSOLID2 160 // Ground Based

#define MASK_WALK 162
#define MASK_AUTOWALK 164

#define MASK_PLAYERSOLID 200
#define MASK_PLAYERSOLID2 202

#define MASK_WATER 180

#define MASK_BLOCK 240

#define MASK_LEAP 250
#define MASK_SOLID3 253
#define MASK_SOLID2 254
#define MASK_SOLID 255

/* Weapon Types */
#define AEXPLOSION 1
#define ASWORD 2
#define APLAYER 4
#define AFIRE 8
#define AICE 16
#define ASTUN 32
#define ASHIELD 64
#define AMAGIC 128


