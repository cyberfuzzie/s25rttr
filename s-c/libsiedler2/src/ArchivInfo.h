// $Id: ArchivInfo.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef ARCHIVINFO_H_INCLUDED
#define ARCHIVINFO_H_INCLUDED

#pragma once

#include "ArchivItem.h"

namespace libsiedler2
{
	/// Klasse für Archivdateien.
	class ArchivInfo 
	{
	public:
		/// Konstruktor von @p ArchivInfo.
		ArchivInfo(void);

		/// Kopierkonstruktor von @p ArchivInfo.
		ArchivInfo(const ArchivInfo *info);

		/// Destruktor von @p ArchivInfo, räumt automatisch auf.
		virtual ~ArchivInfo(void);

		/// erstellt den Datensatz in einer bestimmten Größe.
		void alloc(unsigned long count);

		/// vergrößert den Datensatz um eine bestimmten Größe.
		void alloc_inc(unsigned long increment);

		/// gibt die angelegten Daten wieder frei.
		void clear(void);

		/// Setzt den Inhalt eines ArchivItems auf das des Übergebenen.
		void set(int index, ArchivItem *item);

		/// kopiert den Inhalt eines ArchivItems auf das des Übergebenen.
		void setC(int index, const ArchivItem *item);

		/// fügt ein Element hinten an.
		void push(ArchivItem *item);

		/// fügt ein Element hinten an und kopiert die Daten von @p item.
		void pushC(const ArchivItem *item);

		/// liefert den Inhalt eines ArchivItems am entsprechenden Index.
		ArchivItem *get(int index);

		/// liefert den Inhalt eines ArchivItems am entsprechenden Index.
		const ArchivItem *get(int index) const;

		/// liefert den Pointer eines ArchivItems am entsprechenden Index.
		ArchivItem **getP(int index);

		/// liefert die Größe des Archivs.
		unsigned long getCount(void) const;

		/// Index-Operator von @p ArchivInfo. 
		ArchivItem *operator[](int index);

		/// Zuweisungsoperator von @p ArchivInfo.
		ArchivInfo& operator=(ArchivInfo &info);

		/// Kopierfunktion von @p ArchivInfo.
		void copy(unsigned int to, unsigned int from, unsigned int count, const ArchivInfo *source);

	protected:
		unsigned long count; ///< Anzahl der Elemente.
		ArchivItem **data;   ///< die Elemente.
	};
}

#endif // ARCHIVINFO_H_INCLUDED
