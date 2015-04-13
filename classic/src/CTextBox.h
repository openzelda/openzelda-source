/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CTextBox.h
//
// Desc: Class for Creating a Text box on the screen
//
//-----------------------------------------------------------------------------
#ifndef CTEXTBOX_H
#define CTEXTBOX_H

#include "GeneralData.h"

#define MAX_LINE			256
#define NUM_LINES			5
#define TEXT_ENLARGE		2

// Structure for holding data on 1 line in Text Box
struct TextLine{
	char	String[MAX_LINE];
	bool	InUse;
	int	 	Length;
	int		Index;
};


//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CTextBox;


//-----------------------------------------------------------------------------
// Name: class CTextBox
// Desc:    
//-----------------------------------------------------------------------------
class CTextBox
{
	char		m_textString[ENT_STRING_MAX];   	// Buffer to hold the complete string
	int			m_totalLength;						// Total length of string buffer
	TextLine	m_line[NUM_LINES];			     	// Holds each line being displayed
	bool		m_inUse;							// Is this Text Box in use?
	int			m_stringIndex;					
	int			m_numLines;							// Number of lines being displayed
	bool		m_fFinishedReading;

	// ================
	// Position Vars
	// ================
	int			m_x;						// Position on screen
	int			m_y;
	int			m_width;					// Dimentions of Window
	int			m_height;

	SDL_Color    m_TextColor;
	int			m_fontHeight;
	float		m_moveCounter;
	bool		m_moreText;					// Is there more text to be displayed?
	SDL_Point		m_answerPos[MAX_ANSWERS];
	int			m_numAnswers;
	int			m_answerOn;
	
	// =================
	// Text Box Surface
	// =================
	//CSurface*	m_pSurface;					// Surface used for Drawing onto


public:
    CTextBox();
    ~CTextBox();

	void InitText(char *String, int Width, bool fPriority);			// Shows the Text Box
	void Draw();
	void DisplayBox();
	void BuildLines();
	void AddWord(int LineOn, int Start, int End);
	void HandleTextBox();
	void MoveUp();
	void DrawFrame();
	void InsertPlayerName();
	void SetTextColor(int wRed, int wGreen, int wBlue, int wAlpha);
	void DrawTextBoxCentre();

	// Access Functions
	bool GetFinishedReading(){return m_fFinishedReading;}
	int GetAnswer(){return m_answerOn;}
};


#endif

