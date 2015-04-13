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
#include <open_zelda>

new mqCollisionBuffer = 2;
stock DisplayCollisionPoints( points[8][POSITION] )
{
	new l,r,q = mqDirection;
	l = (mqDirection >= 7 ? 0 : mqDirection + 1);
	r = (mqDirection == 0 ? 7 : mqDirection - 1);

	for ( new c = 0; c < 8; c++ )
	{
		GraphicsDraw("", RECTANGLE, points[c].x, points[c].y, 5, 2,2, 0xFF00FFAA);
	}
	GraphicsDraw("", RECTANGLE, points[l].x, points[r].y, 5, 2,2, 0x0000FFAA);
	GraphicsDraw("", RECTANGLE, points[r].x, points[l].y, 5, 2,2, 0x0000FFAA);
	GraphicsDraw("", RECTANGLE, points[q].x, points[q].y, 5, 2,2, 0x0000FFAA);
}

stock UpdateCollisionPoints( points[8][POSITION], bx, by )
{
	points[SOUTH].x = mqDisplayArea.x + (mqDisplayArea.w/2) + mqDisplayOffset.x + bx;
	points[SOUTH].y = mqDisplayArea.y + (mqDisplayArea.h) + mqDisplayOffset.y + by;

	points[SOUTHEAST].x = mqDisplayArea.x + (mqDisplayArea.w) + mqDisplayOffset.x + bx - mqCollisionBuffer;
	points[SOUTHEAST].y = mqDisplayArea.y + (mqDisplayArea.h) + mqDisplayOffset.y + by - mqCollisionBuffer;

	points[EAST].x = mqDisplayArea.x + (mqDisplayArea.w) + mqDisplayOffset.x + bx;
	points[EAST].y = mqDisplayArea.y + (mqDisplayArea.h/2) + mqDisplayOffset.y + by;

	points[NORTHEAST].x = mqDisplayArea.x + (mqDisplayArea.w) + mqDisplayOffset.x + bx - mqCollisionBuffer;
	points[NORTHEAST].y = mqDisplayArea.y + mqDisplayOffset.y + by + mqCollisionBuffer;

	points[NORTH].x = mqDisplayArea.x + (mqDisplayArea.w/2) + mqDisplayOffset.x + bx;
	points[NORTH].y = mqDisplayArea.y + mqDisplayOffset.y + by;

	points[NORTHWEST].x = mqDisplayArea.x + mqDisplayOffset.x + bx + mqCollisionBuffer;
	points[NORTHWEST].y = mqDisplayArea.y + mqDisplayOffset.y + by + mqCollisionBuffer;

	points[WEST].x = mqDisplayArea.x + mqDisplayOffset.x + bx;
	points[WEST].y = mqDisplayArea.y + (mqDisplayArea.h/2) + mqDisplayOffset.y + by;

	points[SOUTHWEST].x = mqDisplayArea.x + mqDisplayOffset.x + bx + mqCollisionBuffer;
	points[SOUTHWEST].y = mqDisplayArea.y + (mqDisplayArea.h) + mqDisplayOffset.y + by - mqCollisionBuffer;

}
