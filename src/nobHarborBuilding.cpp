// $Id: nobHarborBuilding.cpp 5018 2009-06-08 18:24:25Z OLiver $
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
#include "nobHarborBuilding.h"
#include "GameWorld.h"
#include "Loader.h"
#include "noExtension.h"
#include "MilitaryConsts.h"
#include "GameClient.h"
#include "GameClientPlayer.h"
#include "Ware.h"
#include "EventManager.h"
#include "noShip.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nobHarborBuilding::nobHarborBuilding(const unsigned short x, const unsigned short y,const unsigned char player,const Nation nation) 
: nobBaseWarehouse(BLD_HARBORBUILDING,x,y,player,nation)
{
	// Alle Waren 0, außer 100 Träger
	memset(&goods,0,sizeof(goods));
	memset(&real_goods,0,sizeof(real_goods));

	// Der Wirtschaftsverwaltung Bescheid sagen
	GAMECLIENT.GetPlayer(player)->AddWarehouse(this);

	// Aktuellen Warenbestand zur aktuellen Inventur dazu addieren
	AddToInventory();

	// Evtl gabs verlorene Waren, die jetzt in den Hafen wieder reinkönnen
	GAMECLIENT.GetPlayer(player)->FindClientForLostWares();

	/// Die Meere herausfinden, an die dieser Hafen grenzt
	for(unsigned i = 0;i<6;++i)
		sea_ids[i] = gwg->IsCoastalPoint(gwg->GetXA(x,y,i), gwg->GetYA(x,y,i));

	// Post versenden
	if(GameClient::inst().GetPlayerID() == this->player)
		GameClient::inst().SendPostMessage(new ImagePostMsgWithLocation(
			_("New storehouse finished"), PMC_GENERAL, x, y, BLD_STOREHOUSE, nation));
}



void nobHarborBuilding::Destroy()
{
	// Der Wirtschaftsverwaltung Bescheid sagen
	GAMECLIENT.GetPlayer(player)->RemoveWarehouse(this);

	Destroy_nobBaseWarehouse();
}

void nobHarborBuilding::Serialize(SerializedGameData * sgd) const
{
	Serialize_nobBaseWarehouse(sgd);
}

nobHarborBuilding::nobHarborBuilding(SerializedGameData * sgd, const unsigned obj_id) 
: nobBaseWarehouse(sgd,obj_id)
{
	
}

// Relative Position des Bauarbeiters 
const Point<int> BUILDER_POS[4] = { Point<int>(0,0), Point<int>(0,0), Point<int>(0,0), Point<int>(0,0) };
/// Relative Position der Brettertürme
const Point<int> BOARDS_POS[4] = { Point<int>(0,0), Point<int>(0,0), Point<int>(0,0), Point<int>(0,0) };
/// Relative Position der Steintürme
const Point<int> STONES_POS[4] = { Point<int>(0,0), Point<int>(0,0), Point<int>(0,0), Point<int>(0,0) };


void nobHarborBuilding::Draw(int x,int y)
{
	// Gebäude an sich zeichnen
  	DrawBaseBuilding(x,y);

	// Läuft gerade eine Expedition?
	if(expedition.active)
	{
		// Waren für die Expedition zeichnen

		// Bretter
		for(unsigned char i = 0;i<expedition.boards;++i)
			GetImage(map_lst, 2200+GD_BOARDS)->Draw(x+BOARDS_POS[nation].x-5,y+BOARDS_POS[nation].y-i*4,0,0,0,0,0,0);
		// Steine
		for(unsigned char i = 0;i<expedition.stones;++i)
			GetImage(map_lst, 2200+GD_STONES)->Draw(x+BOARDS_POS[nation].x+8,y+BOARDS_POS[nation].y-i*4,0,0,0,0,0,0);

		// Und den Bauarbeiter, falls er schon da ist
		if(expedition.builder)
		{
			unsigned id = GameClient::inst().GetGlobalAnimation(1000,1,10,this->x+this->y);

			const int WALKING_DISTANCE = 30;

			// Wegstrecke, die er von einem Punkt vom anderen schon gelaufen ist
			int walking_distance = (id%500)*WALKING_DISTANCE/500;
			// Id vom laufen
			unsigned walking_id = (id/32)%8;

			int right_point = x - 20 + BUILDER_POS[nation].x;

			if(id < 500)
			{
				LOADER.GetBobN("jobs")->Draw(23,0,false,walking_id,right_point-walking_distance,
					y+BUILDER_POS[nation].y,COLORS[GAMECLIENT.GetPlayer(player)->color]);
				//DrawShadow(right_point-walking_distance,y,walking_id,0);
			}
			else
			{
				LOADER.GetBobN("jobs")->Draw(23,3,false,walking_id,
					right_point-WALKING_DISTANCE+walking_distance,y+BUILDER_POS[nation].y,
					COLORS[GAMECLIENT.GetPlayer(player)->color]);
				//DrawShadow(right_point-WALKING_DISTANCE+walking_distance,y,walking_id,0);
			}		
		}
			
	}
}


void nobHarborBuilding::HandleEvent(const unsigned int id)
{
	switch(id)
	{
	// Waren-Bestell-Event
	case 10:
		{
			// Mal wieder schauen, ob es Waren für unsere Expedition gibt
			OrderExpeditionWares();
		} break;
	default: HandleBaseEvent(id);
	}
}

/// Startet eine Expedition oder stoppt sie, wenn bereits eine stattfindet
void nobHarborBuilding::StartExpedition()
{
	// Schon eine Expedition gestartet?
	if(expedition.active)
	{
		// Dann diese stoppen
		expedition.active = false;

		// Waren zurücktransferieren
		real_goods.goods[GD_BOARDS] += expedition.boards;
		goods.goods[GD_BOARDS] += expedition.boards;
		real_goods.goods[GD_STONES] += expedition.stones;
		goods.goods[GD_STONES] += expedition.stones;

		if(expedition.builder)
		{
			++real_goods.people[JOB_BUILDER];
			++goods.people[JOB_BUILDER];
			// Evtl. Abnehmer für die Figur wieder finden
			GAMECLIENT.GetPlayer(player)->FindWarehouseForAllJobs(JOB_BUILDER);
		}

		return;
	}

	// Initialisierung
	expedition.active = true;
	
	// In unseren Warenbestand gucken und die erforderlichen Bretter und Steine sowie den 
	// Bauarbeiter holen, falls vorhanden
	expedition.boards = min(unsigned(BUILDING_COSTS[nation][BLD_HARBORBUILDING].boards), real_goods.goods[GD_BOARDS]);
	expedition.stones = min(unsigned(BUILDING_COSTS[nation][BLD_HARBORBUILDING].stones), real_goods.goods[GD_STONES]);
	real_goods.goods[GD_BOARDS] -= expedition.boards;
	goods.goods[GD_BOARDS] -= expedition.boards;
	real_goods.goods[GD_STONES] -= expedition.stones;
	goods.goods[GD_STONES] -= expedition.stones;

	if(real_goods.people[JOB_BUILDER])
	{
		expedition.builder = true;
		--real_goods.people[JOB_BUILDER];
		--goods.people[JOB_BUILDER];
	}
	else
	{
		expedition.builder = false;
		// Bauarbeiter bestellen
		GameClient::inst().GetPlayer(player)->AddJobWanted(JOB_BUILDER,this);
	}

	// Ggf. Waren bestellen, die noch fehlen
	OrderExpeditionWares();

}


/// Bestellt die zusätzlichen erforderlichen Waren für eine Expedition
void nobHarborBuilding::OrderExpeditionWares()
{
	// Waren in der Bestellungsliste mit beachten
	unsigned boards = 0, stones = 0;
	for(list<Ware*>::iterator it = dependent_wares.begin();it.valid();++it)
	{
		if((*it)->type == GD_BOARDS)
			++boards;
		if((*it)->type == GD_STONES)
			++stones;
	}

	// Prüfen, ob jeweils noch weitere Waren bestellt werden müssen
	unsigned todo_boards = 0;
	if(boards + expedition.boards < BUILDING_COSTS[nation][BLD_HARBORBUILDING].boards)
	{
		todo_boards = BUILDING_COSTS[nation][BLD_HARBORBUILDING].boards - (boards + expedition.boards);
		Ware * ware;
		do
		{
			ware = this->players[player].getElement(player)->OrderWare(GD_BOARDS,this);
			if(ware)
			{
				dependent_wares.push_back(ware);
				--todo_boards;
			}
		} while(ware && todo_boards);
	}

	unsigned todo_stones = 0;
	if(stones + expedition.stones < BUILDING_COSTS[nation][BLD_HARBORBUILDING].stones)
	{
		todo_stones = BUILDING_COSTS[nation][BLD_HARBORBUILDING].stones - (stones + expedition.stones);
		Ware * ware;
		do
		{
			ware = this->players[player].getElement(player)->OrderWare(GD_STONES,this);
			if(ware)
			{
				dependent_wares.push_back(ware);
				--todo_stones;
			}
		} while(ware && todo_stones);
	}

	// Wenn immer noch nicht alles da ist, später noch einmal bestellen
	orderware_ev = em->AddEvent(this,210,10);

}


/// Eine bestellte Ware konnte doch nicht kommen
void nobHarborBuilding::WareLost(Ware * ware)
{
	// ggf. neue Waren für Expedition bestellen
	if(expedition.active && (ware->type == GD_BOARDS || ware->type == GD_STONES))
		OrderExpeditionWares();
}



/// Schiff ist angekommen
void nobHarborBuilding::ShipArrived(noShip * ship)
{
	// Verfügbare Aufgaben abklappern

	// Steht Expedition zum Start bereit
	if(expedition.active && expedition.builder 
		&& expedition.boards == BUILDING_COSTS[nation][BLD_HARBORBUILDING].boards
		&& expedition.stones == BUILDING_COSTS[nation][BLD_HARBORBUILDING].stones)
	{
		// Aufräumen am Hafen
		expedition.active = false;
		// Expedition starten
		ship->StartExpedition();
	}
	else
	{
		assert(false);
	}
}

/// Legt eine Ware im Lagerhaus ab
void nobHarborBuilding::AddWare(Ware * ware)
{

	// Brauchen wir die Ware?
	if(expedition.active)
	{
		if((ware->type == GD_BOARDS && expedition.boards < BUILDING_COSTS[nation][BLD_HARBORBUILDING].boards)
			|| (ware->type == GD_STONES && expedition.stones < BUILDING_COSTS[nation][BLD_HARBORBUILDING].stones))
		{
			if(ware->type == GD_BOARDS) ++expedition.boards;
			else if(ware->type == GD_STONES) ++expedition.stones;

			// Ware nicht mehr abhängig
			RemoveDependentWare(ware);
			// Dann zweigen wir die einfach mal für die Expedition ab
			GAMECLIENT.GetPlayer(player)->RemoveWare(ware);
			delete ware;

			// Ggf. ist jetzt alles benötigte da
			CheckExpeditionReady();
			return;
		}
	}

	nobBaseWarehouse::AddWare(ware);

}

/// Prüft, ob eine Expedition von den Waren her vollständig ist und ruft ggf. das Schiff
void nobHarborBuilding::CheckExpeditionReady()
{
	// Alles da?
	if(expedition.boards < BUILDING_COSTS[nation][BLD_HARBORBUILDING].boards)
		return;
	if(expedition.stones < BUILDING_COSTS[nation][BLD_HARBORBUILDING].stones)
		return;
	if(!expedition.builder)
		return;

	// Dann bestellen wir mal das Schiff
	players->getElement(player)->OrderShip(this);
}



/// Schiff konnte nicht mehr kommen
void nobHarborBuilding::ShipLost(noShip * ship)
{
	// Neues Schiff bestellen
	players->getElement(player)->OrderShip(this);
}

/// Gibt die Hafenplatz-ID zurück, auf der der Hafen steht
unsigned nobHarborBuilding::GetHarborPosID() const
{
	return gwg->GetHarborPointID(x,y);
}

