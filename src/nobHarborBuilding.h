// $Id: nobHarborBuilding.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NOB_HARBORBUILDING_H_
#define NOB_HARBORBUILDING_H_

#include "nobBaseWarehouse.h"

class noShip;

class nobHarborBuilding : public nobBaseWarehouse
{
	struct ExpeditionInfo
	{
		ExpeditionInfo() : active(false), boards(0), stones(0), builder(false) {}

		/// Expedition in Vorbereitung?
		bool active;
		/// Anzahl an Brettern und Steinen, die bereits angesammelt wurden
		unsigned boards,stones;
		/// Bauarbeiter schon da?
		bool builder;
	} expedition;

	/// Bestell-Ware-Event
	EventManager::EventPointer orderware_ev;
	/// Die Meeres-IDs aller angrenzenden Meere (jeweils für die 6 drumherumliegenden Küstenpunkte)
	unsigned short sea_ids[6];

private:

	/// Bestellt die zusätzlichen erforderlichen Waren für eine Expedition
	void OrderExpeditionWares();
	/// Prüft, ob eine Expedition von den Waren her vollständig ist und ruft ggf. das Schiff
	void CheckExpeditionReady();

public:

	nobHarborBuilding(const unsigned short x, const unsigned short y,const unsigned char player,const Nation nation);
	nobHarborBuilding(SerializedGameData * sgd, const unsigned obj_id);

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
	
	/// Startet eine Expedition oder stoppt sie, wenn bereits eine stattfindet
	void StartExpedition();
	/// Ist Expedition in Vorbereitung?
	bool IsExpeditionActive() const { return expedition.active; }
	/// Schiff ist angekommen
	void ShipArrived(noShip * ship);
	/// Schiff konnte nicht mehr kommen
	void ShipLost(noShip * ship);

	/// Gibt die Hafenplatz-ID zurück, auf der der Hafen steht
	unsigned GetHarborPosID() const;

	
};


#endif
