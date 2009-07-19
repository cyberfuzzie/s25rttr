// $Id: AIPlayerJH.h 5299 2009-07-19 15:52:20Z jh $
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
#include "MapConsts.h"
#include "GameConsts.h"

#include <queue>

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

	// Sucht Bäume in der nähe von x,y und gibt Schwerpunkt des Waldes zurück
	bool FindWood(MapCoord x, MapCoord y, MapCoord &wood_x, MapCoord &wood_y);

	// Versucht in der Nähe von x,y zu bauen
	bool BuildNear(MapCoord x, MapCoord y, BuildingType bld);

	// Erzeugt eine Karte mit Holzbeständen mit Radius radius um Punkt x,y 
	void CreateWoodMap(std::vector<unsigned short>& woodMap, MapCoord x, MapCoord y, unsigned radius);

	// Erhöht in einer (Wood-)map die Werte todo
	void IncrementMap(std::vector<unsigned short>& map, int mapSize, unsigned short x, unsigned short y, unsigned short radius);

	// Sucht besten Punkt in einer WoodMap, todo signatur doof - alle daten in der map speichern?
	bool FindWood(std::vector<unsigned short>& woodMap, unsigned short radius, MapCoord& x, MapCoord& y);

	// Fügt ein Gebäude zur Bauwarteschlange hinzu
	void AddBuilding(MapCoord x, MapCoord y, BuildingType bld);

	// Baut das vorderste Gebäude in der Bauwarteschlange
	void BuildFromQueue();

	// Prüft ob das oberste Element der Queue schon platziert wurde und veranlasst Wegebau, gibt true zurück wenn Gebäude angeschlossen
	void CheckBuildingQueue();

	// Entfernt das vorderste Element aus der Queue, prüft ob DONE
	void RemoveFromBuildingQueue();

	enum BuildJobStatus
	{
		BJ_WAITING,
		BJ_BUILDING,
		BJ_CONNECTING,
		BJ_DONE,
		BJ_ERROR
	};

	// Ein Bauauftrag für die Queue
	struct BuildJob 
	{
		MapCoord x;
		MapCoord y;
		BuildingType building;
		BuildJobStatus status;

		BuildJob(MapCoord x, MapCoord y, BuildingType building, BuildJobStatus status) : x(x), y(y), 
			building(building), status(status) { }
	};

	// Gebäude-Queue
	std::queue<BuildJob*> buildingQueue;

	// Tmp zum spielen:
	std::vector<unsigned short> woodMap;
};

#endif //!AIPLAYERJH_H_INCLUDED
