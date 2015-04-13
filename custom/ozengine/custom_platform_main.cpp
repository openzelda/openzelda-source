/****************************
Copyright © 2007-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "portal/portal.h"
#include "core.h"
#include "config.h"

#include "elix_file.hpp"
#include "elix_string.hpp"

std::string game_url = "";

int (*openzelda_play)( SDL_Window * game_window, const char * quest );

bool is_classic_quest( std::string quest_filename )
{
	if ( quest_filename.at(0) == '\'' )
	{
		quest_filename = quest_filename.substr(1, quest_filename.length() -2);
	}

	if ( elix::string::HasSuffix( quest_filename, ".qst.gz" ) )
	{
		return true;
	}
	return false;

}

bool launch_classic( std::string game )
{
	void * objectHandle = SDL_LoadObject("libozc.dll");

	if ( objectHandle )
	{
		SDL_Window * window = lux::core->GetWindow();
		int w, h;

		openzelda_play = (int (*)( SDL_Window *, const char *))SDL_LoadFunction(objectHandle, "openzelda_play");

		SDL_GetWindowSize(window, &w, &h);
		openzelda_play( window, game.c_str() );
		SDL_SetWindowSize(window, w, h);

		SDL_UnloadObject( objectHandle );
		return true;
	}
	else
	{
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", "Can not load Open Zelda Classic Edition plugin", NULL);
	}
	return false;
}


#if defined(ANDROID_NDK)
#include <android/asset_manager.h>
	extern "C" int main( int argc, char *argv[] )
	{
		if ( argc )
		{
			std::string base_executable = (argc ? argv[0] : "/lux" );
			lux::engine = new LuxEngine( base_executable );
			if ( lux::engine->Start("puttytris.game") )
			{
				lux::engine->Loop();
			}
			lux::engine->Close();
			delete lux::engine;
		}
		std::cout << "Error can not file executable." << std::endl;
		return 0;
	}

#elif defined(STANDALONE)

	extern "C" int main( int argc, char *argv[] )
	{
		if ( argc )
		{
			std::string base_executable = (argc ? argv[0] : "/lux" );
			lux::engine = new LuxEngine( base_executable );
			//lux::global_config->SetString("project.file", );
			if ( lux::engine->Start(argv[0]) )
			{
				lux::engine->Loop();
			}
			lux::engine->Close();
			delete lux::engine;
		}
		std::cout << "Error can not file executable." << std::endl;
		return 0;
	}

#elif defined(EMSCRIPTEN)
#include <emscripten.h>

bool running = false;
extern "C" {
	void loopGame()
	{
		lux::engine->Refresh();
	}

	int32_t loadGame( char * file )
	{
		lux::engine = new LuxEngine( "/luxengine" );
		return lux::engine->Start( file );
	}

	void startGame( )
	{
		emscripten_set_main_loop( loopGame, 0, 0);
		running = true;
	}

	void pauseGame()
	{
		if ( running )
			emscripten_cancel_main_loop();
		else
			startGame();
	}

	void endGame()
	{
		emscripten_cancel_main_loop();
		lux::engine->Close();
	}

}
extern "C" int main( int argc, char *argv[] )
{
	return 0;
}
#else
void * clientWindowID = NULL;
std::string project_file = "";

void main_args( int argc, char *argv[] )
{
	int c = argc;
	while ( c > 1 ) {
		--c;
		if ( strncmp(argv[c], "--windowid=",11) == 0 )
		{
			clientWindowID = (void*)strtoul( argv[c]+11, NULL, 0 );

		}
		else if ( argv[c][0] != '-' )
		{
			project_file = argv[c];
			LuxPortal::add_previous_game( argv[c] );
			LuxPortal::use = false;
			LuxPortal::active = false;
		}
		else
		{
			// Unknown argument.
		}
	}
}

extern "C" int main( int argc, char *argv[] )
{
	std::string base_executable = (argc ? argv[0] : "/luxengine" );

	main_args( argc, argv );


	if ( clientWindowID != 0 )
	{
		lux::engine = new LuxEngine( clientWindowID );
		if ( lux::engine->Start(project_file) )
		{
		   lux::engine->Loop();
		}
		lux::engine->Close();
		delete lux::engine;
	}
	else
	{
		lux::engine = new LuxEngine( base_executable );
		LuxPortal::open();
		if ( LuxPortal::use )
		{
			while ( LuxPortal::active )
			{
				if ( LuxPortal::run() )
				{
					GameInfoValues selected_game = LuxPortal::get_selected();

					if ( selected_game.type == GI_FILE  )
					{
						if ( lux::engine->Start(selected_game.url) )
						{
							lux::engine->Loop();
						}
						lux::engine->Close();
					}
					else if ( selected_game.type == GI_SPECIAL  )
					{
						launch_classic(selected_game.url);
					}
				}
			}
		}
		else
		{
			if ( is_classic_quest( project_file ) )
			{
				launch_classic( project_file );
			}
			else
			{
				if ( lux::engine->Start( project_file ) )
				{
					lux::engine->Loop();
				}
				lux::engine->Close();
			}
		}
		LuxPortal::close();


		delete lux::engine;
	}
	return 0;
}

#endif





