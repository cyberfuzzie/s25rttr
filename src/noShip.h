// $Id: noShip.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NO_SHIP_H_
#define NO_SHIP_H_

#include "noMovable.h"
#include "MapConsts.h"
#include <list>

class nobHarborBuilding;

/// Maximale Beladung eines Schiffs abhängig vom Volk des Spielers
const unsigned SHIP_CAPACITY[NATION_COUNT] = { 40, 40, 40, 40 };

class noFigure;
class Ware;

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
		STATE_EXPEDITION_DRIVING,
		STATE_TRANSPORT_LOADING, // Schiff wird mit Waren/Figuren erst noch beladen, bleibt also für kurze Zeit am Hafen
		STATE_TRANSPORT_DRIVING, /// Schiff transportiert Waren/Figuren von einen Ort zum anderen
		STATE_TRANSPORT_UNLOADING /// Entlädt Schiff am Zielhafen, kurze Zeit ankern, bevor Waren im Hafengebäude ankommen..

	} state;

	/// Das Meer, auf dem dieses Schiff fährt
	unsigned short sea_id;
	/// Zielpunkt des Schiffes
	unsigned goal_harbor_id;
	/// Anlegepunkt am Zielhafen, d.h. die Richtung relativ zum Zielpunkt
	unsigned char goal_dir;
	/// Namen des Schiffs
	std::string name;
	/// Schiffsroute und Position
	unsigned pos;
	std::vector<unsigned char> route;
	/// Ladung des Schiffes
	std::list<noFigure*> figures;
	std::list<Ware*> wares;
	
private:

	/// entscheidet, was nach einem gefahrenen Abschnitt weiter zu tun ist
	void Driven();
	/// Fängt an zu fahren
	void StartDriving(const unsigned char dir);

	void HandleState_GoToHarbor();
	void HandleState_ExpeditionDriving();
	void HandleState_TransportDriving();

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
	void DrawDriving(int& x, int& y);
	/// Zeichnet normales Fahren auf dem Meer mit Gütern
	void DrawDrivingWithWares(int& x, int& y);

	/// Startet die eigentliche Transportaktion, nachdem das Schiff beladen wurde
	void StartTransport();

	/// Fängt an mit idlen und setzt nötigen Sachen auf NULL
	void StartIdling();

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
	/// Ist das Schiff gerade irgendwie am Expeditionieren und hat entsprechenden Kram an Bord?
	bool IsOnExpedition() const
	{ return (state == STATE_EXPEDITION_LOADING || state == STATE_EXPEDITION_WAITING 
	|| state ==	STATE_EXPEDITION_DRIVING); }

	/// Beim Warten bei der Expedition: Gibt die Hafenpunkt-ID zurück, wo es sich gerade befindet
	unsigned GetCurrentHarbor() const;

	/// Fährt zum Hafen, um dort eine Mission (Expedition) zu erledigen
	void GoToHarbor(nobHarborBuilding * hb, const std::vector<unsigned char>& route);
	/// Startet eine Expedition
	void StartExpedition();
	/// Weist das Schiff an, in einer bestimmten Richtung die Expedition fortzusetzen
	void ContinueExpedition(const unsigned char dir);
	/// Gibt zurück, ob das Schiff jetzt in der Lage wäre, eine Kolonie zu gründen
	bool IsAbleToFoundColony() const;
	/// Weist das Schiff an, an der aktuellen Position einen Hafen zu gründen
	void FoundColony();
	/// Gibt zurück, ob das Schiff einen bestimmten Hafen ansteuert
	bool IsGoingToHarbor(nobHarborBuilding * hb) const;

	/// Belädt das Schiff mit Waren und Figuren, um eine Transportfahrt zu starten
	void PrepareTransport(Point<MapCoord> goal, const std::list<noFigure*>& figures, const std::list<Ware*>& wares);

	/// Sagt dem Schiff, das ein bestimmter Hafen zerstört wurde
	void HarborDestroyed(nobHarborBuilding * hb);

	/// Gibt Liste der Waren an Bord zurück
	const std::list<Ware *> &GetWares() const { return wares; }

	/// Gibt Liste der Menschen an Bord zurück
	const std::list<noFigure *> &GetFigures() const { return figures; }


};


#endif

