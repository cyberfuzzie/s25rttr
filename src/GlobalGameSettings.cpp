// $Id: GlobalGameSettings.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "GlobalGameSettings.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

unsigned char * GlobalGameSettings::Serialize() const
{
	unsigned char * buffer = new unsigned char[GGS_BUFFER_SIZE];

	buffer[0] = static_cast<unsigned char>(game_speed);
	buffer[1] = static_cast<unsigned char>(game_objective);
	buffer[2] = static_cast<unsigned char>(start_wares);
	buffer[3] = lock_teams ? 1 : 0;
	buffer[4] = static_cast<unsigned char>(exploration);
	buffer[5] = team_view ? 1 : 0;
	buffer[6] = static_cast<unsigned char>(demolition_prohibition);

	return buffer;
}

void GlobalGameSettings::Deserialize(const unsigned char * buffer)
{
	game_speed = static_cast<GameSpeed>(buffer[0]);
	game_objective = static_cast<GameObjective>(buffer[1]);
	start_wares = static_cast<StartWares>(buffer[2]);
	lock_teams = (buffer[3] == 1) ? true : false;
	exploration = static_cast<Exploration>(buffer[4]);
	team_view = (buffer[5] == 1) ? true : false;
	demolition_prohibition = static_cast<DemolitionProhibition>(buffer[6]);
}
