// $Id: nofWorkman.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NOF_WORKMAN_H_
#define NOF_WORKMAN_H_

#include "nofBuildingWorker.h"

/// Ein Handwerker ist jemand, der seine Waren nur im Geb�ude herstellt und diese raustr�gt
/// Allen gemeinsam ist, dass sie einen festen Arbeitsablauf haben:
/// Warten -- Arbeiten -- Warten -- Ware raustragen -- wieder reinkommen -- ...
class nofWorkman : public nofBuildingWorker
{
private:

	// Funktionen, die nur von der Basisklasse  aufgerufen werden, wenn...
	void WalkedDerived(); // man gelaufen ist
	/// Gibt den Warentyp zur�ck, welche der Arbeiter erzeugen will
	virtual GoodType ProduceWare() = 0;
	/// Abgeleitete Klasse informieren, wenn man fertig ist mit Arbeiten
	virtual void WorkFinished();

public:

	nofWorkman(const Job job,const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace);
	nofWorkman(SerializedGameData * sgd, const unsigned obj_id);

	/// Serialisierungsfunktionen
	protected:	void Serialize_nofWorkman(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_nofWorkman(sgd); }

	void HandleDerivedEvent(const unsigned int id);


};

#endif
