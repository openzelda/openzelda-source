/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <SDL2/SDL.h>
#include <stdio.h>

int (*openzelda_play)( SDL_Window * game_window, const char * quest );

extern "C" int main( int argc, char *argv[] )
{
	char so_path[512] = "";
	char quest[512]="";
	void * objectHandle;

	char * base_path = SDL_GetBasePath();


	snprintf( so_path, 511,"%s/%s", base_path, LIBOZFILE );

	objectHandle = SDL_LoadObject(so_path);
	if ( !objectHandle )
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Unable to load %s\n%s\n", LIBOZFILE, SDL_GetError());
		exit(1);
	}


	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}


	SDL_Window * window = SDL_CreateWindow("Open Zelda - Classic Edition", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE );

	openzelda_play = (int (*)( SDL_Window *, const char *))SDL_LoadFunction(objectHandle, "openzelda_play");

	if ( argc > 1 )
	{
		snprintf(quest, 512, "%s", argv[1] );
	}
	else
	{
		//snprintf(quest, 512, "%s/quests/default.qst.gz", SDL_GetBasePath() );
	}

	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"quest: %s\n", quest);
	openzelda_play( window, quest );


	SDL_DestroyWindow(window);

	SDL_free(base_path);

	return 0;
}



