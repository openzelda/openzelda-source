/****************************
Copyright © 2002-2012 Open Zelda
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
// ShapeBuffer.h: interface for the CShapeBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAPEBUFFER_H__4E23174F_AF00_4B3F_B80C_5770A5BE3317__INCLUDED_)
#define AFX_SHAPEBUFFER_H__4E23174F_AF00_4B3F_B80C_5770A5BE3317__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "GeneralData.h"

#define MAX_SHAPES 200



// Name: struct _ShapeBuffer
struct _ShapeBuffer{
		RECT2D	sRect;
		float	fAngle;
		SDL_Color	dwColour;
		int		nType;		// 0- rectange 1- line
};


class CShapeBuffer
{
	_ShapeBuffer	m_Shapes[MAX_SHAPES];
	int				m_nShapes;

public:
	CShapeBuffer();
	virtual ~CShapeBuffer();

	void AddShape(int nType, RECT2D sRect, float fAngle, SDL_Color dwColour);
	void DrawShapes();
	void ClearBuffer(){ m_nShapes = 0;}
};

#endif // !defined(AFX_SHAPEBUFFER_H__4E23174F_AF00_4B3F_B80C_5770A5BE3317__INCLUDED_)
