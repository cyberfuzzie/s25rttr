// $Id: nobShipYard.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NOB_SHIPWRIGHT_H_
#define NOB_SHIPWRIGHT_H_

#include "nobUsual.h"


/// Extraklasse für ein Schiffsbauer-Gebäude, da hier extra Optionen eingestellt werden müssen
class nobShipYard : public nobUsual
{
public:

	/// Modi für den Schiffsbauer
	enum Mode
	{
		BOATS, // baut kleine Boote
		SHIPS // baut große Schiffe
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

	/// Gibt aktuellen Modus zurück
	Mode GetMode() const { return mode; }
	/// Schaltet Modus entsprechend um
	void ToggleMode();
};



#endif

