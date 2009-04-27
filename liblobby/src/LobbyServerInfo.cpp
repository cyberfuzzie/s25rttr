// $Id: LobbyServerInfo.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
int LobbyServerInfo::serialize(char *data) const
{
	unsigned int n = name.length()+1;
	unsigned int h = host.length()+1;
	unsigned int v = version.length()+1;
	unsigned int m = map.length()+1;
	if(data)
	{
		memcpy(&data[0], &serverid, 4);

		strncpy(&data[4], name.c_str(), name.length());
		strncpy(&data[4+n], host.c_str(), host.length());

		memcpy(&data[4+n+h], &port, 2);

		strncpy(&data[6+n+h], version.c_str(), version.length());

		memcpy(&data[6+n+h+v], &ping, 4);

		strncpy(&data[10+n+h+v], map.c_str(), map.length());

		memcpy(&data[10+n+h+v+m], &curplayers, 4);
		memcpy(&data[14+n+h+v+m], &maxplayers, 4);
		memcpy(&data[18+n+h+v+m], &has_password, 4);
	}

	return 22+n+h+v+m;
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
int LobbyServerInfo::deserialize(const char *data)
{
	if(!data)
		return 0;

	memcpy(&serverid, &data[0], 4);

	name = &data[4];
	unsigned int n = name.length()+1;

	host = &data[4+n];
	unsigned int h = host.length()+1;

	memcpy(&port, &data[4+n+h], 2);

	version = &data[6+n+h];
	unsigned int v = version.length()+1;

	memcpy(&ping, &data[6+n+h+v], 4);

	map = &data[10+n+h+v];
	unsigned int m = map.length()+1;

	memcpy(&curplayers, &data[10+n+h+v+m], 4);
	memcpy(&maxplayers, &data[14+n+h+v+m], 4);
	memcpy(&has_password, &data[18+n+h+v+m], 4);

	return 22+n+h+v+m;
}
