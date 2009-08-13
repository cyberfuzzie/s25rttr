// $Id: AIPlayerJH.h 5405 2009-08-13 21:23:32Z jh $
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
#include "GameClientPlayer.h"
#include "AIJHHelper.h"

#include <queue>
#include <list>

class noFlag;
class noBaseBuilding;
class noRoadNode;
class nobBaseMilitary;
class AIPlayerJH;
class nobMilitary;
class nobBaseMilitary;


/// Klasse für die besser JH-KI
class AIPlayerJH : public AIBase
{
	friend class AIJH::BuildJob;
public:
	AIPlayerJH(const unsigned char playerid, const GameWorldBase * const gwb, const GameClientPlayer * const player,
		const GameClientPlayerList * const players, const GlobalGameSettings * const ggs,
		const AI::Level level);

protected:

	// wofür isn das?
	struct Param_RoadPath
	{
		bool boat_road;
	};

	struct Coords
	{
		MapCoord x;
		MapCoord y;
		Coords(MapCoord x, MapCoord y) : x(x), y(y) { }
	};

	void RunGF(const unsigned gf);

  /// Finds flags in the area of y,y
	void FindFlags(std::vector<const noFlag*>& flags, unsigned short x, unsigned short y, unsigned short radius);

	/// Tests all building sites for road connection and connects them if necessary
	// [currently not used]
	void ConnectBuildingSites();

	/// Connects a specific flag to a roadsystem nearby and returns true if succesful. Also returns the route of the future road.
	bool ConnectFlagToRoadSytem(const noFlag *flag, std::vector<unsigned char>& route);

	/// Builds a street between two roadnodes and sets flags on it.
	bool BuildRoad(const noRoadNode *start, const noRoadNode *target);

	/// Test whether the player should resign or not
	bool TestDefeat();

	/// resigned yes/no
	bool defeated;


	/// Refreshes the number of buildings by asking the GameClientPlayer and recalcs some wanted buildings
	void RefreshBuildingCount();

	/// Number of buildings and building sites of this player (refreshed by RefreshBuildingCount())
	BuildingCount buildingCounts;

	
	/// Executes a job form the job queue
	void ExecuteAIJob();

	/// The current job the AI is working on
	AIJH::Job *currentJob;

	/// Contains the jobs the AI should try to execute, for example build jobs
	std::queue<AIJH::Job*> aiJobs;

	/// List of coordinates at which military buildings should be
	std::list<Coords> milBuildings;

	/// List of coordinates at which military buildingsites should be
	std::list<Coords> milBuildingSites;

	/// Checks the list of military buildingsites and puts the coordinates into the list of military buildings if building is finished
	void CheckNewMilitaryBuildings();

	/// Nodes containing some information about every map node
	std::vector<AIJH::Node> nodes;

	/// Initializes the nodes on start of the game
	void InitNodes();

	/// Updates the nodes around a position 
	void UpdateNodesAround(MapCoord x, MapCoord y, unsigned radius);

	/// Returns the resource on a specific point
	AIJH::Resource CalcResource(MapCoord x, MapCoord y);

	/// Resource maps, containing a rating for every map point concerning a resource
	std::vector<std::vector<int> > resourceMaps;

	/// Initialize the resource maps
	void InitResourceMaps();

	/// Changes a single resource map around point x,y in radius; to every point around x,y distanceFromCenter * value is added
	void ChangeResourceMap(MapCoord x, MapCoord y, unsigned radius, std::vector<int> &resMap, int value);

	/// Finds a good position for a specific resource in an area using the resource maps, 
	/// first position satisfying threshold is returned, returns false if no such position found
	bool FindGoodPosition(MapCoord &x, MapCoord &y, AIJH::Resource res, int threshold, BuildingQuality size, int radius = -1, bool inTerritory = true);

	/// Finds the best position for a specific resource in an area using the resource maps, 
	/// satisfying the minimum value, returns false if no such position is found
	bool FindBestPosition(MapCoord &x, MapCoord &y, AIJH::Resource res, BuildingQuality size, int minimum, int radius = -1, bool inTerritory = true);
	bool FindBestPosition(MapCoord &x, MapCoord &y, AIJH::Resource res, BuildingQuality size, int radius = -1, bool inTerritory = true) 
	{ return FindBestPosition(x,y,res,size,1,radius,inTerritory); }

	/// Finds a position for the desired building size
	bool SimpleFindPosition(MapCoord &x, MapCoord &y, BuildingQuality size, int radius = -1);

	/// Checks whether a flag is connected to the road system or not (connected = has path to HQ)
	bool IsConnectedToRoadSystem(const noFlag *flag);

	/// Recalculate the Buildingquality around a certain point
	void RecalcBQAround(const MapCoord x, const MapCoord y);

	/// Randomly chooses a military building, prefering bigger buildings if enemy nearby
	BuildingType ChooseMilitaryBuilding(MapCoord x, MapCoord y);

	/// Does some actions after a new military building is occupied
	void HandleNewMilitaryBuilingOccupied(const Coords& coords);

	/// Does some actions regulary on a military building
	// [currently not used]
	void HandleRetryMilitaryBuilding(const Coords& coords);

	/// Returns the number of buildings and buildingsites of a specific typ
	unsigned GetBuildingCount(BuildingType type);

	/// Contains how many buildings of every type is wanted
	std::vector<unsigned> buildingsWanted;

	/// Checks whether a building type is wanted atm
	bool Wanted(BuildingType type);

	/// Initializes the wanted-buildings-vector
	void InitBuildingsWanted();

	/// Counter to remember which military building was last checked
	// [currently not used]
	unsigned militaryBuildingToCheck;

	/// Used to check military buildings from time to time
	// [currently not used]
	void CheckExistingMilitaryBuildings();

	/// Sends a chat messsage to all players
	void Chat(std::string lala);

	/// Tries to attack the enemy
	void TryToAttack();


protected:


	// Required by the AIJobs:

	std::vector<gc::GameCommand*> &GetGCS() { return gcs; }
	const GameWorldBase *GetGWB() { return gwb; }
	unsigned char GetPlayerID() { return playerid; }

};


#endif //!AIPLAYERJH_H_INCLUDED
