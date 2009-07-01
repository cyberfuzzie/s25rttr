// $Id: glArchivItem_Bitmap_Player.cpp 5156 2009-07-01 17:35:43Z FloSoft $
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

void glArchivItem_Bitmap_Player::Draw(short dst_x, short dst_y, short dst_w, short dst_h, short src_x, short src_y, short src_w, short src_h, const unsigned player_color, const unsigned color, int only_player)
{
	if(texture == 0 || ptexture == 0)
		GenerateTexture();
	if(texture == 0 || ptexture == 0)
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

	if(only_player <= 0)
	{
		glColor4ub( GetRed(color), GetGreen(color), GetBlue(color),  GetAlpha(color));
		glBindTexture(GL_TEXTURE_2D, texture);

		glBegin(GL_QUADS);
		DrawVertex( (float)(dst_x-nx),         (float)(dst_y-ny),         (float)src_x,         (float)src_y);
		DrawVertex( (float)(dst_x-nx),         (float)(dst_y-ny + dst_h), (float)src_x,         (float)(src_y+src_h));
		DrawVertex( (float)(dst_x-nx + dst_w), (float)(dst_y-ny + dst_h), (float)(src_x+src_w), (float)(src_y+src_h));
		DrawVertex( (float)(dst_x-nx + dst_w), (float)(dst_y-ny),         (float)(src_x+src_w), (float)src_y);
		glEnd();
	}

	if(only_player >= 0)
	{
		glColor4ub( GetRed(player_color), GetGreen(player_color), GetBlue(player_color),  GetAlpha(player_color));
		glBindTexture(GL_TEXTURE_2D, ptexture);

		glBegin(GL_QUADS);
		DrawVertex( (float)(dst_x-nx),         (float)(dst_y-ny),         (float)src_x,         (float)src_y);
		DrawVertex( (float)(dst_x-nx),         (float)(dst_y-ny + dst_h), (float)src_x,         (float)(src_y+src_h));
		DrawVertex( (float)(dst_x-nx + dst_w), (float)(dst_y-ny + dst_h), (float)(src_x+src_w), (float)(src_y+src_h));
		DrawVertex( (float)(dst_x-nx + dst_w), (float)(dst_y-ny),         (float)(src_x+src_w), (float)src_y);
		glEnd();
	}
}

void glArchivItem_Bitmap_Player::GenerateTexture(void)
{
	texture = VideoDriverWrapper::inst().GenerateTexture();
	ptexture = VideoDriverWrapper::inst().GenerateTexture();
	
	//if(!palette)
	setPalette(GetPalette(6));

	int iformat = GL_RGBA, dformat = GL_BGRA; //GL_BGRA_EXT;

	unsigned char *buffer = new unsigned char[tex_width*tex_height*4];

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	memset(buffer, 0, tex_width*tex_height*4);
	printHelper(buffer, tex_width, tex_height, libsiedler2::FORMAT_RGBA, palette, 128, 0, 0, 0, 0, 0, 0, false);
	glTexImage2D(GL_TEXTURE_2D, 0, iformat, tex_width, tex_height, 0, dformat, GL_UNSIGNED_BYTE, buffer);

	// Playertextur erzeugen
	glBindTexture(GL_TEXTURE_2D, ptexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	memset(buffer, 0, tex_width*tex_height*4);
	printHelper(buffer, tex_width, tex_height, libsiedler2::FORMAT_RGBA, palette, 128, 0, 0, 0, 0, 0, 0, true);
	glTexImage2D(GL_TEXTURE_2D, 0, iformat, tex_width, tex_height, 0, dformat, GL_UNSIGNED_BYTE, buffer);

	delete[] buffer;
}

void glArchivItem_Bitmap_Player::setFilter(unsigned int filter)
{
	if(this->filter == filter)
		return;

	this->filter = filter;

	// neugenerierung der Textur anstoﬂen
	if(texture != 0)
	{
		glDeleteTextures(1, (const GLuint*)&texture);
		texture = 0;
	}
	if(ptexture != 0)
	{
		glDeleteTextures(1, (const GLuint*)&ptexture);
		ptexture = 0;
	}
}
