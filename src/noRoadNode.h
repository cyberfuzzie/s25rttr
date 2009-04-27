// $Id: noRoadNode.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NO_ROADNODE_H_
#define NO_ROADNODE_H_

#include "noCoordBase.h"

class RoadSegment;
class noFigure;
class Ware;
// Enumforwarddeklaration bei VC nutzen
#ifdef _MSC_VER
enum Job;
#else
#include "GameConsts.h"
#endif

// Basisklasse für Gebäude und Flagge (alles, was als "Straßenknoten" dient
class noRoadNode : public noCoordBase
{
protected:

	unsigned char player;

public:

	RoadSegment	* routes[6];

public:

	noRoadNode(const NodalObjectType nop,const unsigned short x, const unsigned short y,const unsigned char player);
	noRoadNode(SerializedGameData * sgd, const unsigned obj_id);

	virtual ~noRoadNode();
	/// Aufräummethoden
protected:	void Destroy_noRoadNode();
public:		void Destroy() { Destroy_noRoadNode(); }

	/// Serialisierungsfunktionen
	protected:	void Serialize_noRoadNode(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_noRoadNode(sgd); }

	noRoadNode * GetNeighbour(const unsigned char dir) const;
	void DestroyRoad(const unsigned char dir);

	unsigned char GetPlayer() const { return player; }

	/// Legt eine Ware am Objekt ab (an allen Straßenknoten (Gebäude, Baustellen und Flaggen) kann man Waren ablegen
	virtual void AddWare(Ware * ware) = 0;

	/// Nur für Flagge, Gebäude können 0 zurückgeben, gibt Wegstrafpunkte für das Pathfinden für Waren, die in eine bestimmte Richtung noch transportiert werden müssen
	virtual unsigned short GetPunishmentPoints(const unsigned char dir) const { return 0; }

};

#endif
