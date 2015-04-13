/****************************
Copyright © 2009-2012  Luke Salisbury

http://creativecommons.org/licenses/by-nc-sa/3.0/

You are free:
to Share — to copy, distribute and transmit the work
to Remix — to adapt the work

Under the following conditions:
Attribution — You must attribute the work in the manner specified by the author or licensor (but not in any way that suggests that they endorse you or your use of the work).
Noncommercial — You may not use this work for commercial purposes.
Share Alike — If you alter, transform, or build upon this work, you may distribute the resulting work only under the same or similar license to this one.

With the understanding that:
Waiver — Any of the above conditions can be waived if you get permission from the copyright holder.
Public Domain — Where the work or any of its elements is in the public domain under applicable law, that status is in no way affected by the license.
Other Rights — In no way are any of the following rights affected by the license:
 - Your fair dealing or fair use rights, or other applicable copyright exceptions and limitations;
 - The author's moral rights;
 - Rights other persons may have either in the work itself or in how the work is used, such as publicity or privacy rights.
Notice — For any reuse or distribution, you must make clear to others the license terms of this work. The best way to do this is with a link to this web page.

****************************/
#include "engine.h"
#include "core.h"
#include "gui.h"
#include "config.h"
#include "display.h"
#include "project_media.h"

extern ObjectEffect default_fx;

namespace lux {
	namespace screen {
		int8_t count = 0;

		ObjectEffect text_effects;//  = { {255,255,255,255}, {222,222,222,255}, 0, 1000, 1000, 0, 0, STYLE_VGRADIENT};
		ObjectEffect footer_effects;//  = { {99,99,99,255}, {32,32,32,255},  0, 1000, 1000, 0, 0, STYLE_VGRADIENT };
		LuxRect text_rect = { 0, 0, 0, 0, 0};
		LuxRect footer_rect = { 0, 0, 0, 56, 0};
		LuxRect footer_text = { 0, 0, 0, 56, 0};

		std::string last_message = "";
		uint32_t time;

		void show( )
		{
			uint16_t width = lux::display->screen_dimension.w / 2;
			uint16_t height = (lux::display->screen_dimension.h / 2) + lux_media::portal_title_height + 10;

			text_rect.y = height + 14;
			text_rect.x = 8;

			footer_rect.y = lux::display->screen_dimension.h - lux_media::portal_title_height;
			footer_rect.w = lux::display->screen_dimension.w;
			footer_rect.h = lux_media::portal_title_height;

			footer_text.x = 8;
			footer_text.y = lux::display->screen_dimension.h - 22;

			lux::display->graphics.DrawText(last_message, text_rect, text_effects, false);
			lux::display->graphics.DrawText("Version: "PROGRAM_VERSION"\nopenzelda.net", footer_text, text_effects, false );
			lux::display->graphics.Show();
		}

		void display( std::string message )
		{
			time = lux::core->GetTime();
			last_message = message;
			lux::screen::show( );
			lux::screen::count++;
		}

		void push( )
		{
			uint32_t current_time = lux::core->GetTime();
			if ( current_time >  time + 30 )
			{
				lux::screen::show( );
				lux::screen::count++;
			}
		}
	}
}

