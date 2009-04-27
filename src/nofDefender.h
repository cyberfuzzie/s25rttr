// $Id: nofDefender.h 4652 2009-03-29 10:10:02Z FloSoft $
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
#ifndef NOF_DEFENDER_H_
#define NOF_DEFENDER_H_

#include "nofActiveSoldier.h"

class nofAttacker;
class nofPassiveSoldier;

/// Verteidiger, der rauskommt, wenn ein Angreifer an die Flagge kommt
class nofDefender : public nofActiveSoldier
{
	// Unser Feind-Freund ;)
	friend class nofAttacker;

private:

	/// angreifender Soldat an der Flagge
	nofAttacker * attacker;

private:

	/// wenn man gelaufen ist
	void Walked();

public:

	nofDefender(const unsigned short x, const unsigned short y,const unsigned char player,nobBaseMilitary * const building,
		const unsigned char rank, nofAttacker * const attacker);
	nofDefender(nofPassiveSoldier * other, nofAttacker * const attacker);
	nofDefender(SerializedGameData * sgd, const unsigned obj_id);

	/// Aufräummethoden
protected:	void Destroy_nofDefender() { Destroy_nofActiveSoldier(); }
public:		void Destroy() { Destroy_nofDefender(); }

	/// Serialisierungsfunktionen
	protected:	void Serialize_nofDefender(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_nofDefender(sgd); }

	GO_Type GetGOT() const { return GOT_NOF_DEFENDER; }

	/// Der Verteidiger geht gerade rein und es kommt ein neuer Angreifer an die Flagge, hiermit wird der Ver-
	/// teidiger darüber informiert, damit er dann gleich wieder umdrehen kann
	void NewAttacker(nofAttacker * attacker) { this->attacker = attacker; }
	/// Der Angreifer konnte nicht mehr an die Flagge kommen
	void AttackerArrested();

	/// Wenn ein Heimat-Militärgebäude bei Missionseinsätzen zerstört wurde
	void HomeDestroyed();
	/// Wenn er noch in der Warteschleife vom Ausgangsgebäude hängt und dieses zerstört wurde
	void HomeDestroyedAtBegin();
	/// Wenn ein Kampf gewonnen wurde
	void WonFighting();
	/// Wenn ein Kampf verloren wurde (Tod)
	void LostFighting();

	/// Guckt, ob der Verteidiger gerade rausläuft zum Kampf an die Flagge und ob ein anderen Mensch
	/// es noch schaffen könnte, vorbeizukommen, bevor der Kampf losgeht (siehe CanPassBeforeFight bei nofAttacker)
	bool CanPassBeforeFight() const;
};


#endif // !NOF_DEFENDER_H_
