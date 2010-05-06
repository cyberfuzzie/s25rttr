// $Id: nobHarborBuilding.h 6408 2010-05-06 14:33:45Z OLiver $
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

#ifndef NOB_HARBORBUILDING_H_
#define NOB_HARBORBUILDING_H_

#include "nobBaseWarehouse.h"
#include "MilitaryConsts.h"
#include <list>

class noShip;
class nofPassiveSoldier;

class nobHarborBuilding : public nobBaseWarehouse
{
	struct ExpeditionInfo
	{
		ExpeditionInfo() : active(false), boards(0), stones(0), builder(false) {}
		ExpeditionInfo(SerializedGameData *sgd);
		void Serialize(SerializedGameData *sgd) const;

		/// Expedition in Vorbereitung?
		bool active;
		/// Anzahl an Brettern und Steinen, die bereits angesammelt wurden
		unsigned boards,stones;
		/// Bauarbeiter schon da?
		bool builder;
	} expedition;

	struct ExplorationExpeditionInfo
	{
		ExplorationExpeditionInfo() : active(false), scouts(0) {}
		ExplorationExpeditionInfo(SerializedGameData *sgd);
		void Serialize(SerializedGameData *sgd) const;

		/// Expedition in Vorbereitung?
		bool active;
		/// Anzahl der Erkunder, die schon da sind
		unsigned scouts;
	} exploration_expedition;


	/// Bestell-Ware-Event
	EventManager::EventPointer orderware_ev;
	/// Die Meeres-IDs aller angrenzenden Meere (jeweils für die 6 drumherumliegenden Küstenpunkte)
	unsigned short sea_ids[6];
	/// Liste von Waren, die weggeschifft werden sollen
	std::list<Ware*> wares_for_ships;
	/// Liste von Menschen, die weggeschifft werden sollen
	struct FigureForShip
	{
		noFigure * fig;
		Point<MapCoord> dest;
	};
	std::list<FigureForShip> figures_for_ships;
	
	/// Liste von angreifenden Soldaten, die verschifft werden sollen
	struct SoldierForShip
	{
		nofAttacker * attacker;
		Point<MapCoord> dest;
	};
	std::list<SoldierForShip> soldiers_for_ships;


private:

	/// Bestellt die zusätzlichen erforderlichen Waren für eine Expedition
	void OrderExpeditionWares();
	/// Prüft, ob eine Expedition von den Waren her vollständig ist und ruft ggf. das Schiff
	void CheckExpeditionReady();
	/// Prüft, ob eine Expedition von den Sp�hern her vollständig ist und ruft ggf. das Schiff
	void CheckExplorationExpeditionReady();
	/// Gibt zurück, ob Expedition vollständig ist
	bool IsExpeditionReady() const;
	/// Gibt zurück, ob Erkundungs-Expedition vollständig ist
	bool IsExplorationExpeditionReady() const;
	/// Abgeleitete kann eine gerade erzeugte Ware ggf. sofort verwenden 
	/// (muss in dem Fall true zurückgeben)
	bool UseWareAtOnce(Ware * ware, noBaseBuilding* const goal);
	/// Dasselbe für Menschen
	bool UseFigureAtOnce(noFigure * fig, noRoadNode* const goal); 
	/// Bestellte Figur, die sich noch inder Warteschlange befindet, kommt nicht mehr und will rausgehauen werden
	void CancelFigure(noFigure * figure);

public:

	nobHarborBuilding(const unsigned short x, const unsigned short y,const unsigned char player,const Nation nation);
	nobHarborBuilding(SerializedGameData * sgd, const unsigned obj_id);

	MapCoord GetMilitaryRadius() const { return HARBOR_ALONE_RADIUS; }

	/// Aufräummethoden
	void Destroy();
	/// Serialisierung
	void Serialize(SerializedGameData *sgd) const;
	GO_Type GetGOT() const { return GOT_NOB_HARBORBUILDING; }
	void Draw(int x, int y);
	void HandleEvent(const unsigned int id);

	/// Eine bestellte Ware konnte doch nicht kommen
	void WareLost(Ware * ware);
	/// Legt eine Ware im Lagerhaus ab
	void AddWare(Ware * ware);
	/// Eine Figur geht ins Lagerhaus
	void AddFigure(noFigure * figure);
	/// Berechnet Wichtigkeit einer neuen Ware f�r den Hafen (Waren werden f�r Expeditionen 
	/// ben�tigt!)
	unsigned CalcDistributionPoints(const GoodType type);
	
	/// Storniert die Bestellung für eine bestimmte Ware, die mit einem Schiff transportiert werden soll
	void CancelWareForShip(Ware * ware);
	
	/// Startet eine Expedition oder stoppt sie, wenn bereits eine stattfindet
	void StartExpedition();	
	/// Startet eine Erkundungs-Expedition oder stoppt sie, wenn bereits eine stattfindet
	void StartExplorationExpedition();	
	/// Ist Expedition in Vorbereitung?
	bool IsExpeditionActive() const { return expedition.active; }
	/// Ist Erkundungs-Expedition in Vorbereitung?
	bool IsExplorationExpeditionActive() const { return exploration_expedition.active; }
	/// Schiff ist angekommen
	void ShipArrived(noShip * ship);
	/// Schiff konnte nicht mehr kommen
	void ShipLost(noShip * ship);

	/// Abfangen, wenn ein Mann nicht mehr kommen kann --> könnte ein Bauarbeiter sein und
	/// wenn wir einen benötigen, müssen wir einen neuen bestellen
	void RemoveDependentFigure(noFigure * figure);

	/// Gibt die Hafenplatz-ID zurück, auf der der Hafen steht
	unsigned GetHarborPosID() const;

	struct ShipConnection
	{
		/// Zielhafen
		noRoadNode * dest;
		/// Kosten für die Strecke in Weglänge eines einfachen Trägers
		unsigned way_costs;
	};
	/// Gibt eine Liste mit möglichen Verbindungen zurück
	void GetShipConnections(std::vector<ShipConnection>& connections) const;

	/// Fügt einen Mensch hinzu, der mit dem Schiff irgendwo hin fahren will
	void AddFigureForShip(noFigure * fig, Point<MapCoord> dest);
	/// Fügt eine Ware hinzu, die mit dem Schiff verschickt werden soll
	void AddWareForShip(Ware * ware);
	
	/// A ware changed its route and doesn't want to use the ship anymore
	void WareDontWantToTravelByShip(Ware * ware);

	/// Gibt Anzahl der Schiffe zurück, die noch für ausstehende Aufgaben benötigt werden
	unsigned GetNeededShipsCount() const;
	/// Gibt die Wichtigkeit an, dass ein Schiff kommen muss (0 -> keine Bedürftigkeit)
	int GetNeedForShip(unsigned ships_coming) const;

	/// Erhält die Waren von einem Schiff und nimmt diese in den Warenbestand auf
	void ReceiveGoodsFromShip(const std::list<noFigure*> figures, const std::list<Ware*> wares);

	struct SeaAttackerBuilding
	{
		/// Das Gebäude selbst
		nobMilitary * building;
		// Dazugehöriger Hafen, wo die Angreifer dann auf das Schiff warten sollen
		nobHarborBuilding * harbor;
		/// Entfernung Hafen - anderer Hafen
		unsigned distance;
		
		bool operator==(const nobMilitary * const building) const
		{
			return (this->building == building);
		};
	};
	
	/// Gibt die Angreifer zurück, die dieser Hafen für einen Seeangriff zur Verfügung stellen kann
	/// defender_harbors sind dabei mögliche Zielhäfen
	void GetAttackerBuildingsForSeaAttack(std::vector<SeaAttackerBuilding> * buildings,
											const std::vector<unsigned>& defender_harbors);
											
	/// F�gt einen Schiffs-Angreifer zum Hafen hinzu
	void AddSeaAttacker(nofAttacker * attacker);
};


#endif
