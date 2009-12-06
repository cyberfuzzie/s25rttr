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
#include "noFigure.h"
#include "Ware.h"
#include "PostMsg.h"

const unsigned SHIP_SPEED = 20;
const unsigned int ship_count = 5;
const std::string ship_names[ship_count] = {"FloSoftius", "Demophobius", "Olivianus", "Spikeonius", "Nastius"};

/// Positionen der Flaggen am Schiff für die 6 unterschiedlichen Richtungen jeweils#
const Point<int> SHIPS_FLAG_POS[6] =
{
	Point<int>(0,0),
	Point<int>(0,0),
	Point<int>(0,0),
	Point<int>(0,0),
	Point<int>(0,0),
	Point<int>(0,0)
};

/// Konstruktor
noShip::noShip(const unsigned short x, const unsigned short y, const unsigned char player) 
: noMovable(NOP_SHIP,x,y), player(player), state(STATE_IDLE), sea_id(0), name(ship_names[Random::inst().Rand(__FILE__,__LINE__,this->obj_id, ship_count)])
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
	sgd->PushUnsignedInt(goal_harbor_id);
	sgd->PushUnsignedChar(goal_dir);
	sgd->PushString(name);
	sgd->PushUnsignedInt(pos);
	sgd->PushUnsignedInt(route.size());
	for(unsigned i =0;i<route.size();++i)
		sgd->PushUnsignedChar(route[i]);
	sgd->PushObjectList(figures,false);
	sgd->PushObjectList(wares,true);
	
}

noShip::noShip(SerializedGameData * sgd, const unsigned obj_id) : 
noMovable(sgd,obj_id),
player(sgd->PopUnsignedChar()),
state(State(sgd->PopUnsignedChar())),
sea_id(sgd->PopUnsignedShort()),
goal_harbor_id(sgd->PopUnsignedInt()),
goal_dir(sgd->PopUnsignedChar()),
name(sgd->PopString()),
pos(sgd->PopUnsignedInt()),
route(sgd->PopUnsignedInt())
{
	for(unsigned i = 0;i<route.size();++i)
		route[i] = sgd->PopUnsignedChar();
	sgd->PopObjectList(figures,GOT_UNKNOWN);
	sgd->PopObjectList(wares,GOT_WARE);
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
			LOADER.GetImageN("boot_z",  1)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
			LOADER.GetImageN("boot_z",  0)->Draw(x,y);
		} break;
	
	case STATE_GOTOHARBOR:
		{
			DrawDriving(x,y);
		} break;
	case STATE_EXPEDITION_LOADING:
	case STATE_TRANSPORT_LOADING:
	case STATE_TRANSPORT_UNLOADING:
		{
			LOADER.GetImageN("boot_z",  ((dir+3)%6)*2 + 1)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
			LOADER.GetImageN("boot_z",  ((dir+3)%6)*2)->Draw(x,y);
		} break;
	case STATE_EXPEDITION_WAITING:
	
		{
			LOADER.GetImageN("boot_z",  ((dir+3)%6)*2 + 1)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
			LOADER.GetImageN("boot_z",  ((dir+3)%6)*2)->Draw(x,y);
			/// Waren zeichnen 
			LOADER.GetImageN("boot_z",  30+((dir+3)%6))->Draw(x,y);
		} break;
	case STATE_EXPEDITION_DRIVING:
	case STATE_TRANSPORT_DRIVING:
		{
			DrawDrivingWithWares(x,y);
		} break;
	}

	LOADER.GetMapImageN(3162+GAMECLIENT.GetGlobalAnimation(8,80,40,obj_id))->
		Draw(x+SHIPS_FLAG_POS[dir].x,y+SHIPS_FLAG_POS[dir].y,0,0,0,0,0,0,COLOR_WHITE, COLORS[gwg->GetPlayer(player)->color]);

}
/// Zeichnet normales Fahren auf dem Meer ohne irgendwelche Güter
void noShip::DrawDriving(int& x, int& y)
{
	// Interpolieren zwischen beiden Knotenpunkten
	CalcWalkingRelative(x,y);

	LOADER.GetImageN("boot_z", 13+((dir+3)%6)*2)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
	LOADER.GetImageN("boot_z", 12+((dir+3)%6)*2)->Draw(x,y);
}

/// Zeichnet normales Fahren auf dem Meer mit Gütern
void noShip::DrawDrivingWithWares(int& x, int& y)
{
	// Interpolieren zwischen beiden Knotenpunkten
	CalcWalkingRelative(x,y);

	LOADER.GetImageN("boot_z", 13+((dir+3)%6)*2)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
	LOADER.GetImageN("boot_z", 12+((dir+3)%6)*2)->Draw(x,y);
	/// Waren zeichnen 
	LOADER.GetImageN("boot_z",  30+((dir+3)%6))->Draw(x,y);
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

					// Spieler benachrichtigen
					if(GameClient::inst().GetPlayerID() == this->player)
						GAMECLIENT.SendPostMessage(new ShipPostMsg(_("A ship is ready for an expedition."), PMC_GENERAL, GAMECLIENT.GetPlayer(player)->nation, x, y));
				} break;
			case STATE_TRANSPORT_LOADING:
				{
					StartTransport();
				} break;
			case STATE_TRANSPORT_UNLOADING:
				{
					// Hafen herausfinden 
					Point<MapCoord> goal_pos(gwg->GetHarborPoint(goal_harbor_id));
					noBase * hb = gwg->GetNO(goal_pos.x,goal_pos.y);
					if(hb->GetGOT() == GOT_NOB_HARBORBUILDING)
					{
						static_cast<nobHarborBuilding*>(hb)->ReceiveGoodsFromShip(figures,wares);
						figures.clear();
						wares.clear();
					}
					else
					{
						// todo
						assert(false);
					}

					// Wieder idlen und ggf. neuen Job suchen
					StartIdling();
					players->getElement(player)->GetJobForShip(this);
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
	gwg->RecalcVisibilitiesAroundPoint(gwg->GetXA(x,y,(dir+3)%6),gwg->GetYA(x,y,(dir+3)%6),
		VISUALRANGE_SHIP+2,player,NULL);
	gwg->RecalcVisibilitiesAroundPoint(x,y,VISUALRANGE_SHIP+2,player,NULL);


	switch(state)
	{
	case STATE_GOTOHARBOR: HandleState_GoToHarbor(); break;
	case STATE_EXPEDITION_DRIVING: HandleState_ExpeditionDriving(); break;
	case STATE_TRANSPORT_DRIVING: HandleState_TransportDriving(); break;
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
	// Hafen schon zerstört?
	if(goal_harbor_id == 0)
	{
		StartIdling();
	}
	else
	{
		Result res = DriveToHarbour();
		switch(res)
		{
		default: return;
		case GOAL_REACHED:
			{

				Point<MapCoord> goal(gwg->GetHarborPoint(goal_harbor_id));
				// Erstmal nichts machen und idlen
				StartIdling();
				// Hafen Bescheid sagen, dass wir da sind (falls er überhaupt noch existiert)
				noBase * nb = gwg->GetNO(goal.x,goal.y);
				if(nb->GetGOT() == GOT_NOB_HARBORBUILDING)
					static_cast<nobHarborBuilding*>(nb)->ShipArrived(this);

				
			} break;
		case NO_ROUTE_FOUND:
			{
				Point<MapCoord> goal(gwg->GetHarborPoint(goal_harbor_id));
				// Dem Hafen Bescheid sagen
				gwg->GetSpecObj<nobHarborBuilding>(goal.x,goal.y)->ShipLost(this);
				// Nichts machen und idlen
				StartIdling();
			} break;
		case HARBOR_DOESNT_EXIST:
			{
				// Nichts machen und idlen
				StartIdling();
			} break;
		}
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

	return DriveToHarbourPlace();

	//nobHarborBuilding * hb = gwg->GetSpecObj<nobHarborBuilding>(goal.x,goal.y);
}

/// Fährt weiter zu Hafenbauplatz
noShip::Result noShip::DriveToHarbourPlace()
{
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


unsigned noShip::GetCurrentHarbor() const
{
	assert(state == STATE_EXPEDITION_WAITING);
	return goal_harbor_id;
}


/// Weist das Schiff an, in einer bestimmten Richtung die Expedition fortzusetzen
void noShip::ContinueExpedition(const unsigned char dir)
{
	if(state != STATE_EXPEDITION_WAITING)
		return;

	assert(state == STATE_EXPEDITION_WAITING);

	// Nächsten Hafenpunkt in dieser Richtung suchen
	unsigned new_goal = gwg->GetNextFreeHarborPoint(x,y,goal_harbor_id,dir,player);

	// Auch ein Ziel gefunden?
	if(!new_goal)
		return;

	MapCoord coastal_x, coastal_y;
	gwg->GetCoastalPoint(new_goal,&coastal_x, &coastal_y,sea_id);

	// Versuchen, Weg zu finden
	if(!gwg->FindShipPath(x,y,coastal_x,coastal_y,&route,NULL))
		return;

	// Dann fahren wir da mal hin
	pos = 0;
	goal_harbor_id = new_goal;
	state = STATE_EXPEDITION_DRIVING;

	StartDriving(route[pos++]);

}

/// Weist das Schiff an, an der aktuellen Position einen Hafen zu gründen
void noShip::FoundColony()
{
	if(state != STATE_EXPEDITION_WAITING)
		return;

	// Kolonie gründen
	if(gwg->FoundColony(goal_harbor_id,player,sea_id))
	{
		// Dann idlen wir wieder
		StartIdling();
		// Neue Arbeit suchen
		players->getElement(player)->GetJobForShip(this);
	}

}


void noShip::HandleState_ExpeditionDriving()
{
	Result res = DriveToHarbourPlace();
	switch(res)
	{
	default: return;
	case GOAL_REACHED:
		{
			// Warten auf weitere Anweisungen
			state = STATE_EXPEDITION_WAITING;

			// Spieler benachrichtigen
			if(GameClient::inst().GetPlayerID() == this->player)
				GAMECLIENT.SendPostMessage(new ShipPostMsg(_("A ship has reached the destination of its expedition."), PMC_GENERAL, GAMECLIENT.GetPlayer(player)->nation, x, y));
		} break;
	case NO_ROUTE_FOUND:
		{
			Point<MapCoord> goal(gwg->GetHarborPoint(goal_harbor_id));
			// Nichts machen und idlen
			StartIdling();
		} break;
	}
}

void noShip::HandleState_TransportDriving()
{
	Result res = DriveToHarbour();
	switch(res)
	{
	default: return;
	case GOAL_REACHED:
		{
			// Waren abladen, dafür wieder kurze Zeit hier ankern
			state = STATE_TRANSPORT_UNLOADING;
			current_ev = em->AddEvent(this,200,1);
			
		} break;
	case NO_ROUTE_FOUND:
		{
			// Nichts machen und idlen
			StartIdling();
		} break;
	case HARBOR_DOESNT_EXIST:
		{
			// Nichts machen und idlen
			StartIdling();
		} break;
	}
}

/// Gibt zurück, ob das Schiff jetzt in der Lage wäre, eine Kolonie zu gründen
bool noShip::IsAbleToFoundColony() const
{
	// Warten wir gerade?
	if(state == STATE_EXPEDITION_WAITING)
	{
		// Ist der Punkt, an dem wir gerade ankern, noch frei?
		if(gwg->IsHarborPointFree(goal_harbor_id,player,sea_id))
			return true;
	}

	return false;
}

/// Gibt zurück, ob das Schiff einen bestimmten Hafen ansteuert
bool noShip::IsGoingToHarbor(nobHarborBuilding * hb) const
{
	return (goal_harbor_id == hb->GetHarborPosID() && 
		(state == STATE_GOTOHARBOR || state == STATE_TRANSPORT_DRIVING
		|| state == STATE_TRANSPORT_LOADING || state == STATE_TRANSPORT_UNLOADING));
}

/// Belädt das Schiff mit Waren und Figuren, um eine Transportfahrt zu starten
void noShip::PrepareTransport(Point<MapCoord> goal, const std::list<noFigure*>& figures, const std::list<Ware*>& wares)
{
	// ID von Zielhafen herausfinden
	noBase * nb = gwg->GetNO(goal.x,goal.y);
	assert(nb->GetGOT() == GOT_NOB_HARBORBUILDING);
	this->goal_harbor_id = static_cast<nobHarborBuilding*>(nb)->GetHarborPosID();

	this->figures = figures;
	this->wares = wares;

	state = STATE_TRANSPORT_LOADING;
	current_ev = em->AddEvent(this,200,1);
}

/// Startet die eigentliche Transportaktion, nachdem das Schiff beladen wurde
void noShip::StartTransport()
{
	state = STATE_TRANSPORT_DRIVING;

	MapCoord coastal_x, coastal_y;
	gwg->GetCoastalPoint(goal_harbor_id,&coastal_x, &coastal_y,sea_id);

	// Versuchen, Weg zu finden
	if(!gwg->FindShipPath(x,y,coastal_x,coastal_y,&route,NULL))
	{
		// todo
		return;
	}
	pos = 0;

	// Einfach weiterfahren
	HandleState_TransportDriving(); 
}

/// Sagt dem Schiff, das ein bestimmter Hafen zerstört wurde
void noShip::HarborDestroyed(nobHarborBuilding * hb)
{
	// Ist unser Ziel betroffen?
	if(hb->GetHarborPosID() == goal_harbor_id && ((state >= STATE_TRANSPORT_LOADING && state <= STATE_TRANSPORT_UNLOADING)))
	{
		// Punkt von dem aus berechnet werden soll, ist normalerweise der normale Standpunkt des Schiffes,
		// allderdings der nächste Punkt, wenn das Schiff gerade am fahren ist (der Zielpunkt der aktuellen
		// Bewegung
		MapCoord current_x = x, current_y = y;
		if(state == STATE_TRANSPORT_DRIVING || state == STATE_GOTOHARBOR)
		{
			current_x = gwg->GetXA(x,y,dir);
			current_y = gwg->GetYA(x,y,dir);
		}

		// Neues Ziel suchen, wo wir ankern können
		if(players->getElement(player)->FindHarborForUnloading(this,current_x,current_y,&goal_harbor_id,&route,hb))
		{

			switch(state)
			{
			default: return;
			case STATE_TRANSPORT_LOADING: // Schiff wird mit Waren/Figuren erst noch beladen, bleibt also für kurze Zeit am Hafen
				{
					state = STATE_TRANSPORT_DRIVING;
					// Hafen suchen, wo wir gerade ankern
					for(unsigned i = 0;i<6;++i)
					{
						noBase * nob = gwg->GetNO(gwg->GetXA(x,y,i),gwg->GetYA(x,y,i));
						if(nob->GetGOT() == GOT_NOB_HARBORBUILDING && nob != hb)
						{
							// Waren wieder abladen
							static_cast<nobHarborBuilding*>(nob)->ReceiveGoodsFromShip(figures,wares);
							// Neuen Job für mich suchen
							StartIdling();
							goal_harbor_id = 0;
							players->getElement(player)->GetJobForShip(this);
							return;
						}
					}

				} // Keinbreak;
			case STATE_TRANSPORT_DRIVING: /// Schiff transportiert Waren/Figuren von einen Ort zum anderen
				{
					// Route wird wieder von vorne abgearbeitet
					pos = 0;

				} break;
			case STATE_TRANSPORT_UNLOADING: /// Entlädt Schiff am Zielhafen, kurze Zeit ankern, bevor Waren im Hafengebäude ankommen..
				{
					// Event zum Abladen abmelden
					em->RemoveEvent(current_ev);
					current_ev = NULL;
					// Route wird wieder von vorne abgearbeitet
					pos = 0;
					state = STATE_TRANSPORT_DRIVING;
				} break;

			}

			// Waren und Figure über verändertes Ziel informieren
			for(std::list<noFigure*>::iterator it = figures.begin();it!=figures.end();++it)
				(*it)->StartShipJourney(gwg->GetHarborPoint(goal_harbor_id));
		}
		else
		{
			// Kein Ziel gefunden, also erstmal idlen
			StartIdling();
			em->RemoveEvent(current_ev);
			current_ev = NULL;
		}
	}
	else if(state == STATE_GOTOHARBOR)
	{
		// Wenn er zum Hafen fährt Bescheid sagen, dass er idlen soll
		StartIdling();
	}
}

/// Fängt an mit idlen und setzt nötigen Sachen auf NULL
void noShip::StartIdling()
{
	//goal_harbor_id = 0;
	state = STATE_IDLE;
}
