/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CTextBox.cpp
//
// Desc: Class for Creating a Text box on the screen.
//       
//
//-----------------------------------------------------------------------------
#define STRICT

#include <iostream>
#include <string.h>
#include "CTextBox.h"
#include "CGame.h"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
extern CGame*	g_pGame;
extern float	g_rDelta;


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CTextBox::CTextBox()
{
	// Initialise some values.
	m_totalLength   = 0;
	m_numLines		= 0;
	m_inUse			= false;
	m_fontHeight    = 13;
	m_moreText		= false;
	m_numAnswers	= 0;
	m_answerOn		= 0;
	m_x				= 100;
	m_y				= 100;
	m_width			= 300;
	m_height		= 200;
	m_TextColor.r = m_TextColor.b = m_TextColor.g = 255;
	m_fFinishedReading = false;

	// Clear the Main String
	memset(m_textString, 0,sizeof(m_textString));
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CTextBox::~CTextBox()
{
}

//-----------------------------------------------------------------------------
// Name: SetTextColor()
// Desc: 
//
//-----------------------------------------------------------------------------
void CTextBox::SetTextColor(int wRed, int wGreen, int wBlue, int wAlpha)
{
	m_TextColor.r = wRed;
	m_TextColor.b = wBlue;
	m_TextColor.g = wGreen;
	m_TextColor.a = wAlpha;
}


//-----------------------------------------------------------------------------
// Name: InitText()
// Desc: Other functions can call this to display the text box on screen
//       With specified text
//
//-----------------------------------------------------------------------------
void CTextBox::InitText(char *String, int Width, bool fPriority)
{	
	int n;
	int32_t textWidth;
	int32_t textHeight;

	// Do not continue if the Text Box is already in use
	if (m_inUse && !fPriority)
		return;

	m_moveCounter = 0;

	// Turn off the Q key
	g_pGame->m_fQKey = false;

	// Pause the game to a suitable level
	g_pGame->SetPauseLevel( 2, -1, NULL );

	// Clear the buffer
	memset(m_textString, 0, sizeof(m_textString));

	// Reset the line structures
	for (n= 0 ;n < NUM_LINES; n++)
	{
		memset(m_line[n].String, 0, sizeof(m_line[n].String));
		m_line[n].InUse = false;
		m_line[n].Length = 0;
		m_line[n].Index  = 0;
	}

	// Copy the String given to the textString buffer
	if (strlen(String) < sizeof(m_textString) - 10)
		strcpy(m_textString, String);
	else
		strncpy(m_textString, String, sizeof(m_textString) - 10);

	strcat(m_textString, " ` ");

	// Go through the string and insert the Player's name
	//InsertPlayerName();
	
	// Initialise some values.
	m_totalLength   =  strlen(m_textString);
	m_inUse			= true;
	m_width			= Width;
	m_height		= 50;
	m_moreText		= false;
	m_moveCounter	= 0;
	m_stringIndex	= 0;


	// Get the length of the string
	g_pGame->GetTextBoxFont()->GetTextExtent( m_textString, &textWidth, &textHeight );

	if (textWidth < m_width)
		m_width = textWidth;

	// Set the Coordinates of the Text Box
	m_x				= (VSCREEN_WIDTH >> 1) - (m_width >> 1);
	m_y				= 100;

	// Fill in the line
	// structures.
	BuildLines();
}


//-----------------------------------------------------------------------------
// Name: InsertPlayerName
// Desc:    
//-----------------------------------------------------------------------------
void CTextBox::InsertPlayerName()
{
	/*
	int n;
	int m;
	char szTemp[1000];
	int len = strlen(m_textString);
	int wResume;

	// Scan through the string - if we come to a ~ then insert the players name
	for (n  = 0; n < len; n++)
	{
		if (m_textString[n] == '~')
		{
			m_textString[n] = ' ';

			memset(szTemp, 0, sizeof(szTemp));
			for (m = 0; m < len - n; m++)
			{
				// Save the rest of the string
				szTemp[m] = m_textString[m + n];
			}

			// Now Write the name to the end of the String
			for (m = 0; m < strlen(g_Data->Player->m_name); m++)
			{
				m_textString[n + m] = g_Data->Player->m_name[m];
			}

			//wResume = n + strlen(g_Data->Player->m_name);

			n += strlen(g_Data->Player->m_name);
			len += strlen(g_Data->Player->m_name);

			// copy the rest of the string back
			for (m = 0; m < strlen(szTemp) ; m++ )
			{
				m_textString[n + m] = szTemp[m];
			}
		}
	}
	*/
}


//-----------------------------------------------------------------------------
// Name: DisplayBox
// Desc: Displays everything  
//
//-----------------------------------------------------------------------------
void CTextBox::DisplayBox()
{
	int m;
	int wYPos;

	// Re-calculate Height based upon
	// how many lines we are displaying
	m_height = ((m_numLines) * m_fontHeight) + 5;

	DrawTextBoxCentre();

	// Now Draw in each line of text which 
	// we built earlier
	for (m = 0; m < NUM_LINES; m++)
	{
		// Calculate the ypos of the Text
		wYPos = (m_y * TEXT_ENLARGE) + 
			    (m * (m_fontHeight * TEXT_ENLARGE) - 
				m_moveCounter * TEXT_ENLARGE) + 5;

	    g_pGame->GetTextBoxFont()->DrawText( m_x * TEXT_ENLARGE, 
											wYPos, 
											m_TextColor, 
											m_line[m].String );
	}

	// Draw the frame for the Text
	DrawFrame();
}


//-----------------------------------------------------------------------------
// Name: BuildLines
// Desc:    
//-----------------------------------------------------------------------------
void CTextBox::BuildLines()
{
	
	// We must scan through the string and
	// divide it up into 4 smaller ones.

	// Scan through each character in the string, every time
	// re-examine the length of the string so far. If the length
	// is greater than the line width (in pixels) then write to
	// the next line.

	// Each time we scan a new char we must record the last space
	// we scanned, so that the string can be broken into words.
	int n;
	int OldI			= 0;
	int NewI			= 0;
	int LineOn			= 0;
	int tempstringIndex = 0;
	char Temp[500];

	int32_t width;
	int32_t height;

	m_answerOn			= 0;
	m_numAnswers		= 0;
	m_numLines			= 0;

	OldI = m_stringIndex;
								  
	// Loop for NUM_LINES lines
	for (LineOn = 0; LineOn < NUM_LINES; LineOn++)
	{
		// Record the end position of the
		// first line
		if (LineOn == 1)
			tempstringIndex = OldI;

		// Set the temporary string to NULL
		memset(Temp,0,sizeof(Temp));

		// Scan through the string until we come
		// to a space
		for (n = NewI + m_stringIndex; n < 65636; n++)
		{
			// if the first char in the line is a 
			// space, remove it.
			if (n==NewI + m_stringIndex)
				if (m_textString[n] == ' ')
				OldI++;
				
			// Have we reached the end of the String?
			if (m_textString[n] == NULL)
			{
				m_numLines = LineOn+1;

				return;
			}

			// Add the current char to the temp string
			Temp[ strlen(Temp)] = m_textString[n];

			// Check to see if the length (in Pixels)
			// of the current line is too long
			g_pGame->GetTextBoxFont()->GetTextExtent( Temp, &width, &height );

			if (width > m_width * TEXT_ENLARGE)
				break;

			// If we reach a ` thats the newline char
			if (m_textString[n] == '`')
			{
				AddWord(LineOn, OldI, n);
				NewI = n+1;
				OldI = n+1;
				break;
			}

			// If we reach a | thats the 'Answer' char
			if (m_textString[n] == '|')
			{
				//NewI = n;
				//OldI = n;
				m_answerPos[m_numAnswers].x = width / 2;
				m_answerPos[m_numAnswers].y = LineOn;
		    	m_numAnswers++;
			}


			// Check for a space - then add the current
			// word to the curret line structure
			if (m_textString[n] == ' ')
			{
				// Got a space
				AddWord(LineOn, OldI, n);
				OldI = n;
			}
		}

		m_stringIndex=0;
		NewI = OldI;
	}

	// If we get here then we have too much
	// text to fit in the box.
	m_moreText    = true;
	m_numLines	  = LineOn;

	m_stringIndex = tempstringIndex;
}

//-----------------------------------------------------------------------------
// Name: AddWord()
// Desc:    
//-----------------------------------------------------------------------------
void CTextBox::AddWord(int LineOn, int Start, int End)
{
	// Add a substring from the main buffer to one of the
	// Line structures
	int n;

	m_line[LineOn].Length = End - Start;
	m_line[LineOn].InUse = true;

	// Copy the substring to the line.
	for (n=0;n < m_line[LineOn].Length;n++)
	{
		if (m_textString[n + Start] != '|')
		m_line[LineOn].String[n + m_line[LineOn].Index] = m_textString[n + Start];
		else
		m_line[LineOn].String[n + m_line[LineOn].Index] = ' ';

	}

	m_line[LineOn].Index  += m_line[LineOn].Length;
}

//-----------------------------------------------------------------------------
// Name: HandleTextBox()
// Desc:     
//-----------------------------------------------------------------------------
void CTextBox::HandleTextBox()
{
	int n;

	m_fFinishedReading = false;
	
	// If this is not in use then return
	if (!m_inUse)
		return;

	// Display everthing to the screen
	DisplayBox();
	
    if (!m_moreText)
	{
		if (m_numAnswers > 0)
        {
            RECT2D Rect;

		    // The arrow sprite's width and height
		    int ArrowWidth  = g_pGame->GetWidthHeight("__arow2", true);
		    int ArrowHeight = g_pGame->GetWidthHeight("__arow2", false);

            // The arrow's coordinates on the screen
		    int ArrowX = m_x + m_answerPos[m_answerOn].x - ArrowWidth ;
		    int ArrowY = m_y + m_answerPos[m_answerOn].y * m_fontHeight + 4;

		    // Place a small arrow next to the
	        // current answer.
		    g_pGame->SimpleDrawCodeLarge(ArrowX, ArrowY, "__arow2", 255, 255, 255, 255);
		
		    // The Player has moved the selection
		    // arrow up.
		    if (g_pGame->m_fUpKey)
		    {
			    g_pGame->m_fUpKey = false;
			    m_answerOn--;

			    // Erase the arrow's last position
		        Rect.left	= ArrowX;
		        Rect.top	= ArrowY;
			    Rect.right	= ArrowX + ArrowWidth;
			    Rect.bottom	= ArrowY + ArrowHeight;
		    }

		    // The Player has moved the selection
	        // arrow down.
		    if (g_pGame->m_fDownKey)
		    {
			    g_pGame->m_fDownKey = false;
			    m_answerOn++;
			
			    // Erase the arrow's last position
			    Rect.left	= ArrowX;
		        Rect.top	= ArrowY;
			    Rect.right	= ArrowX + ArrowWidth;
			    Rect.bottom	= ArrowY + ArrowHeight;
		    }

		    // Some Bounds checking
		    if (m_answerOn < 0)
	            m_answerOn = m_numAnswers-1;

		    if (m_answerOn > m_numAnswers-1)
			    m_answerOn = 0;

		    // If the player has selected an answer
		    if (g_pGame->m_fQKey)
			    m_numAnswers =0;
        }    
	
        if(m_numAnswers == 0)
        {
		    // UnPause the game
			g_pGame->SetPauseLevel(0, -1, NULL);

            m_inUse	= false;
            g_pGame->m_fQKey = false;
			
			// Disable to Q Key for a while
		    g_pGame->m_rQKeyTimeOut = 0.6;
		    m_fFinishedReading = true;
			return;
		}    
	}
    else
	{
		if (g_pGame->m_fQKey)
		{
			// There is more text to come so we
			// have to handle it.
			if (m_moveCounter < m_fontHeight - 1)
				MoveUp();
			else
			{
				m_moveCounter = 0;
			
    			// Reset the line structures
				for (n = 0; n < NUM_LINES; n++)
				{
					memset(m_line[n].String, 0, sizeof(m_line[n].String));
					m_line[n].InUse = false;
					m_line[n].Length = 0;
					m_line[n].Index  = 0;
				}

				m_moreText=false;
				g_pGame->m_fQKey = false;
				g_pGame->m_rQKeyTimeOut = 0.6;
				BuildLines();
			}
        }			
    }
}

//-----------------------------------------------------------------------------
// Name: MoveUp()
// Desc: Moves the text up 1 line   
// 
//-----------------------------------------------------------------------------
void CTextBox::MoveUp()
{
	// increment the move counter
	m_moveCounter += (40 * g_rDelta);
}


//-----------------------------------------------------------------------------
// Name: DrawTextBoxCentre()
// Desc: 
// 
//-----------------------------------------------------------------------------
void CTextBox::DrawTextBoxCentre()
{
	// Get the Width and Height of 1 TextBox frame Sprite
	int wWidth  = g_pGame->GetWidthHeight("__box01", true) / 2;
	int wHeight = g_pGame->GetWidthHeight("__box01", false) / 2;

	// Draw the middle of the Text box - alpha blended
	g_pGame->SimpleTileCode((m_x - wWidth) * 2, 
		                    (m_y - wHeight) * 2, 
							(m_width * 2) + (wWidth * 4),
							(m_height * 2) + (wHeight * 4),
							"__box09", 
		                    255,255,255,80);

}



//-----------------------------------------------------------------------------
// Name: DrawFrame()
// Desc: Draws the Text Box Frame from Sprites
// 
//-----------------------------------------------------------------------------
void CTextBox::DrawFrame()
{
	int m;

	// Get the Width and Height of 1 TextBox frame Sprite
	int wWidth  = g_pGame->GetWidthHeight("__box01", true) / 2;
	int wHeight = g_pGame->GetWidthHeight("__box01", false) / 2;

	if ( wWidth < 2 || wHeight < 2 )
		return;
	// Now draw the frame on the textbox layer
	// Start by drawing each line
	for (m = m_x; m < m_width + m_x; m += wWidth)
	{
		// Draw the top.
		g_pGame->SimpleDrawCodeLarge(m, m_y - wHeight, "__box05", 255,255,255,255);

		// Draw the Bottom.
		g_pGame->SimpleDrawCodeLarge(m, m_y + m_height, "__box07", 255,255,255,255);                         
	}

	for (m = m_y; m < m_height + m_y; m += wHeight)
	{
		// Draw the right side.
		g_pGame->SimpleDrawCodeLarge(m_x - wWidth, m, "__box08", 255,255,255,255);

		// Draw the left side.
		g_pGame->SimpleDrawCodeLarge(m_x + m_width, m, "__box06", 255,255,255,255);
	}

	// Now draw in Each corner
	// Top-Left
	g_pGame->SimpleDrawCodeLarge(m_x - wWidth, m_y - wHeight, "__box01", 255,255,255,255);

	// Top-Right
	g_pGame->SimpleDrawCodeLarge(m_x + m_width, m_y - wHeight, "__box02", 255,255,255,255);
	
	// Bottom-Left
	g_pGame->SimpleDrawCodeLarge(m_x - wWidth, m_y + m_height, "__box04", 255,255,255,255);

	// Bottom-Right
	g_pGame->SimpleDrawCodeLarge(m_x + m_width, m_y + m_height, "__box03", 255,255,255,255);

	
	// If there is more text to be displayed
	// then display a small arrow below the text
	if (m_moreText)
	{
		g_pGame->SimpleDrawCodeLarge(m_x + m_width - 20, 
			                         m_y + m_height - 10 + 6, "__arow1", 255,255,255,255);
	}
}



