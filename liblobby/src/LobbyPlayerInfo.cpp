// $Id: LobbyPlayerInfo.cpp 6040 2010-02-17 20:12:22Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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
 *  Deserialisierungs-Konstruktor von @p LobbyPlayerInfo.
 *
 *  @author FloSoft
 */
LobbyPlayerInfo::LobbyPlayerInfo(const unsigned playerid, Serializer *ser)
{
	deserialize(ser);
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
 *  @param[in] ser der benutzte Serializer
 *
 *  @author FloSoft
 */
void LobbyPlayerInfo::serialize(Serializer *ser) const
{
	ser->PushUnsignedInt(playerid);
	ser->PushString(name);
	ser->PushString(version);
	ser->PushSignedInt(punkte);
	ser->PushUnsignedInt(gewonnen);
	ser->PushUnsignedInt(verloren);
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  deserialisiert die Daten.
 *
 *  @param[in] ser der benutzte Serializer
 *
 *  @author FloSoft
 */
void LobbyPlayerInfo::deserialize(Serializer *ser)
{
	playerid = ser->PopUnsignedInt();
	name = ser->PopString();
	version = ser->PopString();
	punkte = ser->PopSignedInt();
	gewonnen = ser->PopUnsignedInt();
	verloren = ser->PopUnsignedInt();
}
