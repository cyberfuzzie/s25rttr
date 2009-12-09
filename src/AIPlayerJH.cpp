// $Id: AIPlayerJH.cpp 5780 2009-12-08 23:13:44Z jh $
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

#include "main.h"
#include "AIPlayerJH.h"

#include "GameClientPlayer.h"
#include "GameWorld.h"
#include "GameCommands.h"
#include "GamePlayerList.h"

#include "nobMilitary.h"
#include "nobHQ.h"
#include "noBuildingSite.h"
#include "noShip.h"

#include "MapGeometry.h"

#include <iostream>

#include "GameMessages.h"
#include "GameServer.h"

// from Pathfinding.cpp
bool IsPointOK_RoadPath(const GameWorldBase& gwb, const MapCoord x, const MapCoord y, const unsigned char dir, const void *param);

AIPlayerJH::AIPlayerJH(const unsigned char playerid, const GameWorldBase * const gwb, const GameClientPlayer * const player,
		const GameClientPlayerList * const players, const GlobalGameSettings * const ggs,
		const AI::Level level) : AIBase(playerid, gwb, player, players, ggs, level), defeated(false)
{
	currentJob = 0;
	buildingsWanted.resize(BUILDING_TYPES_COUNT);
	militaryBuildingToCheck = 0;
	InitNodes();
	InitResourceMaps();
	InitBuildingsWanted();
	RefreshBuildingCount();
	AddStoreHouse(player->hqx, player->hqy);
	SaveResourceMapsToFile();
}

/// Wird jeden GF aufgerufen und die KI kann hier entsprechende Handlungen vollziehen
void AIPlayerJH::RunGF(const unsigned gf)
{
	if (gf == 1)
	{

	}

	if (defeated)
		return;

	if (TestDefeat())
		return;

	if ((gf + (playerid * 2)) % 20 == 0)
	{
		RefreshBuildingCount();
		ExecuteAIJob();
	}

	if ((gf + playerid * 10) % 1000 == 0)
	{
		//CheckExistingMilitaryBuildings();
		TryToAttack();
	}

	if ((gf + playerid * 10) % 100 == 0)
	{
		CheckNewMilitaryBuildings();
	}



	if (gf == 5)
	{
		aiJobs.push_back(new AIJH::BuildJob(this, BLD_HARBORBUILDING, player->hqx, player->hqy));
		aiJobs.push_back(new AIJH::BuildJob(this, BLD_SAWMILL));
		aiJobs.push_back(new AIJH::BuildJob(this, BLD_FORESTER));
		aiJobs.push_back(new AIJH::BuildJob(this, BLD_WOODCUTTER));
		aiJobs.push_back(new AIJH::BuildJob(this, BLD_WOODCUTTER));
		aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(player->hqx, player->hqy)));
		aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(player->hqx, player->hqy)));
		aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(player->hqx, player->hqy)));
		aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(player->hqx, player->hqy)));
		aiJobs.push_back(new AIJH::BuildJob(this, BLD_QUARRY));
		aiJobs.push_back(new AIJH::BuildJob(this, BLD_FISHERY));
		
	}

	if (gf == 100)
	{
		Chat(_("Hi, I'm an artifical player and I'm not very good yet!"));
	}
	if (gf == 120)
	{
		Chat(_("And I may crash your game sometimes..."));

		// Set military settings to some nicer default values
		std::vector<unsigned char> milSettings;
		milSettings.resize(7);
		milSettings[0] = 10;
		milSettings[1] = 5;
		milSettings[2] = 5;
		milSettings[3] = 5;
		milSettings[4] = 1;
		milSettings[5] = 10;
		milSettings[6] = 10;
		gcs.push_back(new gc::ChangeMilitary(milSettings));
	}

	if ((gf % 1000) == 0)
	{
		if (Wanted(BLD_SAWMILL))
		{
			aiJobs.push_back(new AIJH::BuildJob(this, BLD_SAWMILL));
		}
	}
}

void AIPlayerJH::FindFlags(std::vector<const noFlag*>& flags, unsigned short x, unsigned short y, unsigned short radius)
{
	flags.clear();
	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				if(gwb->GetSpecObj<noFlag>(tx2,ty2))
				{
					flags.push_back(gwb->GetSpecObj<noFlag>(tx2,ty2));
				}
			}
		}
	}
}


noFlag *AIPlayerJH::FindTargetStoreHouseFlag(MapCoord x, MapCoord y)
{
	unsigned minDistance = 999;
	Coords minTarget(0xFF,0xFF);
	bool found = false;
	const noBaseBuilding *bld;
	for (std::list<Coords>::iterator it = storeHouses.begin(); it != storeHouses.end(); it++)
	{
		if ((bld = gwb->GetSpecObj<noBaseBuilding>((*it).x, (*it).y)))
		{
			if (bld->GetBuildingType() != BLD_STOREHOUSE && bld->GetBuildingType() != BLD_HEADQUARTERS)
				continue;
		
			unsigned dist = CalcDistance(x, y, (*it).x, (*it).y);

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

bool AIPlayerJH::ConnectFlagToRoadSytem(const noFlag *flag, std::vector<unsigned char>& route)
{
	// Radius in dem nach würdigen Fahnen gesucht wird
	const unsigned short maxSearchRadius = 10;

	// Ziel, das möglichst schnell erreichbar sein soll (TODO müsste dann evtl. auch Lager/Hafen sein)
	//noFlag *targetFlag = gwb->GetSpecObj<nobHQ>(player->hqx, player->hqy)->GetFlag();
	noFlag *targetFlag = FindTargetStoreHouseFlag(flag->GetX(), flag->GetY());

	// Flaggen in der Umgebung holen
	std::vector<const noFlag*> flags;
	FindFlags(flags, flag->GetX(), flag->GetY(), maxSearchRadius);

	unsigned shortest = 0;
	unsigned int shortestLength = 99999;
	std::vector<unsigned char> tmpRoute;
	bool found = false;
	
	// Jede Flagge testen...
	for(unsigned i=0; i<flags.size(); ++i)
	{
		tmpRoute.clear();
		unsigned int length;
		Param_RoadPath prp = { false };
		
		// Gibts überhaupt einen Pfad zu dieser Flagge
		bool path_found = gwb->FindFreePath(flag->GetX(),flag->GetY(),
		                  flags[i]->GetX(),flags[i]->GetY(),false,100,&tmpRoute,&length,NULL,IsPointOK_RoadPath,NULL, &prp);

		// Wenn ja, dann gucken ob dieser Pfad möglichst kurz zum "höheren" Ziel (HQ im Moment) ist
		if (path_found)
		{
			unsigned char first_dir;
			unsigned int hqlength = 0;

			// Führt manchmal zu Problemen, temporär raus; TODO was anderes überlegen (ganz ohne ist auch doof)

			// Strecke von der potenziellen Zielfahne bis zum HQ
			bool hq_path_found = gwb->FindPathOnRoads(flags[i], targetFlag, false, &hqlength, &first_dir, NULL, NULL);

			// Gewählte Fahne hat leider auch kein Anschluß ans HQ, zu schade!
			if (!hq_path_found)
				// Und ist auch nicht zufällig die HQ-Flagge selber...
				if (flags[i]->GetX() != targetFlag->GetX() || flags[i]->GetY() != targetFlag->GetY())
					continue;
			

			// Sind wir mit der Fahne schon verbunden? Einmal reicht!
			bool alreadyConnected = gwb->FindPathOnRoads(flags[i], flag, false, NULL, &first_dir, NULL, NULL);
			if (alreadyConnected)
				continue;
			
			// Ansonsten haben wir einen Pfad!
			found = true;
			
			// Kürzer als der letzte? Nehmen!
			if (2 * length + hqlength < shortestLength)
			{
				shortest = i;
				shortestLength = 2 * length+hqlength;
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

bool AIPlayerJH::BuildRoad(const noRoadNode *start, const noRoadNode *target, std::vector<unsigned char> &route)
{
	bool foundPath;
		
	// Gucken obs einen Weg gibt
	if (route.empty())
	{
		Param_RoadPath prp = { false };

		foundPath = gwb->FindFreePath(start->GetX(),start->GetY(),
											target->GetX(),target->GetY(),false,100,&route,NULL,NULL,IsPointOK_RoadPath,NULL, &prp);
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

bool AIPlayerJH::TestDefeat()
{
	const nobHQ *hq = gwb->GetSpecObj<nobHQ>(player->hqx, player->hqy);
	if (!hq)
	{
		defeated = true;
		gcs.push_back(new gc::Surrender());
		Chat(_("Oh, no, you destroyed my headquarter! I surrender!"));
		return true;
	}
	return false;
}

AIJH::Resource AIPlayerJH::CalcResource(MapCoord x, MapCoord y)
{
	AIJH::Resource res = AIJH::NOTHING;

	// Unterirdische Ressourcen
	unsigned char subres = gwb->GetNode(x,y).resources;
	if (subres > 0x40+0*8 && subres < 0x48+0*8)
		res = AIJH::COAL;
	else if (subres > 0x40+1*8 && subres < 0x48+1*8)
		res = AIJH::IRONORE;
	else if (subres > 0x40+2*8 && subres < 0x48+2*8)
		res = AIJH::GOLD;
	else if (subres > 0x40+3*8 && subres < 0x48+3*8)
		res = AIJH::GRANITE;

	if (subres > 0x80 && subres < 0x90)
		res = AIJH::FISH;

	// Überirdische Resourcen
	NodalObjectType no = gwb->GetNO(x,y)->GetType();

	if (no == NOP_TREE)
		res = AIJH::WOOD;
	else if(no == NOP_GRANITE)
		res = AIJH::STONES;
	else if(res == AIJH::NOTHING && (no == NOP_NOTHING || no == NOP_ENVIRONMENT))
	{
		// Terrain prüfen
		unsigned char t;
		bool good = true;
		for(unsigned char i = 0;i<6;++i)
		{
			if (gwb->GetPointRoad(x,y,i))
			{
				good = false;
				break;
			}
			t = gwb->GetTerrainAround(x,y,i);
			if(t != 3 && (t<8 || t>12))
			{
				good = false;
				break;
			}
		}
		if (good)
			res = AIJH::PLANTSPACE;
	}
	return res;
}

void AIPlayerJH::InitReachableNodes()
{
	unsigned short width = gwb->GetWidth();
	unsigned short height = gwb->GetHeight();

	std::queue<std::pair<MapCoord, MapCoord> > toCheck;

	// Alle auf not reachable setzen
	for (unsigned short y=0; y<height; ++y)
	{
		for (unsigned short x=0; x<width; ++x)
		{
			unsigned i = x + y * width;
			nodes[i].reachable = false;
			const noFlag *myFlag = 0;
			if (( myFlag = gwb->GetSpecObj<noFlag>(x,y)))
			{
				if (myFlag->GetPlayer() == playerid)
				{
					nodes[i].reachable = true;
					toCheck.push(std::make_pair(x,y));
				}
			}
		}
	}

	IterativeReachableNodeChecker(toCheck);
}

void AIPlayerJH::IterativeReachableNodeChecker(std::queue<std::pair<MapCoord, MapCoord> >& toCheck)
{
	unsigned short width = gwb->GetWidth();

	// TODO auch mal bootswege bauen können
	Param_RoadPath prp = { false };

	while(toCheck.size() > 0)
	{
		// Reachable coordinate
		MapCoord rx = toCheck.front().first;
		MapCoord ry = toCheck.front().second;

		// Coordinates to test around this reachable coordinate
		for (unsigned dir=0; dir<6; ++dir)
		{
			MapCoord nx = gwb->GetXA(rx, ry, dir);
			MapCoord ny = gwb->GetYA(rx, ry, dir);
			unsigned ni = nx + ny * width;

			// already reached, don't test again
			if (nodes[ni].reachable)
				continue;

			// Test whether point is reachable; yes->add to check list
			if (IsPointOK_RoadPath(*gwb, nx, ny, (dir+3)%6, &prp))
			{
				nodes[ni].reachable = true;
				toCheck.push(std::make_pair(nx, ny));
			}
		}
		toCheck.pop();
	}
}


void AIPlayerJH::UpdateReachableNodes(MapCoord x, MapCoord y, unsigned radius)
{
	unsigned short width = gwb->GetWidth();

	std::queue<std::pair<MapCoord, MapCoord> > toCheck;

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				unsigned i = tx2 + ty2 * width;
				nodes[i].reachable = false;
				const noFlag *myFlag = 0;
				if (( myFlag = gwb->GetSpecObj<noFlag>(tx2,ty2)))
				{
					if (myFlag->GetPlayer() == playerid)
					{
						nodes[i].reachable = true;
						toCheck.push(std::make_pair(tx2,ty2));
					}
				}
			}
		}
	}
	IterativeReachableNodeChecker(toCheck);
}

void AIPlayerJH::InitNodes()
{
	unsigned short width = gwb->GetWidth();
	unsigned short height = gwb->GetHeight();
	unsigned int playerID = player->getPlayerID();

	nodes.resize(width * height);

	InitReachableNodes();

	for (unsigned short y=0; y<height; ++y)
	{
		for (unsigned short x=0; x<width; ++x)
		{
			unsigned i = x + y * width;

			// if reachable, we'll calc bq
			if (nodes[i].reachable)
			{
				nodes[i].owned = true;
				nodes[i].bq = gwb->CalcBQ(x, y, playerID);
			}
			else
			{
				nodes[i].owned = false;
				nodes[i].bq = BQ_NOTHING;
			}

			nodes[i].res = CalcResource(x, y);
			nodes[i].border = (gwb->GetNode(x, y).boundary_stones[0] == (playerid + 1));
			nodes[i].farmed = false;
		}
	}
}



void AIPlayerJH::InitResourceMaps()
{
	unsigned short width = gwb->GetWidth();
	unsigned short height = gwb->GetHeight();

	resourceMaps.resize(AIJH::RES_TYPE_COUNT);
	for (unsigned res=0; res<AIJH::RES_TYPE_COUNT; ++res)
	{
		resourceMaps[res].resize(width * height);
		for (unsigned short y=0; y<height; ++y)
		{
			for (unsigned short x=0; x<width; ++x)
			{
				unsigned i = y * width + x;
				//resourceMaps[res][i] = 0;
				if (nodes[i].res == (AIJH::Resource)res && (AIJH::Resource)res != AIJH::BORDERLAND)
				{
					ChangeResourceMap(x, y, AIJH::RES_RADIUS[res], resourceMaps[res], 1);
				}

				// Grenzgebiet"ressource"
				else if (nodes[i].border && (AIJH::Resource)res == AIJH::BORDERLAND)
				{
					ChangeResourceMap(x, y, AIJH::RES_RADIUS[AIJH::BORDERLAND], resourceMaps[AIJH::BORDERLAND], 1);
				}
			}
		}
	}
}

void AIPlayerJH::SetFarmedNodes(MapCoord x, MapCoord y)
{
	// Radius in dem Bausplatz für Felder blockiert wird
	const unsigned radius = 3;

	unsigned short width = gwb->GetWidth();
	
	nodes[x + y * width].farmed = true;

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				unsigned i = tx2 + ty2 * width;
				nodes[i].farmed = true;;
			}
		}
	}
}

void AIPlayerJH::ChangeResourceMap(MapCoord x, MapCoord y, unsigned radius, std::vector<int> &resMap, int value)
{
	unsigned short width = gwb->GetWidth();

	resMap[x + y * width] += value * radius;

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				unsigned i = tx2 + ty2 * width;
				resMap[i] += value * (radius-r);
			}
		}
	}


}

bool AIPlayerJH::FindGoodPosition(MapCoord &x, MapCoord &y, AIJH::Resource res, int threshold, BuildingQuality size, int radius, bool inTerritory)
{
	unsigned short width = gwb->GetWidth();
	unsigned short height = gwb->GetHeight();

	if (x >= width || y >= height)
	{
		x = player->hqx;
		y = player->hqy;
	}

	// TODO was besseres wär schön ;)
	if (radius == -1)
		radius = 30;

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				unsigned i = tx2 + ty2 * width;
				if (resourceMaps[res][i] >= threshold)
				{
					if ((inTerritory && !nodes[i].owned) || nodes[i].farmed)
						continue;
					if ( (nodes[i].bq >= size && nodes[i].bq < BQ_MINE) // normales Gebäude
						|| (nodes[i].bq == size))	// auch Bergwerke
					{
						x = tx2;
						y = ty2;
						return true;
					}
				}
			}
		}
	}
	return false;
}


bool AIPlayerJH::FindBestPosition(MapCoord &x, MapCoord &y, AIJH::Resource res, BuildingQuality size, int minimum, int radius, bool inTerritory)
{
	unsigned short width = gwb->GetWidth();
	unsigned short height = gwb->GetHeight();

	if (x >= width || y >= height)
	{
		x = player->hqx;
		y = player->hqy;
	}

	// TODO was besseres wär schön ;)
	if (radius == -1)
		radius = 30;

	int best_x = 0, best_y = 0, best_value;
	best_value = -1;

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				unsigned i = tx2 + ty2 * width;
				if (resourceMaps[res][i] > best_value)
				{
					if (!nodes[i].reachable || (inTerritory && !nodes[i].owned) || nodes[i].farmed)
						continue;
					if ( (nodes[i].bq >= size && nodes[i].bq < BQ_MINE) // normales Gebäude
						|| (nodes[i].bq == size))	// auch Bergwerke
					{
						best_x = tx2;
						best_y = ty2;
						best_value = resourceMaps[res][i];
					}
				}
			}
		}
	}

	if (best_value >= minimum)
	{
		x = best_x;
		y = best_y;
		return true;
	}
	return false;
}

void AIPlayerJH::UpdateNodesAround(MapCoord x, MapCoord y, unsigned radius)
{
	unsigned width = gwb->GetWidth();

	UpdateReachableNodes(x, y, radius);

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				unsigned i = tx2 + ty2 * width;

				nodes[i].owned = (gwb->GetNode(tx2, ty2).owner == playerid + 1);

				if (nodes[i].owned)
				{
					nodes[i].bq = gwb->CalcBQ(tx2, ty2, playerid);
				}
				else
				{
					nodes[i].owned = false;
					nodes[i].bq = BQ_NOTHING;
				}

				AIJH::Resource res = CalcResource(tx2, ty2);
				if (res != nodes[i].res)
				{
					// Altes entfernen:
					if (nodes[i].res != AIJH::NOTHING)
						ChangeResourceMap(tx2, ty2, AIJH::RES_RADIUS[nodes[i].res], resourceMaps[nodes[i].res], -1);
					// Neues Hinzufügen:
					if (res != AIJH::NOTHING)
						ChangeResourceMap(tx2, ty2, AIJH::RES_RADIUS[res], resourceMaps[res], 1);

					nodes[i].res = res;
				}

				bool borderland = (gwb->GetNode(tx2, ty2).boundary_stones[0] != 0);
				if (borderland != nodes[i].border)
				{
					if (borderland)
					{
						//std::cout << tx2 << " / " << ty2 << " Border dazugekommen" << std::endl;
						ChangeResourceMap(tx2, ty2, AIJH::RES_RADIUS[AIJH::BORDERLAND], resourceMaps[AIJH::BORDERLAND], 1);
					}
					else
					{
						//std::cout << tx2 << " / " << ty2 << " Border verschwunden" << std::endl;
						ChangeResourceMap(tx2, ty2, AIJH::RES_RADIUS[AIJH::BORDERLAND], resourceMaps[AIJH::BORDERLAND], -1);
					}
				}

			}
		}
	}
}

void AIPlayerJH::ExecuteAIJob()
{
	if (currentJob)
	{
		if (currentJob->GetStatus() == AIJH::JOB_FINISHED)
		{
			delete currentJob;
			currentJob = 0;
		}
	}
	if (currentJob)
	{
		if (currentJob->GetStatus() == AIJH::JOB_FAILED)
		{
			// TODO fehlerbehandlung?
			//std::cout << "Job failed." << std::endl;
			delete currentJob;
			currentJob = 0;
		}
	}

	if (!currentJob)
	{
		if (aiJobs.size() > 0)
		{
			currentJob = aiJobs.front();
			aiJobs.pop_front();
		}
		else
		{
			AIEvent::Base *ev = eventManager.GetEvent();
			if (ev)
				currentJob = new AIJH::EventJob(this, ev);
		}
	}

	if (currentJob)
		currentJob->ExecuteJob();
}

bool AIPlayerJH::IsConnectedToRoadSystem(const noFlag *flag)
{
	// TODO target ist atm immer das HQ
	//const noFlag *targetFlag = gwb->GetSpecObj<noFlag>(gwb->GetXA(player->hqx, player->hqy, 4), gwb->GetYA(player->hqx, player->hqy, 4));
	noFlag *targetFlag = this->FindTargetStoreHouseFlag(flag->GetX(), flag->GetY());
	if (targetFlag)
		return gwb->FindPathOnRoads(flag, targetFlag, false, NULL, NULL, NULL, NULL);
	else 
		return false;
}

void AIPlayerJH::RecalcBQAround(const MapCoord x, const MapCoord y)
{
	// Drumherum BQ neu berechnen, da diese sich ja jetzt hätten ändern können
	unsigned index = x + y * gwb->GetWidth();

	nodes[index].bq = gwb->CalcBQ(x,y,playerid);
	for(unsigned char i = 0;i<6;++i)
	{
		index = gwb->GetXA(x,y,i) + gwb->GetYA(x,y,i) * gwb->GetWidth();
		nodes[index].bq = gwb->CalcBQ(gwb->GetXA(x,y,i), gwb->GetYA(x,y,i),playerid);
	}
	for(unsigned i = 0;i<12;++i)
	{
		index = gwb->GetXA2(x,y,i) + gwb->GetYA2(x,y,i) * gwb->GetWidth();
		nodes[index].bq = gwb->CalcBQ(gwb->GetXA2(x,y,i),gwb->GetYA2(x,y,i),playerid);
	}
}

void AIPlayerJH::CheckNewMilitaryBuildings()
{
	for (std::list<Coords>::iterator it = milBuildingSites.begin(); it != milBuildingSites.end(); it++)
	{
		const nobMilitary *mil;
		if ((mil = gwb->GetSpecObj<nobMilitary>((*it).x, (*it).y)))
		{
			if (!mil->IsNewBuilt())
			{
				HandleNewMilitaryBuilingOccupied(*it);
				milBuildings.push_back(Coords(mil->GetX(), mil->GetY()));
				milBuildingSites.erase(it);
				break;
			}
		}
	}
}

BuildingType AIPlayerJH::ChooseMilitaryBuilding(MapCoord x, MapCoord y)
{
	BuildingType bld = BLD_BARRACKS;

	if ((rand() % 3) == 0)
		bld = BLD_GUARDHOUSE;

	list<nobBaseMilitary*> military;
	gwb->LookForMilitaryBuildings(military, x, y, 2);
	for(list<nobBaseMilitary*>::iterator it = military.begin();it.valid();++it)
	{
		unsigned distance = CalcDistance((*it)->GetX(), (*it)->GetY(), x, y);

		// Prüfen ob Feind in der Nähe
		if ((*it)->GetPlayer() != player->getPlayerID() && distance < 30)
		{
			int randmil = rand();

			if (randmil % 8 == 0)
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


bool AIPlayerJH::SimpleFindPosition(MapCoord &x, MapCoord &y, BuildingQuality size, int radius)
{
	unsigned short width = gwb->GetWidth();
	unsigned short height = gwb->GetHeight();

	if (x >= width || y >= height)
	{
		x = player->hqx;
		y = player->hqy;
	}

	// TODO was besseres wär schön ;)
	if (radius == -1)
		radius = 30;

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				unsigned i = tx2 + ty2 * width;

				if (!nodes[i].reachable || !nodes[i].owned || nodes[i].farmed)
					continue;
				if ( (nodes[i].bq >= size && nodes[i].bq < BQ_MINE) // normales Gebäude
					|| (nodes[i].bq == size))	// auch Bergwerke
				{
					x = tx2;
					y = ty2;
					return true;
				}
			}
		}
	}

	return false;
}

void AIPlayerJH::HandleNewMilitaryBuilingOccupied(const Coords& coords)
{
	MapCoord x = coords.x;
	MapCoord y = coords.y;
	UpdateNodesAround(x, y, 11); // todo: fix radius
	RefreshBuildingCount();

	const nobMilitary *mil = gwb->GetSpecObj<nobMilitary>(x, y);
	if (mil)
	{
		if ((mil->GetBuildingType() == BLD_BARRACKS || mil->GetBuildingType() == BLD_GUARDHOUSE) && mil->GetFrontierDistance() == 0 && !mil->IsGoldDisabled())
		{
			gcs.push_back(new gc::StopGold(x, y));
		}
	}

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_HARBORBUILDING, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(x, y), x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(x, y), x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(x, y), x, y));

	// Temporär only
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_FORESTER, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_WOODCUTTER, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_QUARRY, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_GOLDMINE, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_COALMINE, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_IRONMINE, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_SAWMILL, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_IRONSMELTER, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_MINT, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_ARMORY, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_FISHERY, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_HUNTER, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_STOREHOUSE, x, y));


	aiJobs.push_back(new AIJH::BuildJob(this, BLD_FARM, x, y));


	aiJobs.push_back(new AIJH::BuildJob(this, BLD_BREWERY, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_MILL, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_PIGFARM, x, y));

	

}

void AIPlayerJH::HandleBuildingFinished(const Coords& coords, BuildingType bld)
{
	switch(bld)
	{
	case BLD_HARBORBUILDING:
		UpdateNodesAround(coords.x, coords.y, 8); // todo: fix radius

		aiJobs.push_back(new AIJH::BuildJob(this, BLD_BARRACKS, coords.x, coords.y));
		aiJobs.push_back(new AIJH::BuildJob(this, BLD_WOODCUTTER, coords.x, coords.y));
		aiJobs.push_back(new AIJH::BuildJob(this, BLD_SAWMILL, coords.x, coords.y));
		aiJobs.push_back(new AIJH::BuildJob(this, BLD_QUARRY, coords.x, coords.y));

		gcs.push_back(new gc::StartExpedition(coords.x, coords.y));
		break;

	case BLD_SHIPYARD:
		gcs.push_back(new gc::ChangeShipYardMode(coords.x, coords.y));
		break;
		
	default:
		break;
	}

}

void AIPlayerJH::HandleExpedition(const Coords& coords)
{
	list<noBase*> objs;
	gwb->GetDynamicObjectsFrom(coords.x, coords.y, objs);
	const noShip *ship = NULL;

	for(list<noBase*>::iterator it = objs.begin();it.valid();++it)
	{
		if((*it)->GetGOT() == GOT_SHIP)
		{
			if(static_cast<noShip*>(*it)->GetPlayer() == playerid)
			{
				if (static_cast<noShip*>(*it)->IsOnExpedition())
					ship = static_cast<noShip*>(*it);
			}
		}
	}


	//const noShip *ship = gwb->GetSpecObj<noShip>(coords.x, coords.y);
	if (ship)
	{
		if (ship->IsAbleToFoundColony())
			gcs.push_back(new gc::ExpeditionCommand(gc::ExpeditionCommand::FOUNDCOLONY, player->GetShipID(ship)));
		else
		{
			unsigned char start = rand() % 6;

			for(unsigned char i = start; i < start + 6; ++i)
			{
				if (gwb->GetNextFreeHarborPoint(coords.x, coords.y, ship->GetCurrentHarbor(), i%6, playerid) > 0)
				{
					gcs.push_back(new gc::ExpeditionCommand(gc::ExpeditionCommand::Action((i%6)+1), player->GetShipID(ship)));
					break;
				}
			}
		}
		
			
	}

}


void AIPlayerJH::HandleNoMoreResourcesReachable(const Coords& coords, BuildingType bld)
{
	MapCoord x = coords.x;
	MapCoord y = coords.y;
	UpdateNodesAround(x, y, 11); // todo: fix radius

	// Destroy old building (once)
	if (gwb->GetNO(x,y)->GetType() == NOP_BUILDING)
		gcs.push_back(new gc::DestroyBuilding(x, y));
	else
		return;
	
	// try to expand, maybe res blocked a passage
	aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(x, y), x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(x, y), x, y));

	// and try to rebuild the same building
	aiJobs.push_back(new AIJH::BuildJob(this, bld));

	// farm is always good!
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_FARM, x, y));
}

void AIPlayerJH::HandleBorderChanged(const Coords& coords)
{
	MapCoord x = coords.x;
	MapCoord y = coords.y;
	UpdateNodesAround(x, y, 11); // todo: fix radius

	const nobMilitary *mil = gwb->GetSpecObj<nobMilitary>(x, y);
	if (mil)
	{
		if (mil->GetFrontierDistance() != 0 && mil->IsGoldDisabled())
		{
			gcs.push_back(new gc::StopGold(x, y));
		}
		if (mil->GetBuildingType() == BLD_BARRACKS || mil->GetBuildingType() == BLD_GUARDHOUSE)
		{
			aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(x, y), x, y));
		}
	}
}

void AIPlayerJH::HandleRetryMilitaryBuilding(const Coords& coords)
{
	MapCoord x = coords.x;
	MapCoord y = coords.y;
	UpdateNodesAround(x, y, 11); // todo: fix radius
	RefreshBuildingCount();

	aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(x, y), x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(x, y), x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, ChooseMilitaryBuilding(x, y), x, y));

	// Temporär only
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_WOODCUTTER, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_QUARRY, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_GOLDMINE, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_COALMINE, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_IRONMINE, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_SAWMILL, x, y));

	aiJobs.push_back(new AIJH::BuildJob(this, BLD_IRONSMELTER, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_MINT, x, y));
	aiJobs.push_back(new AIJH::BuildJob(this, BLD_ARMORY, x, y));
}


void AIPlayerJH::RefreshBuildingCount()
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
}

void AIPlayerJH::InitBuildingsWanted()
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
	buildingsWanted[BLD_FARM] = 16;
	buildingsWanted[BLD_HARBORBUILDING] = 99;
	buildingsWanted[BLD_SHIPYARD] = 1;
}

unsigned AIPlayerJH::GetBuildingCount(BuildingType type)
{
	return buildingCounts.building_counts[type] + buildingCounts.building_site_counts[type];
}

bool AIPlayerJH::Wanted(BuildingType type)
{
	if ((type >= BLD_BARRACKS && type <= BLD_FORTRESS) || type == BLD_STOREHOUSE)
		return true;
	return GetBuildingCount(type) < buildingsWanted[type];
}

void AIPlayerJH::CheckExistingMilitaryBuildings()
{
	if (milBuildings.size() == 0)
		return;
	std::list<Coords>::iterator it = milBuildings.begin();

	if (militaryBuildingToCheck >= milBuildings.size())
		militaryBuildingToCheck = 0;

	std::advance(it,militaryBuildingToCheck);

	assert(it != milBuildings.end());

	HandleRetryMilitaryBuilding(*it);

	const nobBaseMilitary *mil;

	if (!(mil = gwb->GetSpecObj<nobBaseMilitary>((*it).x, (*it).y)))
	{
		// Gebäude wurde wohl zerstört
		milBuildings.erase(it);
		return;
	}

	if (mil->GetPlayer() != playerid)
	{
		// Gebäude gehört nicht mehr uns O_o
		milBuildings.erase(it);
		return;
	}

	militaryBuildingToCheck++;	
}


void AIPlayerJH::Chat(std::string message)
{
	GameMessage_Server_Chat chat = GameMessage_Server_Chat(playerid,CD_ALL,message);
	GameServer::inst().AIChat(chat);
}

void AIPlayerJH::TryToAttack() 
{
	for (std::list<Coords>::iterator it = milBuildings.begin(); it != milBuildings.end(); it++)
	{
		const nobMilitary *mil;
		if ((mil = gwb->GetSpecObj<nobMilitary>((*it).x, (*it).y)))
		{
			if (mil->GetFrontierDistance() == 0)
				continue;

			list<nobBaseMilitary *> buildings;
			gwb->LookForMilitaryBuildings(buildings,(*it).x, (*it).y,2);
			for(list<nobBaseMilitary*>::iterator it2 = buildings.begin();it2.valid();++it2)
			{
				if (CalcDistance((*it).x, (*it).y, (*it2)->GetX(), (*it2)->GetY()) < BASE_ATTACKING_DISTANCE 
					&& player->IsPlayerAttackable((*it2)->GetPlayer()) && gwb->GetNode((*it2)->GetX(), (*it2)->GetY()).fow[playerid].visibility == VIS_VISIBLE)
				{
					gcs.push_back(new gc::Attack((*it2)->GetX(), (*it2)->GetY(), mil->GetTroopsCount() - 1, true));
				}
			}
		}
	}
}


void AIPlayerJH::RecalcGround(MapCoord x_building, MapCoord y_building, std::vector<unsigned char> &route_road)
{
	MapCoord x = x_building;
	MapCoord y = y_building;

	// building itself
	RecalcBQAround(x, y);
	if (GetAINode(x, y).res == AIJH::PLANTSPACE)
	{
		ChangeResourceMap(x, y, AIJH::RES_RADIUS[AIJH::PLANTSPACE], resourceMaps[AIJH::PLANTSPACE], -1);
		GetAINode(x, y).res = AIJH::NOTHING;
	}

	// flag of building
	gwb->GetPointA(x, y, 4);
	RecalcBQAround(x, y);
	if (GetAINode(x, y).res == AIJH::PLANTSPACE)
	{
		ChangeResourceMap(x, y, AIJH::RES_RADIUS[AIJH::PLANTSPACE], resourceMaps[AIJH::PLANTSPACE], -1);
		GetAINode(x, y).res = AIJH::NOTHING;
	}

	// along the road
	for (unsigned i=0; i<route_road.size(); ++i)
	{
		gwb->GetPointA(x, y, route_road[i]);
		RecalcBQAround(x, y);
		// Auch Plantspace entsprechend anpassen:
		if (GetAINode(x, y).res == AIJH::PLANTSPACE)
		{
			ChangeResourceMap(x, y, AIJH::RES_RADIUS[AIJH::PLANTSPACE], resourceMaps[AIJH::PLANTSPACE], -1);
			GetAINode(x, y).res = AIJH::NOTHING;
		}
	}
}



bool AIPlayerJH::BuildAlternativeRoad(const noFlag *flag, std::vector<unsigned char> &route)
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
		Param_RoadPath prp = { false };
		
		// Gibts überhaupt einen Pfad zu dieser Flagge
		bool path_found = gwb->FindFreePath(flag->GetX(),flag->GetY(),
		                  flags[i]->GetX(),flags[i]->GetY(),false,100,&route,&newLength,NULL,IsPointOK_RoadPath,NULL, &prp);

		// Wenn ja, dann gucken ob unser momentaner Weg zu dieser Flagge vielleicht voll weit ist und sich eine Straße lohnt
		if (path_found)
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

bool AIPlayerJH::FindStoreHousePosition(MapCoord &x, MapCoord &y, unsigned radius)
{
	// max distance to warehouse/hq
	const unsigned maxDistance = 20;

	MapCoord fx = gwb->GetXA(x,y,4);
	MapCoord fy = gwb->GetYA(x,y,4);

	unsigned minDist = 999;
	for (std::list<Coords>::iterator it = storeHouses.begin(); it != storeHouses.end(); it++)
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
	return SimpleFindPosition(x, y, BUILDING_SIZE[BLD_STOREHOUSE], radius);
}

void AIPlayerJH::SaveResourceMapsToFile()
{
#ifdef DEBUG_AI
	for(unsigned i=0; i<AIJH::RES_TYPE_COUNT; ++i)
	{
		std::stringstream ss;
		ss << "resmap-" << i << ".log";
		FILE * file = fopen(ss.str().c_str(),"w");
		for (unsigned y=0; y<gwb->GetHeight(); ++y)
		{
			if (y % 2 == 1)
				fprintf(file,"  ");
			for (unsigned x=0; x<gwb->GetWidth(); ++x)
			{
				fprintf(file,"%i   ",resourceMaps[i][x + y * gwb->GetWidth()]);		
			}
			fprintf(file,"\n");
		}
		fclose(file);
	}
#endif
}

int AIPlayerJH::GetResMapValue(MapCoord x, MapCoord y, AIJH::Resource res)
{
	return resourceMaps[res][x + y * gwb->GetWidth()];
}

void AIPlayerJH::SendAIEvent(AIEvent::Base *ev) 
{
	eventManager.AddAIEvent(ev);
}

