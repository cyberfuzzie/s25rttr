// $Id: nofFarmhand.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef NOF_FARMHAND_H_
#define NOF_FARMHAND_H_

#include "nofBuildingWorker.h"


/// Ein Landarbeiter geht raus aus seiner Hütte und arbeitet in "freier Natur"
class nofFarmhand : public nofBuildingWorker
{
protected:

	/// Arbeitsziel, das der Arbeiter ansteuert
	unsigned short dest_x,dest_y;

private:

	/// Funktionen, die nur von der Basisklasse (noFigure) aufgerufen werden, wenn...
	void WalkedDerived();

	/// Arbeit musste wegen Arbeitsplatzverlust abgebrochen werden
	void WorkAborted();
	/// Arbeit musste wegen Arbeitsplatzverlust abgebrochen werden (an abgeleitete Klassen)
	virtual void WorkAborted_Farmhand();


	///// Fängt das "Warten-vor-dem-Arbeiten" an, falls er arbeiten kann (müssen ja bestimmte "Naturobjekte" gegeben sein)
	//void TryToWork();
	/// Findet heraus, ob der Beruf an diesem Punkt arbeiten kann
	bool IsPointAvailable(const unsigned short x, const unsigned short y);
	/// Fragt abgeleitete Klasse, ob hier Platz bzw ob hier ein Baum etc steht, den z.B. der Holzfäller braucht
	virtual bool IsPointGood(const unsigned short x, const unsigned short y) = 0;

	/// Läuft zum Arbeitspunkt
	void WalkToWorkpoint();
	/// Trifft Vorbereitungen fürs nach Hause - Laufen
	void StartWalkingHome();
	/// Läuft wieder zu seiner Hütte zurück
	void WalkHome();

	///// Abgeleitete Klasse informieren, wenn man anfängt zum Arbeitspunkt zu laufen (Reservierungen)
	//virtual void WalkingStarted() = 0;
	/// Abgeleitete Klasse informieren, wenn sie anfängt zu arbeiten (Vorbereitungen)
	virtual void WorkStarted() = 0;
	/// Abgeleitete Klasse informieren, wenn fertig ist mit Arbeiten
	virtual void WorkFinished() = 0;
	/// Wird aufgerufen, wenn der Arbeiter seine Arbeit plötzlich abrechen

public:

	nofFarmhand(const Job job,const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace);
	nofFarmhand(SerializedGameData * sgd, const unsigned obj_id);

	/// Aufräummethoden
protected:	void Destroy_nofFarmhand() { Destroy_nofBuildingWorker(); }
public:		void Destroy() { Destroy_nofFarmhand(); }

	/// Serialisierungsfunktionen
	protected:	void Serialize_nofFarmhand(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_nofFarmhand(sgd); }



	void HandleDerivedEvent(const unsigned int id);

	
};


#endif
