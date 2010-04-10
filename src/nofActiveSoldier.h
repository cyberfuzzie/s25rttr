// $Id: nofActiveSoldier.h 6304 2010-04-10 21:24:08Z OLiver $
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
#ifndef NOF_ACTIVESOLDIER_H_
#define NOF_ACTIVESOLDIER_H_

#include "nofSoldier.h"

/// Basisklasse für alle 3 Typen von Soldaten (Angreifern, Verteidigern und aggressiven Verteidigern)
class nofActiveSoldier : public nofSoldier
{
public:

	friend class noFighting;

	/// was der Soldat so schönes macht...
	enum SoldierState
	{
		STATE_FIGUREWORK = 0, /// auf Arbeit gehen usw, alles was die noFigure übernimmt
		STATE_WAITINGFORFIGHT, /// wartet vor dem Kampf auf seinen Duellanten (Kampf aggr. Verteidiger vs Angreifer)
		STATE_WALKINGHOME, // läuft nach einer Mission wieder zurück nach Hause

		STATE_ATTACKING_WALKINGTOGOAL, // Angreifer läuft zum Ziel
		STATE_ATTACKING_WAITINGAROUNDBUILDING, // Angreifer wartet um das Gebäude herum auf den Kampf
		STATE_ATTACKING_WAITINGFORDEFENDER, // wartet an der Flagge, bis der Verteidiger rauskommt
		STATE_ATTACKING_CAPTURINGFIRST, // nimmt als erster ein gegenerisches Gebäude ein
		STATE_ATTACKING_CAPTURINGNEXT, // weitere Soldaten, die in das bereits eingenommen Gebäude gehen
		STATE_ATTACKING_ATTACKINGFLAG, // geht zur Flagge, um sich mit dem dort wartenden Verteidiger zu bekämpfen
		//STATE_ATTACKING_SUCCEEDING, // rückt auf einen bestimmten Platz um die Flagge herum nach, der frei geworden ist
		STATE_ATTACKING_FIGHTINGVSAGGRESSIVEDEFENDER,  // kämpft mit einem aggressiven Verteidiger
		STATE_ATTACKING_FIGHTINGVSDEFENDER, // kämpft mit normalem Verteidiger an der Flagge
		
		STATE_SEAATTACKING_GOTOHARBOR, // geht von seinem Heimatmilitärgebäude zum Starthafen
		STATE_SEAATTACKING_WAITINHARBOR, // wartet im Hafen auf das ankommende Schiff
		STATE_SEAATTACKING_ONSHIP, // befindet sich auf dem Schiff auf dem Weg zum Zielpunkt
		STATE_SEAATTACKING_RETURNTOSHIP, // befindet sich an der Zielposition auf dem Weg zurück zum Schiff

		STATE_AGGRESSIVEDEFENDING_WALKINGTOAGGRESSOR,
		STATE_AGGRESSIVEDEFENDING_FIGHTING,// kämpft mit einem Angreifer

		STATE_DEFENDING_WALKINGTO, // läuft VOR Verteidigung ZUR Flagge
		STATE_DEFENDING_WALKINGFROM, // läuft NACH Verteidigung VON DER Flagge WEG
		STATE_DEFENDING_WAITING, // wartet an der Flagge auf weitere Angreifer
		STATE_DEFENDING_FIGHTING, // kämpft mit dem Angreifer

		STATE_ATTACKING_WALKINGTOFIGHTSPOT, // Läuft zu einem Kampfplatz um dort mit einem anderen Attacker zu kämpfen
		STATE_ATTACKING_FIGHTVSATTACKER // Kämpft gegen einen zufällig angetroffenen Attacker
	};

protected:

	/// was der Soldat so schönes macht...
	enum SoldierState state;

protected:

	/// Geht nach Hause (über freien Weg!) zurück
	void ReturnHome();
	/// Nach-Hause-Gehen, wird nach jedem gelaufenen Stück aufgerufen
	void WalkingHome();
	
	/// Prüft feindliche Leute auf Straßen in der Umgebung und vertreibt diese
	void ExpelEnemies();

private:

	// informieren, wenn ...
	void GoalReached(); // das Ziel erreicht wurde
	
	/// Gibt den Sichtradius dieser Figur zurück (0, falls nicht-spähend)
	virtual unsigned GetVisualRange() const;

public:

	/// Konstruktor
	nofActiveSoldier(const unsigned short x, const unsigned short y,const unsigned char player,
		nobBaseMilitary * const home,const unsigned char rank, const SoldierState init_state);
	/// (Copy-)Konstruktor
	nofActiveSoldier(const nofSoldier& other, const SoldierState init_state);
	nofActiveSoldier(SerializedGameData * sgd, const unsigned obj_id);

	/// Aufräummethoden
	protected:	void Destroy_nofActiveSoldier() { Destroy_nofSoldier(); }
	public:		void Destroy() { Destroy_nofActiveSoldier(); }

	/// Serialisierungsfunktionen
	protected:	void Serialize_nofActiveSoldier(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_nofActiveSoldier(sgd); }

	/// Soldaten zeichnen
	void Draw(int x, int y);

	/// Eventhandling
	virtual void HandleDerivedEvent(const unsigned int id);

	/// Sagt den verschiedenen Zielen Bescheid, dass wir doch nicht mehr kommen können
	virtual void InformTargetsAboutCancelling() = 0;
	/// Wenn ein Heimat-Militärgebäude bei Missionseinsätzen zerstört wurde
	virtual void HomeDestroyed() = 0;
	/// Wenn er noch in der Warteschleife vom Ausgangsgebäude hängt und dieses zerstört wurde
	virtual void HomeDestroyedAtBegin() = 0;
	/// Wenn ein Kampf gewonnen wurde
	virtual void WonFighting() = 0;
	/// Wenn ein Kampf verloren wurde (Tod)
	virtual void LostFighting() = 0;


	/// Gibt State zurück
	SoldierState GetState() const { return state; }
};

#endif // !NOF_ACTIVESOLDIER_H_
