// $Id: Settings.cpp 5171 2009-07-02 20:21:42Z FloSoft $
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

const char *Settings::SETTINGS_VERSION = "7";

///////////////////////////////////////////////////////////////////////////////
// Konstruktor
Settings::Settings(void)
{
}

bool Settings::LoadDefaults()
{
	width		= 800;
	height		= 600;

	language = "";

	fullscreen	= false;
	vsync		= false;
	vbo         = false;

	password = "";
	email = "";
	save_password = false;

	char tmp_name[256];

#ifdef _WIN32
	DWORD size = 256;
	GetUserNameA(tmp_name, &size);
#else
	strncpy(tmp_name, getenv("USER"), 256);
#endif // !_WIN32

	name = tmp_name;

	audio_driver = "";
	video_driver = "";

	musik = false;
	musik_volume = 30;
	effekte = true;
	effekte_volume = 75;

	char dir[512];
	Loader::GetFilePath(dir, SETTINGSDIR);

	#ifdef _WIN32
		CreateDirectory(dir, NULL);
	#else
		mkdir(dir, 0750);
	#endif

	autosave_interval = 0;

	last_ip = "";

	playlist = "S2_Standard";

	Save();

	LANGUAGES.setLanguage(language);

	// enhs loads the default in the constructor
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Routine zum Laden der Konfiguration
bool Settings::Load(void)
{
	if(!LOADER.LoadSettings() && LOADER.settings.getCount() < 21)
	{
		warning("No or corrupt \"%s\" found, using default values.\n", FILE_PATHS[0]);
		return LoadDefaults();
	}

	std::string version = GetTxt(settings, 0);
	if(version != SETTINGS_VERSION)
	{
		warning("\"%s\" found, but wrong version: \"%s\" != \"%s\", using default values.\n", FILE_PATHS[0], version.c_str(), SETTINGS_VERSION);
		return LoadDefaults();
	}

	std::string gameversion = GetTxt(settings, 1);
	if(gameversion != GetWindowRevision())
		warning("Your Applicationversion has changed - please recheck your settings!\n");


	width = atoi(GetTxt(settings, 2));
	height = atoi(GetTxt(settings, 3));

	if(width == 0 || height == 0)
	{
		warning("Corrupt \"%s\" found, reverting to default values.\n", FILE_PATHS[0]);
		return LoadDefaults();
	}

	language = GetTxt(settings, 4) ? GetTxt(settings, 4)  : "";

	fullscreen = (atoi(GetTxt(settings, 5)) ? true : false);
	vsync = (atoi(GetTxt(settings, 6)) ? true : false);
	vbo = (atoi(GetTxt(settings, 7)) ? true : false);

	if(GetTxtItem(settings, 8)->getLength() > 0)
		name = GetTxt(settings, 8);
	if(GetTxtItem(settings, 9)->getLength() > 0)
		password = GetTxt(settings, 9);
	save_password = (password.size() > 0);

	if(GetTxtItem(settings, 10)->getLength() > 0)
		email = GetTxt(settings, 10);

	if(GetTxtItem(settings, 11)->getLength() > 0)
		audio_driver = GetTxt(settings, 11);
	if(GetTxtItem(settings, 12)->getLength() > 0)
		video_driver = GetTxt(settings, 12);

	musik = (atoi(GetTxt(settings, 13)) ? true : false);
	musik_volume = atoi(GetTxt(settings, 14));
	effekte = (atoi(GetTxt(settings, 15)) ? true : false);
	effekte_volume = atoi(GetTxt(settings, 16));
	autosave_interval = atoi(GetTxt(settings, 17));
	if(GetTxtItem(settings, 18)->getLength() > 0)
		last_ip = GetTxt(settings, 18);
	if(GetTxtItem(settings, 19)->getLength() > 0)
		playlist = GetTxt(settings, 19);

	if(name == "")
	{
		char tmp_name[256];
#ifdef _WIN32
		DWORD size = 256;
		GetUserNameA(tmp_name, &size);
#else
		strncpy(tmp_name, getenv("USER"), 256);
#endif // !_WIN32

		name = tmp_name;
	}

	LANGUAGES.setLanguage(language);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Routine zum Speichern der Konfiguration
void Settings::Save(void)
{
	if(LOADER.settings.getCount() < 21)
	{
		LOADER.settings.clear();
		LOADER.settings.alloc(21);
		libsiedler2::ArchivItem_Text text;
		for(unsigned int i = 0; i < 21; ++i)
			LOADER.settings.setC(i, &text);
	}

	GetTxtItem(settings, 0)->setText(SETTINGS_VERSION);
	GetTxtItem(settings, 1)->setText(GetWindowRevision());

	char text[256];

	snprintf(text, 256, "%d", width);
	GetTxtItem(settings, 2)->setText(text);

	snprintf(text, 256, "%d", height);
	GetTxtItem(settings, 3)->setText(text);

	GetTxtItem(settings, 4)->setText(language.c_str());

	snprintf(text, 256, "%d", fullscreen);
	GetTxtItem(settings, 5)->setText(text);
	snprintf(text, 256, "%d", vsync);
	GetTxtItem(settings, 6)->setText(text);
	snprintf(text, 256, "%d", vbo);
	GetTxtItem(settings, 7)->setText(text);

	GetTxtItem(settings, 8)->setText(name.c_str());
	GetTxtItem(settings, 9)->setText(password.c_str());
	GetTxtItem(settings, 10)->setText(email.c_str());

	GetTxtItem(settings, 11)->setText(audio_driver.c_str());
	GetTxtItem(settings, 12)->setText(video_driver.c_str());

	snprintf(text, 256, "%d", musik);
	GetTxtItem(settings, 13)->setText(text);
	snprintf(text, 256, "%d", musik_volume);
	GetTxtItem(settings, 14)->setText(text);
	snprintf(text, 256, "%d", effekte);
	GetTxtItem(settings, 15)->setText(text);
	snprintf(text, 256, "%d", effekte_volume);
	GetTxtItem(settings, 16)->setText(text);
	snprintf(text, 256, "%d", effekte_volume);
	GetTxtItem(settings, 16)->setText(text);
	snprintf(text, 256, "%u", autosave_interval);
	GetTxtItem(settings, 17)->setText(text);
	GetTxtItem(settings, 18)->setText(last_ip.c_str());
	GetTxtItem(settings, 19)->setText(playlist.c_str());

	LOADER.SaveSettings();
}
