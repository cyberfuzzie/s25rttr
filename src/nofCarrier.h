// $Id: nofCarrier.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NOF_CARRIER_H_
#define NOF_CARRIER_H_

#include "noFigure.h"
#include "GlobalVars.h"

class RoadSegment;
class Ware;
class noRoadNode;

enum CarrierState
{
	CARRS_FIGUREWORK = 0, // Aufgaben der Figur
	CARRS_WAITFORWARE, // auf Weg auf Ware warten
	CARRS_GOTOMIDDLEOFROAD, // zur Mitte seines Weges gehen
	CARRS_FETCHWARE, // Ware holen
	CARRS_CARRYWARE, // Ware zur Flagge tragen
	CARRS_CARRYWARETOBUILDING, // Ware zum Gebäude schaffen
	CARRS_LEAVEBUILDING, // kommt aus Gebäude wieder raus (bzw kommt von Baustelle zurück) zum Weg
	CARRS_WAITFORWARESPACE, // wartet vor der Flagge auf einen freien Platz
	CARRS_GOBACKFROMFLAG // geht von der Flagge zurück, weil kein Platz mehr frei war


};

// Stellt einen Träcer da
class nofCarrier : public noFigure
{
public:

	/// Träger-"Typ"
	enum CarrierType
	{
		CT_NORMAL, // Normaler Träger
		CT_DONKEY, // Esel
		CT_BOAT // Träger mit Boot
	};

private:

	CarrierType ct;
	/// Was der Träger gerade so treibt
	CarrierState state;
	/// Ist er dick?
	bool fat;
	// Weg, auf dem er arbeitet
	RoadSegment * workplace;
	/// Ware, die er gerade trägt (0 = nichts)
	Ware * carried_ware;
	/// Rechne-Produktivität-aus-Event
	EventManager::EventPointer productivity_ev;
	// Letzte errechnete Produktivität
	unsigned productivity;
	/// Wieviel GF von einer bestimmten Anzahl in diesem Event-Zeitraum gearbeitet wurde
	unsigned worked_gf;
	/// Zu welchem GF das letzte Mal das Arbeiten angefangen wurde
	unsigned since_working_gf;
	/// Bestimmt GF der nächsten Trägeranimation
	unsigned next_animation;

private:

	void GoalReached();
	void Walked();
	void AbrogateWorkplace();
	void HandleDerivedEvent(const unsigned int id);

	/// Nach dem Tragen der Ware, guckt der Träger an beiden Flagge, obs Waren gibt, holt/trägt diese ggf oder geht ansonsten wieder in die Mitte
	void LookForWares();
	/// Nimmt eine Ware auf an der aktuellen Flagge und dreht sich um, um sie zu tragen (fetch_dir ist die Richtung der Waren, die der Träger aufnehmen will)
	void FetchWare(const bool swap_wares);

	/// Prüft, ob die getragene Ware dann von dem Weg zum Gebäude will
	bool WantInBuilding();

	/// Für Produktivitätsmessungen: fängt an zu arbeiten
	void StartWorking();
	/// Für Produktivitätsmessungen: hört auf zu arbeiten
	void StopWorking();

	/// Bestimmt neuen Animationszeitpunkt
	void SetNewAnimationMoment();
	
public:

	nofCarrier(const CarrierType ct, const unsigned short x, const unsigned short y,const unsigned char player,RoadSegment * workplace,noRoadNode * const goal);
	nofCarrier(SerializedGameData * sgd, const unsigned obj_id);

	~nofCarrier();

	/// Serialisierungsfunktionen
	protected:	void Serialize_nofCarrier(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_nofCarrier(sgd); }

		/// Aufräummethoden
protected:	void Destroy_nofCarrier();
public:		void Destroy() { Destroy_nofCarrier(); }

	GO_Type GetGOT() const { return GOT_NOF_CARRIER; }

	/// Gibt Träger-Typ zurück
	CarrierType GetCarrierType() const { return ct; }
	/// Was macht der Träger gerade?
	CarrierState GetCarrierState() const { return state; }
	/// Gibt Träger-Produktivität in % zurück
	unsigned GetProductivity() const { return productivity; }

	void Draw(int x, int y);

	/// Wird aufgerufen, wenn der Weg des Trägers abgerissen wurde
	void LostWork();

	/// Wird aufgerufen, wenn der Arbeitsplatz des Trägers durch eine Flagge geteilt wurde
	/// der Träger sucht sich damit einen der beiden als neuen Arbeitsplatz, geht zur Mitte und ruft einen neuen Träger
	/// für das 2. Wegstück
	void RoadSplitted(RoadSegment * rs1, RoadSegment * rs2);

	/// Sagt dem Träger Bescheid, dass es an einer Flagge noch eine Ware zu transportieren gibt
	bool AddWareJob(const noRoadNode * rn);
	/// Das Gegnteil von AddWareJob: wenn eine Ware nicht mehr transportiert werden will, sagt sie dem Träger Bescheid,
	/// damit er nicht unnötig dort hinläuft zur Flagge
	void RemoveWareJob();

	/// Benachrichtigt den Träger, wenn an einer auf seinem Weg an einer Flagge wieder ein freier Platz ist
	/// gibt zurück, ob der Träger diesen freien Platz auch nutzen wird
	bool SpaceAtFlag(const bool flag);

	/// Gibt erste Flagge des Arbeitsweges zurück, falls solch einer existiert
	noRoadNode * GetFirstFlag() const;
	noRoadNode * GetSecondFlag() const;

};

#endif
