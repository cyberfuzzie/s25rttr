// $Id: GameClientPlayer.h 6458 2010-05-31 11:38:51Z FloSoft $
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

#ifndef GAMECLIENTPLAYER_H_
#define GAMECLIENTPLAYER_H_

#include "GamePlayerInfo.h"
#include "GameConsts.h"
#include <list>
#include <algorithm>
#include "MapConsts.h"
#include "PostMsg.h"


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
class noShip;
class nobHarborBuilding;
class GameWorldGame;
class GameMessage_GameCommand;
class nofPassiveSoldier;


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
	std::list<nobBaseWarehouse*> warehouses;
	/// Liste von Häfen
	std::list<nobHarborBuilding*> harbors;
	///// Liste von unbesetzten Straßen (ohne Träger) von dem Spieler
	//std::list<RoadSegment*> unoccupied_roads;
	/// Lister aller Straßen von dem Spieler
	std::list<RoadSegment*> roads;

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
	std::list<JobNeeded> jobs_wanted;

	/// Listen der einzelnen Gebäudetypen (nur nobUsuals!)
	std::list<nobUsual*> buildings[30];
	/// Liste von sämtlichen Baustellen
	std::list<noBuildingSite*> building_sites;
	/// Liste von allen Militärgebäuden
	std::list<nobMilitary*> military_buildings;
	/// Liste von sämtlichen Waren, die herumgetragen werden und an Fahnen liegen
	std::list<Ware*> ware_list;
	/// Liste von Geologen und Spähern, die an eine Flagge gebunden sind
	std::list<nofFlagWorker*> flagworkers;
	/// Liste von Schiffen dieses Spielers
	std::vector<noShip*> ships;
	


	/// Liste, welchen nächsten 10 Angreifern Verteidiger entgegenlaufen sollen
	bool defenders[5];
	unsigned short defenders_pos;

	/// Inventur
	Goods global_inventory;

	/// Bündnisse mit anderen Spielern
	struct Pact
	{
		/// Bündnis schon akzeptiert oder nur vorgeschlagen?
		bool accepted;
		/// Dauer (in GF), 0 = kein Bündnise, 0xFFFFFFFF = Bündnis auf Ewigkeit
		unsigned duration;
		/// Startzeitpunkt (in GF)
		unsigned start;
		/// Will dieser Spieler (also der this-Pointer) diesen Vertrag auflösen?
		bool want_cancel;

		Pact() : accepted(false), duration(0), start(0), want_cancel(false) {}
		Pact(SerializedGameData *ser);
		void Serialize(SerializedGameData *ser);
	};
	/// Bündnisse dieses Spielers mit anderen Spielern
	Pact pacts[MAX_PLAYERS][PACTS_COUNT];

	/*/// Bündnisvorschläge, die von einem anderen Spieler an diesen Spieler gemacht wurden
	struct PactSuggestion
	{
		/// Zeitpunkt des Vorschlags durch den Spieler (damit nicht weit im Nachhinein der Vertrag durch
		/// den anderen Spieler getätigt werden kann, obwohl ersterer vielleicht gar nicht mehr dazu bereit ist
		unsigned suggestion_time;
		/// Der andere Spieler
		unsigned char player;
		/// Bündnisart
		PactType pt;
		/// Dauer des Bündnisses 
		unsigned duration;

		PactSuggestion(const unsigned suggestion_time, const unsigned char player, const PactType pt, const unsigned duration) 
			: suggestion_time(suggestion_time), player(player), pt(pt), duration(duration) {}
		PactSuggestion(Serializer * ser);
		void Serialize(Serializer * ser);
	};

	std::list<PactSuggestion> pact_suggestions;*/

public:

	/// Laggt der Spieler?
	bool is_lagging;
	/// Empfangene GC für diesen Spieler
	std::list<GameMessage_GameCommand> gc_queue;

	/// Koordinaten des HQs des Spielers
	unsigned short hqx,hqy;

	// Informationen über die Verteilung
	struct
	{
		unsigned char percent_buildings[40];
		std::list<BuildingType> client_buildings; // alle Gebäude, die diese Ware bekommen, zusammengefasst
		std::vector<unsigned char> goals;
		unsigned selected_goal;
	} distribution[WARE_TYPES_COUNT];

	/// Art der Reihenfolge (0 = nach Auftraggebung, ansonsten nach build_order)
	unsigned char order_type;
	/// Baureihenfolge
	std::vector <unsigned char> build_order;
	/// Prioritäten der Waren im Transport
	unsigned char transport[WARE_TYPES_COUNT];
	/// Militäreinstellungen (die vom Militärmenü)
	std::vector <unsigned char> military_settings;
	/// Werkzeugeinstellungen (in der Reihenfolge wie im Fenster!)
	std::vector <unsigned char> tools_settings;

private:

	// Sucht Weg für Job zu entsprechenden noRoadNode
	bool FindWarehouseForJob(const Job job, noRoadNode * goal);
	/// Prüft, ob der Spieler besiegt wurde
	void TestDefeat();
	/// Bündnis (real, d.h. spielentscheidend) abschließen
	void MakePact(const PactType pt, const unsigned char other_player, const unsigned duration);

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
	/// Gibt dem Spieler brekannt, das eine Straße abgerissen wurde
	void RoadDestroyed();
	/// Sucht einen Träger für die Straße und ruft ggf den Träger aus dem jeweiligen nächsten Lagerhaus
	bool FindCarrierForRoad(RoadSegment * rs);
	/// Warenhaus zur Warenhausliste hinzufügen
	void AddWarehouse(nobBaseWarehouse * wh) { warehouses.push_back(wh); }
	/// Warenhaus aus Warenhausliste entfernen
	void RemoveWarehouse(nobBaseWarehouse * wh) { warehouses.remove(wh); TestDefeat(); }
	/// Hafen zur Warenhausliste hinzufügen
	void AddHarbor(nobHarborBuilding * hb);
	/// Hafen aus Warenhausliste entfernen
	void RemoveHarbor(nobHarborBuilding * hb) { harbors.remove(hb); }
	/// (Unbesetzte) Straße aus der Liste entfernen
	void DeleteRoad(RoadSegment * rs) { roads.remove(rs); }
	//bool TestRoads(RoadSegment * rs) { return roads.search(rs).valid(); }

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

	const std::list<noBuildingSite*>& GetBuildingSites() const { return building_sites; }

	const std::list<nobMilitary *>& GetMilitaryBuildings() const { return military_buildings; }

	/// Gibt Liste von Gebäuden des Spieler zurück
	const std::list<nobUsual*>& GetBuildings(const BuildingType type);
	/// Liefert die Anzahl aller Gebäude einzeln
	void GetBuildingCount(BuildingCount& bc) const;
	/// Berechnet die durschnittlichen Produktivität eines jeden Gebäudetyps
	/// (erwartet als Argument ein 40-er Array!)
	void CalcProductivities(std::vector<unsigned short>& productivities); 

	/// Berechnet die durschnittlichen Produktivität aller Gebäude
	unsigned short CalcAverageProductivitiy();


	/// Gibt Priorität der Baustelle zurück (entscheidet selbständig, welche Reihenfolge usw)
	/// je kleiner die Rückgabe, destro größer die Priorität!
	unsigned GetBuidingSitePriority(const noBuildingSite * building_site);

	/// Berechnet die Verteilung der Waren auf die einzelnen Gebäude neu
	void RecalcDistribution();
	/// Berechnet die Verteilung einer (bestimmten) Ware
	void RecalcDistributionOfWare(const GoodType ware);
	/// Konvertiert die Daten vom wp_transport in "unser" Prioritäten-Format und setzt es
	void ConvertTransportData(const std::vector<unsigned char>& transport_data);

	/// Ware zur globalen Warenliste hinzufügen und entfernen
	void RegisterWare(Ware * ware) { ware_list.push_back(ware); }
	void RemoveWare(Ware * ware) { ware_list.remove(ware); }
	bool IsWareRegistred(Ware * ware) { return (std::find(ware_list.begin(),ware_list.end(),ware) != ware_list.end()); }
	bool IsWareDependent(Ware * ware);

	/// Fügt Waren zur Inventur hinzu
  void IncreaseInventoryWare(const GoodType ware, const unsigned count) { global_inventory.goods[ConvertShields(ware)]+=count; }
	void DecreaseInventoryWare(const GoodType ware, const unsigned count) { assert(global_inventory.goods[ConvertShields(ware)]>=count); global_inventory.goods[ConvertShields(ware)]-=count; }
  void IncreaseInventoryJob(const Job job, const unsigned count) { global_inventory.people[job]+=count; }
	void DecreaseInventoryJob(const Job job, const unsigned count) { assert(global_inventory.people[job]>=count); global_inventory.people[job]-=count; }

	/// Gibt Inventory-Settings zurück
	const Goods * GetInventory() const { return &global_inventory; }

	/// Setzt neue Militäreinstellungen
	void ChangeMilitarySettings(const std::vector<unsigned char>& military_settings);
	/// Setzt neue Werkzeugeinstellungen
	void ChangeToolsSettings(const std::vector<unsigned char>& tools_settings);
	/// Setzt neue Verteilungseinstellungen
	void ChangeDistribution(const std::vector<unsigned char>& distribution_settings);
	/// Setzt neue Baureihenfolge-Einstellungen
	void ChangeBuildOrder(const unsigned char order_type, const std::vector<unsigned char>& oder_data);

	/// Darf der andere Spieler von mir angegriffen werden?
	bool IsPlayerAttackable(const unsigned char player) const;
	/// Ist ein anderer Spieler ein richtiger Verbündeter von uns, d.h. Teamsicht, Unterstützung durch aggressive Verteidiger usw.?
	bool IsAlly(const unsigned char player) const;
	/// Truppen bestellen
	void OrderTroops(nobMilitary * goal, unsigned count);
	/// Prüft die Besatzung von allen Militärgebäuden und reguliert entsprechend (bei Veränderung der Militäreinstellungen)
	void RegulateAllTroops();
	/// Pr�ft von allen Milit�rgeb�uden die Fahnen neu
	void RecalcMilitaryFlags();
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
	void RemoveFlagWorker(nofFlagWorker * flagworker) { flagworkers.remove(flagworker); }
	/// Wird aufgerufen, wenn eine Flagge abgerissen wurde, damit das den Flaggen-Arbeitern gesagt werden kann
	void FlagDestroyed(noFlag * flag);

	/// Gibt erstes Lagerhaus zurück
	nobBaseWarehouse * GetFirstWH() { return *warehouses.begin(); }

	/// Registriert ein Schiff beim Einwohnermeldeamt
	void RegisterShip(noShip * ship);
	/// Meldet das Schiff wieder ab
	void RemoveShip(noShip * ship);
	/// Versucht, für ein untätiges Schiff eine Arbeit zu suchen
	void GetJobForShip(noShip * ship);
	/// Schiff für Hafen bestellen, zweiter Parameter gibt an, ob das Schiff in einer Jobliste
	/// vermerkt werden soll, also unbedingt eins gebraucht wird 
	void OrderShip(nobHarborBuilding * hb);
	/// Gibt die ID eines Schiffes zurück
	unsigned GetShipID(const noShip * const ship) const;
	/// Gibt ein Schiff anhand der ID zurück bzw. NULL, wenn keines mit der ID existiert
	noShip * GetShipByID(const unsigned ship_id) const;
	/// Gibt die Gesamtanzahl von Schiffen zurück
	unsigned GetShipCount() const { return ships.size(); }
	/// Gibt eine Liste mit allen Häfen dieses Spieler zurück, die an ein bestimmtes Meer angrenzen
	void GetHarborBuildings(std::vector<nobHarborBuilding*>& harbor_buildings, const unsigned short sea_id) const;
	/// Gibt die Anzahl der Schiffe, die einen bestimmten Hafen ansteuern, zurück
	unsigned GetShipsToHarbor(nobHarborBuilding * hb) const;
	/// Gibt der Wirtschaft Bescheid, dass ein Hafen zerstört wurde
	void HarborDestroyed(nobHarborBuilding * hb);
	/// Sucht einen Hafen in der Nähe, wo dieses Schiff seine Waren abladen kann
	/// gibt true zurück, falls erfolgreich
	bool FindHarborForUnloading(noShip * ship, const MapCoord start_x, const MapCoord start_y, unsigned * goal_harbor_id, std::vector<unsigned char> * route,
		nobHarborBuilding * exception);

	/// Gibt eine Liste der verfügbaren Häfen zurück
	const std::list<nobHarborBuilding*>& GetHarbors() const { return harbors; }

	/// Er gibt auf
	void Surrender();

	/// Macht Bündnisvorschlag an diesen Spieler
	void SuggestPact(const unsigned char other_player, const PactType pt, const unsigned duration);
	/// Akzeptiert ein bestimmtes Bündnis, welches an diesen Spieler gemacht wurde
	void AcceptPact(const unsigned id, const PactType pt, const unsigned char other_player);
	/// Gibt Einverständnis, dass dieser Spieler den Pakt auflösen will
	/// Falls dieser Spieler einen Bündnisvorschlag gemacht hat, wird dieser dagegen zurückgenommen
	void CancelPact(const PactType pt, const unsigned char other_player);
	/// Zeigt an, ob ein Pakt besteht
	enum PactState
	{
		NO_PACT = 0, /// Kein Pakt geschlossen
		IN_PROGRESS, /// Pakt angeboten, aber noch nicht akzeptiert
		ACCEPTED /// Bündnis in Kraft
	};
	PactState GetPactState(const PactType pt, const unsigned char other_player) const;
	/// Gibt die verbleibende Dauer zurück, die ein Bündnis noch laufen wird (0xFFFFFFFF = für immer)
	unsigned GetRemainingPactTime(const PactType pt, const unsigned char other_player) const;
	/// Setzt die initialen Bündnisse ahand der Teams
	void MakeStartPacts();
	/// Testet die Bündnisse, ob sie nicht schon abgelaufen sind
	void TestPacts();
		


  // Statistik-Sachen

  void SetStatisticValue(StatisticType type, unsigned int value);
  void ChangeStatisticValue(StatisticType type, int change);

	void IncreaseMerchandiseStatistic(GoodType type);
 
  void StatisticStep();
  
  struct Statistic
  {
    // 30 Datensätze pro Typ
    unsigned int data[STAT_TYPE_COUNT][STAT_STEP_COUNT];
		// und das gleiche für die Warenstatistik
		unsigned short merchandiseData[STAT_MERCHANDISE_TYPE_COUNT][STAT_STEP_COUNT];
    // Index, der gerade 'vorne' (rechts im Statistikfenster) ist
    unsigned short currentIndex;
    // Counter, bei jedem vierten Update jeweils Daten zu den längerfristigen Statistiken kopieren
    unsigned short counter;
  };

  const Statistic& GetStatistic(StatisticTime time) { return statistic[time]; };

	// Testet ob Notfallprogramm aktiviert werden muss und tut dies dann
	void TestForEmergencyProgramm();

private:
  // Statistikdaten
  Statistic statistic[STAT_TIME_COUNT];

  // Die Statistikwerte die 'aktuell' gemessen werden
  unsigned int statisticCurrentData[STAT_TYPE_COUNT];
	unsigned short statisticCurrentMerchandiseData[STAT_MERCHANDISE_TYPE_COUNT];

  unsigned short incrStatIndex(unsigned short i) { return (i==STAT_STEP_COUNT-1) ? 0 : ++i; }
  unsigned short decrStatIndex(unsigned short i) { return (i==0) ? STAT_STEP_COUNT-1 : --i; }
	unsigned short decrStatIndex(unsigned short i, unsigned short amount) { return (i < amount) ? STAT_STEP_COUNT - (amount - i) - 1 : i - amount; }

	// Notfall-Programm aktiviert ja/nein (Es gehen nur noch Res an Holzfäller- und Sägewerk-Baustellen raus)
	bool emergency;

public:
	bool hasEmergency() const { return emergency; }

	/// Testet ob der Spieler noch mehr Katapulte bauen darf
	bool CanBuildCatapult() const;

};

#endif


