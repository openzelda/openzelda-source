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
 *     2012/01/30  [luke]: new file. 
 ***********************************************/ 
/* Public Events
 *  wrapper around EntityPublicFunction so user don't use the wrong code
 */


#define PUBLIC_EVENT_HIT		public Hit( entityId:attacker, angle, dist, attack, damage, x, y, rect )
#define PUBLIC_EVENT_AWAKING	public Awaking( entityId:attacker, rect )
#define PUBLIC_EVENT_PRESSED	public Pressed( entityId:attacker )
#define PUBLIC_EVENT_PUSH		public Push( entityId:attacker, rect, angle )
#define PUBLIC_EVENT_HURT		public Hurt( type, damage, angle )
#define PUBLIC_EVENT_PICKUP		public Pickup( entityId:attacker )



/***
<function name="TransitionPlayer">
	<summary>Changes the Section, while keeping the same grid location.</summary>
	<param name="section[]" default="" type="s"></param>
	<param name="gridx" default="-1" type="n"></param>
	<param name="gridy" default="-1" type="n"></param>
</function>
***/

stock entityId:transitionEntity = 0;

stock TransitionPlayer( entityId:player, entityId:target, nmapid = 0, nsection{} = "", ngridx = -1, ngridy = -1 )
{
	new section{64};
	if ( !transitionEntity )
	{
		transitionEntity = entityId:EntityHash("transition");
	}
	StringCopy(section, nsection);
	if ( ngridx < 0 && ngridy < 0 && !nmapid  )
		SectionGet(section, ngridx, ngridy);
	return EntityPublicFunction(transitionEntity, "SetTarget", ''nnnsnn'', player, target, nmapid, section, ngridx, ngridy );
}

stock CallEntityHit( entityId:entity, entityId:attacker, angle, dist, attack, damage, x, y, rect )
{
	return EntityPublicFunction( entity, "Hit", [ 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 0 ], attacker, angle, dist, attack, damage, x, y, rect);
}

stock CallEntityAwaking( entityId:entity, entityId:attacker, rect)
{
	return EntityPublicFunction( entity, "Awaking", ''nn'', attacker, rect);
}


stock CallEntityPressed( entityId:entity, attacker, rect, angle)
{
	return EntityPublicFunction( entity, "Pressed", ''nnn'', attacker, rect, angle);
}


stock CallEntityHurt( entityId:entity, weapon_type, damage, angle)
{
	return EntityPublicFunction( entity, "Hurt", ''nnn'', weapon_type, damage, angle);
}


stock CallEntityPush( entityId:entity, attacker, rect, angle)
{
	return EntityPublicFunction( entity, "Push", ''nnn'', attacker, rect, angle);
}

stock CallEntityPickup( entityId:entity, attacker )
{
	return EntityPublicFunction( entity, "Pickup", ''n'', attacker );
}

stock CallEntityUse( entityId:entity, displayObject, direction )
{
	return EntityPublicFunction( entity, "Use", ''nn'', displayObject, direction );
}

/*
	else if ( type == TYPE_TRANSPORT )
	{
		//MovePlayer(player, dir)
		if ( EntityPublicFunction(current, "MovePlayer", "sn", selfid, _dir_) )
		{
			 _state_ = STANDING;
		}
	}


*/