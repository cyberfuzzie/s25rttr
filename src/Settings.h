// $Id: Settings.h 4959 2009-05-26 16:17:23Z Demophobie $
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
#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
// Header
#include "Singleton.h"
#include "enhOption.h"

///////////////////////////////////////////////////////////////////////////////
// Klasse für die Konfiguration
class Settings : public Singleton<Settings>
{
	static const char *SETTINGS_VERSION;
public:
	Settings(void);

	bool Load(void); // Lädt Einstellungen
	void Save(void); // Speichert Einstellungen


protected:
	bool LoadDefaults();

public:
	unsigned short width;
	unsigned short height;
	std::string language;
	std::string name;
	std::string password;
	std::string email;
	bool fullscreen;
	bool vsync;
	bool vbo;
	bool save_password;
	std::string audio_driver;
	std::string video_driver;
	bool musik;
	unsigned char musik_volume;
	bool effekte;
	unsigned char effekte_volume;
	unsigned autosave_interval;
	/// Zuletzt eingegebene IP merken
	std::string last_ip;
	/// Musicplayer playlist name
	std::string playlist;
	/// Enhancement options
	enhOptions* enhs;
};

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#define SETTINGS Settings::inst()

#endif // SETTINGS_H_INCLUDED
