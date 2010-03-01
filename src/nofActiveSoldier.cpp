// $Id: nofActiveSoldier.cpp 5853 2010-01-04 16:14:16Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header

#include "main.h"
#include "nofActiveSoldier.h"
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

nofActiveSoldier::nofActiveSoldier(const unsigned short x, const unsigned short y,const unsigned char player,
				 nobBaseMilitary * const home,const unsigned char rank, const SoldierState init_state)
				 : nofSoldier(x,y,player,home,rank), state(init_state)

{
}

nofActiveSoldier::nofActiveSoldier(const nofSoldier& other, const SoldierState init_state) :
  nofSoldier(other), state(init_state) {}


void nofActiveSoldier::Serialize_nofActiveSoldier(SerializedGameData * sgd) const
{
	Serialize_nofSoldier(sgd);

	sgd->PushUnsignedChar(static_cast<unsigned char>(state));
}


nofActiveSoldier::nofActiveSoldier(SerializedGameData * sgd, const unsigned obj_id) : nofSoldier(sgd,obj_id),
state(SoldierState(sgd->PopUnsignedChar()))
{
}



void nofActiveSoldier::GoalReached()
{
	// Dürfte nicht passieren!
	assert(false);
}

void nofActiveSoldier::ReturnHome()
{
	state = STATE_WALKINGHOME;
	WalkingHome();
}


void nofActiveSoldier::WalkingHome()
{
	// Ist evtl. unser Heimatgebäude zerstört?
	if(!building)
	{
		// Rumirren
		state = STATE_FIGUREWORK;
		StartWandering();
		Wander();

		return;
	}


	// Wieder zurück nach Hause laufen

	// Sind wir schon an der Flagge?
	if(x == building->GetFlag()->GetX() && y == building->GetFlag()->GetY())
	{
		// Dann INS Gebäude laufen
		StartWalking(1);
	}
	// oder vielleicht auch schon im Gebäude drin?
	else if(x == building->GetX() && y == building->GetY())
	{
		// Wir sind da!
		building->AddActiveSoldier(this);
		// Mich entfernen hier
		gwg->RemoveFigure(this,x,y);
		// Wir sind jetzt auch nicht mehr auf Mission
		building->SoldierLost(this);
	}
	// oder finden wir gar keinen Weg mehr?
	else if((dir = gwg->FindHumanPath(x,y,building->GetFlag()->GetX(),building->GetFlag()->GetY(),100)) == 0xFF)
	{
		// Kein Weg gefunden --> Rumirren
		StartWandering();
		state = STATE_FIGUREWORK;
		Wander();

		// Dem Heimatgebäude Bescheid sagen
		building->SoldierLost(this);
	}
	// oder ist alles ok? :)
	else
	{
		// nach Hause laufen
		StartWalking(dir);
	}
}


void nofActiveSoldier::Draw(int x, int y)

{
	switch(state)
	{
	default:
		break;
	case STATE_WAITINGFORFIGHT:
	case STATE_ATTACKING_WAITINGAROUNDBUILDING:
	case STATE_ATTACKING_WAITINGFORDEFENDER:
	case STATE_DEFENDING_WAITING:
		{
			// wenn er wartet, steht er nur rum
			LOADER.GetBobN("jobs")->Draw(30+NATION_RTTR_TO_S2[GAMECLIENT.GetPlayer(player)->nation]*6+job-JOB_PRIVATE,
				dir,false,2,x,y,COLORS[GAMECLIENT.GetPlayer(player)->color]);
			DrawShadow(x,y,2,dir);
		} break;
	case STATE_FIGUREWORK:
	case STATE_ATTACKING_WALKINGTOGOAL:
	case STATE_AGGRESSIVEDEFENDING_WALKINGTOAGGRESSOR:
	case STATE_WALKINGHOME:
	case STATE_DEFENDING_WALKINGTO:
	case STATE_DEFENDING_WALKINGFROM:
	case STATE_ATTACKING_CAPTURINGFIRST:
	case STATE_ATTACKING_CAPTURINGNEXT:
	case STATE_ATTACKING_ATTACKINGFLAG:
	case STATE_ATTACKING_WALKINGTOFIGHTSPOT:
		{
			DrawSoldierWalking(x,y);
		} break;
	}

	/*char str[255];
	sprintf(str,"%u",obj_id);
	LOADER.GetFontN("resource",0)->Draw(x,y,str,0,0xFFFF0000);*/
}

void nofActiveSoldier::HandleDerivedEvent(const unsigned int id)
{
	// Das dürfte nich aufgerufen werden!
	assert(false);
}
