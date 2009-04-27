// $Id: GlobalGameSettings.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef GGS_H_
#define GGS_H_

#include "Singleton.h"
#include "GameConsts.h"
#include "GameProtocol.h"

class GlobalGameSettings
{
public:

	// TODO: letzte Settings aus der settings.bin möglichst laden
	GlobalGameSettings() : game_speed(GS_FAST), game_objective(GO_NONE), start_wares(SWR_NORMAL), 
		lock_teams(true), exploration(EXP_FOGOFWAR), team_view(true), demolition_prohibition(DP_NEARFRONTIERS) {}


	enum GameSpeed { GS_VERYSLOW = 0,GS_SLOW , GS_NORMAL, GS_FAST, GS_VERYFAST } game_speed;
	enum GameObjective { GO_NONE = 0, GO_TOTALDOMINATION, GO_CONQUER3_4 } game_objective;
	enum StartWares { SWR_LOW = 0, SWR_NORMAL, SWR_ALOT } start_wares;
	bool lock_teams;
	enum Exploration { EXP_DISABLED = 0, EXP_CLASSIC, EXP_FOGOFWAR, EXP_FOGOFWARE_EXPLORED } exploration;
	bool team_view;
	/// Abriss bei Angriff erlaubt?
	enum DemolitionProhibition { DP_OFF = 0, DP_UNDERATTACK, DP_NEARFRONTIERS } demolition_prohibition;
	
	/// Größe des Serialisierungsbuffers
	static const unsigned GGS_BUFFER_SIZE = 7;

	/// Serialisierung und Deserialisierung
	unsigned char * Serialize() const;
	void Deserialize(const unsigned char * buffer);
};

#define GGS GlobalGameSettings::inst()

#endif
