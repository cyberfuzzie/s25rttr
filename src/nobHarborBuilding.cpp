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


void nobHarborBuilding::Draw(int x,int y)
{
	// Gebäude an sich zeichnen
  	DrawBaseBuilding(x,y);
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
