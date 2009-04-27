// $Id: LobbyPlayerInfo.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
int LobbyPlayerInfo::serialize(char *data) const
{
	unsigned int n = name.length()+1;
	unsigned int v = version.length()+1;

	if(data)
	{
		memcpy(&data[0], &playerid, 4);

		strncpy(&data[4], name.c_str(), name.length());
		strncpy(&data[4+n], version.c_str(), version.length());

		memcpy(&data[4+n+v], &punkte, 4);
		memcpy(&data[8+n+v], &gewonnen, 4);
		memcpy(&data[12+n+v], &verloren, 4);
	}

	return 16+n+v;
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
int LobbyPlayerInfo::deserialize(const char *data)
{
	if(!data)
		return 0;

	memcpy(&playerid, &data[0], 4);

	name = &data[4];
	unsigned int n = name.length()+1;

	version = &data[4+n];
	unsigned int v = version.length()+1;

	memcpy(&punkte, &data[4+n+v], 4);
	memcpy(&gewonnen, &data[8+n+v], 4);
	memcpy(&verloren, &data[12+n+v], 4);

	return 16+n+v;
}
