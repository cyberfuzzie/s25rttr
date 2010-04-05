// $Id: nofAttacker.cpp
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
#include "PostMsg.h"
#include "nobHarborBuilding.h"
#include "noShip.h"

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
should_haunted(GAMECLIENT.GetPlayer(attacked_goal->GetPlayer())->ShouldSendDefender()), blocking_event(0), encounteredEnemy(0),
harbor_x(0xffff), harbor_y(0xffff), ship_x(0xffff), ship_y(0xffff)
{
	// Dem Haus Bescheid sagen
	static_cast<nobMilitary*>(building)->SoldierOnMission(other,this);
	// Das Haus soll uns rausschicken
	building->AddLeavingFigure(this);
	// Dem Ziel Bescheid sagen
	attacked_goal->LinkAggressor(this);
}

nofAttacker::nofAttacker(nofPassiveSoldier * other,nobBaseMilitary * const attacked_goal,const nobHarborBuilding * const harbor)
: nofActiveSoldier(*other,STATE_SEAATTACKING_GOTOHARBOR), 

attacked_goal(attacked_goal), defender(false),
should_haunted(GAMECLIENT.GetPlayer(attacked_goal->GetPlayer())->ShouldSendDefender()), blocking_event(0), encounteredEnemy(0),
	harbor_x(harbor->GetX()), harbor_y(harbor->GetY()), ship_x(0xffff), ship_y(0xffff)
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
		sgd->PushObject(encounteredEnemy, false);
		sgd->PushUnsignedShort(fightSpot_x);
		sgd->PushUnsignedShort(fightSpot_y);

		if(state == STATE_ATTACKING_WAITINGFORDEFENDER)
			sgd->PushObject(blocking_event,true);
			
		sgd->PushUnsignedShort(harbor_x);
		sgd->PushUnsignedShort(harbor_y);
		sgd->PushUnsignedShort(ship_x);
		sgd->PushUnsignedShort(ship_y);
		sgd->PushUnsignedInt(ship_obj_id);
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
		encounteredEnemy = sgd->PopObject<nofActiveSoldier>(GOT_UNKNOWN);
		fightSpot_x = sgd->PopSignedShort();
		fightSpot_y = sgd->PopSignedShort();

		if(state == STATE_ATTACKING_WAITINGFORDEFENDER)
			blocking_event = sgd->PopObject<EventManager::Event>(GOT_EVENT);
			
		harbor_x = sgd->PopUnsignedShort();
		harbor_y = sgd->PopUnsignedShort();
		ship_x = sgd->PopUnsignedShort();
		ship_y = sgd->PopUnsignedShort();
		ship_obj_id = sgd->PopUnsignedInt();
	}
	else
	{
		attacked_goal = 0;
		defender = 0;
		should_haunted = 0;
		radius = 0;
		encounteredEnemy = 0;
		fightSpot_x = 0;
		fightSpot_y = 0;
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
	case STATE_ATTACKING_WALKINGTOFIGHTSPOT:
		{
			WalkingToFightSpot();
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
				if( (dir = gwg->FindHumanPath(x,y,flag_x,flag_y,5,true)) == 0xFF)
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
					// Ggf. Schiff Bescheid sagen (Schiffs-Angreifer)
					if(ship_obj_id)
						CancelAtShip();
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
		
	case STATE_SEAATTACKING_GOTOHARBOR: // geht von seinem Heimatmilitärgebäude zum Starthafen
		{
			
			// Gucken, ob der Abflughafen auch noch steht und sich in unserer Hand befindet
			bool valid_harbor = true;
			noBase * hb = gwg->GetNO(harbor_x,harbor_y);
			if(hb->GetGOT() != GOT_NOB_HARBORBUILDING)
				valid_harbor = false;
			else if(static_cast<nobHarborBuilding*>(hb)->GetPlayer() != player)
				valid_harbor = false;	
				
			// Nicht mehr oder das angegriffene Gebäude kaputt? Dann müssen wir die ganze Aktion abbrechen
			if(!valid_harbor || !attacked_goal)
			{
				// Dann gehen wir halt wieder nach Hause
				ReturnHomeMissionAttacking();
				return;
			}
			
			// Sind wir schon da?
			if(x == harbor_x && y == harbor_y)
			{
				// Uns zum Hafen hinzufügen
				state = STATE_SEAATTACKING_WAITINHARBOR;
				gwg->RemoveFigure(this,x,y);
				gwg->GetSpecObj<nobHarborBuilding>(x,y)->AddSeaAttacker(this);
				
				return;
			}
			
			// Erstmal Flagge ansteuern
			MapCoord harbor_flag_x = gwg->GetXA(harbor_x, harbor_y,4);
			MapCoord harbor_flag_y = gwg->GetYA(harbor_x, harbor_y,4);
			
			// Wenn wir an der Flagge bereits sind, in den Hafen eintreten
			if(x == harbor_flag_x && y == harbor_flag_y)
				StartWalking(1);
			else
			{
			
				// Weg zum Hafen suchen
				unsigned char dir = gwg->FindHumanPath(x,y,harbor_flag_x,harbor_flag_y,20,NULL,NULL);
				if(dir == 0xff)
				{
					// Kein Weg gefunden? Dann auch abbrechen!
					ReturnHomeMissionAttacking();
					return;
				}
				
				// Und schön weiterlaufen
				StartWalking(dir);
			}
			
		} break;
	case STATE_SEAATTACKING_WAITINHARBOR: // wartet im Hafen auf das ankommende Schiff
		{
		} break;
	case STATE_SEAATTACKING_ONSHIP: // befindet sich auf dem Schiff auf dem Weg zum Zielpunkt
		{
			// Auweia, das darf nicht passieren
			assert(false); 
		} break;
	case STATE_SEAATTACKING_RETURNTOSHIP: // befindet sich an der Zielposition auf dem Weg zurück zum Schiff
		{
			HandleState_SeaAttack_ReturnToShip();
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

			// Ggf. Schiff Bescheid sagen (Schiffs-Angreifer)
			if(ship_obj_id)
				CancelAtShip();

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
	case STATE_ATTACKING_WALKINGTOFIGHTSPOT:
	case STATE_ATTACKING_FIGHTVSATTACKER:
	case STATE_SEAATTACKING_GOTOHARBOR:
	case STATE_SEAATTACKING_WAITINHARBOR:
	case STATE_SEAATTACKING_ONSHIP:
	case STATE_SEAATTACKING_RETURNTOSHIP:
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

/// Sagt dem Heimatgebäude Bescheid, dass er nicht mehr nach Hause kommen wird
void nofAttacker::CancelAtHomeMilitaryBuilding()
{
	if(building)
		building->SoldierLost(this);
}

/// Wenn ein Kampf gewonnen wurde
void nofAttacker::WonFighting()
{
	defender = 0;
	encounteredEnemy = 0;

	// Ist evtl. unser Heimatgebäude zerstört?
	if(!building && state != STATE_ATTACKING_FIGHTINGVSDEFENDER)
	{
		// Dann dem Ziel Bescheid sagen, falls es existiert (evtl. wurdes zufällig zur selben Zeit zerstört)
		if(attacked_goal)
		{
			attacked_goal->UnlinkAggressor(this);
			attacked_goal = 0;
		}

		// Ggf. Schiff Bescheid sagen (Schiffs-Angreifer)
		if(ship_obj_id)
			CancelAtShip();

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
		if (state == STATE_ATTACKING_FIGHTVSATTACKER)
			encounteredEnemy = 0;
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

	// Ggf. Schiff Bescheid sagen
	if(ship_obj_id)
		this->CancelAtShip();
}


void nofAttacker::ReturnHomeMissionAttacking()
{
	// Zielen Bescheid sagen
	InformTargetsAboutCancelling();
	// Schiffsangreifer?
	if(ship_obj_id)
	{
		state = STATE_SEAATTACKING_RETURNTOSHIP;
		HandleState_SeaAttack_ReturnToShip();
	}
	else
		// Und nach Hause gehen
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

		// Ggf. Schiff Bescheid sagen (Schiffs-Angreifer)
		if(ship_obj_id)
			CancelAtShip();

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
		attacked_goal->FindAnAttackerPlace(goal_x,goal_y,radius,this);

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

				// Post schicken "Wir werden angegriffen" TODO evtl. unschön, da jeder Attacker das dann aufruft
				if(attacked_goal->GetPlayer() == GameClient::inst().GetPlayerID())
					GAMECLIENT.SendPostMessage(
						new ImagePostMsgWithLocation(_("We are under attack!"), PMC_MILITARY, x, y, 
						attacked_goal->GetBuildingType(), attacked_goal->GetNation()));

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
				MapCoord flag_x = attacked_goal->GetFlag()->GetX();
				MapCoord flag_y = attacked_goal->GetFlag()->GetY();
				if(y == flag_y && x <= flag_x) dir = 3;
				else if(y == flag_y && x > flag_x) dir = 0;
				else if(y < flag_y && x < flag_x) dir = 4;
				else if(y < flag_y && x >  flag_x) dir = 5;
				else if(y > flag_y && x < flag_x) dir = 2;
				else if(y > flag_y && x >  flag_x) dir = 1;
				else if(x ==  flag_x)
				{
					if(y < flag_y && !(SafeDiff(y,flag_y)&1)) dir = 4;
					else if(y < flag_y && (SafeDiff(y,flag_y)&1))
					{
						if(y&1) dir = 5; else dir = 4;
					}
					else if(y > flag_y && !(SafeDiff(y,flag_y)&1)) dir = 2;
					else if(y > flag_y && (SafeDiff(y,flag_y)&1))
					{
						if(y&1) dir = 1; else dir = 2;
					}
				}

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
			if (x != defender->GetX() || y != defender->GetY())
			{
				// Wenn der Verteidiger in der Nähe ist, zu ihm laufen
				// Achtung: nicht in die Hütte des Verteidigers laufen!!
				if(gwg->CalcDistance(x,y,defender->GetX(),defender->GetY())<5
					&& !(defender->GetX() == attacked_goal->GetX() &&
					defender->GetY() == attacked_goal->GetY() ))
					// zum Verteidiger laufen, falls er in der Nähe ist
					dir = gwg->FindHumanPath(x,y,defender->GetX(),
					defender->GetY(),100,true);
				else
					dir = gwg->FindHumanPath(x,y,goal_x,goal_y,100,true);
			}
		}
		else
			// Zur Flagge des angegriffenen Gebäudes laufen
			dir = gwg->FindHumanPath(x,y,goal_x,goal_y,100,true);
	}
	else
	{
		// Ansonsten findet man sowieso keinen Weg mehr dahin
		dir = 0xFF;
	}

	/// Bin ich beim Verteidiger angekommen, dann muss das vermerkt werden und wir dürfen nicht nach Hause
	// gehen, da nämlich in dem Fall KEIN Weg gefunden wird
	if(defender)
	{
		// Bin ich schon beim Verteidiger?
		if(x == defender->GetX() && y == defender->GetY() && static_cast<nofAggressiveDefender*>(defender)->state == STATE_WAITINGFORFIGHT)
		{
			// Dann die Köpfe rollen lassen...
			gwg->AddFigure(new noFighting(defender,this),x,y);
			state = STATE_ATTACKING_FIGHTINGVSAGGRESSIVEDEFENDER;
			static_cast<nofAggressiveDefender*>(defender)->state = STATE_AGGRESSIVEDEFENDING_FIGHTING;
			return;
		}
	}

	// Feindliche Soldaten im Umkreis von 2 suchen
	if (!encounteredEnemy)
	{
		if ((encounteredEnemy = FindValidEnemyNearby()))
		{
			if (EncounterEnemy())
				return;
		}
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
						&& gwg->CalcDistance(x,y,(*it)->GetX(),(*it)->GetY()) < 15
						&& GameClient::inst().GetPlayer(attacked_goal->GetPlayer())->IsAlly((*it)->GetPlayer())  && 
						GameClient::inst().GetPlayer(player)->IsPlayerAttackable((*it)->GetPlayer()))
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
			// ist der Verteidiger in meiner Nähe und wartet er noch nicht auf mich
			// und ist da noch Platz für einen Kampf?
			// Nicht an die Flagge des angegriffenen Militärgebäudes stellen!
			if(gwg->CalcDistance(x,y,defender->GetX(),defender->GetY())<3 &&
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
	unsigned char tmp_dir = gwg->FindHumanPath(x,y,attacked_goal->GetFlag()->GetX(),
		attacked_goal->GetFlag()->GetY(),3,true);

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

			// Ggf. Schiff Bescheid sagen (Schiffs-Angreifer)
			if(ship_obj_id)
				CancelAtShip();

			// Rumirren
			state = STATE_FIGUREWORK;
			StartWandering();
			Wander();

			return;
		}

		// weiter zur Flagge laufen
		if((dir = gwg->FindHumanPath(x,y,flag_x,flag_y,10,true)) == 0xFF)
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
	case STATE_ATTACKING_FIGHTVSATTACKER:
	case STATE_ATTACKING_WALKINGTOFIGHTSPOT:
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
}


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

/// Sagt den verschiedenen Zielen Bescheid, dass wir doch nicht mehr kommen können
void nofAttacker::InformTargetsAboutCancelling()
{
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

	if (encounteredEnemy)
	{
		static_cast<nofAttacker*>(encounteredEnemy)->LostEncounteredEnemy();
		encounteredEnemy = 0;
	}
}

nofActiveSoldier *nofAttacker::FindValidEnemyNearby()
{
	MapCoord tx, ty;

	// Vektor mit potenziellen Opfern
	std::vector<nofActiveSoldier*> soldiersNearby;

	// Im Radius 1 suchen...
	for(unsigned dir=0; dir<6; ++dir)
	{
		tx = gwg->GetXA(x, y, dir);
		ty = gwg->GetYA(x, y, dir);
		list<noBase*> objects;
		gwg->GetDynamicObjectsFrom(tx, ty, objects);
		for(list<noBase*>::iterator it = objects.begin(); it.valid(); ++it)
			if (dynamic_cast<nofActiveSoldier*>(*it))
				soldiersNearby.push_back(dynamic_cast<nofActiveSoldier*>(*it));
	}

	// ... und im Radius 2
	for(unsigned dir=0; dir<12; ++dir)
	{
		tx = gwg->GetXA2(x, y, dir);
		ty = gwg->GetYA2(x, y, dir);
		list<noBase*> objects;
		gwg->GetDynamicObjectsFrom(tx, ty, objects);
		for(list<noBase*>::iterator it = objects.begin(); it.valid(); ++it)
			if (dynamic_cast<nofActiveSoldier*>(*it))
				soldiersNearby.push_back(dynamic_cast<nofActiveSoldier*>(*it));
	}


	// Prüfen ob die Gefundenen gerade nix besseres zu tun haben
	for(unsigned i=0; i<soldiersNearby.size(); ++i)
	{
		// Wenn einer zu einem Angriffsziel läuft oder um ein Angriffsziel rumsteht und Feind ist, ist er ein gutes Opfer
		if ((soldiersNearby[i]->GetState() == STATE_ATTACKING_WALKINGTOGOAL 
			|| soldiersNearby[i]->GetState() == STATE_ATTACKING_WAITINGAROUNDBUILDING)
			&& !GAMECLIENT.GetPlayer(soldiersNearby[i]->GetPlayer())->IsAlly(this->player))
		{
			return soldiersNearby[i];
		}
	}

	// keinen gefunden
	return NULL;
}

bool nofAttacker::GoToFightEncounteredAttacker(nofAttacker *enemy, MapCoord tx, MapCoord ty)
{
	// Wenn wir grad rumstanden, möchten wir lieber, dass der gegner zu uns kommt, wenn er das tut, bleiben wir stehen
	if (state == STATE_ATTACKING_WAITINGAROUNDBUILDING && tx == x && ty == y)
	{
		state = STATE_WAITINGFORFIGHT;
	}
	// Ansonsten gehen wir zu dem von ihm vorgeschlagenen Kampfplatz
	else 
	{
		unsigned char dir = gwg->FindHumanPath(x,y,tx,ty,10);
		if (dir == 0xFF)
			return false;
		state = STATE_ATTACKING_WALKINGTOFIGHTSPOT;
		// Falls wir stehen, losgehen
		if (!moving)
			StartWalking(dir);
	}
	
	// Feind und Kampfplatz merken
	encounteredEnemy = enemy;
	fightSpot_x = tx;
	fightSpot_y = ty;

	// Falls ein Verteidiger mit uns geliebäugelt hat, dem absagen
	if (defender)
	{
		static_cast<nofAggressiveDefender*>(defender)->AttackerLost();
		defender = 0;
	}
	return true;
}

bool nofAttacker::GetFightSpotNear(MapCoord &x, MapCoord &y)
{
	MapCoord tx, ty;
	unsigned tries = 0;
	// Zufällig mit einer Richtung anfangen
	unsigned char first_dir = Random::inst().Rand(__FILE__,__LINE__,obj_id,6);
	unsigned char walk_dir;

	// Solange Nachbarfelder prüfen bis ein erreichbares, für Kampf geeignetes gefunden wurde oder alle getestet wurden
	do 
	{
		tx = gwg->GetXA(x, y, (first_dir + tries) % 6);
		ty = gwg->GetYA(x, y, (first_dir + tries) % 6);
		walk_dir = gwg->FindHumanPath(x,y,tx,ty,5);
		tries++;
	} 
	while(tries < 6 && (!gwg->ValidPointForFighting(tx, ty) || walk_dir == 0xFF));

	// Wenn vor Ablauf aller 6 Versuch was gefunden wurde, dann nehmen wir das als Ziel
	if (tries != 6)
	{
		x = tx;
		y = ty;
		return true;
	}
	return false;
}

void nofAttacker::WalkingToFightSpot()
{
	// Schon am Platz angekommen?
	if (x == fightSpot_x && y == fightSpot_y)
	{
		// Gegner auch schon da?
		if (encounteredEnemy->GetX() == fightSpot_x && encounteredEnemy->GetY() == fightSpot_y && encounteredEnemy->GetState() == STATE_WAITINGFORFIGHT)
		{
			// Krieg!
			gwg->AddFigure(new noFighting(encounteredEnemy, this), x, y);
			state = STATE_ATTACKING_FIGHTVSATTACKER;
			static_cast<nofAttacker*>(encounteredEnemy)->state = STATE_ATTACKING_FIGHTVSATTACKER;
			return;
		}
		// Sonst gucken ob hier schon was anderes los ist oder auf den Gegner warten
		else
		{
			// Kann man hier wirklich noch kämpfen oder ist hier inzwischen was los (Anderer Kampf, ...)?
			if (!gwg->ValidPointForFighting(x, y))
			{
				MapCoord tx = x;
				MapCoord ty = y;

				// Nebenan probieren
				if (GetFightSpotNear(tx, ty))
				{
					fightSpot_x = tx;
					fightSpot_y = ty;
					if (static_cast<nofAttacker*>(encounteredEnemy)->GoToFightEncounteredAttacker(this, fightSpot_x, fightSpot_y))
					{
						state = STATE_ATTACKING_WALKINGTOFIGHTSPOT;
						unsigned char walk_dir = gwg->FindHumanPath(x,y,fightSpot_x,fightSpot_y,10);

						// Falls wir da mysteriöserweise nicht hinkönnen: absagen
						if (walk_dir == 0xFF)
						{
							static_cast<nofAttacker*>(encounteredEnemy)->LostEncounteredEnemy();
							encounteredEnemy = 0;
							state = STATE_ATTACKING_WALKINGTOGOAL;
							MissAttackingWalk();
							return;
						}

						// Sonst losgehen
						StartWalking(walk_dir);
						return;
					}
				}
				else
				{
					static_cast<nofAttacker*>(encounteredEnemy)->LostEncounteredEnemy();
					encounteredEnemy = 0;
					state = STATE_ATTACKING_WALKINGTOGOAL;
					MissAttackingWalk();
				}
			}
			// Platz ist noch ok, wir warten auf unseren Feind
			else
			{
				state = STATE_WAITINGFORFIGHT;
				return;
			}
		}
	}
	// Noch nicht am Platz... Hingehen!
	else
	{
		dir = gwg->FindHumanPath(x,y,fightSpot_x,fightSpot_y,100);
		StartWalking(dir);
		return;
	}
}


bool nofAttacker::EncounterEnemy()
{
	assert(encounteredEnemy);

	// Potenzielle Koordinaten für Kampf
	MapCoord px, py;

	// Unser Ziel steht grad rum, also gehen wir zu ihm
	if (encounteredEnemy->GetState() == STATE_ATTACKING_WAITINGAROUNDBUILDING)
	{
		px = encounteredEnemy->GetX();
		py = encounteredEnemy->GetY();
	}
	// sonst soll er mal lieber zu uns kommen, weil wir gerade sicher an einem 'echten' Knotenpunkt sind
	else
	{
		px = x;
		py = y;
	}

	// Kann man da kämpfen? (Den Feind als Ausnahme nehmen, weil der da evtl. selbst wartend den Kampfplatz blockiert)
	if (!gwg->ValidPointForFighting(px,py, encounteredEnemy))
	{
		// Kann man da in der Nähe kämpfen?
		if (!GetFightSpotNear(px, py))
		{
			// Dann halt nicht!
			return false;
		}	
	}

	// Kampfplatz merken
	fightSpot_x = px;
	fightSpot_y = py;

	nofAttacker* enemy = static_cast<nofAttacker*>(encounteredEnemy);

	// Versuchen den Gegner zum Kampfplatz zu bestellen
	if (enemy->GoToFightEncounteredAttacker(this, px, py))
	{
		// Wir kämpfen bei mir, ich warte hier einfach auf den Gegner
		if (fightSpot_x == x && fightSpot_y == y)
		{
			state = STATE_WAITINGFORFIGHT;
		}
		// Wir kämpfen woanders, ich geh hin
		else
		{
			state = STATE_ATTACKING_WALKINGTOFIGHTSPOT;
			unsigned char walk_dir;
			walk_dir = gwg->FindHumanPath(x,y,fightSpot_x,fightSpot_y,10);

			// Ich kann gar nicht hin, dann blasen wir die ganze Sache ab
			if (walk_dir == 0xFF)
			{
				enemy->LostEncounteredEnemy();
				encounteredEnemy = 0;
				return false;
			}

			// Ansonsten losgehen
			StartWalking(walk_dir);
		}

		// Verteiger absagen, falls vorhanden
		if (defender)
		{
			static_cast<nofAggressiveDefender*>(defender)->AttackerLost();
			defender = 0;
		}
		return true;
	}

	// Unser Feind kann nicht zum Kampfplatz kommen
	encounteredEnemy = 0;
	return false;
}

void nofAttacker::LostEncounteredEnemy()
{
	// Gegner entfernen
	encounteredEnemy = 0;

	state = STATE_ATTACKING_WALKINGTOGOAL;

	// Wenn wir stehen, losgehen
	if (!moving)
	{
		MissAttackingWalk();
	}
}

/// Startet den Angriff am Landungspunkt vom Schiff
void nofAttacker::StartAttackOnOtherIsland(const MapCoord ship_x, const MapCoord ship_y, const unsigned ship_id)
{
	x = this->ship_x = ship_x;
	y = this->ship_y = ship_y;
	this->ship_obj_id = ship_id;

	state = STATE_ATTACKING_WALKINGTOGOAL;

	// Normal weiterlaufen
	MissAttackingWalk();
}

/// Sagt Schiffsangreifern, dass sie mit dem Schiff zurück fahren
void nofAttacker::StartReturnViaShip()
{
	goal = building;
	state = STATE_FIGUREWORK;
	on_ship = true;
}

/// Für Schiffsangreifer: Sagt dem Schiff Bescheid, dass wir nicht mehr kommen
void nofAttacker::CancelAtShip()
{
	// Alle Figuren durchgehen
	list<noBase*> figures;
	gwg->GetDynamicObjectsFrom(ship_x, ship_y,figures);
	for(list<noBase*>::iterator it = figures.begin();it.valid();++it)
	{
		if((*it)->GetObjId() == ship_obj_id)
		{
			noShip * ship = static_cast<noShip*>(*it);
			ship->SeaAttackerWishesNoReturn();
			return;
		}
	}

}

/// Behandelt das Laufen zurück zum Schiff
void nofAttacker::HandleState_SeaAttack_ReturnToShip()
{
	// Ist evtl. unser Heimatgebäude zerstört?
	if(!building)
	{
		// Rumirren
		state = STATE_FIGUREWORK;
		StartWandering();
		Wander();

		// Schiff Bescheid sagen
		CancelAtShip();

		return;
	}

	// Sind wir schon im Schiff?
	if(x == ship_x && y == ship_y)
	{
		// Alle Figuren durchgehen
		list<noBase*> figures;
		gwg->GetDynamicObjectsFrom(x, y,figures);
		for(list<noBase*>::iterator it = figures.begin();it.valid();++it)
		{
			if((*it)->GetObjId() == ship_obj_id)
			{
				noShip * ship = static_cast<noShip*>(*it);
				// Und von der Landkarte tilgen
				gwg->RemoveFigure(this,x,y);
				// Uns zum Schiff hinzufügen
				ship->AddAttacker(this);
				

				state = STATE_FIGUREWORK;
				fs = FS_GOTOGOAL;
				StartReturnViaShip();
				return;
			}
		}

		// Kein Schiff gefunden? Das kann eigentlich nich sein!
		// Dann rumirren
		StartWandering();
		state = STATE_FIGUREWORK;
		Wander();
	}
	// oder finden wir gar keinen Weg mehr?
	else if((dir = gwg->FindHumanPath(x,y,ship_x,ship_y,100)) == 0xFF)
	{
		// Kein Weg gefunden --> Rumirren
		StartWandering();
		state = STATE_FIGUREWORK;
		Wander();

		// Dem Heimatgebäude Bescheid sagen
		building->SoldierLost(this);
		// Und dem Schiff
		CancelAtShip();
	}
	// oder ist alles ok? :)
	else
	{
		// weiterlaufen
		StartWalking(dir);
	}

}

