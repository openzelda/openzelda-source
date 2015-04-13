/* Default Controller setup
 * To use your own names with your game, 
 * create a game_controller.inc in the 
 * the routines directory.
 * game_controller.inc should include 
 * '#define _controller_included' to stop
 * this file taking over.
 *
 * Note: For custom layout, you need to set
 * control.setX and control.buttonsX (X for 
 * player) to the values you wish to use.
 * http://mokoi.info/docs/Meg#CustomControls
*/

#tryinclude <game_controller>

#if defined _controller_included
	#endinput
#endif
#define _controller_included

#define BUTTON_ACTION1 0 
#define BUTTON_ACTION2 1
#define BUTTON_ACTION3 2
#define BUTTON_ACTION4 3
#define BUTTON_ACTION5 4
#define BUTTON_ACTION6 5

#define BUTTON_MENU 6

#define BUTTON_ARROWRIGHT 7
#define BUTTON_ARROWLEFT 8
#define BUTTON_ARROWDOWN 9
#define BUTTON_ARROWUP 10

#define BUTTON_MOUSELEFT 11
#define BUTTON_MOUSEMIDDLE 12
#define BUTTON_MOUSERIGHT 13
#define BUTTON_MISC 14
#define BUTTON_BACK 15

#define BUTTON_CONFIRM 16
#define BUTTON_CANCEL 17
#define BUTTON_POINTER_PRESS 18

#define AXIS_X1 0
#define AXIS_Y1 1
#define AXIS_Z1 2

#define AXIS_X2 3
#define AXIS_Y2 4
#define AXIS_Z2 5

#define ICON_ACTION1 "❶"
#define ICON_ACTION2 "❷"
#define ICON_ACTION3 "❸"
#define ICON_ACTION4 "❹"
#define ICON_ACTION5 "❺"
#define ICON_ACTION6 "❻"

#define ICON_MENU "❼"

#define ICON_ARROWRIGHT "❽"
#define ICON_ARROWLEFT "❾"
#define ICON_ARROWDOWN "❿"
#define ICON_ARROWUP "➀"

#define ICON_MOUSELEFT "➁"
#define ICON_MOUSEMIDDLE "➂"
#define ICON_MOUSERIGHT "➃"
#define ICON_MISC "➄"
#define ICON_BACK "➅"

#define ICON_CONFIRM "➆"
#define ICON_CANCEL "➇"
#define ICON_POINTER_PRESS "➈"

#define ICON_LEFT1 ◀
#define ICON_RIGHT1 ▶
#define ICON_UP1 ▲
#define ICON_DOWN1 ▼
#define ICON_IN1 ◐
#define ICON_OUT1 ◑

#define ICON_LEFT2 ◁
#define ICON_RIGHT2 ▷
#define ICON_UP2 △
#define ICON_DOWN2 ▽
#define ICON_IN2 ◒
#define ICON_OUT2 ◓

