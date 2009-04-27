// $Id: glArchivItem_Bitmap_Direct.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef GLARCHIVITEM_BITMAP_DIRECT_H_INCLUDED
#define GLARCHIVITEM_BITMAP_DIRECT_H_INCLUDED

#pragma once

/// Klasse für GL-Direct-Bitmaps.
class glArchivItem_Bitmap_Direct : public glArchivItem_Bitmap
{
public:
	/// Konstruktor von @p glArchivItem_Bitmap_Direct.
	glArchivItem_Bitmap_Direct(void);
	/// Kopierkonstruktor von @p glArchivItem_Bitmap_Direct.
	glArchivItem_Bitmap_Direct(const glArchivItem_Bitmap_Direct *item);

	/// setzt einen Pixel auf einen bestimmten Wert.
	virtual void tex_setPixel(unsigned short x, unsigned short y, unsigned char color, const libsiedler2::ArchivItem_Palette *palette);
	/// setzt einen Pixel auf einen bestimmten Wert.
	virtual void tex_setPixel(unsigned short x, unsigned short y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	
	/// liefert die Farbwerte eines Pixels als uc-Array: {r,g,b,a}
	unsigned char * tex_getPixel(const unsigned short x, const unsigned short y);


	/// lädt die Bilddaten aus einer Datei.
	virtual int load(FILE *file, const libsiedler2::ArchivItem_Palette *palette) { return 254; }
	/// schreibt die Bilddaten in eine Datei.
	virtual int write(FILE *file, const libsiedler2::ArchivItem_Palette *palette) const { return 254; }
};

#endif // !GLARCHIVITEM_BITMAP_DIRECT_H_INCLUDED
