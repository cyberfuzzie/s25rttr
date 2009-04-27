// $Id: nofPlaner.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NOF_PLANER_H_
#define NOF_PLANER_H_

#include "noFigure.h"

class noBuildingSite;

/// Der Planierer
class nofPlaner : public noFigure
{
	/// Was der Planierer gerade so schönes macht
	enum PlanerState
	{
		STATE_FIGUREWORK = 0,
		STATE_WALKING, /// läuft zum nächsten Punkt, um zu graben
		STATE_PLANING /// planiert einen Punkt (Abspielen der Animation
	} state;

	/// Arbeitsstelle des Planierers
	noBuildingSite * building_site;

	/// Wie rum er geht
	enum PlaningDir
	{
		PD_NOTWORKING, 
		PD_CLOCKWISE, /// Uhrzeigersinn
		PD_COUNTERCLOCKWISE /// entgegen Uhrzeigersinn
	} pd;

private:

	void GoalReached();
	void Walked();
	void AbrogateWorkplace();
	void HandleDerivedEvent(const unsigned int id);

public:

	nofPlaner(const unsigned short x, const unsigned short y,const unsigned char player,noBuildingSite * building_site);
	nofPlaner(SerializedGameData * sgd, const unsigned obj_id);

	/// Serialisierungsfunktionen
	protected:	void Serialize_nofPlaner(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_nofPlaner(sgd); }

	GO_Type GetGOT() const { return GOT_NOF_PLANER; }

	void Draw(int x, int y);

	/// Wird von der Baustelle aus aufgerufen, um den Bauarbeiter zu sagen, dass er gehen kann
	void LostWork();
};



#endif
