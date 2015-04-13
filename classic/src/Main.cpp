/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#define STRICT

#include <time.h>
#include <stdio.h>
#include <string>

#include "GeneralData.h"
#include "Script.h"
#include "CGame.h"
#include "Socket.h"
#include "FS_Functions.h"	// File System Functions
#include "OpenZelda_private.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

// External Globals
CGame *			g_pGame			= NULL;
bool			g_bShowFPS		= false;	// Show FPS counter?
bool			g_fWindowed		= true;		// are we windowed or not?
bool			g_block			= false;	// FPS Capped
int32_t			g_wScreenWidth	= 640;
int32_t			g_wScreenHeight	= 480;
CSocket *		g_pSocket		= NULL;		// Networking
SDL_Rect		g_rcWindow;
SDL_Rect		g_rcViewport;
SDL_Window *	native_window	= NULL;
SDL_Renderer *	native_renderer = NULL;

SDL_Window *	mask_window		= NULL;
SDL_Renderer *	mask_renderer	= NULL;

CFontList *		temp_font_list = NULL;

// Input
Uint8 * 		keystate		= NULL;

// Timing Values
bool g_bUpdateFPS;
uint32_t		g_dwLastTick	= 0;		// Value used for Timing
float			g_rFPS			= 0;		// The Frame Rate
float			g_rDelta		= 0;

// Function Definitions
void FreeApp();
bool LoadNewQuest( const char * fileName);
int GetBaseDir( std::string path );
int32_t ProcessNextFrame();


CGame * GetGameObject()
{
	return g_pGame;
}

SDL_Renderer * GetRendererObject()
{
	return native_renderer;
}



void OpenZeldaGameLoop()
{

	const SDL_MessageBoxButtonData buttons[] = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Yes" },
		{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "No" },
	};
	const SDL_MessageBoxData messageboxdata = {
		SDL_MESSAGEBOX_INFORMATION,
		NULL,
		"Quit",
		"Do you wish to quit Open Zelda Classic Edition?",
		SDL_arraysize(buttons),
		buttons,
		NULL
	};

	bool running = true;
	float key_timer = 0.00;
	while( running )
	{
		if(key_timer < .1)
		{
			key_timer += g_rDelta;
		}
		else
		{
			SDL_Event _event;
			while (SDL_PollEvent (&_event))
			{
				switch (_event.type)
				{
					case SDL_KEYDOWN:
						if ( _event.key.keysym.sym == SDLK_ESCAPE )
						{
							int buttonid;
							SDL_ShowMessageBox(&messageboxdata, &buttonid);
							if ( buttonid == 0 )
							{
								running = false;
							}
						}
						else if  ( _event.key.keysym.sym == SDLK_F1 )
						{
							if (g_pGame)
								g_pGame->GetSaveLoad()->PreLoad();
						}
						else if  ( _event.key.keysym.sym == SDLK_F2 )
						{
							if (g_pGame)
								g_pGame->GetSaveLoad()->PreSave();
						}
						else
							CheckKeysDown(_event.key.keysym.sym);
					break;
					case SDL_KEYUP:
						CheckKeysUp(_event.key.keysym.sym);
					break;
					case SDL_DROPFILE:
					{
						// In case if dropped file
						char * dropped_filedir = _event.drop.file;
						// Shows directory of dropped file
						SDL_ShowSimpleMessageBox(
							SDL_MESSAGEBOX_INFORMATION,
							"Loading New Quest",
							dropped_filedir,
							native_window
						);

						LoadNewQuest( dropped_filedir );

						SDL_free(dropped_filedir);    // Free dropped_filedir memory
						break;
					}
					case SDL_QUIT:
					{
						int buttonid;
						SDL_ShowMessageBox(&messageboxdata, &buttonid);
						if ( buttonid == 0 )
						{
							running = false;
						}

						break;
					}
					default:
						break;
				}
			}
			/*
			int mouse_x, mouse_y;
			Uint8 mouse = SDL_GetMouseState(&mouse_x, &mouse_y);
			*/
			key_timer = 0.00;
		}
		ProcessNextFrame();
	}
}


extern "C" int openzelda_play( SDL_Window *	game_window, const char * quest )
{
	InitCheck(); // Init error checking

	// Seed the random number generator
	srand( time(NULL) );

	// Initialise the Timing Value
	g_dwLastTick = SDL_GetTicks();


	native_window = game_window;
	SDL_SetWindowSize(native_window, g_wScreenWidth, g_wScreenHeight);
	native_renderer = SDL_CreateRenderer(native_window, -1, SDL_RENDERER_ACCELERATED );
	if ( !native_renderer ) {
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s %s\n", "Unable to init display:", SDL_GetError());
		exit(1);
	}
	SDL_RenderSetLogicalSize(native_renderer, g_wScreenWidth, g_wScreenHeight);
	SDL_SetRenderDrawColor(native_renderer, 0, 0, 0, 255);
	SDL_RenderClear(native_renderer);

/*
	SDL_CreateWindowAndRenderer( VSCREEN_WIDTH, VSCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &mask_window, &mask_renderer );
	SDL_RenderSetLogicalSize(mask_renderer, VSCREEN_WIDTH, VSCREEN_HEIGHT);
	SDL_SetRenderDrawColor(mask_renderer, 128, 128, 128, 255);
*/
	//==============================
	// Initialise the Game Interface
	//==============================

	if ( quest && quest[0] == 0 )
	{
		temp_font_list = new CFontList();
		OpenZeldaGameLoop();
		delete temp_font_list;
	}
	else if ( LoadNewQuest(quest) )
	{
		OpenZeldaGameLoop();
	}

	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"%s\n", "Shuting down");

	FreeApp();
/*
	SDL_DestroyRenderer( mask_renderer );
	SDL_DestroyWindow( mask_window );
*/

	SDL_DestroyRenderer( native_renderer );

	return true;
}

/*
extern "C" int main( int argc, char *argv[] )
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	//SDL_WINDOWPOS_UNDEFINED
	SDL_Window * window = SDL_CreateWindow(PRODUCT_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, g_wScreenWidth, g_wScreenHeight, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE );

	return openzelda_play( window, argc, argv );
}
*/

// APRIL 2010 - luke
float mouse_scale[2] = {1.0f, 1.0f};


//-----------------------------------------------------------------------------
// Name: GetBaseDir()
//-----------------------------------------------------------------------------
int GetBaseDir( std::string path )
{
	#ifdef WIN32
	int lastslash = path.find_last_of( '\\', path.length() );
	#else
	int lastslash = path.find_last_of( '/', path.length() );
	#endif

	if ( lastslash == -1 )
	{
		path = "./";
	}
	if ( lastslash >= 1 )
	{
		path = path.substr( 0, lastslash+1 );
	}

	return FS_SetGamedir( (char*)path.c_str() );
}

//-----------------------------------------------------------------------------
// Name: FreeApp()
// Desc: Frees the Application memory
//-----------------------------------------------------------------------------
void FreeApp()
{
	SAFE_DELETE ( g_pGame )
	SAFE_DELETE ( g_pSocket );
}


void WaitForQuest()
{
	SDL_Color whi = {255, 255, 255,255};
	SDL_Color yel = {255, 215, 0,255};
	SDL_SetRenderDrawColor(GetRendererObject(), 32, 32, 32, 255);
	SDL_RenderClear( GetRendererObject() );

	/* Checkerboard Background */
	const static SDL_Rect rects[150] = { \
		{0, 0, 32, 32}, {64, 0, 32, 32}, {128, 0, 32, 32}, {192, 0, 32, 32}, {256, 0, 32, 32}, {320, 0, 32, 32}, {384, 0, 32, 32}, {448, 0, 32, 32}, {512, 0, 32, 32}, {576, 0, 32, 32},\
		{0, 64, 32, 32}, {64, 64, 32, 32}, {128, 64, 32, 32}, {192, 64, 32, 32}, {256, 64, 32, 32}, {320, 64, 32, 32}, {384, 64, 32, 32}, {448, 64, 32, 32}, {512, 64, 32, 32}, {576, 64, 32, 32},\
		{0, 128, 32, 32}, {64, 128, 32, 32}, {128, 128, 32, 32}, {192, 128, 32, 32}, {256, 128, 32, 32}, {320, 128, 32, 32}, {384, 128, 32, 32}, {448, 128, 32, 32}, {512, 128, 32, 32}, {576, 128, 32, 32},\
		{0, 192, 32, 32}, {64, 192, 32, 32}, {128, 192, 32, 32}, {192, 192, 32, 32}, {256, 192, 32, 32}, {320, 192, 32, 32}, {384, 192, 32, 32}, {448, 192, 32, 32}, {512, 192, 32, 32}, {576, 192, 32, 32},\
		{0, 256, 32, 32}, {64, 256, 32, 32}, {128, 256, 32, 32}, {192, 256, 32, 32}, {256, 256, 32, 32}, {320, 256, 32, 32}, {384, 256, 32, 32}, {448, 256, 32, 32}, {512, 256, 32, 32}, {576, 256, 32, 32},\
		{0, 320, 32, 32}, {64, 320, 32, 32}, {128, 320, 32, 32}, {192, 320, 32, 32}, {256, 320, 32, 32}, {320, 320, 32, 32}, {384, 320, 32, 32}, {448, 320, 32, 32}, {512, 320, 32, 32}, {576, 320, 32, 32},\
		{0, 384, 32, 32}, {64, 384, 32, 32}, {128, 384, 32, 32}, {192, 384, 32, 32}, {256, 384, 32, 32}, {320, 384, 32, 32}, {384, 384, 32, 32}, {448, 384, 32, 32}, {512, 384, 32, 32}, {576, 384, 32, 32},\
		{0, 448, 32, 32}, {64, 448, 32, 32}, {128, 448, 32, 32}, {192, 448, 32, 32}, {256, 448, 32, 32}, {320, 448, 32, 32}, {384, 448, 32, 32}, {448, 448, 32, 32}, {512, 448, 32, 32}, {576, 448, 32, 32},\
		{32, 32, 32, 32}, {96, 32, 32, 32}, {160, 32, 32, 32}, {224, 32, 32, 32}, {288, 32, 32, 32}, {352, 32, 32, 32}, {416, 32, 32, 32}, {480, 32, 32, 32}, {544, 32, 32, 32}, {608, 32, 32, 32},\
		{32, 96, 32, 32}, {96, 96, 32, 32}, {160, 96, 32, 32}, {224, 96, 32, 32}, {288, 96, 32, 32}, {352, 96, 32, 32}, {416, 96, 32, 32}, {480, 96, 32, 32}, {544, 96, 32, 32}, {608, 96, 32, 32},\
		{32, 160, 32, 32}, {96, 160, 32, 32}, {160, 160, 32, 32}, {224, 160, 32, 32}, {288, 160, 32, 32}, {352, 160, 32, 32}, {416, 160, 32, 32}, {480, 160, 32, 32}, {544, 160, 32, 32}, {608, 160, 32, 32},\
		{32, 224, 32, 32}, {96, 224, 32, 32}, {160, 224, 32, 32}, {224, 224, 32, 32}, {288, 224, 32, 32}, {352, 224, 32, 32}, {416, 224, 32, 32}, {480, 224, 32, 32}, {544, 224, 32, 32}, {608, 224, 32, 32},\
		{32, 288, 32, 32}, {96, 288, 32, 32}, {160, 288, 32, 32}, {224, 288, 32, 32}, {288, 288, 32, 32}, {352, 288, 32, 32}, {416, 288, 32, 32}, {480, 288, 32, 32}, {544, 288, 32, 32}, {608, 288, 32, 32},\
		{32, 352, 32, 32}, {96, 352, 32, 32}, {160, 352, 32, 32}, {224, 352, 32, 32}, {288, 352, 32, 32}, {352, 352, 32, 32}, {416, 352, 32, 32}, {480, 352, 32, 32}, {544, 352, 32, 32}, {608, 352, 32, 32},\
		{32, 416, 32, 32}, {96, 416, 32, 32}, {160, 416, 32, 32}, {224, 416, 32, 32}, {288, 416, 32, 32}, {352, 416, 32, 32}, {416, 416, 32, 32}, {480, 416, 32, 32}, {544, 416, 32, 32}, {608, 416, 32, 32}\
	};

	SDL_SetRenderDrawColor(GetRendererObject(), 96, 96, 96, 255);
	SDL_RenderFillRects( GetRendererObject(), rects, 150 );

	temp_font_list->DrawText( 272, 240, 8, whi, "Drop Quest Here" );

	temp_font_list->DrawText( 320, 220, 8, yel, "\x1e" );
	temp_font_list->DrawText( 316, 225, 8, yel, "\x1e" );
	temp_font_list->DrawText( 324, 225, 8, yel, "\x1e" );


	SDL_RenderPresent( GetRendererObject() );
}

//-----------------------------------------------------------------------------
// Name: ProcessNextFrame()
// Desc: Called whenever the program is idle
//
//-----------------------------------------------------------------------------
int32_t ProcessNextFrame()
{
	///////////////////////////////
	// Perform Timing calculations
	///////////////////////////////
	//
	static uint32_t dwFrames = 0L;
	static uint32_t dwLastTime = 0L;

	// Calculate how much time has passed since the last time
	uint32_t dwCurrTick = SDL_GetTicks();
	float rTickDiff = dwCurrTick - g_dwLastTick;

	g_dwLastTick = dwCurrTick;

	// Count the number of frames rendered
	++dwFrames;

	if (g_bShowFPS)
	{
		// Perform some frame rate calculations
		if( dwCurrTick - dwLastTime > 1000 )
		{
			g_rFPS	 = (float)dwFrames / ((float)((dwCurrTick - dwLastTime)) / 1000);
			dwLastTime = dwCurrTick;
			dwFrames = 0;
			g_bUpdateFPS = true;
			char s[255];
			snprintf(s, 254, PRODUCT_NAME" (FPS = %.1f)", g_rFPS );
			SDL_SetWindowTitle( native_window, s);

		}
	}

	// Record our Global Time Delta value
	g_rDelta = rTickDiff / 1000.0f;

	// Limit the max Time Delta in case of very slow circumstances
	// This will have the effect of slowing the game down rather than
	// making everything jumpy
	if (g_rDelta > 0.02)
		g_rDelta = 0.02;

	if ( g_pGame )
		g_pGame->Render();
	else
	{
		WaitForQuest();
	}

	//SDL_Delay(10);

	return true;
}


//-----------------------------------------------------------------------------
// Name: LoadNewQuest()
// Desc:
//
//-----------------------------------------------------------------------------
bool LoadNewQuest(const char * fileName)
{
	FreeApp();

	g_pGame = new CGame();
	if ( !g_pGame->fatalError )
	{
		SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION,"%s\n", "Main() - Calling CGame::Initialise()");
		g_pGame->Initialise( fileName );

		return true;
	}
	else
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
								 "Error",
								 "Can't write to disk",
								 NULL);
	}
	return false;
}


void SetJoyKeys()
{
	if (!g_pGame)
		return;
	static float timer = 0.00;

	if(timer < 0.10)
	{
		timer += g_rDelta;
		return;
	}
	timer = 0.00;
}

