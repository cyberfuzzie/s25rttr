// $Id: glArchivItem_Bitmap_Player.cpp 5853 2010-01-04 16:14:16Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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
#include "glArchivItem_Bitmap_Player.h"

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

void glArchivItem_Bitmap_Player::Draw(short dst_x, short dst_y, short dst_w, short dst_h, short src_x, short src_y, short src_w, short src_h, const unsigned int color, const unsigned int player_color)
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

	glColor4ub( GetRed(color), GetGreen(color), GetBlue(color),  GetAlpha(color));
	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_QUADS);
	DrawVertex( (float)(dst_x-nx),         (float)(dst_y-ny),         (float)src_x/2.0f,         (float)src_y);
	DrawVertex( (float)(dst_x-nx),         (float)(dst_y-ny + dst_h), (float)src_x/2.0f,         (float)(src_y+src_h));
	DrawVertex( (float)(dst_x-nx + dst_w), (float)(dst_y-ny + dst_h), (float)(src_x+src_w)/2.0f, (float)(src_y+src_h));
	DrawVertex( (float)(dst_x-nx + dst_w), (float)(dst_y-ny),         (float)(src_x+src_w)/2.0f, (float)src_y);
	glEnd();

	glColor4ub( GetRed(player_color), GetGreen(player_color), GetBlue(player_color),  GetAlpha(player_color));

	src_x += tex_width;

	glBegin(GL_QUADS);
	DrawVertex( (float)(dst_x-nx),         (float)(dst_y-ny),         (float)src_x/2.0f,         (float)src_y);
	DrawVertex( (float)(dst_x-nx),         (float)(dst_y-ny + dst_h), (float)src_x/2.0f,         (float)(src_y+src_h));
	DrawVertex( (float)(dst_x-nx + dst_w), (float)(dst_y-ny + dst_h), (float)(src_x+src_w)/2.0f, (float)(src_y+src_h));
	DrawVertex( (float)(dst_x-nx + dst_w), (float)(dst_y-ny),         (float)(src_x+src_w)/2.0f, (float)src_y);
	glEnd();
}

void glArchivItem_Bitmap_Player::GenerateTexture(void)
{
	texture = VideoDriverWrapper::inst().GenerateTexture();

	// wieso nicht pal5???
	setPalette(LOADER.GetPaletteN("colors"));

	int iformat = GL_RGBA, dformat = GL_BGRA; //GL_BGRA_EXT;

	unsigned char *buffer = new unsigned char[tex_width*2*tex_height*4];

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	memset(buffer, 0, tex_width*2*tex_height*4);
	printHelper(buffer, tex_width*2, tex_height, libsiedler2::FORMAT_RGBA, palette, 128, 0, 0, 0, 0, 0, 0, false);
	printHelper(buffer, tex_width*2, tex_height, libsiedler2::FORMAT_RGBA, palette, 128, tex_width, 0, 0, 0, 0, 0, true);
	glTexImage2D(GL_TEXTURE_2D, 0, iformat, tex_width*2, tex_height, 0, dformat, GL_UNSIGNED_BYTE, buffer);

	delete[] buffer;
}
