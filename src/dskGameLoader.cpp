// $Id: dskGameLoader.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "dskGameLoader.h"

#include "Loader.h"
#include "controls.h"
#include "WindowManager.h"
#include "VideoDriverWrapper.h"
#include "GameClient.h"
#include "LobbyClient.h"

#include "dskGameInterface.h"
#include "dskMainMenu.h"
#include "dskLobby.h"
#include "dskDirectIP.h"

#include "iwMsgbox.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p dskGameLoader.
 *  Startet das Spiel und l�dt alles Notwendige.
 *
 *  @author FloSoft
 */
dskGameLoader::dskGameLoader(GameWorldViewer * gwv)
	: Desktop(GetImage(pics, rand() % 20)), 
	position(0), gwv(gwv)
{
	AddTimer(1, 50);

	AddText(10, VideoDriverWrapper::inst().GetScreenWidth() / 2, VideoDriverWrapper::inst().GetScreenHeight() - 50, "", COLOR_YELLOW, glArchivItem_Font::DF_CENTER, LargeFont);

	for(unsigned int i = 0; i < 8; ++i)
		AddText(10+i, 30, 30 + i*20, "", COLOR_GREEN, 0, LargeFont);

	LOBBYCLIENT.SetInterface(this);
	GAMECLIENT.SetInterface(this);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
void dskGameLoader::Msg_MsgBoxResult(const unsigned int msgbox_id, const MsgboxResult mbr)
{
	if(msgbox_id == 0) // Verbindung zu Server verloren?
	{
		GAMECLIENT.Stop();

		if(LOBBYCLIENT.LoggedIn()) // steht die Lobbyverbindung noch?
			WindowManager::inst().Switch(new dskLobby);
		else
			WindowManager::inst().Switch(new dskDirectIP);
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author OLiver
 */
void dskGameLoader::Msg_PaintBefore()
{
	DrawRectangle(0, 0, VideoDriverWrapper::inst().GetScreenWidth(), VideoDriverWrapper::inst().GetScreenHeight(), COLOR_BLACK);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  
 *
 *  @author FloSoft
 */
void dskGameLoader::Msg_Timer(const unsigned int ctrl_id)
{
	static bool load_nations[Loader::NATION_COUNT];

	ctrlTimer *timer = GetCtrl<ctrlTimer>(1);
	ctrlText *text = GetCtrl<ctrlText>(10+position);
	int interval = 500;

	timer->Stop();

	switch(position)
	{
	case 0: // Kartename anzeigen
		{
			text->SetText(GameClient::inst().GetMapTitle());
			interval = 50;
		} break;

	case 1: // Karte geladen
		{
			text->SetText(_("Map was loaded and pinned at the wall..."));
			interval = 50;
		} break;

	case 2: // Nationen ermitteln
		{
			memset(load_nations, 0, sizeof(bool) * Loader::NATION_COUNT);
			for(unsigned char i = 0; i < GAMECLIENT.GetPlayerCount(); ++i)
				load_nations[GAMECLIENT.GetPlayer(i)->nation] = true;

			text->SetText(_("Tribal chiefs assembled around the table..."));
			interval = 50;
		} break;

	case 3: // Objekte laden
		{
			if(!LOADER.LoadGame(gwv->GetLandscapeType(), load_nations))
			{
				LC_Status_Error(_("Failed to load map objects."));
				return;
			}

			text->SetText(_("Game crate was picked and spread out..."));
			interval = 50;
		} break;

	case 4: // Welt erstellen
		{
			if(!LOADER.LoadTerrain(gwv->GetLandscapeType()))
			{
				LC_Status_Error(_("Failed to load terrain data."));
				return;
			}

			// TODO: richtige Messages senden, um das zu laden /*GetMap()->GenerateOpenGL();*/

			text->SetText(_("World was put together and glued..."));
			interval = 50;
		} break;

	case 5: // nochmal text anzeigen
		{
			text->SetText(_("And let's go!"));
			text->SetColor(COLOR_RED);
			interval = 50;
		} break;

	case 6: // zum Spiel wechseln
		{
			WindowManager::inst().Switch(new dskGameInterface);
			return;
		} break;
	}

	++position;
	timer->Start(interval);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  (Lobby-)Status: Benutzerdefinierter Fehler (kann auch Conn-Loss o.� sein)
 *
 *  @author FloSoft
 */
void dskGameLoader::LC_Status_Error(const std::string &error)
{
	WindowManager::inst().Show(new iwMsgbox(_("Error"), error, this, MSB_OK, MSB_EXCLAMATIONRED, 0));
}
