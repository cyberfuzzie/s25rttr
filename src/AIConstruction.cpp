// $Id: AIConstruction.cpp 6458 2010-05-31 11:38:51Z FloSoft $
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

#include "AIConstruction.h"
#include "GameCommands.h"
#include "nobBaseMilitary.h"
#include "MapGeometry.h"
#include <iostream>

// from Pathfinding.cpp
bool IsPointOK_RoadPath(const GameWorldBase& gwb, const MapCoord x, const MapCoord y, const unsigned char dir, const void *param);

AIConstruction::AIConstruction(const GameWorldBase * const gwb, std::vector<gc::GameCommand*> &gcs, const GameClientPlayer * const player, 
															 unsigned char playerid)
: gwb(gwb), gcs(gcs), player(player), playerid(playerid)
{
	buildingsWanted.resize(BUILDING_TYPES_COUNT);
	RefreshBuildingCount();
	InitBuildingsWanted();
	AddStoreHouse(player->hqx, player->hqy);
}

AIConstruction::~AIConstruction(void)
{
}

void AIConstruction::AddBuildJob(AIJH::BuildJob *job, bool front)
{
	if (front)
		buildJobs.push_front(job);
	else
		buildJobs.push_back(job);
}

AIJH::Job *AIConstruction::GetBuildJob()
{
	if (buildJobs.empty())
		return NULL;
	
	AIJH::Job *job = buildJobs.front();
	buildJobs.pop_front();
	return job;
}

void AIConstruction::AddConnectFlagJob(AIPlayerJH *aijh, const noFlag *flag)
{
	buildJobs.push_front(new AIJH::ConnectJob(aijh, flag->GetX(), flag->GetY()));
}


void AIConstruction::FindFlags(std::vector<const noFlag*>& flags, unsigned short x, unsigned short y, unsigned short radius, 
															 unsigned short real_x, unsigned short real_y, unsigned short real_radius, bool clear)
{
	if (clear)
		flags.clear();

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				if(gwb->CalcDistance(tx2, ty2, real_x, real_y) <= real_radius && gwb->GetSpecObj<noFlag>(tx2,ty2))
				{
					flags.push_back(gwb->GetSpecObj<noFlag>(tx2,ty2));
				}
			}
		}
	}
}

void AIConstruction::FindFlags(std::vector<const noFlag*>& flags, unsigned short x, unsigned short y, unsigned short radius, bool clear)
{
	if (clear)
		flags.clear();

	// TODO Performance Killer!
	/*
	if (radius > 10)
	{
		list<nobBaseMilitary*> military;
		gwb->LookForMilitaryBuildings(military, x, y, 2);
		for(list<nobBaseMilitary*>::iterator it = military.begin();it.valid();++it)
		{
			unsigned distance = gwb->CalcDistance((*it)->GetX(), (*it)->GetY(), x, y);
			if (distance < radius && (*it)->GetPlayer() == player->getPlayerID())
			{
				FindFlags(flags, (*it)->GetX(), (*it)->GetY(), 10, x, y, radius, false);
			}
		}
	}
	*/

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				const noFlag *flag = gwb->GetSpecObj<noFlag>(tx2,ty2);
				if(flag && flag->GetPlayer() == player->getPlayerID())
				{
					flags.push_back(flag);
					if (flags.size() > 30)
					{
						return;
					}
				}
			}
		}
	}
}

bool AIConstruction::ConnectFlagToRoadSytem(const noFlag *flag, std::vector<unsigned char>& route, unsigned int maxSearchRadius)
{
	// TODO: die methode kann  ganz schön böse Laufzeiten bekommen... Optimieren?

	// Radius in dem nach würdigen Fahnen gesucht wird
	//const unsigned short maxSearchRadius = 10;

	// Ziel, das möglichst schnell erreichbar sein soll 
	//noFlag *targetFlag = gwb->GetSpecObj<nobHQ>(player->hqx, player->hqy)->GetFlag();
	noFlag *targetFlag = FindTargetStoreHouseFlag(flag->GetX(), flag->GetY());

	// Flaggen in der Umgebung holen
	std::vector<const noFlag*> flags;
	FindFlags(flags, flag->GetX(), flag->GetY(), maxSearchRadius);

#ifdef DEBUG_AI
	std::cout << "FindFlagsNum: " << flags.size() << std::endl;
#endif

	unsigned shortest = 0;
	unsigned int shortestLength = 99999;
	std::vector<unsigned char> tmpRoute;
	bool found = false;
	
	// Jede Flagge testen...
	for(unsigned i=0; i<flags.size(); ++i)
	{
		tmpRoute.clear();
		unsigned int length;
		
		bool boat = false;
		// Gibts überhaupt einen Pfad zu dieser Flagge
		bool pathFound = gwb->FindFreePath(flag->GetX(),flag->GetY(),
		                  flags[i]->GetX(),flags[i]->GetY(),false,100,&tmpRoute,&length,NULL,IsPointOK_RoadPath,NULL, (void *) &boat);

		// Wenn ja, dann gucken ob dieser Pfad möglichst kurz zum "höheren" Ziel (allgemeines Lager im Moment) ist
		if (pathFound)
		{
			unsigned char first_dir;
			unsigned int distance = 0;

			// Strecke von der potenziellen Zielfahne bis zum Lager
			bool pathFound = gwb->FindPathOnRoads(flags[i], targetFlag, false, &distance, &first_dir, NULL, NULL);

			// Gewählte Fahne hat leider auch kein Anschluß an ein Lager, zu schade!
			if (!pathFound)
				// Und ist auch nicht zufällig die Lager-Flagge selber...
				if (flags[i]->GetX() != targetFlag->GetX() || flags[i]->GetY() != targetFlag->GetY())
					continue;

			// Sind wir mit der Fahne schon verbunden? Einmal reicht!
			bool alreadyConnected = gwb->FindPathOnRoads(flags[i], flag, false, NULL, &first_dir, NULL, NULL);
			if (alreadyConnected)
				continue;
			
			// Ansonsten haben wir einen Pfad!
			found = true;
			
			// Kürzer als der letzte? Nehmen! Existierende Strecke höher gewichten (2), damit möglichst kurze Baustrecken
			// bevorzugt werden bei ähnlich langen Wegmöglichkeiten
			if (2 * length + distance < shortestLength)
			{
				shortest = i;
				shortestLength = 2 * length + distance;
				route = tmpRoute;
			}
		}
	}

	if (found)
	{
		return BuildRoad(flag, flags[shortest], route);
	}
	return false;
}

bool AIConstruction::BuildRoad(const noRoadNode *start, const noRoadNode *target, std::vector<unsigned char> &route)
{
	bool foundPath;
		
	// Gucken obs einen Weg gibt
	if (route.empty())
	{
		bool boat = false;
		foundPath = gwb->FindFreePath(start->GetX(),start->GetY(),
											target->GetX(),target->GetY(),false,100,&route,NULL,NULL,IsPointOK_RoadPath,NULL, (void *) &boat);
	}
	else
	{
		// Wenn Route übergeben wurde, davon ausgehen dass diese auch existiert
		foundPath = true;
	}

	// Wenn Pfad gefunden, Befehl zum Straße bauen und Flagen setzen geben
	if (foundPath)
	{
		MapCoord x = start->GetX();
		MapCoord y = start->GetY();
		gcs.push_back(new gc::BuildRoad(x, y, false, route));


		// Flaggen auf der Straße setzen
		for(unsigned i=0; i<route.size(); ++i)
		{
			gwb->GetPointA(x, y, route[i]);
			// Alle zwei Teilstücke versuchen eine Flagge zu bauen
			if (i % 2 == 1)
			{
				gcs.push_back(new gc::SetFlag(x,y));
			}
		}
		return true;
	}
	return false;
}

bool AIConstruction::IsConnectedToRoadSystem(const noFlag *flag)
{
	noFlag *targetFlag = this->FindTargetStoreHouseFlag(flag->GetX(), flag->GetY());
	if (targetFlag)
		return gwb->FindPathOnRoads(flag, targetFlag, false, NULL, NULL, NULL, NULL);
	else 
		return false;
}

BuildingType AIConstruction::ChooseMilitaryBuilding(MapCoord x, MapCoord y)
{
	BuildingType bld = BLD_BARRACKS;

	if ((rand() % 3) == 0)
		bld = BLD_GUARDHOUSE;

	std::list<nobBaseMilitary*> military;
	gwb->LookForMilitaryBuildings(military, x, y, 3);
	for(std::list<nobBaseMilitary*>::iterator it = military.begin();it!=military.end();++it)
	{
		unsigned distance = gwb->CalcDistance((*it)->GetX(), (*it)->GetY(), x, y);

		// Prüfen ob Feind in der Nähe
		if ((*it)->GetPlayer() != playerid && distance < 35)
		{
			int randmil = rand();

			if (randmil % 8 == 0 && player->CanBuildCatapult())
				bld = BLD_CATAPULT;
			else if (randmil % 2 == 0)
				bld = BLD_FORTRESS;
			else
				bld = BLD_WATCHTOWER;

			break;
		}
	}

	return bld;
}

unsigned AIConstruction::GetBuildingCount(BuildingType type)
{
	return buildingCounts.building_counts[type] + buildingCounts.building_site_counts[type];
}

bool AIConstruction::Wanted(BuildingType type)
{
	if (type == BLD_CATAPULT && !player->CanBuildCatapult())
		return false;
	if ((type >= BLD_BARRACKS && type <= BLD_FORTRESS) || type == BLD_STOREHOUSE)
		return true;
	return GetBuildingCount(type) < buildingsWanted[type];
}

void AIConstruction::RefreshBuildingCount()
{
	player->GetBuildingCount(buildingCounts);

	buildingsWanted[BLD_SAWMILL] = 1 + GetBuildingCount(BLD_WOODCUTTER) / 2;
	buildingsWanted[BLD_IRONSMELTER] = GetBuildingCount(BLD_IRONMINE);
	buildingsWanted[BLD_MINT] = GetBuildingCount(BLD_GOLDMINE);
	buildingsWanted[BLD_ARMORY] = GetBuildingCount(BLD_IRONSMELTER);
	buildingsWanted[BLD_BREWERY] = (GetBuildingCount(BLD_ARMORY) > 0 && GetBuildingCount(BLD_FARM) > 0) ? 1 : 0;

	buildingsWanted[BLD_MILL] = (buildingCounts.building_counts[BLD_FARM] + 2) / 4;
	buildingsWanted[BLD_BAKERY] = buildingsWanted[BLD_MILL];

	buildingsWanted[BLD_PIGFARM] = buildingCounts.building_counts[BLD_FARM] / 4;
	buildingsWanted[BLD_SLAUGHTERHOUSE] = buildingsWanted[BLD_PIGFARM];

	buildingsWanted[BLD_WELL] = buildingsWanted[BLD_BAKERY] + buildingsWanted[BLD_PIGFARM]
		+ buildingsWanted[BLD_DONKEYBREEDER] + buildingsWanted[BLD_BREWERY];

	if (GetBuildingCount(BLD_FARM) > 8)
	{
		buildingsWanted[BLD_COALMINE] = 6;
		buildingsWanted[BLD_IRONMINE] = 2;
		buildingsWanted[BLD_GOLDMINE] = 2;
	}
}

void AIConstruction::InitBuildingsWanted()
{
	buildingsWanted[BLD_FORESTER] = 1;
	buildingsWanted[BLD_SAWMILL] = 1;
	buildingsWanted[BLD_WOODCUTTER] = 12;
	buildingsWanted[BLD_QUARRY] = 6;
	buildingsWanted[BLD_GRANITEMINE] = 0;
	buildingsWanted[BLD_COALMINE] = 3;
	buildingsWanted[BLD_IRONMINE] = 1;
	buildingsWanted[BLD_GOLDMINE] = 1;
	buildingsWanted[BLD_CATAPULT] = 5;
	buildingsWanted[BLD_FISHERY] = 6;
	buildingsWanted[BLD_QUARRY] = 6;
	buildingsWanted[BLD_HUNTER] = 2;
	buildingsWanted[BLD_FARM] = player->GetInventory()->goods[GD_SCYTHE] + player->GetInventory()->people[JOB_FARMER];
	buildingsWanted[BLD_HARBORBUILDING] = 99;
	buildingsWanted[BLD_SHIPYARD] = 1;
}

bool AIConstruction::BuildAlternativeRoad(const noFlag *flag, std::vector<unsigned char> &route)
{
	// Radius in dem nach würdigen Fahnen gesucht wird
	const unsigned short maxRoadLength = 10;
	// Faktor um den der Weg kürzer sein muss als ein vorhander Pfad, um gebaut zu werden
	const unsigned short lengthFactor = 5;


	// Flaggen in der Umgebung holen
	std::vector<const noFlag*> flags;
	FindFlags(flags, flag->GetX(), flag->GetY(), maxRoadLength);

	// Jede Flagge testen...
	for(unsigned i=0; i<flags.size(); ++i)
	{
		//std::vector<unsigned char> new_route;
		route.clear();
		unsigned int newLength;
		
		bool boat = false;
		// Gibts überhaupt einen Pfad zu dieser Flagge
		bool pathFound = gwb->FindFreePath(flag->GetX(),flag->GetY(),
		                  flags[i]->GetX(),flags[i]->GetY(),false,100,&route,&newLength,NULL,IsPointOK_RoadPath,NULL, (void *) &boat);

		// Wenn ja, dann gucken ob unser momentaner Weg zu dieser Flagge vielleicht voll weit ist und sich eine Straße lohnt
		if (pathFound)
		{
			unsigned int oldLength = 0;

			// Aktuelle Strecke zu der Flagge
			bool pathAvailable = gwb->FindPathOnRoads(flags[i], flag, false, &oldLength, NULL, NULL, NULL);

			// Lohnt sich die Straße?
			if (!pathAvailable || newLength * lengthFactor < oldLength)
			{
				return BuildRoad(flag, flags[i], route);
			}
		}
	}

	return false;
}

bool AIConstruction::FindStoreHousePosition(MapCoord &x, MapCoord &y, unsigned radius)
{
	// max distance to warehouse/hq
	const unsigned maxDistance = 20;

	MapCoord fx = gwb->GetXA(x,y,4);
	MapCoord fy = gwb->GetYA(x,y,4);

	unsigned minDist = std::numeric_limits<unsigned>::max();
	for (std::list<AIJH::Coords>::iterator it = storeHouses.begin(); it != storeHouses.end(); it++)
	{
		const noBaseBuilding *bld;
		if ((bld = gwb->GetSpecObj<noBaseBuilding>((*it).x, (*it).y)))
		{
			if (bld->GetBuildingType() != BLD_STOREHOUSE && bld->GetBuildingType() != BLD_HEADQUARTERS)
				continue;

			const noFlag *targetFlag = gwb->GetSpecObj<noFlag>(fx,fy);
			unsigned dist;
			bool pathAvailable = gwb->FindPathOnRoads(bld->GetFlag(), targetFlag, false, &dist, NULL, NULL, NULL);

			if (!pathAvailable)
				continue;

			if (dist < minDist)
			{
				minDist = dist;
			}
			if (minDist <= maxDistance)
			{
				return false;
			}
		}
	}
	//assert(false); // fix pos berechnung für lagerhaus beim aufruf, nicht hier
	return true; //SimpleFindPosition(x, y, BUILDING_SIZE[BLD_STOREHOUSE], radius);
}

noFlag *AIConstruction::FindTargetStoreHouseFlag(MapCoord x, MapCoord y)
{
	unsigned minDistance = std::numeric_limits<unsigned>::max();
	AIJH::Coords minTarget(0xFF,0xFF);
	bool found = false;
	const noBaseBuilding *bld;
	for (std::list<AIJH::Coords>::iterator it = storeHouses.begin(); it != storeHouses.end(); it++)
	{
		if ((bld = gwb->GetSpecObj<noBaseBuilding>((*it).x, (*it).y)))
		{
			if (bld->GetBuildingType() != BLD_STOREHOUSE && bld->GetBuildingType() != BLD_HEADQUARTERS)
				continue;
		
			unsigned dist = gwb->CalcDistance(x, y, (*it).x, (*it).y);

			if (dist < minDistance)
			{
				minDistance = dist;
				minTarget = *it;
				found = true;
			}
		}
	}
	if (!found)
		return NULL;
	else
	{
		bld = gwb->GetSpecObj<noBaseBuilding>(minTarget.x, minTarget.y);
		return bld->GetFlag();
	}
}
