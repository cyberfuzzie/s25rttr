// $Id: glArchivItem_Bitmap.h 5155 2009-07-01 15:37:53Z FloSoft $
//
// Copyright (c) 2005-2009 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Siedler II.5 RTTR.
//
// Siedler II.5 RTTR is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Siedler II.5 RTTR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Siedler II.5 RTTR. If not, see <http://www.gnu.org/licenses/>.
#ifndef GLARCHIVITEM_BITMAP_INCLUDED
#define GLARCHIVITEM_BITMAP_INCLUDED

#pragma once

#include "colors.h"

/// Basisklasse für GL-Bitmapitems.
class glArchivItem_Bitmap : public virtual libsiedler2::baseArchivItem_Bitmap
{
public:
	/// Konstruktor von @p glArchivItem_Bitmap.
	glArchivItem_Bitmap(void);
	/// Kopiekonstruktor von @p glArchivItem_Bitmap.
	glArchivItem_Bitmap(const glArchivItem_Bitmap *item);

	/// Destruktor von @p glArchivItem_Bitmap.
	virtual ~glArchivItem_Bitmap(void);

	/// Erzeugt und zeichnet die Textur.
	virtual void Draw(short dst_x, short dst_y, short dst_w = 0, short dst_h = 0, short src_x = 0, short src_y = 0, short src_w = 0, short src_h = 0, unsigned int color = COLOR_WHITE)
{
	if(texture == 0)
		GenerateTexture();
	if(texture == 0)
		return;

	if(src_w == 0)
		src_w = width;
	if(src_h == 0)
		src_h = height;
	if(dst_w == 0)
		dst_w = src_w;
	if(dst_h == 0)
		dst_h = src_h;

	glEnable(GL_TEXTURE_2D);

	glColor4ub( GetRed(color), GetGreen(color), GetBlue(color), GetAlpha(color));
	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_QUADS);
	DrawVertex( (float)(dst_x-nx),         (float)(dst_y-ny),         (float)src_x,         (float)src_y);
	DrawVertex( (float)(dst_x-nx),         (float)(dst_y-ny + dst_h), (float)src_x,         (float)(src_y+src_h));
	DrawVertex( (float)(dst_x-nx + dst_w), (float)(dst_y-ny + dst_h), (float)(src_x+src_w), (float)(src_y+src_h));
	DrawVertex( (float)(dst_x-nx + dst_w), (float)(dst_y-ny),         (float)(src_x+src_w), (float)src_y);
	glEnd();
}

	/// liefert das GL-Textur-Handle.
	unsigned int GetTexture();
	/// Löscht die GL-Textur (z.B fürs Neuerstellen)
	virtual void DeleteTexture();
	/// Setzt den Texturfilter auf einen bestimmten Wert.
	virtual void setFilter(unsigned int filter);

protected:
	/// Erzeugt die Textur.
	virtual void GenerateTexture();
	/// Zeichnet einen Vertex inkl Texturkoordinaten.
	inline void DrawVertex(float x, float y, float tx, float ty)
	{
		glTexCoord2f(tx/tex_width, ty/tex_height);
		glVertex2f(x, y);
	}
protected:
	unsigned int texture; ///< Das GL-Textur-Handle
	unsigned int filter;  ///< Der aktuell gewählte Texturfilter
};

#endif // !GLARCHIVITEM_BITMAP_INCLUDED
