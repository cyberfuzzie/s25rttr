// $Id: noCharburnerPile.h 6458 2010-05-31 11:38:51Z FloSoft $
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
