// $Id: noMovable.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NO_MOVABLE_H_
#define NO_MOVABLE_H_

#include "noCoordBase.h"
#include "GameConsts.h"
#include "list.h"
#include "EventManager.h"

/// Anzahl Animationsschritte bei dem jeweiligen Anstieg
const unsigned short ASCENT_ANIMATION_STEPS[7] = {16,16,16,16,24,32,48};

class noMovable : public noCoordBase
{
protected:

	unsigned char dir; // Richtung, in die es gerade läcft
	unsigned char ascent; // Anstieg beim Laufen (0-2 runter, 3 gerade, 4-6 hoch)
	EventManager::EventPointer current_ev;
	/// Falls er unterwegs angehalten ist: wie weit war er schon gelaufen (0 wenn nicht)
	unsigned pause_walked_gf;
	/// Wenn er angehalten hat, wie lange das Laufevent war
	unsigned pause_event_length;
	/// Läuft es gerade (zum Debuggen)
	bool moving;

protected:

	/// Pausiert ein gerade laufendes Wesen
	void PauseWalking();
	

public:

	noMovable(const NodalObjectType nop,const unsigned short x, const unsigned short y);
	noMovable(SerializedGameData * sgd, const unsigned obj_id);

	/// Aufräummethoden
protected:	void Destroy_noMovable() { Destroy_noCoordBase(); }
public:		void Destroy() { Destroy_noMovable(); }

	/// Serialisierungsfunktionen
	protected:	void Serialize_noMovable(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_noMovable(sgd); }

	/// In aktueller Richtung ein Stück zurücklegen
	void Walk();
	// Starten zu Laufen, Event anmelden
	void StartMoving(const unsigned char dir, unsigned gf_length);
	// Interpoliert die Positon zwischen zwei Knotenpunkten
	void CalcRelative(int &x, int &y,int x1 = 0, int y1 = 0,int x2 = 0,int y2 = 0);
	// Steht er in der zwischen 2 Wegpunkten?
	bool IsStandingBetweenNodes() const { return (pause_walked_gf>0) ? true : false; }
};




#endif
