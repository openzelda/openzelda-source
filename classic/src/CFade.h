/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
//-----------------------------------------------------------------------------
// File: CFade.h
//
// Desc: 
// 
//-----------------------------------------------------------------------------
#ifndef CFADE_H
#define CFADE_H


//-----------------------------------------------------------------------------
// Classes defined in this header file 
//-----------------------------------------------------------------------------
class CFade;

//-----------------------------------------------------------------------------
// Name: class CFade
// Desc: 
//-----------------------------------------------------------------------------
class CFade
{
	int		m_wRedTarg;
	int		m_wGreenTarg;
	int		m_wBlueTarg;

	float   m_rCurrentRed;
	float   m_rCurrentGreen;
	float   m_rCurrentBlue;
	float   m_rFadeRate;
    
public:
	
    CFade();
    ~CFade();

	bool FadeTo(int r, int g, int b, int wSpeed);
	void DoFade();
	void CombineColor(int* wInputCol, int rgb);


	int	 GetRed(){return (int)m_wRedTarg;}
	int	 GetGreen(){return (int)m_wGreenTarg;}
	int	 GetBlue(){return (int)m_wBlueTarg;}


};


#endif

