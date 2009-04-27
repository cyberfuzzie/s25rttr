// $Id: TerritoryRegion.h 4652 2009-03-29 10:10:02Z FloSoft $
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

#ifndef TERRITORY_REGION_H_
#define TERRITORY_REGION_H_

/// TerritoryRegion ist ein Rechteck aus der Karte quasi "ausgeschnitten", die für die Berechnung bei Militärgebäuden-
/// aktionen (Neubau, Übernahme, Abriss) benötigt wird von RecalcTerritory

class nobBaseMilitary;

class TerritoryRegion
{
	/// Lage des Ausschnittes in der Karte
	const unsigned short x1, y1, x2, y2;
	/// Größe der Karte (wird aus x1,y1...) berechnet
	const unsigned short width,height;

	/// Beschreibung eines Knotenpunktes
	struct TRNode
	{
		/// Spieler-index (+1, da 0 = besitzlos!)
		unsigned char owner;
		/// Entfernung vom Militärgebäude
		unsigned char radius;
	} * nodes;

private:

	/// Testet einen Punkt, ob der neue Spieler ihn übernehmen kann und übernimmt ihn ggf.
	void TestNode(const int x, int y,const unsigned char player, const unsigned char radius);
	/// Unterfunktionen von AdjustBorders, vergleicht 2 Punkte, ob sie von unterschiedlichen Spielern sind und setzt
	/// Punkt ggf. zu gar keinem Spieler, 2. Funktion wird für Punkte im 2er Abstand verwendet, da es dort ein bisschen anders läuft!
	void AdjustNodes(const unsigned short x1, const unsigned short y1, const unsigned short x2, const unsigned short y2);
	void AdjustNodes2(const unsigned short x1, const unsigned short y1, const unsigned short x2, const unsigned short y2);

public:


	TerritoryRegion(const unsigned short x1, const unsigned short y1, const unsigned short x2, const unsigned short y2);
	~TerritoryRegion();

	/// Berechnet ein Militärgebäude mit ein
	void CalcTerritoryOfBuilding(const nobBaseMilitary * const building);

	// Liefert den Besitzer eines Punktes (mit absoluten Koordinaten, werden automatisch in relative umgerechnet!)
	unsigned char GetOwner(const unsigned short x, const unsigned short y)
	{ return nodes[(y-y1)*(x2-x1)+(x-x1)].owner; }
	/// Liefert Radius mit dem der Punkt besetzt wurde
	unsigned char GetRadius(const unsigned short x, const unsigned short y) const
	{ return nodes[(y-y1)*(x2-x1)+(x-x1)].radius; }

	// Korrigiert die Grenzen (schneidet vom aktuellen Territorium immer noch die äußeren Punkte ab für die Grenzpfähle)
	void AdjustBorders();


};

#endif


