// $Id: GameManager.cpp 5259 2009-07-13 15:53:31Z FloSoft $
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
#include "GameManager.h"

#include "GlobalVars.h"
#include "Settings.h"

#include "SoundManager.h"
#include "WindowManager.h"
#include "VideoDriverWrapper.h"
#include "AudioDriverWrapper.h"

#include "LobbyClient.h"
#include "GameServer.h"
#include "GameClient.h"

#include "dskSplash.h"
#include "dskMainMenu.h"
#include "dskLobby.h"
#include "iwMusicPlayer.h"

#include "MusicPlayer.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @pGameManager.
 *
 *  @author OLiver
 */
GameManager::GameManager(void)
{
	frames = 0;
	framerate = 0;
	frame_time = 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Spiel starten
 *
 *  @author FloSoft
 */
bool GameManager::Start()
{
	// Einstellungen laden
	if(!SETTINGS.Load())
		return false;

	/// Videotreiber laden
	if(!VideoDriverWrapper::inst().LoadDriver())
	{
		LOG.lprintf("Video driver couldn't be loaded!\n");
		return false;
	}

	// Im Vollbildmodus überprüfen, ob Video-Mode überhaupt existiert
	if(SETTINGS.video.fullscreen)
	{
		std::vector<VideoDriver::VideoMode> available_video_modes;
		VideoDriverWrapper::inst().ListVideoModes(available_video_modes);

		bool found = false;
		for(size_t i = 0;i<available_video_modes.size();++i)
		{
			if(available_video_modes[i].width == SETTINGS.video.width &&
				available_video_modes[i].height == SETTINGS.video.height)
				found = true;
		}

		if(!found && available_video_modes.size())
		{
			// Nicht gefunden, erste gültige Auflösung nehmen
			SETTINGS.video.width = available_video_modes[0].width;
			SETTINGS.video.height = available_video_modes[0].height;
		}
	}

	// Fenster erstellen
	if(!VideoDriverWrapper::inst().CreateScreen(SETTINGS.video.width,SETTINGS.video.height,SETTINGS.video.fullscreen))
		return false;

	/// Audiodriver laden
	if(!AudioDriverWrapper::inst().LoadDriver())
	{
		LOG.lprintf("Audio driver couldn't be loaded!\n");
		//return false;
	}

	/// Lautstärken gleich mit setzen
	AudioDriverWrapper::inst().SetMasterEffectVolume(SETTINGS.sound.effekte_volume);
	AudioDriverWrapper::inst().SetMasterMusicVolume(SETTINGS.sound.musik_volume);

	// Treibereinstellungen abspeichern
	SETTINGS.Save();

	LOG.lprintf("\nStarte das Spiel\n");
	if(!StartMenu())
		return false;

	MusicPlayer::inst().Load(iwMusicPlayer::GetFullPlaylistPath(SETTINGS.sound.playlist));
	MusicPlayer::inst().StartPlaying();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Spiel beenden.
 *
 *  @author FloSoft
 */
void GameManager::Stop()
{
	// Global Einstellungen speichern
	SETTINGS.Save();

	// Fenster beenden
	VideoDriverWrapper::inst().DestroyScreen();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Hauptschleife.
 *
 *  @author FloSoft
 */
bool GameManager::Run()
{
	// Nachrichtenschleife
	if(!VideoDriverWrapper::inst().Run())
		GLOBALVARS.notdone = false;

	LOBBYCLIENT.Run();

	GAMECLIENT.Run();
	GAMESERVER.Run();

	WindowManager::inst().Draw();

	if(!GLOBALVARS.ingame)
	{
		// Mauszeiger zeichnen
		if(VideoDriverWrapper::inst().IsLeftDown())
			LOADER.GetImageN("resource", 31)->Draw(VideoDriverWrapper::inst().GetMouseX(), VideoDriverWrapper::inst().GetMouseY(), 0, 0, 0, 0, 0, 0);
		else
			LOADER.GetImageN("resource", 30)->Draw(VideoDriverWrapper::inst().GetMouseX(), VideoDriverWrapper::inst().GetMouseY(), 0, 0, 0, 0, 0, 0);
	}

	// Framerate berechnen
	if(VideoDriverWrapper::inst().GetTickCount() - frame_time > 1000)
	{
		framerate = frames;
		frames = 0;
		frame_time = VideoDriverWrapper::inst().GetTickCount();
	}

	// und zeichnen
	char frame_str[64];
	sprintf(frame_str, "%d fps", framerate);

	SmallFont->Draw( VideoDriverWrapper::inst().GetScreenWidth(), 0, frame_str, glArchivItem_Font::DF_RIGHT, COLOR_YELLOW);

	// Zeichenpuffer wechseln
	VideoDriverWrapper::inst().SwapBuffers();

	++frames;

	// Fenstermanager aufräumen
	if(GLOBALVARS.notdone == false)
	{
		WindowManager::inst().CleanUp();
	}

	return GLOBALVARS.notdone;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Startet und lädt das Menü.
 *
 *  @author FloSoft
 */
bool GameManager::StartMenu()
{
	// generelle Daten laden
	if(!LOADER.LoadFilesAtStart())
	{
		error("Einige Dateien konnten nicht geladen werden.\n"
			"Stellen Sie sicher, dass die Siedler 2 Gold-Edition im gleichen \n"
			"Verzeichnis wie Return to the Roots installiert ist.");

		error("Einige Dateien konnten nicht geladen werden.\n"
			"Please ensure that the Settlers 2 Gold-Edition is installed \n"
			"in the same directory as Return to the Roots.");

		return false;
	}

	// Splash-Screen anzeigen
	WindowManager::inst().Switch(new dskSplash);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  zeigt das Hauptmenü.
 *
 *  @author FloSoft
 */
bool GameManager::ShowMenu()
{
	GAMECLIENT.Stop();
	GAMESERVER.Stop();
	SOUNDMANAGER.StopAll();

	GameClient::inst().SetInterface(NULL);

	// Wir sind nicht mehr im Spiel
	GLOBALVARS.ingame = false;

	if(LOBBYCLIENT.LoggedIn())
		// Lobby zeigen
		WindowManager::inst().Switch(new dskLobby);
	else
		// Hauptmenü zeigen
		WindowManager::inst().Switch(new dskMainMenu);

	return true;
}
