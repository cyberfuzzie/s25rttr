// $Id: LobbyServerInfo.cpp 4878 2009-05-17 11:40:50Z OLiver $
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
#include "LobbyServerInfo.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
  #define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/*
 *  Konstruktor von @p LobbyServerInfo.
 *
 *  @author FloSoft
 */
LobbyServerInfo::LobbyServerInfo(void)
{
	clear();
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  leert die Informationen.
 *
 *  @author FloSoft
 */
void LobbyServerInfo::clear(void)
{
	serverid = 0;
	name = "";
	host = "";
	version = "";
	ping = 0;
	map = "";
	curplayers = 0;
	maxplayers = 0;
	has_password = false;
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  serialisiert die Daten.
 *
 *  @param[in,out] data Datensatz, muss groß genug sein
 *
 *  @return liefert die Größe der Daten zurück.
 *
 *  @author FloSoft
 */
void LobbyServerInfo::serialize(Message * msg) const
{
	msg->pushUI(serverid);
	msg->pushStr(name);
	msg->pushStr(host);
	msg->pushUS(port);
	msg->pushStr(version);
	msg->pushUI(ping);
	msg->pushStr(map);
	msg->pushUI(curplayers);
	msg->pushUI(maxplayers);
	msg->pushB(has_password);
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  deserialisiert die Daten.
 *
 *  @param[in] data Datensatz, muss groß genug sein
 *
 *  @return liefert die Größe der gelesenen Daten zurück.
 *
 *  @author FloSoft
 */
void LobbyServerInfo::deserialize(Message * msg)
{
	serverid = msg->popUI();
	name = msg->popStr();
	host = msg->popStr();
	port = msg->popUS();
	version = msg->popStr();
	ping = msg->popUI();
	map = msg->popStr();
	curplayers = msg->popUI();
	maxplayers = msg->popUI();
	has_password = msg->popB();
}
