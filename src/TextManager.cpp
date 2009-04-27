// $Id: TextManager.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "TextManager.h"

#include "files.h"
#include "ListDir.h"
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/// "Notsprache", die genommen wird, wenn eine Gruppe in der ausgewählten Sprache nicht existiert
const std::string default_language = "en";

/// Zuordnung IDs-Gruppennamen
const std::string TEXT_GROUP_STRINGS[TXT_COUNT] =
{
	"mainmenus",
	"options"
};


/// Läd einen String aus einer Gruppe
const std::string& TextManager::GetText(const TextGroup tg, unsigned id)
{
	--id;

	assert(tg < TXT_COUNT);
	assert(id < groups[tg].size());

	return groups[tg][id];

}

/// Wählt eine Sprache aus (Kurzcode!)
bool TextManager::LoadLanguage(const std::string& language)
{
	// Alle Gruppen entsprechend laden
	for(unsigned g = 0;g<TXT_COUNT;++g)
	{
		// Pfad zusammenbauen
		std::string path = FILE_PATHS[57] + language + "/" + TEXT_GROUP_STRINGS[g] + ".txt";

			// Versuchen, Datei zu öffnen
		std::ifstream file(path.c_str());

		// Existiert Datei nicht?
		if(!file.good())
		{
			// Gruppe der Standardsprache laden
			path = FILE_PATHS[57] + default_language + "/" + TEXT_GROUP_STRINGS[g] + ".txt";

			file.open(path.c_str());
			// Wieder nicht?
			if(!file.good())
			{
				printf("TextManager::LoadLanguage: ERROR: Group \"%s\" couldn't be loaded! (Language: %s, Default Language: %s)\n",
					TEXT_GROUP_STRINGS[g].c_str(),language.c_str(),default_language.c_str());

				// Dann wars wohl nichts
				return false;
			}
		}

		// Datei einlesen

		// Zuerst Zeilen zählen
		char tmp[512];
		unsigned count = 0;

		while(true)
		{
			file.getline(tmp,512);

			++count;

			if(file.eof())
				break;
		}

		groups[g].resize(count);

		file.clear();
		file.seekg(0);

		// Schließlich richtig auslesen
		for(unsigned i = 0;i<count;++i)
		{
			file.getline(tmp,512);
			groups[g][i] = tmp;
		}

		file.close();
	}

	return true;
}


void CreateLanguageList(std::vector<std::string>& languages,
									 std::vector<std::string>& language_codes)
{
	// Erstmal leeren ggf.
	languages.clear();
	language_codes.clear();

	// Sprachen (Codes davon) raussuchen, also die Sprachordner
	list<std::string> language_codes_list;

	std::string path = FILE_PATHS[57];
	path += "*";
	ListDir(path,0,0,&language_codes_list);

	// Speicher reservieren
	languages.resize(language_codes_list.size());
	language_codes.resize(language_codes_list.size());

	int i = 0;
	for(list<std::string>::iterator it = language_codes_list.begin();it.valid();++it,++i)
	{
		// Überprüfen, ob das ein gültiger Sprachordner ist --> index-Datei muss darin liegen
		std::string index = *it + "/" + "index";

		std::ifstream file(index.c_str());

		if(!file.good())
		{
			// Index wieder zurück, damit keine leeren Stellen im Array sind
			--i;
			// Keine index-Datei vorhanden, dann nicht nehmen, kein gültiger Sprachordner
			continue;
		}

		// ansonsten Sprachname und Sprachname in Englisch auslesen
		std::string lang, lang_en;

		std::getline(file,lang);
		std::getline(file,lang_en);

		file.close();

		// Gesamtstring bilden und schließlich hinzufügen
		std::string lang_whole = lang + " (" + lang_en + ")";
		languages[i] = lang_whole;
		size_t pos = it->find_last_of('/');
		assert(pos != std::string::npos);
		language_codes[i] = it->substr(pos+1);
	}

	// Array-Größen korrigieren
	languages.resize(i);
	language_codes.resize(i);

}