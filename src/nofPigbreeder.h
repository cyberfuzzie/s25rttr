// $Id: nofPigbreeder.h 6458 2010-05-31 11:38:51Z FloSoft $
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

#ifndef NOF_PIGBREEDER_H_
#define NOF_PIGBREEDER_H_

#include "nofWorkman.h"

class nobUsualBuilding;

/// Klasse für den Schreiner
class nofPigbreeder : public nofWorkman
{
	/// Zeichnet ihn beim Arbeiten
	void DrawWorking(int x, int y);
	/// Gibt die ID in JOBS.BOB zurück, wenn der Beruf Waren rausträgt (bzw rein)
	unsigned short GetCarryID() const { return 73; }
	/// Der Arbeiter erzeugt eine Ware
	GoodType ProduceWare();

public:

	nofPigbreeder(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace);
	nofPigbreeder(SerializedGameData * sgd, const unsigned obj_id);

	GO_Type GetGOT() const { return GOT_NOF_PIGBREEDER; }

	/// Ggf. Schweinesounds abspielen
	void MakePigSounds();
};

#endif
