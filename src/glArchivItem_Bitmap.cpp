// $Id: glArchivItem_Bitmap.cpp 5110 2009-06-26 06:45:50Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "glArchivItem_Bitmap.h"

#include "VideoDriverWrapper.h"
#include "GlobalVars.h"
#include "Loader.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class glArchivItem_Bitmap
 *
 *  Basisklasse f¸r GL-Bitmapitems.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var glArchivItem_Bitmap::texture
 *
 *  OpenGL-Textur des Bildes.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Konstruktor von @p glArchivItem_Bitmap.
 *
 *  @author FloSoft
 */
glArchivItem_Bitmap::glArchivItem_Bitmap(void)
	: baseArchivItem_Bitmap(), texture(0), filter(GL_NEAREST)
{
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Kopiekonstruktor von @p glArchivItem_Bitmap.
 *
 *  @author FloSoft
 */
glArchivItem_Bitmap::glArchivItem_Bitmap(const glArchivItem_Bitmap *item)
	: baseArchivItem_Bitmap(item), texture(0), filter(GL_NEAREST)
{
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Destruktor von @p glArchivItem_Bitmap.
 *
 *  @author FloSoft
 */
glArchivItem_Bitmap::~glArchivItem_Bitmap(void)
{
	DeleteTexture();
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Zeichnet die Textur.
 *
 *  @author FloSoft
 */
void glArchivItem_Bitmap::Draw(short dst_x, short dst_y, short dst_w, short dst_h, short src_x, short src_y, short src_w, short src_h, unsigned int color)
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

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Liefert das GL-Textur-Handle.
 *
 *  @author FloSoft
 */
unsigned int glArchivItem_Bitmap::GetTexture()
{
	if(texture == 0)
		GenerateTexture();
	return texture;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Lˆscht die GL-Textur (z.B f¸rs Neuerstellen)
 *
 *  @author FloSoft
 */
void glArchivItem_Bitmap::DeleteTexture()
{
	glDeleteTextures(1, (const GLuint*)&texture);
	texture = 0;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Setzt den Texturfilter auf einen bestimmten Wert.
 *
 *  @author FloSoft
 */
void glArchivItem_Bitmap::setFilter(unsigned int filter)
{
	if(this->filter == filter)
		return;

	this->filter = filter;

	// neugenerierung der Textur anstoﬂen
	if(texture != 0)
		DeleteTexture();
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Erzeugt die Textur.
 *
 *  @author FloSoft
 */
void glArchivItem_Bitmap::GenerateTexture(void)
{
	texture = VideoDriverWrapper::inst().GenerateTexture();

	if(!palette)
		setPalette(GetPalette(0));
	
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	int iformat = GL_RGBA, dformat = GL_BGRA;

	unsigned char *buffer = new unsigned char[tex_width*tex_height*4];

	memset(buffer, 0, tex_width*tex_height*4);
	print(buffer, tex_width, tex_height, libsiedler2::FORMAT_RGBA, palette, 0, 0, 0, 0, 0, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, iformat, tex_width, tex_height, 0, dformat, GL_UNSIGNED_BYTE, buffer);

	delete[] buffer;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Zeichnet einen Vertex inkl Texturkoordinaten.
 *
 *  @author FloSoft
 */
void glArchivItem_Bitmap::DrawVertex(float x, float y, float tx, float ty)
{
	glTexCoord2f(tx/tex_width, ty/tex_height);
	glVertex2f(x, y);
}
