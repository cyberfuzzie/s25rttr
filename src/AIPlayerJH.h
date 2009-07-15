// $Id: AIPlayerJH.h 5273 2009-07-15 20:51:49Z jh $
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
#ifndef AIPLAYERJH_H_INCLUDED
#define AIPLAYERJH_H_INCLUDED

#pragma once

#include "AIBase.h"

class noFlag;
class noBaseBuilding;
class noRoadNode;

/// Klasse für die besser JH-KI
class AIPlayerJH : public AIBase
{
public:
	AIPlayerJH(const unsigned char playerid, const GameWorldBase * const gwb, const GameClientPlayer * const player,
		const GameClientPlayerList * const players, const GlobalGameSettings * const ggs,
		const AI::Level level);

private:

	// wofür isn das?
	struct Param_RoadPath
	{
		/// Straßenbaumodus erlaubt?
		bool boat_road;
	};

	// Pathfinding-Kram und calcbq braucht was non-constes... TODO
	GameWorldBase *gwb_;

	void RunGF(const unsigned gf);

  // Findet Flaggen in der Umgebung von x,y
	void FindFlags(std::vector<const noFlag*>& flags, unsigned short x, unsigned short y, unsigned short radius);

	// Expandiert ums HQ herum
	void ExpandAroundHQ();

	// Expandiert um andere Militärgebäude herum
	void Expand();

	// Verbindet Baustellen mit dem Wegenetz
	void ConnectBuildingSites();

	//bool ConnectBuildingToRoadSystem(const noBaseBuilding * building);

	// Verbindet eine Flagge mit dem Wegenetz
	bool ConnectFlagToRoadSytem(const noFlag *flag);

	// Baut eine Straße mit Fahnen
	bool BuildRoad(const noRoadNode *start, const noRoadNode *target);
};

#endif //!AIPLAYERJH_H_INCLUDED
