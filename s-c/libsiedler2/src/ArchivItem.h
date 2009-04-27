// $Id: ArchivItem.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef ARCHIVITEM_H_INCLUDED
#define ARCHIVITEM_H_INCLUDED

#pragma once

namespace libsiedler2
{
	/// Basisklasse der Archivelemente.
	class ArchivItem
	{
	public:
		/// Konstruktor von @p ArchivItem.
		ArchivItem();

		/// Kopierkonstruktor von @p ArchivItem.
		ArchivItem(const ArchivItem *item);

		/// virtueller Destruktor von @p ArchivItem.
		virtual ~ArchivItem();

		/// setzt den Bobtype des Items.
		void setBobType(unsigned short bobtype);

		/// liefert den Bobtype des Items.
		unsigned short getBobType(void) const;

		/// setzt den Namen des Items.
		void setName(const char *name);

		/// liefert den Namen des Items.
		const char *getName(void) const;

		/// lädt die Daten aus einer Datei.
		virtual int load(FILE *file);

		/// schreibt die Daten in eine Datei.
		virtual int write(FILE *file) const;

	protected:
		unsigned short bobtype; ///< Bobtype des Elements.
		char name[17];          ///< Name des Elements.
	};
}

#endif // !ARCHIVITEM_H_INCLUDED
