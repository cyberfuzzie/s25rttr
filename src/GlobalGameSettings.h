// $Id: GlobalGameSettings.h 6458 2010-05-31 11:38:51Z FloSoft $
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
#ifndef GLOBALGAMESETTINGS_H_INCLUDED
#define GLOBALGAMESETTINGS_H_INCLUDED

#pragma once

class Serializer;

class GlobalGameSettings
{
public:
	GlobalGameSettings() : game_speed(GS_FAST), game_objective(GO_NONE), start_wares(SWR_NORMAL), lock_teams(true), exploration(EXP_FOGOFWAR), team_view(true) {}

	/// Serialisierung und Deserialisierung
	void Serialize(Serializer * ser) const;
	void Deserialize(Serializer * ser);

public:
	enum GameSpeed { GS_VERYSLOW = 0,GS_SLOW , GS_NORMAL, GS_FAST, GS_VERYFAST } game_speed;
	enum GameObjective { GO_NONE = 0, GO_TOTALDOMINATION, GO_CONQUER3_4 } game_objective;
	enum StartWares { SWR_LOW = 0, SWR_NORMAL, SWR_ALOT } start_wares;
	bool lock_teams;
	enum Exploration { EXP_DISABLED = 0, EXP_CLASSIC, EXP_FOGOFWAR, EXP_FOGOFWARE_EXPLORED } exploration;
	bool team_view;
};

#endif // !GLOBALGAMESETTINGS_H_INCLUDED
