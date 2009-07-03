// $Id: noShip.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "noShip.h"
#include "Loader.h"
#include "macros.h"
#include "GameClient.h"
#include "Random.h"
#include "EventManager.h"
#include "GameWorld.h"
#include "SerializedGameData.h"
#include "nobHarborBuilding.h"

const unsigned SHIP_SPEED = 20;


/// Konstruktor
noShip::noShip(const unsigned short x, const unsigned short y, const unsigned char player) 
: noMovable(NOP_SHIP,x,y), player(player), state(STATE_IDLE), sea_id(0), name("Give me a name")
{
	// Meer ermitteln, auf dem dieses Schiff fährt
	for(unsigned i = 0;i<6;++i)
	{
		unsigned short sea_id = gwg->GetNodeAround(x,y,i).sea_id;
		if(sea_id)
			this->sea_id = sea_id;
	}

	// Auf irgendeinem Meer müssen wir ja sein
	assert(sea_id > 0);

	gwg->AddFigure(this,x,y);

	// Schiff registrieren lassen
	players->getElement(player)->RegisterShip(this);
}

void noShip::Serialize(SerializedGameData * sgd) const
{
	Serialize_noMovable(sgd);

	sgd->PushUnsignedChar(player);
	sgd->PushUnsignedChar(static_cast<unsigned char>(state));
	sgd->PushUnsignedShort(sea_id);
}

noShip::noShip(SerializedGameData * sgd, const unsigned obj_id) : noMovable(sgd,obj_id),
player(player),
state(State(sgd->PopUnsignedChar())),
sea_id(sgd->PopUnsignedShort())
{
	
}

void noShip::Destroy()
{
	// Schiff wieder abmelden
	players->getElement(player)->RemoveShip(this);
}


void noShip::Draw(int x, int y)
{
 	switch(state)
	{
	default:
		break;
	case STATE_IDLE:
		{
			GetImage(boot_lst, 1)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
			GetImage(boot_lst, 0)->Draw(x,y);
		} break;
	case STATE_GOTOHARBOR:
		{
			DrawDriving(x,y);
		} break;
	case STATE_EXPEDITION_LOADING:
		{
			GetImage(boot_lst, ((dir+3)%6)*2 + 1)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
			GetImage(boot_lst, ((dir+3)%6)*2)->Draw(x,y);
		} break;
	case STATE_EXPEDITION_WAITING:
		{
			GetImage(boot_lst, ((dir+3)%6)*2 + 1)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
			GetImage(boot_lst, ((dir+3)%6)*2)->Draw(x,y);
			/// Waren zeichnen 
			GetImage(boot_lst, 30+((dir+3)%6))->Draw(x,y);
		} break;
	}

}
/// Zeichnet normales Fahren auf dem Meer ohne irgendwelche Güter
void noShip::DrawDriving(int x, int y)
{
	// Interpolieren zwischen beiden Knotenpunkten
	CalcWalkingRelative(x,y);

	GetImage(boot_lst,13+((dir+3)%6)*2)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
	GetImage(boot_lst,12+((dir+3)%6)*2)->Draw(x,y);
}


void noShip::HandleEvent(const unsigned int id)
{
	current_ev = 0;

	switch(id)
	{
	// Laufevent
	case 0:
		{
			// neue Position einnehmen
			Walk();

			// entscheiden, was als nächstes zu tun ist
			Driven();
		} break;
	default:
		{
			switch(state)
			{
			default: break;
			case STATE_EXPEDITION_LOADING:
				{
					// Schiff ist nun bereit und Expedition kann beginnen
					state = STATE_EXPEDITION_WAITING;
				} break;
			}
		} break;

	}

}

void noShip::StartDriving(const unsigned char dir)
{
	StartMoving(dir,SHIP_SPEED);
}

void noShip::Driven()
{
	switch(state)
	{
	case STATE_GOTOHARBOR:
		HandleState_GoToHarbor();
		break;
	default:
		break;
	}
}


/// Fährt zum Hafen, um dort eine Mission (Expedition) zu erledigen
void noShip::GoToHarbor(nobHarborBuilding * hb, const std::vector<unsigned char>& route)
{
	state = STATE_GOTOHARBOR;

	goal_harbor_id = gwg->GetNode(hb->GetX(),hb->GetY()).harbor_id;

	// Route merken
	this->route = route;
	pos = 1;

	// losfahren
	StartDriving(route[0]);
}

void noShip::HandleState_GoToHarbor()
{
	Result res = DriveToHarbour();
	switch(res)
	{
	default: return;
	case GOAL_REACHED:
		{
			Point<MapCoord> goal(gwg->GetHarborPoint(goal_harbor_id));
			// Hafen Bescheid sagen, dass wir da sind
			gwg->GetSpecObj<nobHarborBuilding>(goal.x,goal.y)->ShipArrived(this);
		} break;
	case NO_ROUTE_FOUND:
		{
			Point<MapCoord> goal(gwg->GetHarborPoint(goal_harbor_id));
			// Dem Hafen Bescheid sagen
			gwg->GetSpecObj<nobHarborBuilding>(goal.x,goal.y)->ShipLost(this);
			// Nichts machen und idlen
			state = STATE_IDLE;
		} break;
	case HARBOR_DOESNT_EXIST:
		{
			// Nichts machen und idlen
			state = STATE_IDLE;
		} break;
	}
}

/// Startet eine Expedition
void noShip::StartExpedition()
{
	/// Schiff wird "beladen", also kurze Zeit am Hafen stehen, bevor wir bereit sind
	state = STATE_EXPEDITION_LOADING;
	current_ev = em->AddEvent(this,200,1);
}

/// Fährt weiter zu einem Hafen
noShip::Result noShip::DriveToHarbour()
{
	Point<MapCoord> goal(gwg->GetHarborPoint(goal_harbor_id));
	

	// Existiert der Hafen überhaupt noch?
	if(gwg->GetGOT(goal.x,goal.y) != GOT_NOB_HARBORBUILDING)
		return HARBOR_DOESNT_EXIST;

	nobHarborBuilding * hb = gwg->GetSpecObj<nobHarborBuilding>(goal.x,goal.y);

	// Sind wir schon da?
	if(pos == route.size())
		return GOAL_REACHED;
	else
	{
		// Punkt, zu dem uns der Hafen hinführen will (Küstenpunkt)
		MapCoord coastal_x, coastal_y;
		gwg->GetCoastalPoint(goal_harbor_id,&coastal_x, &coastal_y,sea_id);

		MapCoord goal_x_route, goal_y_route;

		// Route überprüfen
		if(!gwg->CheckShipRoute(x,y,route,pos,&goal_x_route,&goal_y_route))
		{
			// Route kann nicht mehr passiert werden --> neue Route suchen
			if(!gwg->FindShipPath(x,y,coastal_x,coastal_y,&route,NULL))
			{
				// Wieder keine gefunden -> raus
				return NO_ROUTE_FOUND;
			}

			// Wir fangen bei der neuen Route wieder von vorne an
			pos = 0;
		}

		// Kommen wir auch mit unser bisherigen Route an dem ursprünglich anvisierten Hafenplatz an?
		if(!(coastal_x == goal_x_route && coastal_y == goal_y_route))
		{
			// Nein, d.h. wenn wir schon nah dran sind, müssen wir die Route neu berechnen
			if(route.size()-pos < 10)
			{
				if(!gwg->FindShipPath(x,y,coastal_x,coastal_y,&route,NULL))
					// Keiner gefunden -> raus
					return NO_ROUTE_FOUND;

				pos = 0;
			}
		}

		StartDriving(route[pos++]);
		return DRIVING;
	}
}

/// Fährt weiter zu Hafenbauplatz
noShip::Result noShip::DriveToHarbourPlace()
{
	return DRIVING;
}


unsigned noShip::GetCurrentHarbor() const
{
	assert(state == STATE_EXPEDITION_WAITING);
	return goal_harbor_id;
}
