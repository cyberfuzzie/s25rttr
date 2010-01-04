// $Id: RoadSegment.h 5853 2010-01-04 16:14:16Z FloSoft $
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
#ifndef ROADSEGMENT_H_INCLUDED
#define ROADSEGMENT_H_INCLUDED

#pragma once

#include "GameObject.h"
#include <vector>

class nofCarrier;
class noRoadNode;
class noFlag;
class noFigure;

class RoadSegment : public GameObject
{
public:
	/// Straßentyp
	enum RoadType
	{
		RT_NORMAL, // Normale Straße bzw. Bergstraße
		RT_DONKEY, // Eselstraße
		RT_BOAT // Wasserstraße
	} rt;
	/// die 2 Roadnodes, die den Weg eingrenzen
	noRoadNode * f1,*f2;
	/// Beschreibung des Weges, ist length groß und liegt als Beschreibung der einzelnen Richtungen vor (von f1 zu f2)
	std::vector<unsigned char> route;
	/// Träger (und ggf. Esel), der auf diesem Weg arbeitet
	nofCarrier * carrier[2];

public:

	/// parametrisierter Konstruktor von @p RoadSegment.
	RoadSegment(const RoadType rt, noRoadNode *const f1, noRoadNode *const f2,
		const std::vector<unsigned char>& route);

	RoadSegment(SerializedGameData * sgd, const unsigned obj_id);

	/// zerstört das Objekt.
	void Destroy(void) { Destroy_RoadSegment(); }
	/// serialisiert das Objekt.
	void Serialize(SerializedGameData *sgd) const { Serialize_RoadSegment(sgd); }

	/// liefert den GO-Type.
	GO_Type GetGOT() const { return GOT_ROADSEGMENT; }
	/// Gibt die ID (0 oder 1) eines RoadNodes dieser Straße zurück (die Flagge muss zu dieser Straße gehören, sonst kommt Müll raus!!)
	bool GetNodeID(const noRoadNode *rn) { return (rn == f2); }
	/// Gibt Straßen-Typ zurück
	RoadType GetRoadType() const { return rt; }

	/// Gibt die Länge der Staße zurück
	unsigned GetLength() const { return route.size(); }

	unsigned char GetDir(const bool dir, const unsigned int id) const
	{
		if(dir)
			return (route[route.size()-id-1]+3)%6;
		else
			return route[id];
	}

	/// zerteilt die Straße
	void SplitRoad(noFlag *splitflag);
	/// Überprüft ob es an den Flaggen noch Waren zu tragen gibt für den Träger. Nur bei Straßen mit 2 Flagge aufrufen, nicht bei Hauseingängen etc. !!
	bool AreWareJobs(const bool flag,unsigned carrier_type,const bool take_ware_immediately) const;
	/// Eine Ware sagt Bescheid, dass sie über dem Weg getragen werden will (rn ist die Flagge, von der sie kommt)
	void AddWareJob(const noRoadNode *rn);

	/// Eine Ware will nicht mehr befördert werden
	void WareJobRemoved(const noFigure * const exception);

	/// Baut die Straße zu einer Eselstraße aus
	void UpgradeDonkeyRoad();
	/// Braucht die Straße einen Esel? Nur wenn sie auch einen Träger schon hat!
	bool NeedDonkey() const { return (rt == RT_DONKEY && carrier[0] && !carrier[1]); }
	/// Hat einen Esel als Arbeiter dazubekommen
	void GotDonkey(nofCarrier * donkey) { assert(!carrier[1]); carrier[1] = donkey; }
	/// Soll versuchen einen Esel zu bekommen
	void TryGetDonkey();


	/// Ein Träger muss kündigen, aus welchen Gründen auch immer
	void CarrierAbrogated(nofCarrier * carrier);

protected:
	/// zerstört das Objekt.
	void Destroy_RoadSegment(void);
	/// serialisiert das Objekt.
	void Serialize_RoadSegment(SerializedGameData *sgd) const;

};

#endif // !ROADSEGMENT_H_INCLUDED
