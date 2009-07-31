// $Id: nofAggressiveDefender.cpp
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
#include "nofAggressiveDefender.h"
#include "nofAttacker.h"
#include "nofPassiveSoldier.h"
#include "nobMilitary.h"
#include "Loader.h"
#include "GameClient.h"
#include "GameConsts.h"
#include "Random.h"
#include "GameWorld.h"
#include "noFighting.h"
#include "SerializedGameData.h"
#include "MapGeometry.h"
#include "nobBaseWarehouse.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofAggressiveDefender::nofAggressiveDefender(const unsigned short x, const unsigned short y,const unsigned char player,
nobBaseMilitary * const home,const unsigned char rank,nofAttacker * const attacker)
: nofActiveSoldier(x,y,player,home,rank,STATE_AGGRESSIVEDEFENDING_WALKINGTOAGGRESSOR), attacker(attacker),
attacked_goal(attacker->GetAttackedGoal())
{
	// Angegriffenem Gebäude Bescheid sagen
	attacked_goal->LinkAggressiveDefender(this);
}

nofAggressiveDefender::nofAggressiveDefender(nofPassiveSoldier * other,nofAttacker * const attacker)
: nofActiveSoldier(*other,STATE_AGGRESSIVEDEFENDING_WALKINGTOAGGRESSOR), attacker(attacker),
attacked_goal(attacker->GetAttackedGoal())
{
	// Angegriffenem Gebäude Bescheid sagen
	attacked_goal->LinkAggressiveDefender(this);
}

nofAggressiveDefender::~nofAggressiveDefender()
{
	//assert(GameClient::inst().GetPlayer(player)->GetFirstWH()->TestOnMission(this) == false);
}

void nofAggressiveDefender::Destroy_nofAggressiveDefender()
{
	Destroy_nofActiveSoldier(); 

	//// Debugging
	//assert(GameClient::inst().GetPlayer(player)->GetFirstWH()->TestOnMission(this) == false);
}

void nofAggressiveDefender::Serialize_nofAggressiveDefender(SerializedGameData * sgd) const
{
	Serialize_nofActiveSoldier(sgd);

	if(state != STATE_WALKINGHOME && state != STATE_FIGUREWORK)
	{
		sgd->PushObject(attacker,true);
		sgd->PushObject(attacked_goal,false);
	}
}

nofAggressiveDefender::nofAggressiveDefender(SerializedGameData * sgd, const unsigned obj_id) : nofActiveSoldier(sgd,obj_id)
{
	if(state != STATE_WALKINGHOME && state != STATE_FIGUREWORK)
	{
		attacker = sgd->PopObject<nofAttacker>(GOT_NOF_ATTACKER);
		attacked_goal = sgd->PopObject<nobBaseMilitary>(GOT_UNKNOWN);
	}
	else
	{
		attacker = 0;
		attacked_goal = 0;
	}
}

void nofAggressiveDefender::Walked()
{
	// Was bestimmtes machen, je nachdem welchen Status wir gerade haben
	switch(state)
	{
	case STATE_WALKINGHOME:
		{
			WalkingHome();
		} break;
	case STATE_AGGRESSIVEDEFENDING_WALKINGTOAGGRESSOR:
		{
			MissAggressiveDefendingWalk();
		} break;
	default:
		break;
	}
}

/// Wenn ein Heimat-Militärgebäude bei Missionseinsätzen zerstört wurde
void nofAggressiveDefender::HomeDestroyed()
{
	switch(state)
	{
	case STATE_WALKINGHOME:
	case STATE_WAITINGFORFIGHT:
	case STATE_AGGRESSIVEDEFENDING_WALKINGTOAGGRESSOR:
	case STATE_AGGRESSIVEDEFENDING_FIGHTING:
		{
			// Die normale Tätigkeit wird erstmal fortgesetzt (Laufen, Kämpfen, wenn er schon an der Fahne ist
			// wird er auch nicht mehr zurückgehen)
			building = 0;
		} break;
	default:
		break;
	}
}

void nofAggressiveDefender::HomeDestroyedAtBegin()
{
	building = 0;

	// angegriffenem Gebäude Bescheid sagen, dass wir doch nicht mehr kommen
	if(attacked_goal)
	{
		attacked_goal->UnlinkAggressiveDefender(this);
		attacked_goal = 0;
	}

	state = STATE_FIGUREWORK;

	// Angreifer Bescheid sagen, dass wir nicht mehr kommen
	attacker->AggressiveDefenderLost();

	// Rumirren
	StartWandering();
	StartWalking(RANDOM.Rand(__FILE__,__LINE__,obj_id,6));
}

/// Wenn ein Kampf gewonnen wurde
void nofAggressiveDefender::WonFighting()
{
	// Angreifer tot
	attacker  = 0;

	// Ist evtl. unser Heimatgebäude zerstört?
	if(!building)
	{
		// Rumirren
		state = STATE_FIGUREWORK;
		StartWandering();
		Wander();

		// Ziel Bescheid sagen
		if(attacked_goal)
			attacked_goal->UnlinkAggressiveDefender(this);

		return;
	}

	// Angreifer ist tot, nach anderen suchen, die in meiner Nähe sind und mich evtl noch mit denen kloppen
	MissionAggressiveDefendingLookForNewAggressor();
}

/// Wenn ein Kampf verloren wurde (Tod)
void nofAggressiveDefender::LostFighting()
{
	// Meinem zu Hause Bescheid sagen, dass ich nicht mehr lebe (damit neue Truppen reinkönnen),
	// falls es noch existiert
	if(building)
		building->SoldierLost(this);

		// Ziel Bescheid sagen, das ich verteidigt hatte
	if(attacked_goal)
		attacked_goal->UnlinkAggressiveDefender(this);
}


void nofAggressiveDefender::MissionAggressiveDefendingLookForNewAggressor()
{
	// Wenns das Zielgebäude nich mehr gibt, gleich nach Hause gehen!
	if(!attacked_goal)
	{
		ReturnHomeMissionAggressiveDefending();
		return;
	}

	/// Vermeiden, dass in FindAggressor nochmal der Soldat zum Loslaufen gezwungen wird, weil er als state
	// noch drinstehen hat, dass er auf einen Kampf wartet
	state = STATE_AGGRESSIVEDEFENDING_WALKINGTOAGGRESSOR;

	// nach anderen suchen, die in meiner Nähe sind und mich evtl noch mit denen kloppen
	if((attacker = attacked_goal
		->FindAggressor(this)))
	{
		// zum Angreifer gehen und mit ihm kämpfen
		MissAggressiveDefendingWalk();
	}
	else
	{
		// keiner will mehr mit mir kämpfen, dann geh ich halt wieder nach Hause
		ReturnHomeMissionAggressiveDefending();
	}
}

void nofAggressiveDefender::AttackedGoalDestroyed()
{
	if(attacker)
	{
		attacker->AggressiveDefenderLost();
		attacker = 0;
	}
	attacked_goal = 0;

	// Stehen wir? Dann losgehen
	if(state == STATE_WAITINGFORFIGHT)
		ReturnHome();
}

void nofAggressiveDefender::MissAggressiveDefendingContinueWalking()
{
	state =  STATE_AGGRESSIVEDEFENDING_WALKINGTOAGGRESSOR;
	MissAggressiveDefendingWalk();
}


void nofAggressiveDefender::MissAggressiveDefendingWalk()
{
	// Ist evtl. unser Heimatgebäude zerstört?
	if(!building)
	{
		// evtl. noch dem Angreifer Bescheid sagen, falls einer existiert
		if(attacker)
		{
			attacker->AggressiveDefenderLost();
			attacker = 0;
		}

		// Ziel Bescheid sagen
		if(attacked_goal)
		{
			attacked_goal->UnlinkAggressiveDefender(this);
			attacked_goal = 0;
		}

		// Rumirren
		state = STATE_FIGUREWORK;
		StartWandering();
		Wander();

		

		return;
	}

	// Wenns das Zielgebäude nich mehr gibt, gleich nach Hause gehen!
	if(!attacked_goal)
	{
		ReturnHomeMissionAggressiveDefending();
		return;
	}

	// Gibts den Angreifer überhaupt noch?
	if(attacker)
		// Zum Angreifer laufen
		dir = gwg->FindHumanPath(x,y,attacker->GetX(),
			attacker->GetY(),100,true);
	else
	{
		// Ansonsten gehts wieder zurück nach Hause
		ReturnHomeMissionAggressiveDefending();
		return;
	}

	// Bin ich schon beim Angreifer, der wartet?
	if(x == attacker->GetX() && y == attacker->GetY() 
		&& (attacker->GetState() == nofActiveSoldier::STATE_WAITINGFORFIGHT 
		|| attacker->GetState() == nofActiveSoldier::STATE_ATTACKING_WAITINGAROUNDBUILDING))
	{
		// Dann die Köpfe rollen lassen...
		gwg->AddFigure(new noFighting(attacker,this),x,y);

		state = STATE_AGGRESSIVEDEFENDING_FIGHTING;
		attacker->state = STATE_ATTACKING_FIGHTINGVSAGGRESSIVEDEFENDER;
		return;
	}


	if(dir == 0xFF)
	{
		// Nach Hause zurück gehen
		ReturnHomeMissionAggressiveDefending();
	}
	else
	{
		// ist der Angreifer in meiner Nähe und wartet er noch nicht auf mich
		// und ist da noch Platz für einen Kampf?
		// Wenn er schon vor dem Militärgebäude wartet, kann ich nicht auf ihn warten
		if(CalcDistance(x,y,attacker->GetX(),attacker->GetY())<3 &&
			attacker->state != STATE_WAITINGFORFIGHT &&
			attacker->state != STATE_ATTACKING_WAITINGAROUNDBUILDING &&
			gwg->ValidPointForFighting(x,y) &&
			!(x == attacked_goal->GetX()+(attacked_goal->GetY()&1)
			&& y == attacked_goal->GetY()+1))
		{
			// Dann warte ich ganz fein auf ihn
			state = STATE_WAITINGFORFIGHT;
		}
		else
		{
			// hinlaufen
			StartWalking(dir);
		}
	}
}

void nofAggressiveDefender::ReturnHomeMissionAggressiveDefending()
{
	// Zielen Bescheid sagen
	InformTargetsAboutCancelling();
	// Und nach Hause gehen
	ReturnHome();
}

void nofAggressiveDefender::AttackerLost()
{
	attacker = 0;

	// Wenn wir auf die gewartet hatten, müssen wir uns einen neuen Angreifer suchen
	if(state == STATE_WAITINGFORFIGHT)
		MissionAggressiveDefendingLookForNewAggressor();
}


void nofAggressiveDefender::NeedForHomeDefence()
{
	// Angreifer Bescheid sagen
	if(attacker)
	{
		attacker->AggressiveDefenderLost();
		attacker = 0;
	}
	// Ziel Bescheid sagen
	if(attacked_goal)
	{
		attacked_goal->UnlinkAggressiveDefender(this);
		attacked_goal = 0;
	}
}

/// Sagt den verschiedenen Zielen Bescheid, dass wir doch nicht mehr kommen können
void nofAggressiveDefender::InformTargetsAboutCancelling()
{
	// Angreifer Bescheid sagen
	if(attacker)
	{
		attacker->AggressiveDefenderLost();
		attacker = 0;
	}
	// Ziel Bescheid sagen
	if(attacked_goal)
	{
		attacked_goal->UnlinkAggressiveDefender(this);
		attacked_goal = 0;
	}
}

