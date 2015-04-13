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
#include <string>
new messages[24];
new msgCount = 0; 
new msgTaking = -1;
new msgListener[64]; 
 
//----------------------------------------------------------------- 
// Name: Hit() 
// Desc: Checks if the player is trying to talk with the NPC 
//----------------------------------------------------------------- 
public Hit( attacker[] ) 
{
	strcopy(msgListener, attacker);
}

//---------------------------------------- 
// Name: AfterTalk() 
//---------------------------------------- 
public AfterTalk() 
{   
	/* Called after the player has finished talking to the NPC (after closing 
	The Dialog box), this code works out the next message to be displayed  
	by looking at the NPCs message map */ 
	 
	if ( GetGotoNextMessage("this", GetValue("this", 3), GetLastAnswer()) ) 
		SetQKey(true);    // Force the player to talk to the NPC some more 
	 
	// Get the index of the next message and put it in value 3 
	SetValue("this", 3, GetNextMessage("this", GetValue("this", 3), GetLastAnswer()));  
} 
 
//---------------------------------------- 
// Name: FacePlayer() 
//---------------------------------------- 
public FacePlayer() 
{ 
   // Get the angle between the NPC and the Player 
   new Angle = CalculateAngle( GetX("this"), GetY("this"), GetX("player1"), GetY("player1")); 
   SetMoveAngle("this", Angle); 
 
   // Set the NPCs direction from its Move Angle 
   SetDirFromAngle("this"); 
} 
 
//---------------------------------------- 
// Name: WalkTo() 
//---------------------------------------- 
public WalkTo( x, y, instruction ) 
{	 
	// Makes the NPC walk to a point on the map 
	// Save the x and y points in the entities values 
	SetValue("this", 0, x); 
	SetValue("this", 1, y); 
		 
	new ex = GetX("this"); 
	new ey = GetY("this"); 
	new dist = 5; 
	 
	if ( ex == x && ey == y )	 
	{ 
		// Increment the instruction value 
		if ( instruction != -1 ) 
			SetValue("this", 4, instruction + 1); 
		return 1; 
	} 
		 
	SetState("this", walking); 
	return 0;  
} 

public Wait( timeToWait, instruction )
{

}

public WaitFor( WhoToWaitFor[], TargetInstruction, instruction ) 
{
	return 0; 
}


public Say( Text[], instruction ) 
{
	return 1; 
}
