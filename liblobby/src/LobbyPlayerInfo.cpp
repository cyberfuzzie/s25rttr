// $Id: LobbyPlayerInfo.cpp 4903 2009-05-21 12:04:21Z OLiver $
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
#include "LobbyPlayerInfo.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/*
 *  Konstruktor von @p LobbyPlayerInfo.
 *
 *  @author FloSoft
 */
LobbyPlayerInfo::LobbyPlayerInfo(void)
{
	clear();
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  leert die Informationen.
 *
 *  @author FloSoft
 */
void LobbyPlayerInfo::clear(void)
{
	playerid = 0;
	name = "";
	version = "";
	punkte = 0;
	gewonnen = 0;
	verloren = 0;
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
void LobbyPlayerInfo::serialize(Message * msg) const
{
	msg->PushUnsignedInt(playerid);
	msg->PushString(name);
	msg->PushString(version);
	msg->PushSignedInt(punkte);
	msg->PushUnsignedInt(gewonnen);
	msg->PushUnsignedInt(verloren);
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
void LobbyPlayerInfo::deserialize(Message * msg)
{
	playerid = msg->PopUnsignedInt();
	name = msg->PopString();
	version = msg->PopString();
	punkte = msg->PopSignedInt();
	gewonnen = msg->PopUnsignedInt();
	verloren = msg->PopUnsignedInt();
}
