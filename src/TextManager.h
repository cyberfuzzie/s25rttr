// $Id: TextManager.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef TEXT_MANAGER_H_
#define TEXT_MANAGER_H_

#include <vector>
#include <string>

#include "Singleton.h"

/// Text-Gruppen
enum TextGroup
{
	TXT_MAINMENUS = 0,
	TXT_OPTIONS,
	TXT_COUNT
};


/// Klasse für das Laden und die Verwaltung der Strings
class TextManager : public Singleton<TextManager>
{
	/// String-Gruppen
	std::vector<std::string> groups[TXT_COUNT];

public:

	/// Läd einen String aus einer Gruppe
	const std::string& GetText(const TextGroup tg, unsigned id);

	/// Wählt eine Sprache aus (Kurzcode!)
	bool LoadLanguage(const std::string& language);
};

/// Erstellt eine Liste mit allen verfügbaren Sprachen
void CreateLanguageList(std::vector<std::string>& languages, std::vector<std::string>& language_codes);

///// Schneller Zugriff auf GetText
//inline const std::string& _(const TextGroup tg,const unsigned id) { return TextManager::inst().GetText(tg,id); }



#endif //!TEXT_MANAGER_H_
