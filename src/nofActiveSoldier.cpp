// $Id: nofActiveSoldier.cpp 6557 2010-07-08 21:19:20Z OLiver $
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
	// mich hinzufügen
	static_cast<nobMilitary*>(building)->AddActiveSoldier(this);

	// und wir können uns auch aus der Laufliste erstmal entfernen
	gwg->RemoveFigure(this,x,y);
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
	case STATE_SEAATTACKING_GOTOHARBOR: // geht von seinem Heimatmilitärgebäude zum Starthafen
	case STATE_SEAATTACKING_RETURNTOSHIP: // befindet sich an der Zielposition auf dem Weg zurück zum Schiff
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

/// Gibt den Sichtradius dieser Figur zurück (0, falls nicht-spähend)
unsigned nofActiveSoldier::GetVisualRange() const
{
	return VISUALRANGE_SOLDIER;
}

/// Prüft feindliche Leute auf Straßen in der Umgebung und vertreibt diese
void nofActiveSoldier::ExpelEnemies()
{
	// Figuren sammeln aus der Umgebung
	std::vector<noFigure*> figures;
	
	// Am Punkt selbst
	for(list<noBase*>::iterator it = gwg->GetFigures(x,y).begin();it.valid();++it)
	{
		if((*it)->GetType() == NOP_FIGURE)
			figures.push_back(static_cast<noFigure*>(*it));
	}
	
	// Und rund herum
	for(unsigned i = 0;i<6;++i)
	{
		// Diese müssen sich entweder auf dem Punkt befinden oder zu diesem laufen
		for(list<noBase*>::iterator it = gwg->GetFigures(gwg->GetXA(x,y,i),gwg->GetYA(x,y,i)).begin();it.valid();++it)
		{
			// Figur?
			// Nicht dass wir noch Hase und Igel stören (Naturschutz!)
			if((*it)->GetType() == NOP_FIGURE)
			{
				noFigure * fig = static_cast<noFigure*>(*it);
				if(fig->GetX() == x && fig->GetY() == y)
					figures.push_back(fig);
				else if(fig->GetDestinationForCurrentMove() == Point<MapCoord>(x,y))
					figures.push_back(fig);
			}
		}
	}
	
	// Mal gucken, was uns alles ins Netz gegangen ist, und aussieben
	// Nicht, dass Erika Steinbach noch böse wird
	for(unsigned i = 0;i<figures.size();++i)
	{
		noFigure * fig = figures[i];
		// Feind von uns und kein Soldat?
		// Außerdem muss er auf der Straße unterwegs sein (keine freiarbeitenden Berufe durcheinanderbringen..)
		if(!players->getElement(player)->IsAlly(fig->GetPlayer()) &&
		!(fig->GetJobType() >= JOB_PRIVATE && fig->GetJobType() <= JOB_GENERAL)
		&& fig->IsWalkingOnRoad())
		{
			// Dann weg mit dem!
			fig->Abrogate();
			fig->StartWandering();
			// Läuft der immer noch nicht? (Träger, die auf Wegen stehen und auf Waren warten)
			if(!fig->IsMoving())
				// Dann machen wir dir aber Beine
				fig->StartWalking(Random::inst().Rand(__FILE__,__LINE__,obj_id,6));
		}
	}
	
	// Straße ist gesäubert, vielleicht nützt dem Feind das ja sogar was..
	//gwg->RoadNodeAvailable(x, y);
}


/// Handle walking for nofActiveSoldier speciefic sates
void nofActiveSoldier::Walked()
{
	switch(state)
	{
	default: return;
	case STATE_WALKINGHOME: WalkingHome(); return;
	case STATE_MEETENEMY: MeetingEnemy(); return;
	}

}



/// Looks for enemies nearby which want to fight with this soldier
/// Returns true if it found one
bool nofActiveSoldier::FindEnemiesNearby()
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


	enemy = NULL;

	// Prüfen ob die Gefundenen gerade nix besseres zu tun haben
	for(unsigned i=0; i<soldiersNearby.size(); ++i)
	{
		// Wenn einer zu einem Angriffsziel läuft oder um ein Angriffsziel rumsteht und Feind ist, ist er ein gutes Opfer
		if (soldiersNearby[i]->IsReadyForFight() && !GAMECLIENT.GetPlayer(soldiersNearby[i]->GetPlayer())->IsAlly(this->player))
		{
			enemy = soldiersNearby[i];
			break;
		}
	}

	// No enemy found? Goodbye
	if(!enemy)
		return false;

	// Try to find fighting spot
	if(!GetFightSpotNear(enemy,&fight_spot))
		// No success? Then no fight
		return false;

	// We try to meet us now
	state = STATE_MEETENEMY;
	// Inform the other soldier
	enemy->MeetEnemy(this,fight_spot);

	// Walk to him
	MeetingEnemy();

	return true;
}

/// Handle state "meet enemy" after each walking step
void nofActiveSoldier::MeetingEnemy()
{
	// Enemy vanished?
	if(!enemy)
	{
		FreeFightEnded();
		Walked();
		return;
	}
	
	// Reached the fighting place?
	if (GetPos() == fight_spot)
	{
		// Enemy already there?
		if (enemy->GetPos() == fight_spot && enemy->GetState() == STATE_WAITINGFORFIGHT)
		{
			// Start fighting
			gwg->AddFigure(new noFighting(enemy, this), x, y);
			state = STATE_FIGHTING;
			enemy->state = STATE_FIGHTING;
			return;
		}
		else
		{
			// Is the fighting point still valid (could be another fight there already e.g.)
			if (!gwg->ValidPointForFighting(x, y))
			{
				// No
				// Abort the whole fighting fun with the enemy
				enemy->FreeFightEnded();
				enemy = NULL;
				Walked();
			}
			// Spot is still ok, let's wait for the enemy
			else
			{
				state = STATE_WAITINGFORFIGHT;
				return;
			}
		}
	}
	// Not at the fighting spot yet, continue walking there
	else
	{
		dir = gwg->FindHumanPath(x,y,fight_spot.x,fight_spot.y,MAX_ATTACKING_RUN_DISTANCE);
		StartWalking(dir);
		return;
	}

}


/// Determines if this soldier is ready for a spontaneous  fight
bool nofActiveSoldier::IsReadyForFight() const
{
	switch(state)
	{
	default: return false;
	case STATE_WALKINGHOME:
	case STATE_AGGRESSIVEDEFENDING_WALKINGTOAGGRESSOR:
	case STATE_ATTACKING_WALKINGTOGOAL:
	case STATE_ATTACKING_WAITINGAROUNDBUILDING:
		return true;
	}
}

/// Informs this soldier that another soldier starts meeting him
void nofActiveSoldier::MeetEnemy(nofActiveSoldier * other, const Point<MapCoord> figh_spot)
{
	// Remember these things
	enemy = other;
	this->fight_spot = figh_spot;

	// In some cases we have to start walking
	if(state == STATE_ATTACKING_WAITINGAROUNDBUILDING)
	{
		MeetingEnemy();
	}
	
}

/// Looks for an appropriate fighting spot between the two soldiers
/// Returns true if successful
bool nofActiveSoldier::GetFightSpotNear(nofActiveSoldier * other, Point<MapCoord> * fight_spot)
{
	// Calc middle between the two soldiers and use this as origin spot for the search of more fight spots
	Point<MapCoord> middle( (x + other->GetX()) / 2, (y + other->GetY()) / 2 );

	// Did we cross the borders ? ( horizontally)
	if(SafeDiff(middle.x,x) > MEET_FOR_FIGHT_DISTANCE)
	{
		// In this case: distance = distance of soldier 1 to left border + distance of soldier 2 to right border
		MapCoord minx = min(x,other->GetX());
		MapCoord maxx = max(x,other->GetX());

		MapCoord diff = minx + (gwg->GetWidth()-maxx);
		middle.x += diff/2;
	}

	// Did we cross the borders ? ( vertically)
	if(SafeDiff(middle.y,y) > MEET_FOR_FIGHT_DISTANCE)
	{
		// In this case: distance = distance of soldier 1 to left border + distance of soldier 2 to right border
		MapCoord miny = min(y,other->GetY());
		MapCoord maxy = max(y,other->GetY());

		MapCoord diff = miny + (gwg->GetHeight()-maxy);
		middle.y += diff/2;
	}

	// We could have crossed the border due to our interpolating across the borders
	gwg->ConvertCoords(middle.x,middle.y,&middle.x,&middle.y);



	for(MapCoord tx=gwg->GetXA(middle.x,middle.y,0), r=1;r<=MEET_FOR_FIGHT_DISTANCE;tx=gwg->GetXA(tx,middle.y,0),++r)
	{
		// Wurde ein Punkt in diesem Radius gefunden?
		bool found_in_radius = false;

		MapCoord tx2 = tx, ty2 = middle.y;
		for(unsigned i = 2;i<8;++i)
		{
			for(MapCoord r2=0;r2<r;gwg->GetPointA(tx2,ty2,i%6),++r2)
			{
				// Did we find a good spot?
				if(gwg->ValidPointForFighting(tx2,ty2,NULL))

				{
					// Great, then let's take this one
					fight_spot->x = tx2;
					fight_spot->y = ty2;
					return true;

				}
			}
		}
	}

	// No point found
	return false;

}