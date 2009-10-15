// $Id: Settings.h 5637 2009-10-15 16:18:56Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Klasse für die Konfiguration
class Settings : public Singleton<Settings>
{
public:
	Settings(void);

	bool Load(void); // Lädt Einstellungen
	void Save(void); // Speichert Einstellungen

protected:
	bool LoadDefaults();

public:
	struct {
	} global;

	struct {
		unsigned short width;
		unsigned short height;
		bool fullscreen;
		bool vsync;
		bool vbo;
	} video;

	struct {
		std::string language;
	} language;

	struct {
		std::string audio;
		std::string video;
	} driver;

	struct {
		bool musik;
		unsigned char musik_volume;
		bool effekte;
		unsigned char effekte_volume;
		std::string playlist; ///< musicplayer playlist name
	} sound;

	struct {
		std::string name;
		std::string password;
		std::string email;
		bool save_password;
	} lobby;

	struct {
		std::string last_ip; ///< last entered ip or hostname
		bool ipv6;           ///< listen/connect on ipv6 as default or not
	} server;

	struct {
		std::string proxy; ///< Serveradresse / Hostname
		unsigned int port; ///< Port
		unsigned char typ; ///< Socks 4 oder 5
	} proxy;

	struct {
		unsigned int autosave_interval;
	} savegames;

private:
	static const unsigned int SETTINGS_VERSION;
	static const unsigned int SETTINGS_SECTIONS;
	static const std::string SETTINGS_SECTION_NAMES[];
};

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#define SETTINGS Settings::inst()

#endif // SETTINGS_H_INCLUDED
