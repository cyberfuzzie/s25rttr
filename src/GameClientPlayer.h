// $Id: GameClientPlayer.h 4746 2009-04-30 20:10:46Z OLiver $
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

#ifndef GAMECLIENTPLAYER_H_
#define GAMECLIENTPLAYER_H_

#include "GamePlayerInfo.h"
#include "GameConsts.h"

class GameWorld;
class noFlag;
class RoadSegment;
class nobBaseWarehouse;
class noRoadNode;
class noBaseBuilding;
class noBuilding;
class noBuildingSite;
class Ware;
class nobMilitary;
class nobUsual;
class nofFlagWorker;
class nobBaseMilitary;
class SerializedGameData;
class nofCarrier;
class GameWorldGame;


/// Informationen über Gebäude-Anzahlen
struct BuildingCount
{
	unsigned building_counts[40];
	unsigned building_site_counts[40];
};


class GameClientPlayer : public GamePlayerInfo
{
private:

	// Zugriff der Spieler auf die Spielwelt
	GameWorldGame * gwg;
	/// Liste der Warenhäuser des Spielers
	list<nobBaseWarehouse*> warehouses;
	///// Liste von unbesetzten Straßen (ohne Träger) von dem Spieler
	//list<RoadSegment*> unoccupied_roads;
	/// Lister aller Straßen von dem Spieler
	list<RoadSegment*> roads;

	struct JobNeeded
	{
		Job job;
		noRoadNode * workplace;
	};

	struct BuildingWhichWantWare
	{
		unsigned char count;
		unsigned char building;
	};

	/// Liste von Baustellen/Gebäuden, die bestimmten Beruf wollen
	list<JobNeeded> jobs_wanted;

	/// Listen der einzelnen Gebäudetypen (nur nobUsuals!)
	list<nobUsual*> buildings[30];
	/// Liste von sämtlichen Baustellen
	list<noBuildingSite*> building_sites;
	/// Liste von allen Militärgebäuden
	list<nobMilitary*> military_buildings;
	/// Liste von sämtlichen Waren, die herumgetragen werden und an Fahnen liegen
	list<Ware*> ware_list;
	/// Liste von Geologen und Spähern, die an eine Flagge gebunden sind
	list<nofFlagWorker*> flagworkers;

	/// Liste, welchen nächsten 10 Angreifern Verteidiger entgegenlaufen sollen
	bool defenders[5];
	unsigned short defenders_pos;

	/// Inventur
	Goods global_inventory;

public:

	/// Laggt der Spieler?
	bool is_lagging;

	/// Koordinaten des HQs des Spielers
	unsigned short hqx,hqy;

	// Informationen über die Verteilung
	struct
	{
		unsigned char percent_buildings[40];
		list<BuildingType> client_buildings; // alle Gebäude, die diese Ware bekommen, zusammengefasst
		std::vector<unsigned char> goals;
		unsigned selected_goal;
	} distribution[WARE_TYPES_COUNT];

	/// Art der Reihenfolge (0 = nach Auftraggebung, ansonsten nach build_order)
	unsigned char order_type;
	/// Baureihenfolge
	unsigned char build_order[31];
	/// Prioritäten der Waren im Transport
	unsigned char transport[WARE_TYPES_COUNT];
	/// Militäreinstellungen (die vom Militärmenü)
	unsigned char military_settings[7];
	/// Werkzeugeinstellungen (in der Reihenfolge wie im Fenster!)
	unsigned char tools_settings[12];

private:


	// Sucht Weg für Job zu entsprechenden noRoadNode
	bool FindWarehouseForJob(const Job job, noRoadNode * goal);
	/// Prüft, ob der Spieler besiegt wurde
	void TestDefeat();

public:
	/// Konstruktor von @p GameClientPlayer.
	GameClientPlayer(const unsigned playerid);

	/// Serialisieren
	void Serialize(SerializedGameData * sgd);
	// Deserialisieren
	void Deserialize(SerializedGameData * sgd);

	/// Tauscht Spieler
	void SwapPlayer(GameClientPlayer& two);

	/// Setzt GameWorld
	void SetGameWorldPointer(GameWorldGame * const gwg) { this->gwg = gwg; }

	/*/// liefert das aktuelle (komplette) inventar.
	void GetInventory(unsigned int *wares, unsigned int *figures);*/


	/// Sucht ein nächstgelegenes Warenhaus für den Punkt 'start', das die Bedingung der Übergebenen Funktion
	/// IsWarehouseGood erfüllt, als letzen Parameter erhält jene Funktion param
	/// - forbidden ist ein optionales Straßenstück, das nicht betreten werden darf,
	/// - to_wh muss auf true gesetzt werden, wenn es zum Lagerhaus geht, ansonsten auf false, in length wird die Wegeslänge zurückgegeben
	nobBaseWarehouse * FindWarehouse(const noRoadNode * const start,bool (*IsWarehouseGood)(nobBaseWarehouse*,const void*),const RoadSegment * const forbidden,const bool to_wh,const void * param,const bool use_boat_roads,unsigned * const length = 0);
	/// Gibt dem Spieler bekannt, das eine neue Straße gebaut wurde
	void NewRoad(RoadSegment * const rs);
	/// Neue Straße hinzufügen
	void AddRoad(RoadSegment * const rs) { roads.push_back(rs); }
	/// Gibt dem Spieler bekannt, das eine Straße abgerissen wurde
	void RoadDestroyed();
	/// Sucht einen Träger für die Straße und ruft ggf den Träger aus dem jeweiligen nächsten Lagerhaus
	bool FindCarrierForRoad(RoadSegment * rs);
	/// Warenhaus zur Warenhausliste hinzufügen
	void AddWarehouse(nobBaseWarehouse * wh) { warehouses.push_back(wh); }
	/// Warenhaus aus Warenhausliste entfernen
	void RemoveWarehouse(nobBaseWarehouse * wh) { warehouses.erase(wh); TestDefeat(); }
	/// (Unbesetzte) Straße aus der Liste entfernen
	void DeleteRoad(RoadSegment * rs) { roads.erase(rs); }
	bool TestRoads(RoadSegment * rs) { return roads.search(rs).valid(); }

	/// Für alle unbesetzen Straßen Weg neu berechnen
	void FindWarehouseForAllRoads();
	/// Lässt alle Baustellen ggf. noch vorhandenes Baumaterial bestellen
	void FindMaterialForBuildingSites();
	/// Fügt ein RoadNode hinzu, der einen bestimmten Job braucht
	void AddJobWanted(const Job job,noRoadNode * workplace);
	/// Entfernt ihn wieder aus der Liste (wenn er dann doch nich mehr gebraucht wird)
	void JobNotWanted(noRoadNode * workplace);
	/// Versucht für alle Arbeitsplätze eine Arbeitskraft zu suchen
	void FindWarehouseForAllJobs(const Job job);
	/// Versucht für alle verlorenen Waren ohne Ziel Lagerhaus zu finden
	void FindClientForLostWares();
	/// Bestellt eine Ware und gibt sie zurück, falls es eine gibt, ansonsten 0
	Ware * OrderWare(const GoodType ware,noBaseBuilding * goal);
	/// Versucht einen Esel zu bestellen, gibt 0 zurück, falls keinen gefunden
	nofCarrier * OrderDonkey(RoadSegment * road);
	/// Versucht für einen Esel eine Straße zu finden, in goal wird die Zielflagge zurückgegeben,
	/// sofern eine Straße gefunden wurde, ansonsten ist das ein Lagerhaus oder 0, falls auch das nich gefunden wurde
	RoadSegment * FindRoadForDonkey(noRoadNode * start,noRoadNode ** goal);


	/// Sucht für eine (neuproduzierte) Ware einen Abnehmer (wenns keinen gibt, wird ein Lagerhaus gesucht, wenn
	/// es auch dorthin keinen Weg gibt, wird 0 zurückgegeben
	noBaseBuilding * FindClientForWare(Ware * ware);
	/// Sucht einen Abnehmer (sprich Militärgebäude), wenn es keinen findet, wird ein Warenhaus zurückgegeben bzw. 0
	nobBaseMilitary * FindClientForCoin(Ware * ware);

	/// Speichert Baustellen Gebäude etc, erklärt sich von selbst
	void AddBuildingSite(noBuildingSite * building_site);
	void RemoveBuildingSite(noBuildingSite * building_site);
	/// Speichert normale Gebäude
	void AddUsualBuilding(nobUsual * building);
	void RemoveUsualBuilding(nobUsual * building);
	/// Speichert Militärgebäude
	void AddMilitaryBuilding(nobMilitary * building);
	void RemoveMilitaryBuilding(nobMilitary * building);

	/// Gibt Liste von Gebäuden des Spieler zurück
	const list<nobUsual*>& GetBuildings(const BuildingType type);
	/// Liefert die Anzahl aller Gebäude einzeln
	void GetBuildingCount(BuildingCount& bc) const;
	/// Berechnet die durschnittlichen Produktivität eines jeden Gebäudetyps
	/// (erwartet als Argument ein 40-er Array!)
	void CalcProductivities(std::vector<unsigned short>& productivities); 

	/// Gibt Priorität der Baustelle zurück (entscheidet selbständig, welche Reihenfolge usw)
	/// je kleiner die Rückgabe, destro größer die Priorität!
	unsigned GetBuidingSitePriority(const noBuildingSite * building_site);

	/// Berechnet die Verteilung der Waren auf die einzelnen Gebäude neu
	void RecalcDistribution();
	/// Berechnet die Verteilung einer (bestimmten) Ware
	void RecalcDistributionOfWare(const GoodType ware);
	/// Konvertiert die Daten vom wp_transport in "unser" Prioritäten-Format und setzt es
	void ConvertTransportData(const unsigned char * const transport_data);

	/// Ware zur globalen Warenliste hinzufügen und entfernen
	void RegisterWare(Ware * ware) { ware_list.push_back(ware); }
	void RemoveWare(Ware * ware) { ware_list.erase(ware); }
	bool IsWareRegistred(Ware * ware) { return (ware_list.search(ware)).valid(); }

	/// Fügt Waren zur Inventur hinzu
	void IncreaseInventoryWare(const GoodType ware, const unsigned count) { global_inventory.goods[ConvertShields(ware)]+=count; }
	void DecreaseInventoryWare(const GoodType ware, const unsigned count) { assert(global_inventory.goods[ConvertShields(ware)]>=count); global_inventory.goods[ConvertShields(ware)]-=count; }
	void IncreaseInventoryJob(const Job job, const unsigned count) { global_inventory.people[job]+=count; }
	void DecreaseInventoryJob(const Job job, const unsigned count) { assert(global_inventory.people[job]>=count); global_inventory.people[job]-=count; }

	/// Gibt Inventory-Settings zurück
	const Goods * GetInventory() const { return &global_inventory; }

	/// Setzt neue Militäreinstellungen
	void ChangeMilitarySettings(const unsigned char * military_settings);
	/// Ist ein anderer Spieler ein Verbündeter von uns?
	bool IsAlly(const unsigned char player) const;
	/// Truppen bestellen
	void OrderTroops(nobMilitary * goal, unsigned count);
	/// Prüft die Besatzung von allen Militärgebäuden und reguliert entsprechend (bei Veränderung der Militäreinstellungen)
	void RegulateAllTroops();
	/// Sucht für EINEN Soldaten ein neues Militärgebäude, als Argument wird Referenz auf die 
	/// entsprechende Soldatenanzahl im Lagerhaus verlangt
	void NewSoldierAvailable(const unsigned& soldier_count);
	/// Aktualisiert die Verteidiger-Liste
	void RefreshDefenderList();
	/// Prüft, ob für einen angreifenden Soldaten ein Verteidger geschickt werden soll
	bool ShouldSendDefender();

	/// Ruft einen Geologen
	void CallFlagWorker(const unsigned short x, const unsigned short y, const Job job);
	/// Registriert einen Geologen bzw. einen Späher an einer bestimmten Flagge, damit diese informiert werden,
	/// wenn die Flagge abgerissen wird
	void RegisterFlagWorker(nofFlagWorker * flagworker) { flagworkers.push_back(flagworker); }
	void RemoveFlagWorker(nofFlagWorker * flagworker) { flagworkers.erase(flagworker); }
	/// Wird aufgerufen, wenn eine Flagge abgerissen wurde, damit das den Flaggen-Arbeitern gesagt werden kann
	void FlagDestroyed(noFlag * flag);

	/// Gibt erstes Lagerhaus zurück
	nobBaseWarehouse * GetFirstWH() { return *warehouses.begin(); }


	/// Er gibt auf
	void Surrender();


};



#endif
