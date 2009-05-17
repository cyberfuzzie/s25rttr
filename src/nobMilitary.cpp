// $Id: nobMilitary.cpp 4877 2009-05-17 09:51:44Z OLiver $
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
#include "nobMilitary.h"
#include "GameClient.h"
#include "GameClientPlayer.h"
#include "GameWorld.h"
#include "PointDistance.h"
#include "MilitaryConsts.h"
#include "Ware.h"
#include "nofPassiveSoldier.h"
#include "nofDefender.h"
#include "nofAggressiveDefender.h"
#include "nofAttacker.h"
#include "Loader.h"
#include "macros.h"
#include "EventManager.h"
#include "Random.h"
#include "nobBaseWarehouse.h"

#include "WindowManager.h"
#include "SerializedGameData.h"
#include "MapGeometry.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nobMilitary::nobMilitary(const BuildingType type,const unsigned short x, const unsigned short y,const unsigned char player,const Nation nation)
: nobBaseMilitary(type,x,y,player,nation), new_built(true), coins(0), disable_coins(false), disable_coins_virtual(false), capturing(false), capturing_soldiers(0), goldorder_event(0), upgrade_event(0)
{
	// Gebäude entsprechend als Militärgebäude registrieren und in ein Militärquadrat eintragen
	GAMECLIENT.GetPlayer(player)->AddMilitaryBuilding(this);
	gwg->GetMilitarySquare(x,y).push_back(this);

	// Größe ermitteln
	switch(type)
	{
	case BLD_BARRACKS: size = 0; break;
	case BLD_GUARDHOUSE: size = 1; break;
	case BLD_WATCHTOWER: size = 2; break;
	case BLD_FORTRESS: size = 3; break;
	default: size = 0xFF; break;
	}

	LookForEnemyBuildings();

	// Tür aufmachen, bis Gebäude besetzt ist
	OpenDoor();

	// Evtl. Goldmünzen bestellen
	SearchCoins();
}

nobMilitary::~nobMilitary()
{
	// Soldaten vernichten
	for(list<nofPassiveSoldier*>::iterator it = troops.begin();it.valid();++it)
		delete (*it);
}

void nobMilitary::Destroy_nobMilitary()
{
	// Wieder aus dem Militärquadrat rauswerfen
	GAMECLIENT.GetPlayer(player)->RemoveMilitaryBuilding(this);
	gwg->GetMilitarySquare(x,y).erase(this);

	// Bestellungen stornieren
	CancelOrders();

	// Soldaten rausschicken
	for(list<nofPassiveSoldier*>::iterator it = troops.begin();it.valid();++it)
		(*it)->InBuildingDestroyed();

	troops.clear();

	// Events ggf. entfernen
	em->RemoveEvent(goldorder_event);
	em->RemoveEvent(upgrade_event);

	// übriggebliebene Goldmünzen in der Inventur abmelden
	GAMECLIENT.GetPlayer(player)->DecreaseInventoryWare(GD_COINS,coins);

	Destroy_nobBaseMilitary();

	// Land drumherum neu berechnen (nur wenn es schon besetzt wurde!)
	// Nach dem BaseDestroy erst, da in diesem erst das Feuer gesetzt, die Straße gelöscht wird usw.
	if(!new_built)
		gwg->RecalcTerritory(this,MILITARY_RADIUS[size],true);
}

void nobMilitary::Serialize_nobMilitary(SerializedGameData * sgd) const
{
	Serialize_nobBaseMilitary(sgd);

	sgd->PushBool(new_built);
	sgd->PushUnsignedChar(coins);
	sgd->PushBool(disable_coins);
	sgd->PushBool(disable_coins_virtual);
	sgd->PushUnsignedChar(frontier_distance);
	sgd->PushUnsignedChar(size);
	sgd->PushBool(capturing);
	sgd->PushUnsignedInt(capturing_soldiers);
	sgd->PushObject(goldorder_event,true);
	sgd->PushObject(upgrade_event,true);

	sgd->PushObjectList(ordered_troops,true);
	sgd->PushObjectList(ordered_coins,true);
	sgd->PushObjectList(troops,true);
}

nobMilitary::nobMilitary(SerializedGameData * sgd, const unsigned obj_id) : nobBaseMilitary(sgd,obj_id),
new_built(sgd->PopBool()),
coins(sgd->PopUnsignedChar()),
disable_coins(sgd->PopBool()),
disable_coins_virtual(sgd->PopBool()),
frontier_distance(sgd->PopUnsignedChar()),
size(sgd->PopUnsignedChar()),
capturing(sgd->PopBool()),
capturing_soldiers(sgd->PopUnsignedInt()),
goldorder_event(sgd->PopObject<EventManager::Event>(GOT_EVENT)),
upgrade_event(sgd->PopObject<EventManager::Event>(GOT_EVENT))
{
	sgd->PopObjectList(ordered_troops,GOT_NOF_PASSIVESOLDIER);
	sgd->PopObjectList(ordered_coins,GOT_WARE);
	sgd->PopObjectList(troops,GOT_NOF_PASSIVESOLDIER);

	// ins Militärquadrat einfügen
	gwg->GetMilitarySquare(x,y).push_back(this);
}


void nobMilitary::Draw(int x, int y)
{
	// Gebäude an sich zeichnen
	DrawBaseBuilding(x,y);


	// (max 4) Besatzungs-Fähnchen zeichnen
	unsigned flags = min<unsigned>(troops.size()+this->leave_house.size(),4);

	for(unsigned i = 0;i<flags;++i)
		GetMapPlayerImage(3162+GAMECLIENT.GetGlobalAnimation(8,80,40,this->x*this->y*i))->Draw(x+TROOPS_FLAGS[nation][size][0],y+TROOPS_FLAGS[nation][size][1]+(i)*3,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);

	// Die Fahne, die anzeigt wie weit das Gebäude von der Grenze entfernt ist, zeichnen
	GetMapBob(3150+frontier_distance*4+GAMECLIENT.GetGlobalAnimation(8,5,5,this->x*this->y*age)/2)
		->Draw(x+BORDER_FLAGS[nation][size][0],y+BORDER_FLAGS[nation][size][1],0,0,0,0,0,0);

	// Wenn Goldzufuhr gestoppt ist, Schild außen am Gebäude zeichnen zeichnen
	if(disable_coins_virtual)
		GetMapBob(46)->Draw(x+BUILDING_SIGN_CONSTS[nation][type].x,y+BUILDING_SIGN_CONSTS[nation][type].y,0,0,0,0,0,0);



	//char number[256];
	//sprintf(number,"%u",this->obj_id);
	//NormalFont->Draw(x,y,number,0,0xFF00FF00);
}

void nobMilitary::HandleEvent(const unsigned int id)
{
	switch(id)
	{
	// "Rausgeh-Event"
	case 0:
		{
			leaving_event = 0;

			// Sind Leute da, die noch rausgehen wollen?
			if(leave_house.size())
			{
				// Dann raus mit denen
				noFigure * soldier = *leave_house.begin();
				gwg->AddFigure(soldier,x,y);

				soldier->ActAtFirst();
				leave_house.pop_front();
			}
			else
			{
				go_out = false;
			}

			// Wenn noch weitere drin sind, die müssen auch noch raus
			if(leave_house.size())
				leaving_event = em->AddEvent(this,30+RANDOM.Rand(__FILE__,__LINE__,obj_id,10));
			else
				go_out = false;

		} break;
	// Goldbestell-Event
	case 1:
		{
			goldorder_event = 0;

			// ggf. nach neuen Goldmünzen suchen
			SearchCoins();
		} break;
	// Beförderungs-Event
	case 2:
		{
			upgrade_event = 0;

			// Soldaten befördern
			// Von hinten durchgehen
			// Wenn der nachfolgende (schwächere) Soldat einen niedrigeren Rang hat,
			// wird dieser ebenfalls befördert usw.!

			// wurde schon ein Soldat befördert (der nächste Soldat muss einen niedrigeren Rang haben, wenn er
			// auch befördert werden soll)
			bool upgraded = false;

			for(list<nofPassiveSoldier*>::iterator it = troops.end();it.valid();--it)
			{
				if(upgraded)
				{
					// Es wurde schon einer befördert, dieser Soldat muss nun einen niedrigeren Rang
					// als der letzte haben, damit er auch noch befördert werden kann, ansonsten Schleife beenden, a
					// niemand weiter mehr befördert werden darf
					if((*it)->GetRank() < (*it.GetNext())->GetRank()-1)
						(*it)->Upgrade();
					else
						break;
				}
				else
				{
					// Lässt sich der Soldat noch befördern?
					if((*it)->GetRank()<4)
					{
						// Dann befördern
						(*it)->Upgrade();
						// Wir haben einen befördert
						upgraded = true;
					}
				}
			}

			// Wurde jemand befördert?
			if(upgraded)
			{
				// Goldmünze verbrauchen
				--coins;
				GAMECLIENT.GetPlayer(player)->DecreaseInventoryWare(GD_COINS,1);

				// Evtl neues Beförderungsevent anmelden
				PrepareUpgrading();
			}

		} break;
	}
}

unsigned short nobMilitary::GetMilitaryRadius() const
{
	return MILITARY_RADIUS[size];
}

void nobMilitary::LookForEnemyBuildings()
{
	// Umgebung nach Militärgebäuden absuchen
	list<nobBaseMilitary*> buildings;
	gwg->LookForMilitaryBuildings(buildings,x,y,4);
	frontier_distance = 0;


	for(list<nobBaseMilitary*>::iterator it = buildings.begin();it.valid();++it)
	{
		// feindliches Militärgebäude?
		if((*it)->GetPlayer() != player && !GAMECLIENT.GetPlayer((*it)->GetPlayer())->IsAlly(player))
		{
			unsigned distance = CalcDistance(x,y,(*it)->GetX(),(*it)->GetY());

		// in nahem Umkreis, also Grenzen berühren sich
		if(distance <= MILITARY_RADIUS[size] + MILITARY_RADIUS[static_cast<nobMilitary*>(*it)->size]) // warum erzeugtn das ne warning in vs2008?
			{
				// Grenznähe entsprechend setzen
				frontier_distance = 2;

				// Wenns ein richtiges Militärgebäude ist, dann dort auch entsprechend setzen
				if((*it)->GetBuildingType() >= BLD_BARRACKS && (*it)->GetBuildingType() <= BLD_FORTRESS)
					static_cast<nobMilitary*>(*it)->NewEnemyMilitaryBuilding(2);
			}
			// in mittlerem Umkreis, also theoretisch angreifbar?
      else if(distance <= BASE_ATTACKING_DISTANCE 
        + TROOPS_COUNT[(*it)->GetNation()][static_cast<nobMilitary*>(*it)->size] * EXTENDED_ATTACKING_DISTANCE)
			{
				// Grenznähe entsprechend setzen
				if(!frontier_distance)
					frontier_distance = 1;

				// Wenns ein richtiges Militärgebäude ist, dann dort auch entsprechend setzen
				if((*it)->GetBuildingType() >= BLD_BARRACKS && (*it)->GetBuildingType() <= BLD_FORTRESS)
					static_cast<nobMilitary*>(*it)->NewEnemyMilitaryBuilding(1);
			}
		}
	}

	// Truppen schicken
	RegulateTroops();

}


void nobMilitary::NewEnemyMilitaryBuilding(const unsigned short distance)
{
	// Neues Grenzgebäude in der Nähe --> Distanz entsprechend setzen
	if(distance == 2)
	{
		// Nah
		frontier_distance = 2;
	}
	// in mittlerem Umkreis?
	else if(distance == 1)
	{
		// Mittel (nur wenns vorher auf weit weg war)
		if(!frontier_distance)
			frontier_distance = 1;
	}

	RegulateTroops();
}


void nobMilitary::RegulateTroops()
{
	// Wenn das Gebäude eingenommen wird, erstmal keine neuen Truppen und warten, wieviele noch reinkommen
	if(capturing)
		return;

	// Zu viele oder zu wenig Truppen?
	int diff;
	if((diff = CalcTroopsCount() - int(troops.size()+ordered_troops.size()+troops_on_mission.size()+((defender)?1:0) + capturing_soldiers)) < 0)
	{
		// Zu viel --> überflüssige Truppen nach Hause schicken

		// Zuerst die bestellten Soldaten wegschicken
		for(list<nofPassiveSoldier*>::iterator it = ordered_troops.begin();diff&&ordered_troops.size();++diff,++it)
		{
			(*it)->NotNeeded();
			ordered_troops.erase(&it);
		}

		// Nur rausschicken, wenn es einen Weg zu einem Lagerhaus gibt!
		if(GameClient::inst().GetPlayer(player)->FindWarehouse(this,FW::NoCondition,0,true,0,false))
		{
			// Dann den Rest (einer muss immer noch drinbleiben!)
			for(list<nofPassiveSoldier*>::iterator it = troops.begin();diff&&troops.size()>1;++diff,++it)
			{
				(*it)->LeaveBuilding();
				AddLeavingFigure(*it);
				troops.erase(&it);
			}
		}

	}
	else if(diff)
	{
		// Zu wenig Truppen --> neue bestellen
		GAMECLIENT.GetPlayer(player)->OrderTroops(this,diff);

	}
}

int nobMilitary::CalcTroopsCount()
{
	return (TROOPS_COUNT[nation][size]-1)*GAMECLIENT.GetPlayer(player)->military_settings[4+frontier_distance]/5 + 1;
}

void nobMilitary::TakeWare(Ware * ware)
{
	// Goldmünze in Bestellliste aufnehmen
	ordered_coins.push_back(ware);
}


void nobMilitary::AddWare(Ware * ware)
{
	// Ein Golstück mehr
	++coins;
	// aus der Bestellliste raushaun
	ordered_coins.erase(ware);

	// Ware vernichten
	GAMECLIENT.GetPlayer(player)->RemoveWare(ware);
	delete ware;

	// Evtl. Soldaten befördern
	PrepareUpgrading();
}

void nobMilitary::WareLost(Ware * ware)
{
	// Ein Goldstück konnte nicht kommen --> aus der Bestellliste entfernen
	ordered_coins.erase(ware);
}

bool nobMilitary::FreePlaceAtFlag()
{
	return false;
}
void nobMilitary::GotWorker(Job job, noFigure * worker)
{
	ordered_troops.push_back(static_cast<nofPassiveSoldier*>(worker));
}

void nobMilitary::CancelOrders()
{
	// Soldaten zurückschicken
	for(list<nofPassiveSoldier*>::iterator it = ordered_troops.begin();it.valid();++it)
		(*it)->NotNeeded();

	ordered_troops.clear();

	// Goldmünzen zurückschicken
	for(list<Ware*>::iterator it = ordered_coins.begin();it.valid();++it)
		WareNotNeeded(*it);

	ordered_coins.clear();
}

void nobMilitary::AddActiveSoldier(nofActiveSoldier * soldier)
{
	

	// aktiver Soldat, eingetroffen werden --> dieser muss erst in einen passiven Soldaten 
	// umoperiert werden (neu erzeugt und alter zerstört) werden
	nofPassiveSoldier * passive_soldier = new nofPassiveSoldier(*soldier);

	// neuen Soldaten einhängen
	AddPassiveSoldier(passive_soldier);

	// alten Soldaten später vernichten
	em->AddToKillList(soldier);

}

void nobMilitary::AddPassiveSoldier(nofPassiveSoldier * soldier)
{
	assert(soldier->GetPlayer() == player);
	assert(troops.size() < unsigned(TROOPS_COUNT[nation][size]));

	bool inserted = false;

	// Nach Rang sortiert einfügen!
	for(list<nofPassiveSoldier*>::iterator it = troops.end(); it.valid(); --it)
	{
		// Ist das einzufügende Item größer als das aktuelle?
		if(soldier->GetRank() >= (*it)->GetRank())
		{
			// ja dann hier einfügen
			troops.insert(it, soldier);
			inserted = true;
			break;
		}
	}

	if(!inserted)
		// Ansonsten ganz nach vorn
		troops.push_front(soldier);


	// und aus den bestllten Truppen raushauen, da er ja jetzt hier ist
	ordered_troops.erase(soldier);


	// Wurde dieses Gebäude zum ersten Mal besetzt?
	if(new_built)
	{
		// Ist nun besetzt
		new_built = false;
		// Alter ab jetzt setzen
		SetAge();
		// Landgrenzen verschieben
		gwg->RecalcTerritory(this,MILITARY_RADIUS[size],false);
		// Tür zumachen
		CloseDoor();
		// Fanfarensound abspieln, falls das Militärgebäude im Sichtbereich ist und unseres ist
		gwg->MilitaryBuildingCaptured(x,y,player);
	}
	else
	{
		// Evtl. Soldaten befördern
		PrepareUpgrading();
	}

	// Goldmünzen suchen, evtl sinds ja neue Soldaten
	SearchCoins();
}


void nobMilitary::SoldierLost(nofSoldier * soldier)
{
	// Soldat konnte nicht (mehr) kommen --> rauswerfen und ggf. neue Soldaten rufen
	ordered_troops.erase(static_cast<nofPassiveSoldier*>(soldier));
	troops_on_mission.erase(static_cast<nofActiveSoldier*>(soldier));
	RegulateTroops();
}

void nobMilitary::SoldierOnMission(nofPassiveSoldier * passive_soldier,nofActiveSoldier * active_soldier)
{
	// Aus der Besatzungsliste raushauen, aber noch mit merken
	troops.erase(passive_soldier);
	troops_on_mission.push_back(active_soldier);
}

list<nofPassiveSoldier*>::iterator nobMilitary::ChooseSoldier()
{
	// Überhaupt Soldaten da?
	if(!troops.size())
		return 0;

	// verschiedene Ränge zählen
	list<nofPassiveSoldier*>::iterator candidates[5] = {0,0,0,0,0}; // Kandidaten für die einzelnen Ränge

	// wie viele verschiedene Ränge?
	unsigned rank_count = 0;

	for(list<nofPassiveSoldier*>::iterator it = troops.begin();it.valid();++it)
	{
		if(!candidates[(*it)->GetRank()].valid())
		{
			++rank_count;
			candidates[(*it)->GetRank()] = it;
		}
	}

	// ID ausrechnen
	unsigned rank = ((rank_count-1)*GAMECLIENT.GetPlayer(player)->military_settings[1]+2)/5;

	unsigned r = 0;

	// richtigen Rang suchen
	for(unsigned i = 0;i<5;++i)
	{
		if(candidates[i].valid())
		{
			if(r == rank)
				// diesen Soldaten wollen wir
				return candidates[i];

			++r;
		}
	}

	return 0;
}

nofAggressiveDefender * nobMilitary::SendDefender(nofAttacker * attacker)
{
	// Sind noch Soldaten da?
	if(troops.size() > 1)
	{
		// Verteidiger auswählen
		list<nofPassiveSoldier*>::iterator soldier = ChooseSoldier();
		// neuen aggressiven Verteidiger daraus erzeugen
		nofAggressiveDefender * ad = new nofAggressiveDefender(*soldier,attacker);
		// soll rausgehen
		AddLeavingFigure(ad);
		// auf die Missionsliste setzen
		troops_on_mission.push_back(ad);
		// alten passiven Soldaten vernichten
		(*soldier)->Destroy();
		delete *soldier;
		// aus den Truppen rauswerfen
		troops.erase(soldier);
		
		return ad;
	}
	else
		return 0;
}

nofDefender * nobMilitary::ProvideDefender(nofAttacker * const attacker)
{
	// Überhaupt Soldaten da?
	if(!troops.size())
		return 0;

	list<nofPassiveSoldier*>::iterator soldier = ChooseSoldier();

	// neuen Verteidiger erzeugen 
	nofDefender * defender = new nofDefender(*soldier,attacker);

	// und vernichten
	(*soldier)->Destroy();
	delete *soldier;

	// aus der Liste entfernen
	troops.erase(soldier);
	
	return defender;
}

void nobMilitary::Capture(const unsigned char new_owner)
{
	// Goldmünzen in der Inventur vom alten Spieler abziehen und dem neuen hinzufügen
	GAMECLIENT.GetPlayer(player)->DecreaseInventoryWare(GD_COINS,coins);
	GAMECLIENT.GetPlayer(new_owner)->IncreaseInventoryWare(GD_COINS,coins);

	// Soldaten, die auf Mission sind, Bescheid sagen
	for(list<nofActiveSoldier*>::iterator it = troops_on_mission.begin();it.valid();++it)
		(*it)->HomeDestroyed();

	// Aggressiv-Verteidigenden Soldaten Bescheid sagen, dass sie nach Hause gehen können
	for(list<nofAggressiveDefender*>::iterator it = aggressive_defenders.begin();it.valid();++it)
		(*it)->AttackedGoalDestroyed();

	troops_on_mission.clear();
	aggressive_defenders.clear();

	// In der Wirtschaftsverwaltung dieses Gebäude jetzt zum neuen Spieler zählen und beim alten raushauen
	GAMECLIENT.GetPlayer(player)->RemoveMilitaryBuilding(this);
	GAMECLIENT.GetPlayer(new_owner)->AddMilitaryBuilding(this);

	// Alten Besitzer merken
	unsigned char old_player = player;

	// neuer Spieler
	player = new_owner;

	// Flagge davor auch übernehmen
	GetFlag()->Capture(new_owner);

	// Territorium neu berechnen
	gwg->RecalcTerritory(this,MILITARY_RADIUS[size],false);

	// Sichtbarkeiten berechnen für alten Spieler
	gwg->RecalcVisibilitiesAroundPoint(GetX(),GetY(),GetMilitaryRadius()+VISUALRANGE_MILITARY+1,old_player, false);

	// Grenzflagge entsprechend neu setzen von den Feinden
	LookForEnemyBuildings();
	// und von den Verbündeten (da ja ein Feindgebäude weg ist)!
	list<nobBaseMilitary*> buildings;
	gwg->LookForMilitaryBuildings(buildings,x,y,4);

	for(list<nobBaseMilitary*>::iterator it = buildings.begin();it.valid();++it)
	{
		// verbündetes Gebäude?
		if(((*it)->GetPlayer() == player || GAMECLIENT.GetPlayer((*it)->GetPlayer())->IsAlly(player))
			&& (*it)->GetBuildingType() >= BLD_BARRACKS && (*it)->GetBuildingType() <= BLD_FORTRESS)
			// Grenzflaggen von dem neu berechnen
			static_cast<nobMilitary*>(*it)->LookForEnemyBuildings();
	}

	// ehemalige Leute dieses Gebäudes nach Hause schicken, die ggf. grad auf dem Weg rein/raus waren
	unsigned short coords[4] = {x,y,x+(y&1),y+1};
	for(unsigned short i = 0;i<2;++i)
	{
		for(list<noBase*>::iterator it = gwg->GetFigures(coords[i*2],coords[i*2+1]).begin(); it.valid(); ++it)
		{
			if((*it)->GetType() == NOP_FIGURE)
			{
				if(static_cast<noFigure*>(*it)->GetCurrentRoad() == routes[4] && static_cast<noFigure*>(*it)->GetPlayer() != new_owner)
				{
					static_cast<noFigure*>(*it)->Abrogate();
					static_cast<noFigure*>(*it)->StartWandering();
				}
			}
		}
	}

	// Gebäude wird nun eingenommen
	capturing = true;

	// Soldat, der zum Erobern reinläuft, ist nun drinne --> Anzahl der erobernden Soldaten entsprechend verringern
	assert(capturing_soldiers);
	--capturing_soldiers;

	// Fanfarensound abspieln, falls das Militärgebäude im Sichtbereich ist und unseres ist
	gwg->MilitaryBuildingCaptured(x,y,player);
}

void nobMilitary::NeedOccupyingTroops(const unsigned char new_owner)
{
	// Brauchen wir noch Soldaten (ein Soldat kommt ja noch rein), keine Soldaten von anderen Spielern 
	// wählen (z.B. "Kollektivangriffen"), manchmal ist es egal, wer reinkommt (new_owner == 0xFF)

	// Soldaten wählen, der am nächsten an der Flagge steht, damit nicht welche von ganze hinten ewige Zeit vor
	// latschen müssen
	nofAttacker * best_attacker = 0;
	unsigned best_radius = 0xFFFFFFFF;

	if(unsigned(CalcTroopsCount()) > troops.size()+capturing_soldiers)
	{
		// Soldaten absuchen
		for(list<nofAttacker*>::iterator it = aggressors.begin();it.valid();++it)
		{
			// Steht der Soldat überhaupt um das Gebäude rum?
			if((*it)->IsAttackerReady() && ((*it)->GetPlayer() == new_owner || new_owner == 0xFF))
			{
				// Näher als der bisher beste?
				if((*it)->GetRadius() < best_radius)
				{
					// Und kommt er überhaupt zur Flagge (könnte ja in der 2. Reihe stehen, sodass die
					// vor ihm ihn den Weg versperren)?
					if(gwg->FindHumanPath((*it)->GetX(),(*it)->GetY(),x+(y&1),y+1,10,false) != 0xFF)
					{
						// Dann is das der bisher beste
						best_attacker = *it;
						best_radius = best_attacker->GetRadius();
					}
				}
			}
		}

		// Einen gefunden?
		if(best_attacker)
		{
			// Dann soll der hingehen
			best_attacker->CaptureBuilding();
			++capturing_soldiers;
			// und raus hier
			return;
		}

		// keine Soldaten mehr gefunden, der Rest (der noch nicht da ist) kann wieder nach Hause gehen
		// Achtung: Hier können Iteratoren gelöscht werden in CapturedBuildingFull, daher Sicherheitsschleife!
		list<nofAttacker*>::iterator next_it;
		for(list<nofAttacker*>::iterator it = aggressors.begin();it.valid();it = next_it)
		{
			next_it = it.GetNext();
			nofAttacker * attacker = *it;
			// Nicht gerade Soldaten löschen, die das Gebäude noch einnehmen!
			if(attacker->GetState() != nofActiveSoldier::STATE_ATTACKING_CAPTURINGNEXT)
			{
				aggressors.erase(it);
				attacker->CapturedBuildingFull();
			}
		}


		// Einnahme beendet
		capturing = false;
	}
	else
	{
		// keine Soldaten mehr benötigt, der Rest kann wieder nach Hause gehen
		// Achtung: Hier können Iteratoren gelöscht werden in CapturedBuildingFull, daher Sicherheitsschleife!
		list<nofAttacker*>::iterator next_it;
		for(list<nofAttacker*>::iterator it = aggressors.begin();it.valid();it = next_it)
		{
			next_it = it.GetNext();
			nofAttacker * attacker = *it;
			// Nicht gerade Soldaten löschen, die das Gebäude noch einnehmen!
			if(attacker->GetState() != nofActiveSoldier::STATE_ATTACKING_CAPTURINGNEXT)
			{
				aggressors.erase(it);
				attacker->CapturedBuildingFull();
			}
		}

		// Einnahme beendet
		capturing = false;

		// Nun die Besetzung prüfen
		RegulateTroops();

	}
}

void nobMilitary::StopGold()
{
	// Umstellen
	disable_coins = !disable_coins;
	// Wenn das von einem fremden Spieler umgestellt wurde (oder vom Replay), muss auch das visuelle umgestellt werden
	if(GAMECLIENT.GetPlayerID() != player || GAMECLIENT.IsReplayModeOn())
		disable_coins_virtual = !disable_coins_virtual;

	// Evtl neue Goldmünzen bestellen, wenn das umgestellt wurde
	if(!disable_coins)
		SearchCoins();
}


unsigned nobMilitary::CalcCoinsPoints()
{
	// Will ich überhaupt Goldmünzen, wenn nich, sofort raus
	if(!WantCoins())
		return 0;

	// 10000 als Basis wählen, damit man auch noch was abziehen kann
	unsigned short points = 10000;

	// Wenn hier schon Münzen drin sind oder welche bestellt sind, wirkt sich das natürlich negativ auf die "Wichtigkeit" aus
	points -= (coins+ordered_coins.size())*30;

	// Beförderbare Soldaten zählen
	for(list<nofPassiveSoldier*>::iterator it = troops.begin();it.valid();++it)
	{
		// Solange es kein General (4) ist, kann der Soldat noch befördert werden
		if((*it)->GetRank() < 4)
			points+=20;
	}

	return points;
}

bool nobMilitary::WantCoins()
{
	// Wenn die Goldzufuhr gestoppt wurde oder Münzvorrat voll ist, will ich gar keine Goldmünzen
	return (!disable_coins && coins + ordered_coins.size() != GOLD_COUNT[nation][size] && !new_built);
}

void nobMilitary::SearchCoins()
{
	// Brauche ich überhaupt Goldmünzen bzw. hab ich vielleicht schon ein Event angemeldet?
	if(WantCoins() && !goldorder_event)
	{
		// Lagerhaus mit Goldmünzen suchen
		FW::Param_Ware p = {GD_COINS,1};
		if(nobBaseWarehouse * wh = GAMECLIENT.GetPlayer(player)->FindWarehouse(this,FW::Condition_Ware,0,false,&p,false))
		{
			// Wenns eins gibt, dort eine Goldmünze bestellen
			Ware * ware = wh->OrderWare(GD_COINS,this);

			if(!ware)
			{
				// Ware dürfte nicht 0 werden, da ja ein Lagerhaus MIT GOLDMÜNZEN bereits gesucht wird
				LOG.lprintf("nobMilitary::SearchCoins: WARNING: ware = 0. Bug alarm!\n");
				return;
			}

			// Goldmünze zu den Bestellungen hinzufügen
			ordered_coins.push_back(ware);

			// Nach einer Weile nochmal nach evtl neuen Goldmünzen gucken
			goldorder_event = em->AddEvent(this,200+RANDOM.Rand(__FILE__,__LINE__,obj_id,400),1);
		}
	}
}

void nobMilitary::PrepareUpgrading()
{
	// Goldmünzen da?
	if(!coins)
		return;

	// Gibts auch noch kein Beförderungsevent?
	if(upgrade_event)
		return;

	// Noch Soldaten, die befördert werden können?
	bool soldiers_available = false;

	for(list<nofPassiveSoldier*>::iterator it = troops.begin();it.valid();++it)
	{
		if((*it)->GetRank() < 4)
		{
			// es wurde ein Soldat gefunden, der befördert werden kann
			soldiers_available = true;
			break;
		}
	}

	if(!soldiers_available)
		return;

	// Alles da --> Beförderungsevent anmelden
	upgrade_event = em->AddEvent(this,UPGRADE_TIME+RANDOM.Rand(__FILE__,__LINE__,obj_id,UPGRADE_TIME_RANDOM),2);
}

void nobMilitary::HitOfCatapultStone()
{
	// Ein Soldat weniger, falls es noch welche gibt
	if(troops.size())
	{
		(*troops.begin())->Die();
		troops.pop_front();
	}

	// Kein Soldat mehr da? Haus abfackeln
	if(!troops.size())
		Destroy();
	else
		// ansonsten noch neue Soldaten ggf. bestellen
		RegulateTroops();
}

/// Darf das Militärgebäude abgerissen werden (Abriss-Verbot berücksichtigen)?
bool nobMilitary::IsDemolitionAllowed() const
{
	if(GameClient::inst().GetGGS().demolition_prohibition
		== GlobalGameSettings::DP_UNDERATTACK)
	{
		// Prüfen, ob das Gebäude angegriffen wird
		if(aggressors.size())
			return false;

	}
	else if(GameClient::inst().GetGGS().demolition_prohibition
		== GlobalGameSettings::DP_NEARFRONTIERS)
	{
		// Prüfen, ob es in Grenznähe steht
		if(frontier_distance == 2)
			return false;
	}

	return true;

}

