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

new screen[2];
new font[2];
new controller = 1;

/* Main Entity has 3 more Public Functions */
forward public save();
forward public load();

forward public KeyboardInput( unicode );
forward public CustomText(message{}, x, y, z, width, height, alpha)
forward public DrawCharacter(achar, x, y, z, colour, alpha);
forward public SetDay(able);
forward public SetPlayer(n);
forward public GetPlayer();

new DayNight[24] = [
	0x4E5CAFFF,\
	0x5964A9FF,\
	0x606BAEFF,\
	0x6A76BAFF,\
	0x6E76BAFF,\
	0x7883C6FF,\
	0x98A3E8FF,\
	0xB6BEEEFF,\
	0xD6DAF3FF,\
	0xF4F5FBFF,\
	0xFFFFFFFF,\
	0xFFFFFFFF,\
	0xF7F8ECFF,\
	0xF7F8E7FF,\
	0xEFF2E1FF,\
	0xEDEDD2FF,\
	0xE8ECC8FF,\
	0xE8E1BEFF,\
	0xE4C47EFF,\
	0xE5A67EFF,\
	0xE59E96FF,\
	0x7D7CC8FF,\
	0x766FD8FF,\
	0x645CBEFF\
];
public hour = 12;
public minute = 0;
public Fixed:seconds = 0.0;
new daynight = 0;
new Fixed:timemod = 500.00;
public debug_game = debug;

public Init( ... )
{
	GameState(1);
	
	screen[0] = MiscGetWidth("__screen__")/2;
	screen[1] = MiscGetHeight("__screen__")/2;
	font[0] = 8;
	font[1] = 16;
	SetDay(0);

	SheetReference("alpha01.png", 1);
	SheetReference("alpha02.png", 1);
	SheetReference("alpha03.png", 1);

	if (debug_game)
	{
		//EntityCreate("tester", "tester", 0, 0, 0, GLOBAL_MAP);
		//EntityCreate("gun", "", 0, 0, 0, GLOBAL_MAP);
	}

	
}

public Close()
{

}

main()
{
	HandleDayNight();
	//HandleDialogBox();

}


HandleDayNight()
{
	seconds += GameFrame2() * timemod;
	if ( seconds >= 60.0 )
	{
		minute++;
		seconds -= 60.0;
	}
	if ( minute >= 60 )
	{
		hour++;
		minute -= 60;
	}
	if ( hour > 23 ) 
		hour -= 24;

	if ( daynight )
	{
		// TODO: Smooth out changes
		new str[6];
		strformat(str, _, true, "%d:%02d", hour, minute);
		CustomText(str, -1, -2, 6000, 100, 1, 255);
		LayerColour(0, DayNight[hour]);
		LayerColour(1, DayNight[hour]);
		LayerColour(2, DayNight[hour]);
		LayerColour(3, DayNight[hour]);
		LayerColour(4, DayNight[hour]);
		LayerColour(5, DayNight[hour]);
	}
}

public SetPlayer(n)
{
	controller = n;
}
public GetPlayer()
{
	return controller;
}

public SetDay(able)
{
	daynight = able;
	if ( !daynight )
	{
		LayerColour(0, 0xFFFFFFFF);
		LayerColour(1, 0xFFFFFFFF);
		LayerColour(2, 0xFFFFFFFF);
		LayerColour(3, 0xFFFFFFFF);
		LayerColour(4, 0xFFFFFFFF);
		LayerColour(5, 0xFFFFFFFF);
	}
}

public DrawCharacter(achar, x, y, z, colour, alpha)
{
	new gfx[40];
	strformat(gfx, _, _, "alpha03.png:%d", achar);
	GraphicsDraw(gfx, SPRITE, x, y, z, 0, 0, NumberClamp(alpha,0,255) + colour);
	return MiscGetWidth(gfx);
}

public CustomText(message{}, x, y, z, width, height, alpha)
{
	new chars = 0;
	new lines = 0;
	new j;
	new colour = 1;
	new nx = x;

	if ( x == -1 )
	{
		new nw = strlen(message);
		nw = (nw > width ? width : nw);
		x = screen[0] - (font[0] * nw);
	}
	else if ( y == -2 )
	{
		new nw = strlen(message);
		nw = (nw > width ? width : nw);
		x = screen[0] + screen[0] - (font[0] * nw * 2);
	}
	if ( y == -1 )
		y = screen[1] - font[1]*2;
	if ( y == -2 )
		y = screen[1] + screen[1] - font[1]*2;
	nx = x;


	while ( message{j} )
	{
		if ( chars > width ) 
		{
			nx = x;
			y += font[1]*2;
			lines++;
			chars = 0;
		}
		if ( lines > height  ) 
			break;
		
		switch ( message{j} )
		{
			case 33 .. 126:
			{
				nx += DrawCharacter(message{j}, nx, y, z, 0xFFFFFF00, alpha);
				chars++;
			}
			case 32:
			{
				nx += font[0]*2;
				colour = 1;
				chars++;
			}
			case 1 .. 9:
				colour = message{j};
			case 10:
			{
				nx = x;
				y += font[1]*2;
				colour = 1;
				chars = 0;
				lines++;
			}
			case 11 .. 31:
			{
			//
			}
			default:
			{
				nx += DrawCharacter('?', nx, y, z, 0xFFFFFF00, alpha);
				//Unsupported UNICODE
			}
		}

		j++;
	}
}

