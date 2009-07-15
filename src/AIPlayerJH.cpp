// $Id: AIPlayerJH.cpp 5272 2009-07-15 16:38:14Z FloSoft $
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

// from Pathfinding.cpp
bool IsPointOK_RoadPath(const GameWorldBase& gwb, const MapCoord x, const MapCoord y, const unsigned char dir, const void *param);

AIPlayerJH::AIPlayerJH(const unsigned char playerid, const GameWorldBase * const gwb, const GameClientPlayer * const player,
		const GameClientPlayerList * const players, const GlobalGameSettings * const ggs,
		const AI::Level level) : AIBase(playerid, gwb, player, players, ggs, level)
{
	lastX = 1;
	lastY = 1;
}

/// Wird jeden GF aufgerufen und die KI kann hier entsprechende Handlungen vollziehen
void AIPlayerJH::RunGF(const unsigned gf)
{
	// nach neuem Barrackenplatz suchen
	if(gf % 10 == 0)
	{
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

		gcs.push_back(new gc::SetBuildingSite(x, y, BLD_BARRACKS));
		lastX = x;
		lastY = y;
		
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

			//gcs.push_back(new gc::SetFlag(x,y));
			gcs.push_back(new gc::SetBuildingSite(x, y, BLD_BARRACKS));
		}
	}

	if (gf % 50 == 20)
	{
		GameWorldBase *gwb_ = const_cast<GameWorldBase*>(gwb);
		const noBuildingSite *mil = gwb->GetSpecObj<noBuildingSite>(lastX, lastY);
		if (false)
		{
			std::vector<unsigned char> new_route;
			const nobHQ *hq = gwb->GetSpecObj<nobHQ>(player->hqx, player->hqy);
			Param_RoadPath prp = { false };
			
			bool path_found = gwb_->FindFreePath(mil->GetFlag()->GetX(),mil->GetFlag()->GetY(),
				hq->GetFlag()->GetX(),hq->GetFlag()->GetY(),false,100,&new_route,NULL,NULL,NULL,IsPointOK_RoadPath,NULL, &prp);
				//gwv->FindRoadPath(mil->GetFlag()->GetX(),mil->GetFlag()->GetY(),
				//hq->GetFlag()->GetX(),hq->GetFlag()->GetY(),new_route, false);
			if (path_found)
				gcs.push_back(new gc::BuildRoad(mil->GetFlag()->GetX(), mil->GetFlag()->GetY(), false, new_route));

		}

		noFlag *hqflag = gwb->GetSpecObj<nobHQ>(player->hqx, player->hqy)->GetFlag();
		const std::list<noBuildingSite*> bs = player->GetBuildingSites();
		for (std::list<noBuildingSite*>::const_iterator it = bs.begin(); it != bs.end(); ++it)
		{
			unsigned char first_dir = 0xFF;
			if (!gwb_->FindPathOnRoads((*it)->GetFlag(), hqflag, false, NULL, NULL, &first_dir, NULL))
			{
				std::vector<const noFlag*> flags;
				FindFlags(flags, (*it)->GetFlag()->GetX(), (*it)->GetFlag()->GetY(), 9);

				unsigned shortest = 0;
				unsigned int shortestLength = 99999;
				bool found = false;
				
				for(unsigned i=0; i<flags.size(); ++i)
				{
					std::vector<unsigned char> new_route2;
					unsigned int length;
					Param_RoadPath prp = { false };
					
					bool path_found = gwb_->FindFreePath((*it)->GetFlag()->GetX(),(*it)->GetFlag()->GetY(),
						flags[i]->GetX(),flags[i]->GetY(),false,100,&new_route2,&length,NULL,NULL,IsPointOK_RoadPath,NULL, &prp);
					if (path_found)
					{
						unsigned char first_dir;
						unsigned int hqlength = 0;
						bool hq_path_found = gwb_->FindPathOnRoads(flags[i], hqflag, false, NULL, &hqlength, &first_dir, NULL);
						found = true;
						if (length+hqlength < shortestLength)
						{
							shortest = i;
							shortestLength = length+hqlength;
						}
					}
				}
				if (found)
				{
					std::vector<unsigned char> new_route2;
					Param_RoadPath prp = { false };
					
					bool path_found = gwb_->FindFreePath((*it)->GetFlag()->GetX(),(*it)->GetFlag()->GetY(),
						flags[shortest]->GetX(),flags[shortest]->GetY(),false,100,&new_route2,NULL,NULL,NULL,IsPointOK_RoadPath,NULL, &prp);
					gcs.push_back(new gc::BuildRoad((*it)->GetFlag()->GetX(), (*it)->GetFlag()->GetY(), false, new_route2));
					break;
				}

				
			}
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
