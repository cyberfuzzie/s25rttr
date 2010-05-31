// $Id: glArchivItem_Bitmap_Player.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef GLARCHIVITEM_BITMAP_PLAYER_H_INCLUDED
#define GLARCHIVITEM_BITMAP_PLAYER_H_INCLUDED

#pragma once

/// Klasse für GL-Player-Bitmaps.
class glArchivItem_Bitmap_Player : public libsiedler2::baseArchivItem_Bitmap_Player, public glArchivItem_Bitmap
{
public:
	/// Konstruktor von @p glArchivItem_Bitmap_Player.
	glArchivItem_Bitmap_Player(void) : baseArchivItem_Bitmap(), baseArchivItem_Bitmap_Player(), glArchivItem_Bitmap() {}

	/// Kopierkonstruktor von @p ArchivItem_Bitmap_Player.
	glArchivItem_Bitmap_Player(const glArchivItem_Bitmap_Player *item) : baseArchivItem_Bitmap(item), baseArchivItem_Bitmap_Player(item), glArchivItem_Bitmap(item) {}

	void Draw(short dst_x, short dst_y, short dst_w , short dst_h , short src_x , short src_y , short src_w , short src_h , const unsigned int color , const unsigned int player_color );

protected:
	void GenerateTexture();
};

#endif // !GLARCHIVITEM_BITMAP_PLAYER_H_INCLUDED
