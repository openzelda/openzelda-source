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
#include <collision>

stock Fixed:mqMovementAngle = 0.0;
stock Fixed:mqMovementSpeed = 0.0;
stock mqLocations[6][4];
stock mqHitMaskCheck[3];
stock bool:mqFlipYMovement = false;
stock bool:mqFlipXMovement = false;
stock bool:mqAllowOffscreenMovement = false

stock round_method:mqMovementRounding[4][2] = [ [round_floor, round_ceil], [round_ceil, round_ceil], [round_ceil, round_floor], [round_floor, round_floor] ]; /* South, west, north, east */


stock Fixed:MovementAngle(Fixed:x_movement, Fixed:y_movement)
{
	return fatan2( x_movement, -y_movement, degrees) + 90.0;
}

stock Fixed:Dir2Angle(d)
{
	d %= 8;
	return fixed(d*45);
}
stock D2A(d)
{
	d %= 8;
	return d*45;
}

stock Angle2Dir(Fixed:a, Fixed:offset = 0.0)
{
	a += offset;
	if ( a < 0.0 )
		a += 360.0;
	else if ( a >= 360.0 )
		a -= 360.0;
	return NumberClamp(fround(a) / 45, 0, 7);
}

stock StorePosition()
{
	for ( new i = 5; i > 0; i-- )
	{
		mqLocations[i][0] = mqLocations[i-1][0];
		mqLocations[i][1] = mqLocations[i-1][1];
		mqLocations[i][2] = mqLocations[i-1][2];
		mqLocations[i][3] = mqLocations[i-1][3];
	}
	mqLocations[0][0] = mqDisplayArea.x;
	mqLocations[0][1] = mqDisplayArea.y;
	mqLocations[0][2] = mqDisplayLayer;
	mqLocations[0][3] = mqDirection;
}




stock EntityMove( maxmask, push_off = true )
{
	new Fixed:speed =  mqMovementSpeed * GameFrame2();
	new results = 0;

	if (speed > 1.0)
	{
		while ( speed > 1.0 )
		{
			results += EntityMoveCode(maxmask,1.0, push_off);
			speed -= 1.0;
		}
	}
	results += EntityMoveCode(maxmask, speed, push_off);
	StorePosition();
	return results;
}

stock EntityMoveCode( maxmask, Fixed:speed, push_off = true )
{
	if (!speed)
		return false;

	new Fixed:movex = ( mqFlipXMovement ? -fsin(mqMovementAngle, degrees) : fsin(mqMovementAngle, degrees) ) * speed;
	new Fixed:movey = ( mqFlipYMovement ? -fcos(mqMovementAngle, degrees) : fcos(mqMovementAngle, degrees) ) * speed;
	new na = 0; // new angle
	new r = false;

	if ( MoveCheck(na, mqMovementAngle, maxmask, movex, movey) )
	{
		mqEntityPosition.x += movex;
		mqEntityPosition.y += movey;
		r = true;
	}
	else if ( push_off )
	{
		if ( na )
		{
			mqMovementAngle += fixed(na);
			movex = fsin(mqMovementAngle, degrees);
			movey = fcos(mqMovementAngle, degrees);
			mqEntityPosition.x += movex/4.0;
			mqEntityPosition.y += movey/4.0;
			r = true;
		}
		else
		{
			mqEntityPosition.x = fixed( fround(mqEntityPosition.x, mqMovementRounding[mqDisplayDirection][0]) );
			mqEntityPosition.y = fixed( fround(mqEntityPosition.y, mqMovementRounding[mqDisplayDirection][1]) );
		}
	}
	else
	{
		mqEntityPosition.x = fixed( fround(mqEntityPosition.x, mqMovementRounding[mqDisplayDirection][0]) );
		mqEntityPosition.y = fixed( fround(mqEntityPosition.y, mqMovementRounding[mqDisplayDirection][1]) );
	}
	EntitySetPosition(mqEntityPosition.x, mqEntityPosition.y, mqEntityPosition.z );
	UpdateDisplayPosition();

	mqMovementAngle %= 360.0;

	return r;

}


stock MoveCheck(&a, Fixed:angle, maximum_mask_value, Fixed:movex, Fixed:movey)
{

	new checks[8][POSITION];

	mqDirection = Angle2Dir(angle, 22.5);
	mqDisplayDirection = NumberClamp((mqDirection/2), 0, 3);

	UpdateCollisionPoints( checks, fround(movex, mqMovementRounding[mqDisplayDirection][0]), fround(movey, mqMovementRounding[mqDisplayDirection][1]) );
	//DisplayCollisionPoints( checks );

	new l,r,q = mqDirection;
	l = (mqDirection >= 7 ? 0 : mqDirection + 1);
	r = (mqDirection == 0 ? 7 : mqDirection - 1);

	mqHitMaskCheck[0] = MaskGetValue( checks[q].x, checks[q].y);
	mqHitMaskCheck[1] = MaskGetValue( checks[l].x, checks[l].y);
	mqHitMaskCheck[2] = MaskGetValue( checks[r].x, checks[r].y);

	if( mqHitMaskCheck[0] > 255 )
	{
		return !a;
	}
	else if ( !mqAllowOffscreenMovement && mqHitMaskCheck[0] < 0 )
	{
		return false;
	}
	else if ( mqHitMaskCheck[0] > maximum_mask_value )
	{
		if ( MaskGetValue( checks[q].x, checks[q].y) > maximum_mask_value )
		{
			a = 0;
			return false;
		}
		return false;
	}
	else
	{
		if ( mqHitMaskCheck[1] > maximum_mask_value )
			a -= 30;
		else
			mqHitMaskCheck[1]  = 0;


		if ( mqHitMaskCheck[2] > maximum_mask_value )
			a += 30;
		else
			mqHitMaskCheck[2] = 0;
	}

	return !a;

}

