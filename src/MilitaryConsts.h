// $Id: MilitaryConsts.h 5853 2010-01-04 16:14:16Z FloSoft $
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

#ifndef MILITARY_CONSTS_H_
#define MILITARY_CONSTS_H_

/// Größe der Militärquadrate (in Knotenpunkten), in die die Welt eingeteilt wurde für Militärgebäude
const unsigned short MILITARY_SQUARE_SIZE = 20;

/// Maximale Entfernungen für "nahe Militärgebäudedistanz" und "mittlere Militärgebäudedistanz"
const unsigned MAX_MILITARY_DISTANCE_NEAR = 18;
const unsigned MAX_MILITARY_DISTANCE_MIDDLE = 26;

/// Basisangriffsreichweite (Angriff mit allen Soldaten möglich)
const unsigned BASE_ATTACKING_DISTANCE = 21;

/// Erweiterte Reichweite, für die jeweils ein Soldat von der Angriffsarmee abgezogen wird
const unsigned EXTENDED_ATTACKING_DISTANCE = 1;

/// Maximale Länge für den Laufweg beim Angriff
const unsigned MAX_ATTACKING_RUN_DISTANCE = 70;

/// Besatzung in den einzelnen Militärgebäuden und nach Nation 
const int TROOPS_COUNT[4][4] =
{
	{2,3,6,9},
	{2,3,6,9},
	{2,3,6,9},
	{2,3,6,9}
};

/// Gold in den einzelnen Militärgebäuden und nach Nation
const unsigned short GOLD_COUNT[4][4] =
{
	{1,2,4,6},
	{1,2,4,6},
	{1,2,4,6},
	{1,2,4,6}
};

/// Radien der Militärgebäude ( die letzten beiden sind HQ und Hafen!)
const unsigned MILITARY_RADIUS[6] =
{ 8,9,10,11,9,4 };

// Radius für einzelne Hafen(baustellen)
const unsigned HARBOR_ALONE_RADIUS = 5;

/// Fahnenpositionen bei den Militärgebäuden

// Besatzungsflaggen (4 Völker x 4 Größen x 2 X+Y) - ab 3162
const signed char TROOPS_FLAGS[4][4][2] =
{
	{{24,-41},{19,-41},{31,-88},{35,-67}},
	{{-9,-49},{14,-59},{16,-63},{0,-44}},
	{{-24,-36},{9,-62},{-2,-80},{23,-75}},
	{{-5,-50},{-5,-51},{-9,-74},{-12,-58}}
};

// Besatzungsflaggen für die HQs
const signed char TROOPS_FLAGS_HQ[4][2] =
{
	{-12,-102},{-19,-94},{-18,-112},{20,-54},
};

/// Grenzflaggen (4 Völker x 4 Größen x 2 X+Y) - ab 3162 
const signed char BORDER_FLAGS[4][4][2] =
{
	{{-6,-36},{7,-48},{-18,-28},{-47,-64}},
	{{17,-45},{-3,-49},{-30,-25},{22,-53}},
	{{28,-19},{29,-18},{-27,-12},{-49,-62}},
	{{24,-19},{24,-19},{17,-52},{-37,-32}},
};


/// maximale Hitpoints der Soldaten von jedem Volk
const unsigned char HITPOINTS[4][5] =
{
	{3,4,5,6,7},
	{3,4,5,6,7},
	{3,4,5,6,7},
	{3,4,5,6,7}
};

/// Sichtweite der Militärgebäude (relativ); wird auf die normale Grenzweite draufaddiert
const unsigned VISUALRANGE_MILITARY = 3;
/// Sichtweite von Spähtürmen (absolut)
const unsigned VISUALRANGE_LOOKOUTTOWER = 20;
/// Sichtweite von Spähern
const unsigned VISUALRANGE_SCOUT = 3;
/// Sichtweite von Soldaten
const unsigned VISUALRANGE_SOLDIER = 2;
/// Sichtweite von Schiffen
const unsigned VISUALRANGE_SHIP = 2;

/// Beförderungszeit von Soldaten ( =UPGRADE_TIME + rand(UPGRADE_TIME_RANDOM) )
const unsigned UPGRADE_TIME = 100;
const unsigned UPGRADE_TIME_RANDOM = 300;
/// Genesungszeit von Soldaten in Häusern, Zeit, die gebraucht wird um sich um einen Hitpoint zu erholen
// ( =CONVALESCE_TIME + rand(CONVALESCE_TIME_RANDOM) )
const unsigned CONVALESCE_TIME = 500;
const unsigned CONVALESCE_TIME_RANDOM = 500;




#endif
