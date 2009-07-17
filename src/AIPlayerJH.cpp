// $Id: AIPlayerJH.cpp 5285 2009-07-17 15:27:02Z jh $
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

#include "MapGeometry.h"

// from Pathfinding.cpp
bool IsPointOK_RoadPath(const GameWorldBase& gwb, const MapCoord x, const MapCoord y, const unsigned char dir, const void *param);

AIPlayerJH::AIPlayerJH(const unsigned char playerid, const GameWorldBase * const gwb, const GameClientPlayer * const player,
		const GameClientPlayerList * const players, const GlobalGameSettings * const ggs,
		const AI::Level level) : AIBase(playerid, gwb, player, players, ggs, level)
{
}

/// Wird jeden GF aufgerufen und die KI kann hier entsprechende Handlungen vollziehen
void AIPlayerJH::RunGF(const unsigned gf)
{
	// nach neuem Barrackenplatz suchen
	if(gf % 50 == 0)
	{
		if (gf < 5000)
			ExpandAroundHQ();

		Expand();
	}

	if (gf % 50 == 20)
	{
		ConnectBuildingSites();
	}

	if (gf == 310)
	{
		MapCoord wood_x, wood_y;
		if (FindWood(player->hqx, player->hqy, wood_x, wood_y))
		{
			BuildNear(wood_x, wood_y, BLD_WOODCUTTER);
		}
		BuildNear(player->hqx, player->hqy, BLD_SAWMILL);
	}
}

void AIPlayerJH::ExpandAroundHQ()
{
	// TODO bissl stumpf noch :S
	// Ums HQ testen...
	int x = player->hqx - (rand()%16 -8);
	if(x < 0)
		x = 0;
	if(x >= gwb->GetWidth())
		x = gwb->GetWidth()-1;
	int y = player->hqy - (rand()%16 -8);
	if(y < 0)
		y = 0;
	if(y >= gwb->GetHeight())
		y = gwb->GetHeight()-1;

	// Bauqualität testen, wenn nicht gut, abbrechen
	BuildingQuality bq = gwb->CalcBQ(x, y, player->getPlayerID());
	if (bq != BQ_HUT && bq != BQ_HOUSE && bq != BQ_CASTLE)
		return;

	// Militärgebäude in der Nähe suchen, wenn zu dicht, abbrechen
	list<nobBaseMilitary*> military;
	gwb->LookForMilitaryBuildings(military, x, y, 6); // TODO das liefert irgendwie viel zu viel :S
	//if (military.size() != 0)	
	//	return;
	for(list<nobBaseMilitary*>::iterator it = military.begin();it.valid();++it)
	{
		if (CalcDistance((*it)->GetX(), (*it)->GetY(), x, y) <= 6)
			return;
	}


	gcs.push_back(new gc::SetBuildingSite(x, y, BLD_BARRACKS));
}

void AIPlayerJH::Expand()
{
	// TODO stumpf
	// Für jedes Militärgebäude einmal probieren einen Platz zu finden
	const std::list<nobMilitary*> milbuildings = player->GetMilitaryBuildings();
	for(std::list<nobMilitary*>::const_iterator it = milbuildings.begin(); it != milbuildings.end(); ++it)
	{
		int x = (*it)->GetX() - (rand()%14 -7);
		if(x < 0)
			x = 0;
		if(x >= gwb->GetWidth())
			x = gwb->GetWidth()-1;
		int y = (*it)->GetY() - (rand()%14 -7);
		if(y < 0)
			y = 0;
		if(y >= gwb->GetHeight())
			y = gwb->GetHeight()-1;

		// Bauqualität testen, wenn nicht gut, abbrechen
		BuildingQuality bq = gwb->CalcBQ(x, y, player->getPlayerID());
		if (bq != BQ_HUT && bq != BQ_HOUSE && bq != BQ_CASTLE)
			continue;

		// Militärgebäude in der Nähe suchen, wenn zu dicht, abbrechen
		list<nobBaseMilitary*> military;
		gwb->LookForMilitaryBuildings(military, x, y, 5);
		//if (military.size() != 0)	
		//	continue;
		for(list<nobBaseMilitary*>::iterator it = military.begin();it.valid();++it)
		{
			if (CalcDistance((*it)->GetX(), (*it)->GetY(), x, y) <= 5)
				return;
		}

		gcs.push_back(new gc::SetBuildingSite(x, y, BLD_BARRACKS));
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

void AIPlayerJH::ConnectBuildingSites()
{
	// Liste von Baustellen holen
	const std::list<noBuildingSite*> sites = player->GetBuildingSites();

	// Ziel, das möglichst schnell erreichbar sein soll (TODO müsste dann evtl. auch Lager/Hafen sein)
	noFlag *targetFlag = gwb->GetSpecObj<nobHQ>(player->hqx, player->hqy)->GetFlag();

	for (std::list<noBuildingSite*>::const_iterator it = sites.begin(); it != sites.end(); ++it)
	{
		unsigned char first_dir = 0xFF;
		// Kann die Baustelle vom HQ erreicht werden? Nein -> Weg bauen!
		if (!gwb->FindPathOnRoads((*it)->GetFlag(), targetFlag, false, NULL, NULL, &first_dir, NULL))
		{
			// Versuchen Weg zu bauen, wenns nicht klappt, Baustelle abreißen
			if (!ConnectFlagToRoadSytem((*it)->GetFlag()))
			{
				gcs.push_back(new gc::DestroyBuilding((*it)->GetX(), (*it)->GetY()));
			}
			else
			{
				// Besser immer nur ein Weg bauen (TODO Wenn berechnete Wege sich kreuzen, geht was kaputt)
				return;
			}
		}
	}
}

bool AIPlayerJH::ConnectFlagToRoadSytem(const noFlag *flag)
{
	// Radius in dem nach würdigen Fahnen gesucht wird
	const unsigned short maxRoadLength = 10;

	// Ziel, das möglichst schnell erreichbar sein soll (TODO müsste dann evtl. auch Lager/Hafen sein)
	noFlag *targetFlag = gwb->GetSpecObj<nobHQ>(player->hqx, player->hqy)->GetFlag();

	// Flaggen in der Umgebung holen
	std::vector<const noFlag*> flags;
	FindFlags(flags, flag->GetX(), flag->GetY(), maxRoadLength);

	unsigned shortest = 0;
	unsigned int shortestLength = 99999;
	bool found = false;
	
	// Jede Flagge testen...
	for(unsigned i=0; i<flags.size(); ++i)
	{
		std::vector<unsigned char> new_route;
		unsigned int length;
		Param_RoadPath prp = { false };
		
		// Gibts überhaupt einen Pfad zu dieser Flagge
		bool path_found = gwb->FindFreePath(flag->GetX(),flag->GetY(),
		                  flags[i]->GetX(),flags[i]->GetY(),false,100,&new_route,&length,NULL,NULL,IsPointOK_RoadPath,NULL, &prp);

		// Wenn ja, dann gucken ob dieser Pfad möglichst kurz zum "höheren" Ziel (HQ im Moment) ist
		if (path_found)
		{
			unsigned char first_dir;
			unsigned int hqlength = 0;
			// Strecke von der potenziellen Zielfahne bis zum HQ
			bool hq_path_found = gwb->FindPathOnRoads(flags[i], targetFlag, false, NULL, &hqlength, &first_dir, NULL);

			// Gewählte Fahne hat leider auch kein Anschluß ans HQ, zu schade!
			if (!hq_path_found)
				// Und ist auch nicht zufällig die HQ-Flagge selber...
				if (flags[i]->GetX() != targetFlag->GetX() || flags[i]->GetY() != targetFlag->GetY())
					continue;
			
			// Ansonsten haben wir einen Pfad!
			found = true;
			
			// Kürzer als der letzte? Nehmen!
			if (length + hqlength < shortestLength)
			{
				shortest = i;
				shortestLength = length+hqlength;
			}
		}
	}

	if (found)
	{
		return BuildRoad(flag, flags[shortest]);
	}
	return false;
}

bool AIPlayerJH::BuildRoad(const noRoadNode *start, const noRoadNode *target)
{
	// Gucken obs einen Weg gibt
	std::vector<unsigned char> route;
	Param_RoadPath prp = { false };

	bool foundPath = gwb->FindFreePath(start->GetX(),start->GetY(),
	                  target->GetX(),target->GetY(),false,100,&route,NULL,NULL,NULL,IsPointOK_RoadPath,NULL, &prp);

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
			if (i % 2 == 1) //&& gwb->GetNode(x,y).bq >= BQ_FLAG) // TODO warum geht nicht mit bq-Abfrage?
			{
				gcs.push_back(new gc::SetFlag(x,y));
			}
		}
		return true;
	}
	return false;
}

bool AIPlayerJH::FindWood(MapCoord x, MapCoord y, MapCoord &wood_x, MapCoord &wood_y)
{
	// Suchradius erstmal fix
	const int radius = 10;

	unsigned treeCount = 0;
	unsigned tree_x = 0;
	unsigned tree_y = 0;

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				if(gwb->GetNO(tx2,ty2)->GetType() == NOP_TREE)
				{
					treeCount++;
					tree_x += tx2;
					tree_y += ty2;
				}
			}
		}
	}
	if (treeCount > 0)
	{
		wood_x = tree_x / treeCount;
		wood_y = tree_y / treeCount;
	}

	return (treeCount > 0);
}

bool AIPlayerJH::BuildNear(MapCoord x, MapCoord y, BuildingType bld)
{
	const int radius = 10;
	BuildingQuality req = BUILDING_SIZE[bld];

	for(MapCoord tx=gwb->GetXA(x,y,0), r=1;r<=radius;tx=gwb->GetXA(tx,y,0),++r)
	{
		MapCoord tx2 = tx, ty2 = y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwb->GetPointA(tx2,ty2,i%6),++r2)
			{
				BuildingQuality bq = gwb->CalcBQ(tx2, ty2, player->getPlayerID());

				switch(req)
				{
					case BQ_HUT: if(!((bq >= BQ_HUT && bq <= BQ_CASTLE) || bq == BQ_HARBOR)) continue; break;
					case BQ_HOUSE: if(!((bq >= BQ_HOUSE && bq <= BQ_CASTLE) || bq == BQ_HARBOR)) continue; break;
					case BQ_CASTLE: if(!( bq == BQ_CASTLE || bq == BQ_HARBOR)) continue; break;
					case BQ_HARBOR: if(bq != BQ_HARBOR) continue; break;
					case BQ_MINE: if(bq != BQ_MINE) continue; break;
					default: break;
				}

				gcs.push_back(new gc::SetBuildingSite(tx2, ty2, bld));
				return true;
			}
		}
	}
	return false;

}
