// $Id: nofAttacker.cpp
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
#include "nofAttacker.h"
#include "nofDefender.h"
#include "nofAggressiveDefender.h"
#include "nofPassiveSoldier.h"
#include "nobMilitary.h"
#include "Loader.h"
#include "GameClient.h"
#include "GameConsts.h"
#include "Random.h"
#include "GameWorld.h"
#include "noFighting.h"
#include "SerializedGameData.h"
#include "nobBaseWarehouse.h"
#include "MapGeometry.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


/// Nach einer bestimmten Zeit, in der der Angreifer an der Flagge des Gebäudes steht, blockt er den Weg
/// nur benutzt bei STATE_ATTACKING_WAITINGFORDEFENDER
/// Dieses Konstante gibt an, wie lange, nachdem er anfängt da zu stehen, er blockt
const unsigned BLOCK_OFFSET = 10;
	

nofAttacker::nofAttacker(nofPassiveSoldier * other,nobBaseMilitary * const attacked_goal)
: nofActiveSoldier(*other,STATE_ATTACKING_WALKINGTOGOAL), attacked_goal(attacked_goal), defender(false),
should_haunted(GAMECLIENT.GetPlayer(attacked_goal->GetPlayer())->ShouldSendDefender()), blocking_event(0)
{
	// Dem Haus Bescheid sagen
	static_cast<nobMilitary*>(building)->SoldierOnMission(other,this);
	// Das Haus soll uns rausschicken
	building->AddLeavingFigure(this);
	// Dem Ziel Bescheid sagen
	attacked_goal->LinkAggressor(this);
}

nofAttacker::~nofAttacker()
{
	
	//unsigned char oplayer = (player == 0) ? 1 : 0;
	//assert(GameClient::inst().GetPlayer(oplayer)->GetFirstWH()->Test(this) == false);
}

void nofAttacker::Destroy_nofAttacker() 
{
	Destroy_nofActiveSoldier();

	/*unsigned char oplayer = (player == 0) ? 1 : 0;
	assert(GameClient::inst().GetPlayer(oplayer)->GetFirstWH()->Test(this) == false);*/
}


void nofAttacker::Serialize_nofAttacker(SerializedGameData * sgd) const
{
	Serialize_nofActiveSoldier(sgd);

	if(state != STATE_WALKINGHOME && state != STATE_FIGUREWORK)
	{
		sgd->PushObject(attacked_goal,false);
		sgd->PushObject(defender,false);
		sgd->PushBool(should_haunted);
		sgd->PushUnsignedShort(radius);

		if(state == STATE_ATTACKING_WAITINGFORDEFENDER)
			sgd->PushObject(blocking_event,true);
	}
}

nofAttacker::nofAttacker(SerializedGameData * sgd, const unsigned obj_id) : nofActiveSoldier(sgd,obj_id)
{
	if(state != STATE_WALKINGHOME && state != STATE_FIGUREWORK)
	{
		attacked_goal = sgd->PopObject<nobBaseMilitary>(GOT_UNKNOWN);
		defender = sgd->PopObject<nofActiveSoldier>(GOT_UNKNOWN);
		should_haunted = sgd->PopBool();
		radius = sgd->PopUnsignedShort();

		if(state == STATE_ATTACKING_WAITINGFORDEFENDER)
			blocking_event = sgd->PopObject<EventManager::Event>(GOT_EVENT);
	}
	else
	{
		attacked_goal = 0;
		defender = 0;
		should_haunted = 0;
		radius = 0;
	}

}


void nofAttacker::Walked()
{
	// Was bestimmtes machen, je nachdem welchen Status wir gerade haben
	switch(state)
	{
	default:
		break;
	case STATE_WALKINGHOME:
		{
			WalkingHome();
		} break;
	case STATE_ATTACKING_WALKINGTOGOAL:
		{
			MissAttackingWalk();
		} break;
	case STATE_ATTACKING_ATTACKINGFLAG:
		{
			// Ist evtl. das Zielgebäude zerstört?
			if(!attacked_goal)
			{
				// Nach Hause gehen
				ReturnHomeMissionAttacking();

				return;
			}

			unsigned short flag_x = attacked_goal->GetFlag()->GetX(),
				flag_y = attacked_goal->GetFlag()->GetY();
			// Sind wir schon da?
			if(x == flag_x && y == flag_y)
			{
				// Mit Verteidiger Kampf beginnen
				gwg->AddFigure(new noFighting(this,defender),x,y);

				state = STATE_ATTACKING_FIGHTINGVSDEFENDER;
				static_cast<nofDefender*>(defender)->state = STATE_DEFENDING_FIGHTING;
			}
			else
			{
				if( (dir = gwg->FindFreePath(x,y,flag_x,flag_y,5)) == 0xFF)
				{
					// es wurde kein Weg mehr gefunden --> neues Plätzchen suchen und warten
					state = STATE_ATTACKING_WALKINGTOGOAL;
					MissAttackingWalk();
					// der Verteidiger muss darüber informiert werden
					static_cast<nofDefender*>(defender)->AttackerArrested();
				}
				else
				{
					// Hinlaufen
					StartWalking(dir);
				}
			}
		} break;
	case STATE_ATTACKING_CAPTURINGFIRST:
		{
			// Ist evtl. das Zielgebäude zerstört?
			if(!attacked_goal)
			{
				// Nach Hause gehen
				ReturnHomeMissionAttacking();

				return;
			}

			
			// Wenn schon welche drin sind, ist wieder ein feindlicher reingegangen
			if(attacked_goal->DefendersAvailable())
			{
				// Wieder rausgehen, Platz reservieren
				if(attacked_goal->GetGOT() == GOT_NOB_MILITARY)
					static_cast<nobMilitary*>(attacked_goal)->StopCapturing();

				state = STATE_ATTACKING_WALKINGTOGOAL;
				StartWalking(4);
				return;
			}
			else
			{
				// Ist das Gebäude ein "normales Militärgebäude", das wir da erobert haben?
				if(attacked_goal->GetBuildingType() >= BLD_BARRACKS &&
				attacked_goal->GetBuildingType() <= BLD_FORTRESS)
				{
					// Meinem Heimatgebäude Bescheid sagen, dass ich nicht mehr komme (falls es noch eins gibt)
					if(building)
						building->SoldierLost(this);
					// Gebäude einnehmen
					static_cast<nobMilitary*>(attacked_goal)->Capture(player);
					// Bin nun kein Angreifer mehr
					attacked_goal->UnlinkAggressor(this);
					// Das ist nun mein neues zu Hause
					building = attacked_goal;
					// mich zum Gebäude hinzufügen und von der Karte entfernen
					attacked_goal->AddActiveSoldier(this);
					gwg->RemoveFigure(this,x,y);
					// ggf. weitere Soldaten rufen, damit das Gebäude voll wird
					static_cast<nobMilitary*>(attacked_goal)->NeedOccupyingTroops(player);
				}
				// oder ein Hauptquartier oder Hafen?
				else
				{
					// abreißen
					attacked_goal->Destroy();
					delete attacked_goal;
					ReturnHomeMissionAttacking();
				}
			}
		} break;
	case STATE_ATTACKING_CAPTURINGNEXT:
		{
			CapturingWalking();
		} break;
	}
}


/// Wenn ein Heimat-Militärgebäude bei Missionseinsätzen zerstört wurde
void nofAttacker::HomeDestroyed()
{
	switch(state)
	{
	case STATE_ATTACKING_WAITINGAROUNDBUILDING:
		{
			// Hier muss sofort reagiert werden, da man steht

			// Angreifer muss zusätzlich seinem Ziel Bescheid sagen
			attacked_goal->UnlinkAggressor(this);
			// Kollegen Bescheid sagen
			if(defender)
				static_cast<nofAggressiveDefender*>(defender)->AttackerLost();

			defender = 0;

			// Rumirren
			building = 0;
			state = STATE_FIGUREWORK;
			StartWandering();
			Wander();

			// und evtl einen Nachrücker für diesen Platz suchen
			attacked_goal->SendSuccessor(x,y,radius,dir);

		

		} break;

	case STATE_WAITINGFORFIGHT:
	case STATE_WALKINGHOME:
	case STATE_ATTACKING_WALKINGTOGOAL:
	case STATE_ATTACKING_CAPTURINGFIRST:
	case STATE_ATTACKING_CAPTURINGNEXT:
	case STATE_ATTACKING_ATTACKINGFLAG:
	case STATE_ATTACKING_WAITINGFORDEFENDER:
	case STATE_ATTACKING_FIGHTINGVSAGGRESSIVEDEFENDER:
	case STATE_ATTACKING_FIGHTINGVSDEFENDER:
		{
			//  Die normale Tätigkeit wird erstmal fortgesetzt (Laufen, Kämpfen, wenn er schon an der Fahne ist
			// wird er auch nicht mehr zurückgehen)
			building = 0;
		} break;
	default:
		break;
	}

}

void nofAttacker::HomeDestroyedAtBegin()
{
	building = 0;

	// angegriffenem Gebäude Bescheid sagen, dass wir doch nicht mehr kommen
	if(attacked_goal)
	{
		attacked_goal->UnlinkAggressor(this);
		attacked_goal = 0;
	}

	state = STATE_FIGUREWORK;

	// Rumirren
	StartWandering();
	StartWalking(RANDOM.Rand(__FILE__,__LINE__,obj_id,6));
}

/// Wenn ein Kampf gewonnen wurde
void nofAttacker::WonFighting()
{
	defender = 0;

	// Ist evtl. unser Heimatgebäude zerstört?
	if(!building && state != STATE_ATTACKING_FIGHTINGVSDEFENDER)
	{
		// Dann dem Ziel Bescheid sagen, falls es existiert (evtl. wurdes zufällig zur selben Zeit zerstört)
		if(attacked_goal)
		{
			attacked_goal->UnlinkAggressor(this);
			attacked_goal = 0;
		}

		// Rumirren
		state = STATE_FIGUREWORK;
		StartWandering();
		Wander();

		return;
	}


	// Ist evtl. unser Ziel-Gebäude zerstört?
	if(!attacked_goal)
	{
		// Nach Hause gehen
		ReturnHomeMissionAttacking();

		return;
	}

	// Greifen wir grad ein Gebäude an?
	if(state == STATE_ATTACKING_FIGHTINGVSDEFENDER)
	{
		// Dann neuen Verteidiger rufen
		if(attacked_goal->CallDefender(this))
		{
			// Verteidiger gefunden --> hinstellen und auf ihn warten
			SwitchStateAttackingWaitingForDefender();
		}
		else
		{
			// kein Verteidiger gefunden --> ins Gebäude laufen und es erobern
			state = STATE_ATTACKING_CAPTURINGFIRST;
			StartWalking(1);

			// Normalen Militärgebäuden schonmal Bescheid sagen
			if(attacked_goal->GetGOT() == GOT_NOB_MILITARY)
				static_cast<nobMilitary*>(attacked_goal)->PrepareCapturing();
		}
	}
	else
	{
		// aggressiver Verteidiger ist tot
		defender = 0;
		// weiterlaufen
		state = STATE_ATTACKING_WALKINGTOGOAL;
		MissAttackingWalk();
	}
}

/// Wenn ein Kampf verloren wurde (Tod)
void nofAttacker::LostFighting()
{
	// Meinem zu Hause Bescheid sagen, dass ich nicht mehr lebe (damit neue Truppen reinkönnen)
	// falls das Gebäude noch existiert
	if(building)
		building->SoldierLost(this);

	// Angreifer müssen zusätzlich ihrem Ziel Bescheid sagen
	if(attacked_goal)
	{
		attacked_goal->UnlinkAggressor(this);
		attacked_goal = 0;
	}
}


void nofAttacker::ReturnHomeMissionAttacking()
{
	//SoldierState prev = state;

	// Kollegen Bescheid sagen, falls es einen gibt
	if(defender)
	{
		static_cast<nofAggressiveDefender*>(defender)->AttackerLost();
		defender = 0;
	}

	// Ziel Bescheid sagen, falls es das noch gibt
	if(attacked_goal)
	{
		attacked_goal->UnlinkAggressor(this);
		attacked_goal = 0;
	}

	//// Umstehenden Figuren Bescheid sagen
	//if(prev == STATE_ATTACKING_WAITINGAROUNDBUILDING)
	//	gwg->RoadNodeAvailable(this->x,this->y);


	ReturnHome();
}

void nofAttacker::MissAttackingWalk()
{
	// Ist evtl. unser Heimatgebäude zerstört?
	if(!building)
	{
		// Dann dem Ziel Bescheid sagen, falls es existiert (evtl. wurdes zufällig zur selben Zeit zerstört)
		if(attacked_goal)
		{
			attacked_goal->UnlinkAggressor(this);
			attacked_goal = 0;
		}

		// evtl. noch dem Verteidiger Bescheid sagen, falls einer existiert
		if(defender)
			static_cast<nofAggressiveDefender*>(defender)->AttackerLost();

		// Rumirren
		state = STATE_FIGUREWORK;
		StartWandering();
		Wander();


		return;
	}

	// Gibts das Ziel überhaupt noch?
	if(attacked_goal)
	{
		// Eine Position rund um das Militärgebäude suchen
		unsigned short goal_x,goal_y;
		unsigned char goal_dir = attacked_goal->FindAnAttackerPlace(goal_x,goal_y,radius,this);

		// Keinen Platz mehr gefunden?
		if(goal_x == 0xFFFF)
		{
			// Dann nach Haus gehen
			ReturnHomeMissionAttacking();
			return;
		}



		// Sind wir evtl schon da?
		if(x == goal_x && y == goal_y)
		{

			// Sind wir direkt an der Flagge?
			if(x == attacked_goal->GetX() + (attacked_goal->GetY()&1)  &&
				y == attacked_goal->GetY()+1)
			{
				// wenn wir einen aggressiv verteidigenden Kollegen hatten, muss dieser wieder zurückgeschickt
				// werden, da wir ja jetzt schon am Ziel sind
				if(defender)
				{
					static_cast<nofAggressiveDefender*>(defender)->AttackerLost();
					defender = 0;
				}

				// Dann Verteidiger rufen
				if(attacked_goal->CallDefender(this))
				{
					// Verteidiger gefunden --> hinstellen und auf ihn warten
					SwitchStateAttackingWaitingForDefender();
					

				}
				else
				{
					// kein Verteidiger gefunden --> ins Gebäude laufen und es erobern
					state = STATE_ATTACKING_CAPTURINGFIRST;
					StartWalking(1);
				}
			}
			else
			{
				// dann hinstellen und warten, bis wir an die Reihe kommmen mit Kämpfen und außerdem diesen Platz
				// reservieren, damit sich kein anderer noch hier hinstellt
				state = STATE_ATTACKING_WAITINGAROUNDBUILDING;
				// zur Flagge hin ausrichten
				dir = goal_dir;


				// Wenn unser aggressiv-verteidigender Kollege auf uns wartet, muss ihm Bescheid gesagt werden, damit
				// er weiterläuft sonst warten ja beide ewig aufeinander
				if(defender)
				{
					if(static_cast<nofAggressiveDefender*>(defender)->state == STATE_WAITINGFORFIGHT)
					{
						static_cast<nofAggressiveDefender*>(defender)->MissAggressiveDefendingContinueWalking();
					}
				}
			}

			return;
		}

		if(defender)
		{
			// Wenn der Verteidiger in der Nähe ist, zu ihm laufen
			// Achtung: nicht in die Hütte des Verteidigers laufen!!
			if(CalcDistance(x,y,defender->GetX(),defender->GetY())<5
				&& !(defender->GetX() == attacked_goal->GetX() &&
				defender->GetY() == attacked_goal->GetY() ))
				// zum Verteidiger laufen, falls er in der Nähe ist
				dir = gwg->FindFreePath(x,y,defender->GetX(),
					defender->GetY(),100);
			else
			// Zur Flagge des angegriffenen Gebäudes laufen
				dir = gwg->FindFreePath(x,y,goal_x,goal_y,100);
		}
		else
			// Zur Flagge des angegriffenen Gebäudes laufen
			dir = gwg->FindFreePath(x,y,goal_x,goal_y,100);
	}
	else
	{
		// Ansonsten findet man sowieso keinen Weg mehr dahin
		dir = 0xFF;
	}


	// Wenn man keinen Weg mehr findet, das Ziel nicht mehr existiert usw. nach Hause zurückkehren
	if(dir == 0xFF)
	{
		ReturnHomeMissionAttacking();
	}
	else
	{
		// Könnte mir noch ein neuer Verteidiger entgegenlaufen?
		if(should_haunted)
		{

			// 20%ige Chance, dass wirklich jemand angreift
			if(RANDOM.Rand(__FILE__,__LINE__,obj_id,10) < 2)
			{
				// Militärgebäude in der Nähe abgrasen
				list<nobBaseMilitary*> buildings;
				gwg->LookForMilitaryBuildings(buildings,x,y,2);

				for(list<nobBaseMilitary*>::iterator it = buildings.begin();it.valid();++it)
				{
					// darf kein HQ sein, außer, das HQ wird selbst angegriffen, darf nicht weiter weg als 15 sein
					// und es darf natürlich auch der entsprechende Feind sein, aber es darf auch nicht derselbe Spieler
					// wie man selbst sein, da das Gebäude ja z.B. schon erobert worden sein kann
					if(((*it)->GetBuildingType() != BLD_HEADQUARTERS || (*it) == attacked_goal)
						&& CalcDistance(x,y,(*it)->GetX(),(*it)->GetY()) < 15
						&& (*it)->GetPlayer() == attacked_goal->GetPlayer() && 
						!GameClient::inst().GetPlayer(player)->IsAlly((*it)->GetPlayer()))
					{
						// ggf. Verteidiger rufen
						if( (defender = (*it)->SendDefender(this)))
						{
							// nun brauchen wir keinen Verteidiger mehr
							should_haunted = false;
							break;
						}
					}
				}
			}

			// hinlaufen
			StartWalking(dir);
		}
		else if(defender)
		{
			// Bin ich schon beim Verteidiger?
			if(x == defender->GetX() && y == defender->GetY() && static_cast<nofAggressiveDefender*>(defender)->state == STATE_WAITINGFORFIGHT)
			{
				// Dann die Köpfe rollen lassen...
				gwg->AddFigure(new noFighting(defender,this),x,y);
				state = STATE_ATTACKING_FIGHTINGVSAGGRESSIVEDEFENDER;
				static_cast<nofAggressiveDefender*>(defender)->state = STATE_AGGRESSIVEDEFENDING_FIGHTING;
			}
			// ist der Verteidiger in meiner Nähe und wartet er noch nicht auf mich
			// und ist da noch Platz für einen Kampf?
			// Nicht an die Flagge des angegriffenen Militärgebäudes stellen!
			else if(CalcDistance(x,y,defender->GetX(),defender->GetY())<3 &&
				static_cast<nofAggressiveDefender*>(defender)->state != STATE_WAITINGFORFIGHT &&
				!(x == attacked_goal->GetX() + (attacked_goal->GetY()&1) && y == attacked_goal->GetY()+1))
			{
				// Können wir auf diesem Platz auch kämpfen?
				if(gwg->ValidPointForFighting(x,y))
				{
					// Dann warte ich ganz fein auf ihn
					state = STATE_WAITINGFORFIGHT;
				}
				else
				{
					// Nein, dann kämpfen wir halt nicht und wir sagen dem anderen, dass er wieder
					// gehen kann
					static_cast<nofAggressiveDefender*>(defender)->AttackerLost();
					defender = 0;
					StartWalking(dir);
				}
			}
			else
			{
				// hinlaufen
				StartWalking(dir);
			}

		}
		else
		{
			// hinlaufen
			StartWalking(dir);
		}
	}
}


void nofAttacker::AttackedGoalDestroyed()
{
	attacked_goal = 0;

	bool was_waiting_for_defender = (state == STATE_ATTACKING_WAITINGFORDEFENDER);

	// Wenn man gerade rumsteht, muss man sich bewegen
	if(state == STATE_ATTACKING_WAITINGFORDEFENDER ||
		state == STATE_ATTACKING_WAITINGAROUNDBUILDING ||
		state == STATE_WAITINGFORFIGHT)
		ReturnHomeMissionAttacking();

	if(was_waiting_for_defender)
	{
		// Block-Event ggf abmelden
		em->RemoveEvent(blocking_event);
		blocking_event = 0;
		gwg->RoadNodeAvailable(x,y);
	}
}

bool nofAttacker::AttackFlag(nofDefender * defender)
{
	// Zur Flagge laufen, findet er einen Weg?
	unsigned char tmp_dir = gwg->FindFreePath(x,y,attacked_goal->GetFlag()->GetX(),
		attacked_goal->GetFlag()->GetY(),3);

	if(tmp_dir != 0xFF)
	{
		// alte Richtung für Nachrücker merken
		unsigned char old_dir = dir;

		// Hat er drumrum gewartet?
		bool waiting_around_building = (state == STATE_ATTACKING_WAITINGAROUNDBUILDING);

		// Ja er hat einen Weg gefunden, also hinlaufen

		// Wenn er steht, muss er loslaufen
		if(waiting_around_building)
			StartWalking(tmp_dir);

		// Kollegen Bescheid sagen, falls es einen gibt
		if(this->defender)
		{
			static_cast<nofAggressiveDefender*>(this->defender)->AttackerLost();
			this->defender = 0;
		}

		state = STATE_ATTACKING_ATTACKINGFLAG;
		this->defender = defender;

		// Hatte er ums Gebäude gewartet?
		if(waiting_around_building)
		{
			// evtl. Nachrücker senden
			attacked_goal->SendSuccessor(x,y,radius,old_dir);
		}

		return true;
	}

	return false;
}
void nofAttacker::AttackFlag()
{
	//SoldierState prev = state;
	// "Normal" zur Flagge laufen
	state = STATE_ATTACKING_WALKINGTOGOAL;
	MissAttackingWalk();
	//// Umstehenden Figuren Bescheid sagen
	//if(prev == STATE_ATTACKING_WAITINGAROUNDBUILDING)
	//	gwg->RoadNodeAvailable(this->x,this->y);
}


void nofAttacker::CaptureBuilding()
{
	// Evtl Verteidiger Bescheid sagen
	if(defender)
	{
		static_cast<nofAggressiveDefender*>(defender)->AttackerLost();
		defender = 0;
	}

	// mit ins Militärgebäude gehen
	state = STATE_ATTACKING_CAPTURINGNEXT;
	// und hinlaufen
	CapturingWalking();
}

void nofAttacker::CapturingWalking()
{
	// Ist evtl. das Zielgebäude zerstört?
	if(!attacked_goal)
	{
		// Nach Hause gehen
		ReturnHomeMissionAttacking();

		return;
	}

	unsigned short flag_x = attacked_goal->GetFlag()->GetX(),
				flag_y = attacked_goal->GetFlag()->GetY();

	// Sind wir schon im Gebäude?
	if(x == attacked_goal->GetX() && y == attacked_goal->GetY())
	{
		// Meinem alten Heimatgebäude Bescheid sagen (falls es noch existiert)
		if(building)
			building->SoldierLost(this);
		// mich von der Karte tilgen-
		gwg->RemoveFigure(this,x,y);
		// Das ist nun mein neues zu Hause
		building = attacked_goal;
		// und zum Gebäude hinzufügen
		attacked_goal->AddActiveSoldier(this);

		// Ein erobernder Soldat weniger
		if(attacked_goal->GetBuildingType() >= BLD_BARRACKS &&
			attacked_goal->GetBuildingType() <= BLD_FORTRESS)
			static_cast<nobMilitary*>(attacked_goal)->CapturingSoldierArrived();

		// außerdem aus der Angreiferliste entfernen
		attacked_goal->UnlinkAggressor(this);
		attacked_goal = 0;
	
	}
	// oder zumindest schonmal an der Flagge?
	else if(x == flag_x && y == flag_y)
	{
		// ins Gebäude laufen
		StartWalking(1);
		// nächsten Angreifer ggf. rufen, der auch reingehen soll
		static_cast<nobMilitary*>(attacked_goal)->NeedOccupyingTroops(player);
	}
	else
	{
		// Ist evtl. unser Heimatgebäude zerstört?
		if(!building)
		{
			// Wenn noch das Ziel existiert (könnte ja zeitgleich abgebrannt worden sein)
			if(attacked_goal)
			{
				// Ziel Bescheid sagen
				attacked_goal->UnlinkAggressor(this);
				// Evtl. neue Besatzer rufen
				static_cast<nobMilitary*>(attacked_goal)->NeedOccupyingTroops(0xFF);

				attacked_goal = 0;
			}

			// Rumirren
			state = STATE_FIGUREWORK;
			StartWandering();
			Wander();

			return;
		}

		// weiter zur Flagge laufen
		if((dir = gwg->FindFreePath(x,y,flag_x,flag_y,10)) == 0xFF)
		{
			// auweia, es wurde kein Weg mehr gefunden

			// Evtl. neue Besatzer rufen
			static_cast<nobMilitary*>(attacked_goal)->NeedOccupyingTroops(0xFF);
			// Nach Hause gehen
			ReturnHomeMissionAttacking();
		}
		else
			StartWalking(dir);
	}
}

void nofAttacker::CapturedBuildingFull()
{
	switch(state)
	{
	default:
		break;

	case STATE_ATTACKING_WAITINGAROUNDBUILDING:
		{
			// nach Hause gehen
			ReturnHomeMissionAttacking();
		} break;
	case STATE_ATTACKING_WALKINGTOGOAL:
	case STATE_ATTACKING_WAITINGFORDEFENDER:
	case STATE_ATTACKING_ATTACKINGFLAG:
	case STATE_WAITINGFORFIGHT:
	case STATE_ATTACKING_FIGHTINGVSAGGRESSIVEDEFENDER:
		{
			// Bei allem anderen läuft man oder kämpft --> auf 0 setzen und wenn man fertig
			// mit der jetzigen Aktion ist, entsprechend handeln (nicht die Einnehmer darüber benachrichten, sonst
			// gehen die nicht rein)
			attacked_goal = 0;
		} break;
	}
}

void nofAttacker::StartSucceeding(const unsigned short x, const unsigned short y, const unsigned short new_radius, const unsigned char dir)
{
	// Wir sollen auf diesen Punkt nachrücken
	state = STATE_ATTACKING_WALKINGTOGOAL;

	// Kollegen Bescheid sagen, falls es einen gibt
	if(defender)
	{
		static_cast<nofAggressiveDefender*>(defender)->AttackerLost();
		defender = 0;
	}

	// Unsere alte Richtung merken für evtl. weitere Nachrücker
	unsigned char old_dir = this->dir;

	// unser alter Platz ist ja nun auch leer, da gibts vielleicht auch einen Nachrücker?
	attacked_goal->SendSuccessor(this->x,this->y,radius,old_dir);

	// Und schonmal loslaufen, da wir ja noch stehen
	MissAttackingWalk();

	
	// Neuen Radius speichern
	radius = new_radius;


	//// Wir sollen auf diesen Punkt nachrücken
	//state = STATE_ATTACKING_SUCCEEDING;
	//succeed_x = x;
	//succeed_y = y;
	//succeed_dir = dir;

	//// Kollegen Bescheid sagen, falls es einen gibt
	//if(defender)
	//{
	//	static_cast<nofAggressiveDefender*>(defender)->AttackerLost();
	//	defender = 0;
	//}

	//// Unsere alte Richtung merken für evtl. weitere Nachrücker
	//unsigned char old_dir = this->dir;

	//// Und schonmal loslaufen, da wir ja noch stehen
	//SucceedingWalk();

	//// unser alter Platz ist ja nun auch leer, da gibts vielleicht auch einen Nachrücker?
	///*bool succeeder =*/ attacked_goal->SendSuccessor(this->x,this->y,radius,old_dir);

	//// Neuen Radius speichern
	//radius = new_radius;
}


//void nofAttacker::SucceedingWalk()
//{
//	// Ist evtl. unser Heimatgebäude zerstört?
//	if(!building)
//	{
//		// Rumirren
//		state = STATE_FIGUREWORK;
//		StartWandering();
//		Wander();
//
//		// Dann dem Ziel Bescheid sagen, falls es existiert (evtl. wurdes zufällig zur selben Zeit zerstört)
//		if(attacked_goal)
//		{
//			attacked_goal->UnlinkAggressor(this);
//			// Neuen Nachrücker suchen
//			attacked_goal->SendSuccessor(this->x,this->y,radius,succeed_dir);
//
//			attacked_goal = 0;
//		}
//
//
//		// evtl. noch dem Verteidiger Bescheid sagen, falls einer existiert
//		if(defender)
//			static_cast<nofAggressiveDefender*>(defender)->AttackerLost();
//
//		return;
//	}
//
//	// Ist evtl. das Zielgebäude zerstört?
//	if(!attacked_goal)
//	{
//		// Nach Hause gehen
//		ReturnHomeMissionAttacking();
//
//		return;
//	}
//
//	// Sind wir schon da?
//	if(succeed_x == x && succeed_y == y)
//	{
//		// dann stellen wir uns hier hin
//		state = STATE_ATTACKING_WAITINGAROUNDBUILDING;
//		// entsprechend ausrichten
//		dir = succeed_dir;
//	}
//	else
//	{
//		// weiterlaufen
//		if((dir = gwg->FindFreePath(x,y,succeed_x,succeed_y,50)) == 0xFF)
//		{
//			// auweia, es wurde kein Weg mehr gefunden
//
//			state = STATE_ATTACKING_WALKINGTOGOAL;
//			// Neuen Nachrücker suchen
//			attacked_goal->SendSuccessor(succeed_x,succeed_y,radius,succeed_dir);
//			// Neuen Platz drumrum finden
//			MissAttackingWalk();
//
//		}
//		else
//			StartWalking(dir);
//	}
//}


void nofAttacker::LetsFight(nofAggressiveDefender * other)
{
	// Mein neues Ziel
	defender = other;

	// wir werden jetzt "gejagt"
	should_haunted = false;

	// wenn ich stehe (z.B. vor der Hütte warte), hinlaufen
	if(state == STATE_ATTACKING_WAITINGAROUNDBUILDING)
	{
		state = STATE_ATTACKING_WALKINGTOGOAL;
		MissAttackingWalk();
	}
}

void nofAttacker::AggressiveDefenderLost()
{
	defender = 0;

	// Wenn wir auf die gewartet hatten, müssen wir uns nun bewegen
	if(state == STATE_WAITINGFORFIGHT)
	{
		state = STATE_ATTACKING_WALKINGTOGOAL;
		MissAttackingWalk();
	}
}

bool nofAttacker::CanPassBeforeFight() const
{
	// Warte ich auf einen Kampf?
	if(state == STATE_WAITINGFORFIGHT)
	{
		assert(defender);

		// Ist mein Verteidiger ein normaler Verteidiger, der aus der Hütte rauskommt?
		if(defender->GetGOT() == GOT_NOF_DEFENDER)
			return static_cast<nofDefender*>(defender)->CanPassBeforeFight();
	}

	return true;
}

void nofAttacker::SwitchStateAttackingWaitingForDefender()
{
	state = STATE_ATTACKING_WAITINGFORDEFENDER;
	// Blockevent anmelden
	blocking_event = em->AddEvent(this,BLOCK_OFFSET,5);
}

void nofAttacker::HandleDerivedEvent(const unsigned int id)
{
	// abfragen, nich dass er evtl schon losgelaufen ist wieder, weil das Gebäude abgebrannt wurde etc.
	if(state == STATE_ATTACKING_WAITINGFORDEFENDER)
	{
		// Figuren stoppen
		gwg->StopOnRoads(x,y);
		blocking_event = 0;
	}
}


bool nofAttacker::IsBlockingRoads() const
{
	if(state != STATE_ATTACKING_WAITINGFORDEFENDER)
		return false;

	// Wenn Block-Event schon abgelaufen ist --> blocking_event = 0, da dürfen sich nicht mehr durch
	// wenn es das noch gibt, ist es noch nicht abgelaufen und die Leute können noch durchgehen
	if(!blocking_event)
		return true;
	else
		return false;
}
