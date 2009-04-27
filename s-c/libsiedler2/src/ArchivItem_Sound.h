// $Id: ArchivItem_Sound.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef ARCHIVITEM_SOUND_H_INCLUDED
#define ARCHIVITEM_SOUND_H_INCLUDED

#pragma once

#include "ArchivItem.h"

namespace libsiedler2
{
	enum SOUNDTYPES
	{
		SOUNDTYPE_NONE = 0,
		SOUNDTYPE_WAVE = 1,
		SOUNDTYPE_MIDI = 2,
		SOUNDTYPE_XMIDI = 3,
		SOUNDTYPE_OTHER = 4
	};

	/// Basis-Basisklasse für Sounditems.
	class baseArchivItem_Sound : public ArchivItem
	{
	public:
		/// Konstruktor von @p ArchivItem_Sound.
		baseArchivItem_Sound(void);

		/// Kopierkonstruktor von @p ArchivItem_Sound.
		baseArchivItem_Sound(const baseArchivItem_Sound *item);

		/// virtueller Destruktor von @p ArchivItem_Sound.
		virtual ~baseArchivItem_Sound(void);

		/// setzt den Typ des Sounds.
		void setType(unsigned short type);

		/// liefert den Typ des Sounds.
		unsigned short getType(void) const;

		/// lädt die Sound-Daten aus einer Datei.
		virtual int load(FILE *file, unsigned int length);

		/// schreibt die Sound-Daten in eine Datei.
		virtual int write(FILE *file) const;

		static baseArchivItem_Sound *findSubType(FILE *file);

	protected:
		unsigned short type;
	};

	/// Basisklasse für Sounditems.
	class ArchivItem_Sound : public virtual baseArchivItem_Sound
	{
	};
}

#endif // !ARCHIVITEM_SOUND_H_INCLUDED
