// $Id: nofShipWright.h 5132 2009-06-27 10:13:02Z OLiver $
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
#ifndef NOF_SHIPWRIGHT_H_
#define NOF_SHIPWRIGHT_H_


#include "nofWorkman.h"

/// Schiffsbauer - erstmal nur provisorisch, da er nur Boote baut
class nofShipWright : public nofWorkman
{
	/// Punkt, an dem das Schiff steht, an dem er gerade arbeitet
	MapCoord ship_x, ship_y;
private:
	/// Zeichnet ihn beim Arbeiten
	void DrawWorking(int x, int y) {}
	/// Gibt die ID in JOBS.BOB zurück, wenn der Beruf Waren rausträgt (bzw rein)
	unsigned short GetCarryID() const { return 90; }
	/// Der Arbeiter erzeugt eine Ware
	GoodType ProduceWare() { return GD_BOAT; }

public:

	nofShipWright(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace);
	nofShipWright(SerializedGameData * sgd, const unsigned obj_id);

	GO_Type GetGOT() const { return GOT_NOF_SHIPWRIGHT; }

	void HandleDerivedEvent(const unsigned int id);

	void Serialize(SerializedGameData *sgd) const;
};



#endif

