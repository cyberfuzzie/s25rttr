// $Id: nobShipYard.h 7521 2011-09-08 20:45:55Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef NOB_SHIPWRIGHT_H_
#define NOB_SHIPWRIGHT_H_

#include "nobUsual.h"


/// Extraklasse f�r ein Schiffsbauer-Geb�ude, da hier extra Optionen eingestellt werden m�ssen
class nobShipYard : public nobUsual
{
public:

	/// Modi f�r den Schiffsbauer
	enum Mode
	{
		BOATS = 0, // baut kleine Boote
		SHIPS // baut gro�e Schiffe
	};

private:

	/// Aktueller Modus vom Schiffsbauer
	Mode mode;

public:

	nobShipYard(const unsigned short x, const unsigned short y,const unsigned char player,const Nation nation);
	nobShipYard(SerializedGameData * sgd, const unsigned obj_id);

	/// Serialisierungsfunktionen
	void Serialize(SerializedGameData *sgd) const;

	GO_Type GetGOT() const { return GOT_NOB_SHIPYARD; }

	/// Gibt aktuellen Modus zur�ck
	Mode GetMode() const { return mode; }
	/// Schaltet Modus entsprechend um
	void ToggleMode();
};



#endif

