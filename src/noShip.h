// $Id: noShip.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NO_SHIP_H_
#define NO_SHIP_H_

#include "noMovable.h"
#include "MapConsts.h"

class nobHarborBuilding;

/// Klasse für die Schiffe
class noShip : public noMovable
{
	/// Spieler des Schiffes
	unsigned char player;

	/// Was macht das Schiff gerade?
	enum State
	{
		STATE_IDLE = 0, /// Schiff hat nix zu tun und hängt irgendwo an der Küste rum 
		STATE_GOTOHARBOR,
		STATE_EXPEDITION_LOADING,
		STATE_EXPEDITION_WAITING,
		STATE_EXPEDITION_DRIVING
	} state;

	/// Das Meer, auf dem dieses Schiff fährt
	unsigned short sea_id;
	/// Zielpunkt des Schiffes
	unsigned goal_harbor_id;
	/// Schiffsroute 
	std::vector<unsigned char> route;
	unsigned pos;
	/// Namen des Schiffs
	std::string name;

private:

	/// entscheidet, was nach einem gefahrenen Abschnitt weiter zu tun ist
	void Driven();
	/// Fängt an zu fahren
	void StartDriving(const unsigned char dir);

	void HandleState_GoToHarbor();

	enum Result
	{
		DRIVING = 0,
		GOAL_REACHED,
		NO_ROUTE_FOUND,
		HARBOR_DOESNT_EXIST
	};


	/// Fährt weiter zu einem Hafen
	Result DriveToHarbour();
	/// Fährt weiter zu Hafenbauplatz
	Result DriveToHarbourPlace();

	/// Zeichnet normales Fahren auf dem Meer ohne irgendwelche Güter
	void DrawDriving(int x, int y);

public:

	/// Konstruktor
	noShip(const unsigned short x, const unsigned short y, const unsigned char player);
	noShip(SerializedGameData * sgd, const unsigned obj_id);

	~noShip() {}

	void Serialize(SerializedGameData *sgd) const;
	void Destroy();

	GO_Type GetGOT() const { return GOT_SHIP; }

	// An x,y zeichnen
	void Draw(int x, int y);
	// Benachrichtigen, wenn neuer gf erreicht wurde
	void HandleEvent(const unsigned int id);

	/// Gibt den Besitzer zurück
	unsigned char GetPlayer() const { return player; }
	/// Hat das Schiff gerade nichts zu tun
	bool IsIdling() const { return (state == STATE_IDLE); }
	/// Gibt die ID des Meeres zurück, auf dem es sich befindet
	unsigned short GetSeaID() const { return sea_id; }
	/// Gibt den Schiffsnamen zurück
	const std::string& GetName() const { return name; }
	/// Führt das Schiff gerade eine Expedition durch und wartet auf weitere Befehle?
	bool IsWaitingForExpeditionInstructions() const 
	{ return (state == STATE_EXPEDITION_WAITING); }
	/// Beim Warten bei der Expedition: Gibt die Hafenpunkt-ID zurück, wo es sich gerade befindet
	unsigned GetCurrentHarbor() const;

	/// Fährt zum Hafen, um dort eine Mission (Expedition) zu erledigen
	void GoToHarbor(nobHarborBuilding * hb, const std::vector<unsigned char>& route);
	/// Startet eine Expedition
	void StartExpedition();
};


#endif

