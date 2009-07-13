// $Id: Settings.cpp 5262 2009-07-13 19:51:14Z FloSoft $
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
#include "Settings.h"

#include "files.h"
#include "Loader.h"
#include "languages.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

const unsigned int Settings::SETTINGS_VERSION = 9;
const unsigned int Settings::SETTINGS_SECTIONS = 8;
const std::string Settings::SETTINGS_SECTION_NAMES[] = {
	"global", "video", "language", "driver", "sound", "lobby", "server", "savegames"
};

///////////////////////////////////////////////////////////////////////////////
// Konstruktor
Settings::Settings(void)
{
}

bool Settings::LoadDefaults()
{
	// global
	// {
	// }

	// video
	// {
		video.width       = 800;
		video.height      = 600;
		video.fullscreen  = false;
		video.vsync       = false;
		video.vbo         = false;
	// }

	// language
	// {
		language.language = "";
	// }

	LANGUAGES.setLanguage(language.language);

	// driver
	// {
		driver.audio = "";
		driver.video = "";
	// }

	// sound
	// {
		sound.musik          = false;
		sound.musik_volume   = 30;
		sound.effekte        = true;
		sound.effekte_volume = 75;
		sound.playlist       = "S2_Standard";
	// }

	// lobby
	// {
		char tmp_name[256];
	#ifdef _WIN32
		DWORD size = 256;
		GetUserNameA(tmp_name, &size);
	#else
		strncpy(tmp_name, getenv("USER"), 256);
	#endif // !_WIN32

		lobby.name = tmp_name;
		lobby.password = "";
		lobby.email = "";
		lobby.save_password = false;
	// }

	// server
	// {
		server.last_ip = "";
	// }

	// savegames
	// {
		savegames.autosave_interval = 0;
	// }

	Save();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Routine zum Laden der Konfiguration
bool Settings::Load(void)
{
	if(!LOADER.LoadSettings() && LOADER.GetInfoN(CONFIG_NAME)->getCount() < 8)
	{
		warning("No or corrupt \"%s\" found, using default values.\n", GetFilePath(FILE_PATHS[0]).c_str());
		return LoadDefaults();
	}

	const libsiedler2::ArchivItem_Ini *global = LOADER.GetSettingsIniN("global");
	const libsiedler2::ArchivItem_Ini *video = LOADER.GetSettingsIniN("video");
	const libsiedler2::ArchivItem_Ini *language = LOADER.GetSettingsIniN("language");
	const libsiedler2::ArchivItem_Ini *driver = LOADER.GetSettingsIniN("driver");
	const libsiedler2::ArchivItem_Ini *sound = LOADER.GetSettingsIniN("sound");
	const libsiedler2::ArchivItem_Ini *lobby = LOADER.GetSettingsIniN("lobby");
	const libsiedler2::ArchivItem_Ini *server = LOADER.GetSettingsIniN("server");
	const libsiedler2::ArchivItem_Ini *savegames = LOADER.GetSettingsIniN("savegames");

	// ist eine der Kategorien nicht vorhanden?
	if(!global || !video || !language || !driver || !sound || !lobby || !server || !savegames ||
		// stimmt die Settingsversion?
		((unsigned int)global->getValueI("version") != SETTINGS_VERSION)
	  )
	{
		// nein, dann Standardeinstellungen laden
		warning("\"%s\" found, but its corrupted or has wrong version. Loading default values.\n", GetFilePath(FILE_PATHS[0]).c_str());
		return LoadDefaults();
	}

	// global
	// {
		// stimmt die Spielrevision überein?
		if(strcmp(global->getValue("gameversion"), GetWindowRevision()) != 0)
			warning("Your Applicationversion has changed - please recheck your settings!\n");
	// };

	// video
	// {
		this->video.width =       video->getValueI("width");
		this->video.height =      video->getValueI("height");
		this->video.fullscreen = (video->getValueI("fullscreen") ? true : false);
		this->video.vsync =      (video->getValueI("vsync") ? true : false);
		this->video.vbo =        (video->getValueI("vbo") ? true : false);
	// };

	if(this->video.width == 0 || this->video.height == 0)
	{
		warning("Corrupted \"%s\" found, reverting to default values.\n", GetFilePath(FILE_PATHS[0]).c_str());
		return LoadDefaults();
	}

	// language
	// {
		this->language.language = language->getValue("language");
	// }

	LANGUAGES.setLanguage(this->language.language);

	// driver
	// {
		this->driver.video = driver->getValue("video");
		this->driver.audio = driver->getValue("audio");
	// }

	// sound
	// {
		this->sound.musik =         (sound->getValueI("musik") ? true : false);
		this->sound.musik_volume =   sound->getValueI("musik_volume");
		this->sound.effekte =       (sound->getValueI("effekte") ? true : false);
		this->sound.effekte_volume = sound->getValueI("effekte_volume");
		this->sound.playlist =       sound->getValue("playlist");
	// }

	// lobby
	// {
		this->lobby.name =           lobby->getValue("name");
		this->lobby.email =          lobby->getValue("email");
		this->lobby.password =       lobby->getValue("password");
		this->lobby.save_password = (lobby->getValueI("save_password") ? true : false);
	// }

	if(this->lobby.name.length() == 0)
	{
		char tmp_name[256];
#ifdef _WIN32
		DWORD size = 256;
		GetUserNameA(tmp_name, &size);
#else
		strncpy(tmp_name, getenv("USER"), 256);
#endif // !_WIN32

		this->lobby.name = tmp_name;
	}

	// server
	// {
		this->server.last_ip = server->getValue("last_ip");
	// }

	// savegames
	// {
		this->savegames.autosave_interval = server->getValueI("autosave_interval");
	// }

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Routine zum Speichern der Konfiguration
void Settings::Save(void)
{
	if(LOADER.GetInfoN(CONFIG_NAME)->getCount() != SETTINGS_SECTIONS)
	{
		libsiedler2::ArchivItem_Ini item;
		LOADER.GetInfoN(CONFIG_NAME)->alloc(SETTINGS_SECTIONS);
		for(unsigned int i = 0; i < SETTINGS_SECTIONS; ++i)
		{
			item.setName(SETTINGS_SECTION_NAMES[i].c_str());
			LOADER.GetInfoN(CONFIG_NAME)->setC(i, &item);
		}
	}

	libsiedler2::ArchivItem_Ini *global = LOADER.GetSettingsIniN("global");
	libsiedler2::ArchivItem_Ini *video = LOADER.GetSettingsIniN("video");
	libsiedler2::ArchivItem_Ini *language = LOADER.GetSettingsIniN("language");
	libsiedler2::ArchivItem_Ini *driver = LOADER.GetSettingsIniN("driver");
	libsiedler2::ArchivItem_Ini *sound = LOADER.GetSettingsIniN("sound");
	libsiedler2::ArchivItem_Ini *lobby = LOADER.GetSettingsIniN("lobby");
	libsiedler2::ArchivItem_Ini *server = LOADER.GetSettingsIniN("server");
	libsiedler2::ArchivItem_Ini *savegames = LOADER.GetSettingsIniN("savegames");

	// ist eine der Kategorien nicht vorhanden?
	assert(global && video && language && driver && sound && lobby && server && savegames);

	// global
	// {
		global->setValue("version", SETTINGS_VERSION);
		global->setValue("gameversion", GetWindowRevision());
	// };

	// video
	// {
		video->setValue("width", this->video.width);
		video->setValue("height", this->video.height);
		video->setValue("fullscreen", (this->video.fullscreen ? 1 : 0) );
		video->setValue("vsync", (this->video.vsync ? 1 : 0) );
		video->setValue("vbo", (this->video.vbo ? 1 : 0) );
	// };

	// language
	// {
		language->setValue("language", this->language.language.c_str());
	// }

	// driver
	// {
		driver->setValue("video", this->driver.video.c_str());
		driver->setValue("audio", this->driver.audio.c_str());
	// }

	// sound
	// {
		sound->setValue("musik", (this->sound.musik ? 1 : 0) );
		sound->setValue("musik_volume", this->sound.musik_volume);
		sound->setValue("effekte", (this->sound.effekte ? 1 : 0) );
		sound->setValue("effekte_volume", this->sound.effekte_volume);
		sound->setValue("playlist", this->sound.playlist);
	// }

	// lobby
	// {
		lobby->setValue("name", this->lobby.name.c_str());
		lobby->setValue("email", this->lobby.email.c_str());
		lobby->setValue("password", this->lobby.password.c_str());
		lobby->setValue("save_password", (this->lobby.save_password ? 1 : 0));
	// }

	// server
	// {
		server->setValue("last_ip", this->server.last_ip.c_str());
	// }

	// savegames
	// {
		server->setValue("autosave_interval", this->savegames.autosave_interval);
	// }

	LOADER.SaveSettings();
}
