// $Id: noFlag.h 5853 2010-01-04 16:14:16Z FloSoft $
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

#ifndef NO_FLAG_H_
#define NO_FLAG_H_

#include "noRoadNode.h"
#include "RoadSegment.h"
#include "MapConsts.h"

class Ware;

class noFlag : public noRoadNode
{
private:
	unsigned short ani_offset;
	FlagType flagtype;
	///// Waren, die an diese Flagge getragen werden (Träger die einen Abstand von 1 haben vor der Flagge,
	///// müssen sich so eine Ware registrieren und die anderen müssen warten, falls es schon zu viele "registrierte"
	///// gibt
	//unsigned char reserved_wares;
	/// Die Waren, die an dieser Flagge liegen
	Ware * wares[8];

	/// Wieviele BWU-Teile es maximal geben soll, also wieviele abgebrannte Lagerhausgruppen
	/// gleichzeitig die Flagge als nicht begehbar deklarieren können
	static const unsigned MAX_BWU = 4;
	/// Nicht erreichbar für Massenflüchtlinge
	struct BurnedWarehouseUnit
	{
		/// ID der Gruppe
		unsigned id;
		/// letzter TÜV, ob man auch nicht hinkommt, in GF
		unsigned last_gf;
	} bwus[MAX_BWU];


public:
	noFlag(const unsigned short x, const unsigned short y,const unsigned char player,const unsigned char dis_dir=0xFF);
	noFlag(SerializedGameData * sgd, const unsigned obj_id);

	~noFlag();

	/// Aufräummethoden
protected:	void Destroy_noFlag();
public:		void Destroy() { Destroy_noFlag(); }

	/// Serialisierungsfunktionen
	protected:	void Serialize_noFlag(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_noFlag(sgd); }

	GO_Type GetGOT() const { return GOT_FLAG; }

	/// Zerstört evtl. vorhandenes Gebäude bzw. Baustelle vor der Flagge
	void DestroyAttachedBuilding();

	FlagType GetFlagType() const { return flagtype; }

	void Draw(int x, int y);

	void GotWorker(Job job, noFigure * worker);

	/// Erzeugt von ihnen selbst ein FOW Objekt als visuelle "Erinnerung" für den Fog of War
	FOWObject * CreateFOWObject() const;

	/// Legt eine Ware an der Flagge ab
	void AddWare(Ware * ware);
	// Gibt die Anzahl der Waren zurück, die an der Flagge liegen
	unsigned GetWareCount() const; 
	/// Wählt eine Ware von einer Flagge aus (anhand der Transportreihenfolge), entfernt sie von der Flagge und gibt sie zurück
	/// wenn swap_wares true ist, bedeutet dies, dass Waren nur ausgetauscht werden und somit nicht die Träger benachrichtigt
	// werden müssen
	Ware * SelectWare(const unsigned char dir,const bool swap_wares,const noFigure * const carrier);
	/// Prüft, ob es Waren gibt, die auf den Weg in Richtung dir getragen werden müssen
	unsigned GetWaresCountForRoad(const unsigned char dir) const;
	/// Gibt Wegstrafpunkte für das Pathfinden für Waren, die in eine bestimmte Richtung noch transportiert werden müssen
	unsigned short GetPunishmentPoints(const unsigned char dir) const;
	/// Gibt Auskunft darüber, ob noch Platz für eine Ware an der Flagge ist
	bool IsSpaceForWare() const { return (GetWareCount()<8); }
	/*/// Versucht eine Ware zu reservieren, wenn erfolgreich -> true, ansonsten wenn Flagge voll ist, false
	bool ReserveWare();*/
	/// Feind übernimmt die Flagge
	void Capture(const unsigned char new_owner);
	/// Baut normale Flaggen zu "gloriösen" aus bei Eselstraßen
	void Upgrade();

	/// Ist diese Flagge für eine bestimmte Lagerhausflüchtlingsgruppe (BWU) nicht zugänglich?
	bool IsImpossibleForBWU(const unsigned bwu_id) const;
	/// Hinzufügen, dass diese Flagge für eine bestimmte Lagerhausgruppe nicht zugänglich ist
	void ImpossibleForBWU(const unsigned bwu_id);

};


#endif
