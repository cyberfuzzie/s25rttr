// $Id: ArchivItem_Font.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef ARCHIVITEM_FONT_H_INCLUDED
#define ARCHIVITEM_FONT_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include "ArchivItem_Palette.h"

namespace libsiedler2
{

/// Klasse für Fonts.
class ArchivItem_Font : public ArchivItem, public ArchivInfo
{
public:
	/// Konstruktor von @p ArchivItem_Font.
	ArchivItem_Font(void);

	/// Kopierkonstruktor von @p ArchivItem_Font.
	ArchivItem_Font(const ArchivItem_Font *item);

	/// Konstruktor von @p ArchivItem_Font mit Laden der Font-Daten.
	ArchivItem_Font(FILE *file, const ArchivItem_Palette *palette);

	/// Destruktor von @p ArchivItem_Font.
	~ArchivItem_Font(void);

	/// lädt die Fontdaten aus einer Datei.
	int load(FILE *file, const ArchivItem_Palette *palette);

	/// schreibt die Fontdaten in eine Datei.
	int write(FILE *file, const ArchivItem_Palette *palette) const;

	/// liefert den X-Buchstabenabstand.
	unsigned char getDx(void) const;

	/// liefert den Y-Buchstabenabstand.
	unsigned char getDy(void) const;

	/// setzt den X-Buchstabenabstand.
	void setDx(unsigned char dx);

	/// setzt den Y-Buchstabenabstand.
	void setDy(unsigned char dy);

protected:
	unsigned char dx; ///< X-Buchstabenabstand.
	unsigned char dy; ///< Y-Buchstabenabstand.
};

}

#endif // !ARCHIVITEM_FONT_H_INCLUDED
