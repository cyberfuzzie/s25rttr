// $Id: nobStorehouse.h 6458 2010-05-31 11:38:51Z FloSoft $
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

#ifndef NOB_STOREHOUSE_H_
#define NOB_STOREHOUSE_H_

#include "nobBaseWarehouse.h"

class nobStorehouse : public nobBaseWarehouse
{

public:

	nobStorehouse(const unsigned short x, const unsigned short y,const unsigned char player,const Nation nation);
	nobStorehouse(SerializedGameData * sgd, const unsigned obj_id);

	/// Aufräummethoden
protected:	void Destroy_nobStorehouse();
public:		void Destroy() { Destroy_nobStorehouse(); }

	/// Serialisierungsfunktionen
protected: void Serialize_nobStorehouse(SerializedGameData * sgd) const;
public: void Serialize(SerializedGameData *sgd) const { Serialize_nobStorehouse(sgd); }

	GO_Type GetGOT() const { return GOT_NOB_STOREHOUSE; }

	void Draw(int x, int y);

	void HandleEvent(const unsigned int id);
};


#endif
