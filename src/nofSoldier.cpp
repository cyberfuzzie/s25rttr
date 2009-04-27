// $Id: nofSoldier.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "nofSoldier.h"

#include "nobMilitary.h"
#include "Loader.h"
#include "GameConsts.h"
#include "Random.h"
#include "GameWorld.h"
#include "noFighting.h"
#include "GameClient.h"
#include "SerializedGameData.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofSoldier::nofSoldier(const unsigned short x, const unsigned short y,const unsigned char player,
					   nobBaseMilitary * const goal ,nobBaseMilitary * const home,const unsigned char rank)
: noFigure(static_cast<Job>(JOB_PRIVATE+rank),x,y,player,goal), building(home), hitpoints(HITPOINTS[GAMECLIENT.GetPlayer(player)->nation][rank])
{
}

nofSoldier::nofSoldier(const unsigned short x, const unsigned short y,const unsigned char player,
		nobBaseMilitary * const home,const unsigned char rank)
: noFigure(static_cast<Job>(JOB_PRIVATE+rank),x,y,player), building(home), hitpoints(HITPOINTS[GAMECLIENT.GetPlayer(player)->nation][rank])
{
}

void nofSoldier::Serialize_nofSoldier(SerializedGameData * sgd) const
{
	Serialize_noFigure(sgd);

	if(fs != FS_WANDER && fs != FS_GOHOME)
		sgd->PushObject(building,false);

	sgd->PushUnsignedChar(hitpoints);
}

nofSoldier::nofSoldier(SerializedGameData * sgd, const unsigned obj_id) : noFigure(sgd,obj_id)
{
	if(fs != FS_WANDER && fs != FS_GOHOME)
		building = sgd->PopObject<nobBaseMilitary>(GOT_UNKNOWN);
	else
		building = 0;

	hitpoints = sgd->PopUnsignedChar();
}

void nofSoldier::DrawSoldierWalking(int x, int y)
{
	DrawWalking(x,y,GetBobFile(jobs_bob),30+NATION_RTTR_TO_S2[GAMECLIENT.GetPlayer(player)->nation]*6+job-JOB_PRIVATE,false);
}

void nofSoldier::AbrogateWorkplace()
{
	// Militärgebäude Bescheid sagen, dass ich nicht kommen kann
	if(building)
	{
		static_cast<nobMilitary*>(building)->SoldierLost(this);
		building = 0;
	}
}
