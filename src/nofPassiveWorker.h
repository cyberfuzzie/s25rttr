// $Id: nofPassiveWorker.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NOF_FLAG_WORKER_H_
#define NOF_FLAG_WORKER_H_

#include "noFigure.h"

class noFlag;

/// Arbeiter, der keine Arbeiten verrichtet, sondern nur entsprechend dem Beruf gezeichnet werden muss
/// und z.B. zum Auslagern benutzt wird ober beim Abbrennen eines Lagerhaus
class nofPassiveWorker : public noFigure
{
private:
	
	/// von noFigure aufgerufen
	void Walked(); // wenn man gelaufen ist
	void GoalReached(); // wenn das Ziel erreicht wurde
	void AbrogateWorkplace();
	void HandleDerivedEvent(const unsigned int id); /// Für alle restlichen Events, die nicht von noFigure behandelt werden
	
public:

	nofPassiveWorker(const Job job,const unsigned short x, const unsigned short y,const unsigned char player,noRoadNode * goal);
	nofPassiveWorker(SerializedGameData * sgd, const unsigned obj_id);

	/// Aufräummethoden
protected:	void Destroy_nofPassiveWorker() {}
public:		void Destroy() { Destroy_nofPassiveWorker(); }

	/// Serialisierungsfunktionen
	protected:	void Serialize_nofPassiveWorker(SerializedGameData * sgd) const {}
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_nofPassiveWorker(sgd); }
	
	/// Zeichnen
	void Draw(int x, int y);
	
	GO_Type GetGOT() const {
		 return GOT_NOF_PASSIVEWORKER;
	}
};


#endif
