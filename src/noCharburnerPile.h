// $Id: noCharburnerPile.h 6582 2010-07-16 11:23:35Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#ifndef NO_CHARBURNERPILE_H_
#define NO_CHARBURNERPILE_H_

#include "noCoordBase.h"
#include "EventManager.h"

class noCharburnerPile : public noCoordBase
{
private:

	/// Status
	enum State
	{
		STATE_GROWING_WAITING, /// Wachsphase, wartet auf den nächsten Wachstumsschub
		STATE_GROWING, /// wächst 
		STATE_NORMAL, /// ist ausgewachsen und verdorrt nach einer Weile
		STATE_WITHERING /// verdorrt (verschwindet)
	} state;

	/// Wachs-Event
	EventManager::EventPointer event;

public:

	noCharburnerPile(const unsigned short x, const unsigned short y);
	noCharburnerPile(SerializedGameData * sgd, const unsigned obj_id);

	~noCharburnerPile();

	/// Aufräummethoden
protected:	void Destroy_noGrainfield();
public:		void Destroy() { Destroy_noGrainfield(); }

	/// Serialisierungsfunktionen
	protected:	void Serialize_noCharburnerPile(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_noCharburnerPile(sgd); }

	GO_Type GetGOT() const { return GOT_CHARBURNERPILE; }

	void Draw(int x,int y);
	void HandleEvent(const unsigned int id);

	BlockingManner GetBM() const { return BM_GRANITE; }
	
};


#endif
