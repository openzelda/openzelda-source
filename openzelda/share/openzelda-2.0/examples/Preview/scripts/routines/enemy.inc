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

#if !defined DEATHLENGTH
	#define DEATHLENGTH 1000
#endif

#include <movement>
#include <public_events>

forward public Reset();


public mqMaxHealth;
public mqHealth;

new mqDamageDealt = 50;
new mqInitialPosition[POSITIONF] = [ -99.0 , -99.0  , -99.0 ];
new bool:mqIsABoss = false;
stock bool:mqEntityResetable = false;
new mqAttacker;
new mqItemDrop{64};
new mqSprite{64};
new mqSheet{64};
new mqDeathTimer = 4200;
new object:mqDeathObject = OBJECT_NONE;

new _STATEEFFECTS:mqCurrentEffect = NOEFFECT;
new mqEffectTimer = 0;

new mqHitColours[5] = [ 0x138813FF, 0xFDD341FF, 0xE15805FF, 0x20D3EEFF, 0xEE2020FF ];

stock error_message{128};


/* States */
#define STATE_FLIP (mqDisplayDirection == 3 ? true : false)
#define STATE_GRAPHIC StateGraphics[mqState][mqDisplayDirection]
stock StateGraphics[STATE_COUNT][4]{32};

stock SetStateGraphic( _STATE:s, parent[], south_graphic[], west_graphic[], north_graphic[], east_graphic[] )
{
	s =  _STATE:NumberClamp( _:s, _:ALIVE, _:DEAD );
	strformat( StateGraphics[s][0], _, true, "%s:%s", parent, south_graphic);
	strformat( StateGraphics[s][1], _, true, "%s:%s", parent, west_graphic);
	strformat( StateGraphics[s][2], _, true, "%s:%s", parent, north_graphic);
	strformat( StateGraphics[s][3], _, true, "%s:%s", parent, east_graphic);
}

/**
* Enemy Init
*
*/
EnemyInit()
{
	mqEntityId = entityId:EntityGetSettingHash("id");
	mqDirection = SOUTH;
	mqDisplayDirection = 0;

	if ( mqInitialPosition.x == -99.0 )
	{
		GetEntityPosition( mqEntityPosition.x, mqEntityPosition.y, mqEntityPosition.z, mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex, mqDisplayLayer);

		mqState = STANDING
		mqType = TYPE_ENEMY;
		
		mqInitialPosition.x = mqEntityPosition.x;
		mqInitialPosition.y = mqEntityPosition.y;
		mqInitialPosition.z = mqEntityPosition.z;
		
		EntityGetSetting("object-image", mqSprite);
		EntityGetSetting("item", mqItemDrop);

		for ( new n = 0; mqSprite{n} && mqSprite{n} != ':' ; n++ )
		{
			mqSheet{n} = mqSprite{n};
		}	

	}
	else
	{
		Reset();
	}
}

public Reset()
{
	mqEntityPosition.x = mqInitialPosition.x;
	mqEntityPosition.y = mqInitialPosition.y;
	mqState = STANDING;
	mqHealth = mqMaxHealth;
	UpdateDisplayPosition();
}

stock HandleDying()
{
	if ( TimerCountdown(mqDeathTimer) )
	{
		mqState = DEAD;
		ObjectDelete(mqDeathObject);
		mqDeathObject = OBJECT_NONE;
		DeathEvent();
		mqDeathTimer = 0;
	}
}

stock CheckHealth()
{
	if ( mqHealth <= 0 )
		Kill();
}

stock RandomizeMovement()
{
	new new_dir;
	do {
		new_dir = random(8);
	}while ( mqDirection != new_dir )

	SetDir(new_dir);
	mqMovementAngle = Dir2Angle(mqDirection);
}


stock Kill( )
{
	mqHealth = 0;
	mqState = DYING;
	AudioPlaySound( ( mqIsABoss ? "boss_killed.wav" : "enemy_killed.wav"), mqDisplayArea.x, mqDisplayArea.y);
	
	if ( mqDeathObject == OBJECT_NONE )
		mqDeathObject = ObjectCreate("deathanim01.png:anim", SPRITE, mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex + 100, 0, 0,WHITE);
	mqDeathTimer = AnimationGetLength("deathanim01.png","anim") ;
	ObjectFlag(mqDeathObject, FLAG_ANIMLOOP, false);
}

stock DeathEvent()
{
	FunctionCall( FunctionID("EnemyClose") );
	if ( mqAttacker )
		EntityPublicFunction(mqAttacker, "AddKill", ['n', '0'], mqMaxHealth );

	if ( mqIsABoss )
	{
		new entityId, function{24};
		entityId = EntityGetSettingHash("on-death-entity" );
		EntityGetSetting("on-death-function", function );
		if ( entityId && function{0} )
			EntityPublicFunction( entityId, function );
	}
	else
	{
		EntityPublicFunction(ENTITY_MAP, "KilledEnemy");
		EntityPublicFunction(EntityHash("itemlib"), "CreateItem", ''nns'', mqDisplayArea.x + (mqDisplayArea.w / 2), mqDisplayArea.y + (mqDisplayArea.y / 2), mqItemDrop);
	}
}

stock CheckForHoles( )
{
	if (mqState == HIT || mqState == FALLING || mqState == DYING )
		return false;

	// Check if the enemy is over a hole
	if ( MaskGetValue(mqDisplayArea.x, mqDisplayArea.y) == MASK_HOLE1 )
	{
		mqState = FALLING; // Set the enemies state to falling
		AudioPlaySound("dropping.wav", mqDisplayArea.x, mqDisplayArea.y);
		return true;
	}
	return false;
}

stock Fall( img, width, height )
{
	new HoleType = MaskGetValue(mqDisplayArea.x, mqDisplayArea.y); // Check what type of hole we are in
	new length = AnimationGetLength(img);

	if ( !length )
	{
		if ( HoleType == MASK_HOLE1 ) // Check for a normal hole
		{
			Kill(); // Kill The enemy
		}
		else  // The enemy falls to another floor
		{
			// Work out the new world coordinates for the enemy
			/*
			new gridx = -1;
			new gridy = -1;
			MapGetGrid(gridx, gridy);
			
			SetX("this", (GetLowerLevelX() * 320) + (x - (screenx * 320)));
			SetY("this", (GetLowerLevelY() * 240) + (y - (screeny * 240)));
			if (screenx == GetLowerLevelX() && screeny == GetLowerLevelY())
			{
				Kill(""); // Kill The enemy
			}
			*/

			mqState = STANDING;
			ClearCollisionSet(SELF, 0, 0);
		} 
	}
}

stock StunnedEffect( count )
{
	static ShakeCount = 0;

	new ShakeOffset[5] = [ -2, -2, 0, 2, 2 ]; 

	if ( count <= 100)
	{
		mqEntityPosition.x += ShakeOffset[ ShakeCount ];
		ShakeCount++;
		if ( ShakeCount >= 5 )
			ShakeCount = 0;
	}

	if ( count <= 0)
		mqState = STANDING;
}


stock StockHit( attacker[], angle, dist, attack, damage, x, y, rect )
{
	if ( damage == 0 && attack&APLAYER != APLAYER )
		return;

	if ( mqState == HIT || mqState == DYING || mqState == GONE )
		return;

	mqState = HIT;

	strcopy( mqAttacker, attacker );
	new child = "*";

	mqAngle = fixed(360 - angle);

	if ( attack&AFIRE == AFIRE )
	{
		mqState = BURNING;
		// Create a Burning fire entity on this enemy
		EntityCreate( "effect_fire", "", mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex, CURRENT_MAP );
		StunCount = 3200;
	}
	else if ( attack&AICE == AICE )
	{
		// Create a frozen effect around the enemy
		child EntityCreate( "effect_freeze", "", mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex, CURRENT_MAP, ''dddd'', mqDisplayArea.x, mqDisplayArea.y, mqDisplayArea.w, mqDisplayArea.h );
		ObjectEffect( obj, 0xFF0000FF );
		StunCount = 3200;
		mqState = STUNNED;
	}
	else if ( attack&ASTUN == ASTUN )
	{
		mqState = STUNNED;
		StunCount = damage * 10;
	}
	else if ( attack&APLAYER == APLAYER )
	{
		EntityPublicFunction( mqAttacker, "Hurt", ''n'', mqDamageDealt );
	}
	else
	{
		mqHealth -= damage;
		HitCount = 1800;
		AudioPlaySound( "enemy_hurt.wav", mqDisplayArea.x, mqDisplayArea.y );
		BeenHit();
	}
	CheckHealth();
}







stock CreateEntityShadow( display_object[EntityGraphic] )
{
	display_object.obj = ObjectCreate( "", CIRCLE, mqDisplayArea.x + display_object.x, mqDisplayArea.y + display_object.y, mqDisplayZIndex + display_object.z, display_object.width, display_object.height, 0x000000AA);
}