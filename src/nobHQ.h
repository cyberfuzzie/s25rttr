// $Id: nobHQ.h 6458 2010-05-31 11:38:51Z FloSoft $
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

#ifndef NOB_HQ_H_
#define NOB_HQ_H_

#include "nobBaseWarehouse.h"
#include "MilitaryConsts.h"

class nobHQ : public nobBaseWarehouse
{
public:


	nobHQ(const unsigned short x, const unsigned short y,const unsigned char player,const Nation nation);
	nobHQ(SerializedGameData * sgd, const unsigned obj_id);

	/// Aufräummethoden
protected:	void Destroy_nobHQ();
public:		void Destroy() { Destroy_nobHQ(); }

	/// Serialisierungsfunktionen
protected: void Serialize_nobHQ(SerializedGameData * sgd) const;
public: void Serialize(SerializedGameData *sgd) const { Serialize_nobHQ(sgd); }

	GO_Type GetGOT() const { return GOT_NOB_HQ; }


	void Draw(int x, int y);

	MapCoord GetMilitaryRadius() const { return MILITARY_RADIUS[4]; }

	void HandleEvent(const unsigned int id);

};


#endif
