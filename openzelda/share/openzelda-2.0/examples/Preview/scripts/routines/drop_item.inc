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
#include <public_events> 


 
forward PUBLIC_EVENT_HIT 
 
new bouncing = false; 
new initialBounce; 
new willTimeOut = false; 
new TempBounce;     
new ImageAlpha = 255; 
new Fixed:bounceVelocity = 0.00; 
new Fixed:bounceY; 
new param; 
new width; 
new height; 
new worth = 0; 
 
new playSound = true; 
new pickable = true; 
 
new object:shadowObject = OBJECT_NONE; 
 
new type = 1; 
new sprite[64]; 
new numstring = 0; 
 
 
public Init(...) 
{ 
	if ( numargs() > 0 ) 
		worth = getarg(0); 
	else 
		worth = 1; 

	mqDisplayArea.w = MiscGetWidth(sprite); 
	mqDisplayArea.h = MiscGetHeight(sprite); 
 
	GetEntityPosition(mqEntityPosition.x, mqEntityPosition.y, mqEntityPosition.z, mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex, mqDisplayLayer);

	InitDropItem( worth );  

	mqDisplayObject = ObjectCreate(sprite, SPRITE, mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex, 0,0,WHITE); 
	shadowObject = ObjectCreate( "", CIRCLE, mqDisplayArea.x, mqDisplayArea.y+24, mqDisplayZIndex-1000, 16, 8, 0x000000DD );

} 
 
 
public Close() 
{ 
	ObjectDelete(mqDisplayObject); 
	ObjectDelete(shadowObject); 
} 


main() 
{ 
	if ( pickable ) 
	{ 
		GetEntityPosition(mqEntityPosition.x, mqEntityPosition.y, mqEntityPosition.z, mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex, mqDisplayLayer);
		CollisionSet(SELF, 0, 0, mqDisplayArea.x, mqDisplayArea.y, mqDisplayArea.w, mqDisplayArea.h);
		ObjectPosition(mqDisplayObject, mqDisplayArea.x, mqDisplayArea.y - fround(bounceY), mqDisplayZIndex, 0, 0); 
	}
	DrawCharacter(worth, mqDisplayArea.x, mqDisplayArea.y, mqDisplayZIndex, WHITE, 255);

	// If the Item has been out for too long then make it disappear 
	if ( willTimeOut ) 
		TimeOut(); 
	 
	if ( bouncing ) 
		Bounce(); 
} 
 
DrawCharacter(number, px, py, pz, colour, alpha)
{
	new gfx[20];
	StringFormat(gfx, _, _, "alpha02.png:%d", number+48);
	GraphicsDraw(gfx, SPRITE, px, py, pz, 0, 0);
	return MiscGetWidth(gfx);
}


TimeOut() 
{ 
	static timer = 0; 
	static alpha = 25500; 
	 
	timer += GameFrame(); 
	 
	if ( timer > 10000 ) 
	{ 
		alpha -= GameFrame(); 
		ImageAlpha = alpha / 10; 
		 
		if ( alpha < 0 ) 
			EntityDelete(); 
	} 
} 
 
public StartBounce() 
{ 
	CollisionSet(SELF, 0, 0); 
	 
	bouncing = true; 
	bounceVelocity = 160.00; 
	bounceY = 0.00; 
	TempBounce = 120; 
	 
	initialBounce = mqEntityPosition.y; 
	willTimeOut = true; 
} 
 
Bounce() 
{ 
	bounceVelocity -= GameFrame(); 
	bounceY += (bounceVelocity * GameFrame()); 
		 
	if ( initialBounce - fround(bounceY) > initialBounce ) 
	{ 
		if ( TempBounce < 20 ) 
		{ 
			bouncing = false; 
			pickable = true; 
		} 
		else 
		{ 
			bounceVelocity = fixed(TempBounce); 
			TempBounce = TempBounce / 2; 
			bounceY = 0.00; 
		} 
	} 
} 
 
PUBLIC_EVENT_HIT 
{ 
	EntityPublicFunction(attacker, "IncreaseItemCount", "nn", type, worth); 
	 
	if ( playSound ) 
		AudioPlaySound("powerup.wav"); 
	 
	EntityDelete(); 
}