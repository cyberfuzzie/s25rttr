// $Id: nofAttacker.h 6274 2010-04-05 13:36:09Z OLiver $
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
#ifndef NOF_ATTACKER_H_
#define NOF_ATTACKER_H_

#include "nofActiveSoldier.h"

class nofDefender;
class nofAggressiveDefender;
class nofPassiveSoldier;
class nobHarborBuilding;

/// Angreifender Soldat
class nofAttacker : public nofActiveSoldier
{
	// Unsere Feind-Freunde ;)
	friend class nofAggressiveDefender;
	friend class nofDefender;

private:
	/// Gebäude, welches der Soldat angreift
	nobBaseMilitary * attacked_goal;
	/// Ist er schon von einem Verteidiger ins Visier genommen wurden? Wenn ja, dann ist das hier
	/// der Pointer zu diesem, wenn nicht, dann 0
	/// kann auch der aggressive Verteidiger sein!
	nofActiveSoldier * defender;
	/// Soll er von nem Verteidiger gejagt werden? (wenn nicht wurde er schon gejagt oder er soll
	/// wegen den Militäreinstellungen nicht gejagt werden
	bool should_haunted;
	/// In welchem Radius steht der Soldat, wenn er um eine Fahne herum wartet?
	unsigned short radius;
	/// Nach einer bestimmten Zeit, in der der Angreifer an der Flagge des Gebäudes steht, blockt er den Weg
	/// nur benutzt bei STATE_ATTACKING_WAITINGFORDEFENDER
	EventManager::EventPointer blocking_event;
	/// Feind, der uns auf dem Weg zu Nahe gekommen ist und bekämpft werden muss //TODO maybe zusammen mit defender behandeln?
	nofActiveSoldier *encounteredEnemy;
	/// vereinbarter Treffpunkt um den encounteredEnemy zu bekämpfen
	MapCoord fightSpot_x, fightSpot_y;
	/// Für Seeangreifer: Stelle, wo sich der Hafen befindet, von wo aus sie losfahren sollen
	MapCoord harbor_x, harbor_y;
	/// Für Seeangreifer: Landepunkt, wo sich das Schiff befindet, mit dem der Angreifer
	/// ggf. wieder nach Hause fahren kann
	MapCoord ship_x, ship_y;
	unsigned ship_obj_id;

private:

	/// wenn man gelaufen ist
	void Walked();
	/// Geht nach Hause für Attacking-Missoin
	void ReturnHomeMissionAttacking();
	/// Läuft weiter
	void MissAttackingWalk();

	/// Geht zum STATE_ATTACKING_WAITINGFORDEFENDER über und meldet gleichzeitig ein Block-Event an
	void SwitchStateAttackingWaitingForDefender();

	/// Sagt den verschiedenen Zielen Bescheid, dass wir doch nicht mehr kommen können
	void InformTargetsAboutCancelling();

	/// Sucht nach feindlichen nofAttackern in der Nähe (gibt NULL zurück wenn keiner da)
	nofActiveSoldier *FindValidEnemyNearby();
	/// Geht zu einem anderen nofAttacker und kämpft mit ihm bzw. versucht das
	bool GoToFightEncounteredAttacker(nofAttacker *enemy, MapCoord tx, MapCoord ty);
	/// Gibt einen Platz zurück, an dem Kampf möglich ist (nur die 6 angrenzenden werden getestet)
	bool GetFightSpotNear(MapCoord &x, MapCoord &y);
	/// Lässt Soldaten zu einem Kampfplatz laufen, um mit einem anderen nofAttacker zu kämpfen
	void WalkingToFightSpot();
	/// Trifft auf anderen Attacker der in der Nähe rumläuft und vereinbart Kampf
	bool EncounterEnemy();
	/// Entfernt den encountered Enemy, setzt normale Tätigkeit fort
	void LostEncounteredEnemy();

	/// Für Schiffsangreifer: Sagt dem Schiff Bescheid, dass wir nicht mehr kommen
	void CancelAtShip();
	/// Behandelt das Laufen zurück zum Schiff
	void HandleState_SeaAttack_ReturnToShip();

public:


	/// Normaler Konstruktor für Angreifer
	nofAttacker(nofPassiveSoldier * other,nobBaseMilitary * const attacked_goal);
	/// Konstruktor für Schiffs-Angreifer, die zuerst einmal zu einem Hafen laufen müssen
	nofAttacker(nofPassiveSoldier * other,nobBaseMilitary * const attacked_goal, 
		const nobHarborBuilding * const harbor);
	nofAttacker(SerializedGameData * sgd, const unsigned obj_id);
	~nofAttacker();

	/// Aufräummethoden
protected:	void Destroy_nofAttacker();
public:		void Destroy() { Destroy_nofAttacker(); }

	/// Serialisierungsfunktionen
	protected:	void Serialize_nofAttacker(SerializedGameData * sgd) const;
	public:		void Serialize(SerializedGameData *sgd) const { Serialize_nofAttacker(sgd); }

	GO_Type GetGOT() const { return GOT_NOF_ATTACKER; }

	void HandleDerivedEvent(const unsigned int id);
	/// Blockt der Angreifer noch?
	bool IsBlockingRoads() const;

	/// Wenn ein Heimat-Militärgebäude bei Missionseinsätzen zerstört wurde
	void HomeDestroyed();
	/// Wenn er noch in der Warteschleife vom Ausgangsgebäude hängt und dieses zerstört wurde
	void HomeDestroyedAtBegin();
	/// Sagt dem Heimatgebäude Bescheid, dass er nicht mehr nach Hause kommen wird
	void CancelAtHomeMilitaryBuilding();

	/// Wenn ein Kampf gewonnen wurde
	void WonFighting();
	/// Wenn ein Kampf verloren wurde (Tod)
	void LostFighting();

	/// Sagt einem angreifenden Soldaten, dass es das Ziel nich mehr gibt
	void AttackedGoalDestroyed();
	/// aggressiv Verteidigender Soldat kann nich mehr kommen
	void AggressiveDefenderLost();
	/// Der Angreifer, der gerade um die Fahne wartet soll zur Fahne laufen, wo der Verteidiger wartet und dort kämpfen
	bool AttackFlag(nofDefender * defender);
	/// Der Platz von einem anderen Kampf wurde frei --> angreifender Soldat soll hinlaufen (KEIN Verteidiger da!)
	void AttackFlag();
	/// Ein um-die-Flagge-wartender Angreifer soll, nachdem das Militärgebäude besetzt wurde, ebenfalls mit reingehen
	void CaptureBuilding();
	/// Siehe oben, wird nach jeder Wegeinheit aufgerufen
	void CapturingWalking();
	/// Ein um-die-Flagge-wartender Angreifer soll wieder nach Hause gehen, da das eingenommene Gebäude bereits
	/// voll besetzt ist
	void CapturedBuildingFull();
	/// Gibt den Radius von einem um-eine-Fahne-herum-wartenden angreifenden Soldaten zurück
	unsigned GetRadius() const { return radius; }
	/// Ein um-die-Flagge-Swartender Angreifer soll auf einen frei gewordenen Platz nachrücken, damit keine
	/// Lücken entstehen
	void StartSucceeding(const unsigned short x, const unsigned short y, const unsigned short new_radius, const unsigned char dir);
	/// Siehe oben, wird nach jeder Wegeinheit aufgerufen
	void SucceedingWalk();

	/// aggressiv-verteidigender Soldat fragt einen Angreifer, ob er Lust zum Kämpfen hätte
	bool WannaFight() const { return (!defender && (state == STATE_ATTACKING_WALKINGTOGOAL
		|| state == STATE_ATTACKING_WAITINGAROUNDBUILDING )); }
	/// aggressiv-verteidigender Soldat will mit einem Angreifer kämpfen oder umgekehrt
	void LetsFight(nofAggressiveDefender * other);

	/// Fragt, ob ein Angreifender Soldat vor dem Gebäude wartet und kämpfen will
	bool IsAttackerReady() const { return (state == STATE_ATTACKING_WAITINGAROUNDBUILDING); }

	/// Liefert das angegriffene Gebäude zurück
	nobBaseMilitary * GetAttackedGoal() const { return attacked_goal; }

	/// Fragt ab, ob der Angreifer direkt vor einem Gebäude wartet, dieses angreift und ob gerade
	/// der Verteidiger des Gebäudes zur Tür rauskommt und wenn ja, ob ein wartender vor der Flagge, der am
	/// potenziellen Kampf vorbei will, es noch schafft vorbeizukommen, bevor der Kampf wieder anfängt
	bool CanPassBeforeFight() const;

	/// Startet den Angriff am Landungspunkt vom Schiff
	void StartAttackOnOtherIsland(const MapCoord ship_x, const MapCoord ship_y, const unsigned ship_id);
	/// Sagt Schiffsangreifern, dass sie mit dem Schiff zurück fahren
	void StartReturnViaShip();
	/// Sagt Bescheid, dass sich die Angreifer nun auf dem Schiff befinden
	void SeaAttackStarted() 
	{ state = STATE_SEAATTACKING_ONSHIP; }
	/// Fragt einen Schiffs-Angreifer auf dem Schiff, ob er schon einmal
	/// draußen war und gekämpft hat
	bool IsSeaAttackCompleted() const { return (state != STATE_SEAATTACKING_ONSHIP); }


};


#endif // !NOF_ATTACKER_H_
