// $Id: iwLobbyServerInfo.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "iwLobbyServerInfo.h"

#include "WindowManager.h"
#include "Loader.h"
#include "controls.h"
#include "LobbyClient.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  aktualisiert das Serverinfo.
 *
 *  @author Devil
 */
void iwLobbyServerInfo::UpdateServerInfo()
{
	static char host[512];

	if(LOBBYCLIENT.refreshserverinfo == true)
	{
		LOBBYCLIENT.refreshserverinfo = false;

		serverinfo = LOBBYCLIENT.GetServerInfo();

		SetTitle(serverinfo->getName());

		GetCtrl<ctrlEdit>(1)->SetText(serverinfo->getMap());
		GetCtrl<ctrlEdit>(4)->SetText(serverinfo->getName());

		snprintf(host, 512, "%s:%d", serverinfo->getHost().c_str(), serverinfo->getPort());
		GetCtrl<ctrlEdit>(6)->SetText(host);

		GetCtrl<ctrlEdit>(8)->SetText(serverinfo->getVersion());
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p iwLobbyServerInfo.
 *
 *  @author Devil
 */
iwLobbyServerInfo::iwLobbyServerInfo(void)
	: IngameWindow(CGI_LOBBYSERVERINFO, 0xFFFF, 0xFFFF, 260, 260, _("Server Information"), GetImage(resource_dat, 41))
{
	AddText(0, 30,  40, _("Map Name:"), COLOR_YELLOW, 0, NormalFont);
	AddEdit(1, 30,  60, 200, 22, TC_GREEN2, NormalFont, 0, false, true);
	AddText(3, 30,  90, _("Server Name:"), COLOR_YELLOW, 0, NormalFont);
	AddEdit(4, 30, 110, 200, 22, TC_GREEN2, NormalFont, 0, false, true);
	AddText(5, 30, 140, _("Host:"), COLOR_YELLOW, 0, NormalFont);
	AddEdit(6, 30, 160, 200, 22, TC_GREEN2, NormalFont, 0, false, true);
	AddText(7, 30, 190, _("Version:"), COLOR_YELLOW, 0, NormalFont);
	AddEdit(8, 30, 210, 200, 22, TC_GREEN2, NormalFont, 0, false, true);

	AddTimer(9, 5000);
	AddTimer(10, 1000);
}


void iwLobbyServerInfo::Msg_Timer(const unsigned int ctrl_id)
{
	switch(ctrl_id)
	{
	case 9: // alle 5 Sek
		{
			LOBBYCLIENT.SendServerInfoRequest(server);
		} break;
	case 10: // alle Sek
		{
			UpdateServerInfo();
		} break;
	}
}


///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt Serverinfo und ServerNummer.
 *
 *  @author Devil
 */
void iwLobbyServerInfo::Set(const LobbyServerInfo *serverinfo, unsigned int server)
{
	this->serverinfo = serverinfo;
	this->server = server;
}
