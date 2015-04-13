/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// ShapeBuffer.cpp: implementation of the CShapeBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "ShapeBuffer.h"
#include "CGame.h"

extern CGame*		g_pGame;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShapeBuffer::CShapeBuffer()
{
	m_nShapes = 0;
}

CShapeBuffer::~CShapeBuffer()
{

}

void CShapeBuffer::AddShape( int nType, RECT2D sRect, float fAngle, SDL_Color dwColour)
{
	if (m_nShapes >= MAX_SHAPES )
		return;

	m_Shapes[m_nShapes].nType = nType;
	m_Shapes[m_nShapes].sRect = sRect;
	m_Shapes[m_nShapes].fAngle = fAngle;
	m_Shapes[m_nShapes].dwColour = dwColour;
	m_nShapes++;
}

void CShapeBuffer::DrawShapes()
{
	RECT2D sRect;
	int n;

	for ( n = 0 ; n < m_nShapes; n++ )
	{
		if ( m_Shapes[n].nType == 0 )
		{

			g_pGame->GetDisplay()->Rectangle(m_Shapes[n].sRect,
											 DegToRad((int)m_Shapes[n].fAngle),
											 m_Shapes[n].dwColour, true);
		}
		if ( m_Shapes[n].nType == 1 )
		{
			sRect = m_Shapes[n].sRect;
			g_pGame->GetDisplay()->Line( sRect.left, sRect.top, sRect.right, sRect.bottom,
										 m_Shapes[n].fAngle, m_Shapes[n].dwColour);
		}
	}


	m_nShapes = 0;
}
