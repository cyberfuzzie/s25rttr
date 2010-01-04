// $Id: ArchivItem_Ini.h 5853 2010-01-04 16:14:16Z FloSoft $
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
#ifndef ARCHIVITEM_INI_H_INCLUDED
#define ARCHIVITEM_INI_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include "ArchivItem_Text.h"

namespace libsiedler2
{

/// Klasse für INI-Dateien (genauergesagt eine Sektion).
class ArchivItem_Ini : public ArchivItem, public ArchivInfo
{
public:
	/// Konstruktor von @p ArchivItem_Ini.
	ArchivItem_Ini(void);

	/// Konstruktor von @p ArchivItem_Ini.
	ArchivItem_Ini(const char *name);

	/// Kopierkonstruktor von @p ArchivItem_Ini.
	ArchivItem_Ini(const ArchivItem_Ini *item);

	/// lädt die INI-Daten aus einer Datei.
	int load(FILE *file);

	/// schreibt die INI-Daten in eine Datei.
	int write(FILE *file) const;

	/// liest einen Wert aus der Ini
	inline const char *getValue(const char *name) const
	{
		const ArchivItem_Text *item = dynamic_cast<const ArchivItem_Text*>(find(name));
		if(item)
		{
			if(item->getText())
				return item->getText();
		}
		return "";
	}

	inline int getValueI(const char *name) const
	{
		return atoi(getValue(name));
	}

	/// fügt einen Eintrag hinzu.
	void addValue(const char *name, const char *value);

	/// schreibt einen Wert in die Ini
	inline void setValue(const char *name, const char *value)
	{
		ArchivItem_Text *item = dynamic_cast<ArchivItem_Text*>(find(name));
		if(item)
		{
			// setText überschreibt Namen, daher nochmals setzen
			item->setText(value);
			item->setName(name);
		}
		else
		{
			// nicht gefunden, also hinzufügen
			addValue(name, value);
		}
	}

	inline void setValue(const char *name, unsigned int value)
	{
		char temp[512];
		snprintf(temp, 256, "%d", value);

		ArchivItem_Text *item = dynamic_cast<ArchivItem_Text*>(find(name));
		if(item)
		{
			// setText überschreibt Namen, daher nochmals setzen
			item->setText(temp);
			item->setName(name);
		}
		else
		{
			// nicht gefunden, also hinzufügen
			addValue(name, temp);
		}
	}
};

}

#endif // !ARCHIVITEM_INI_H_INCLUDED
