// $Id: TerritoryRegion.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "TerritoryRegion.h"

#include "nobBaseMilitary.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

TerritoryRegion::TerritoryRegion(const unsigned short x1, const unsigned short y1, const unsigned short x2, const unsigned short y2)
: x1(x1), y1(y1), x2(x2), y2(y2), width(x2-x1), height(y2-y1)
{
	// Feld erzeugen
	nodes = new TRNode[(x2-x1)*(y2-y1)];

	// und erstmal hat es niemand im Besitz
	memset(nodes,0,sizeof(TRNode)*(x2-x1)*(y2-y1));
}

TerritoryRegion::~TerritoryRegion()
{
	// Feld löschen
	delete [] nodes;
}

void TerritoryRegion::TestNode(const int x, const int y,const unsigned char player, const unsigned char radius)
{
	// Gucken, ob der Punkt überhaupt mit in diese Region gehört
	if(x < int(x1) || x >= int(x2) || y < int(y1) || y >= int(y2))
		return;

	/// Wenn das Militargebäude jetzt näher dran ist, dann geht dieser Punkt in den Besitz vom jeweiligen Spieler
	/// oder wenn es halt gar nicht besetzt ist
 	if(radius < nodes[(y-y1)*(x2-x1)+(x-x1)].radius || !nodes[(y-y1)*(x2-x1)+(x-x1)].owner)
	{
		nodes[(y-y1)*(x2-x1)+(x-x1)].owner = player+1;
		nodes[(y-y1)*(x2-x1)+(x-x1)].radius = radius;
	}
}

void TerritoryRegion::CalcTerritoryOfBuilding(const nobBaseMilitary * const building)
{
	unsigned short radius = building->GetMilitaryRadius();

	// Punkt, auf dem das Militärgebäude steht
	int x = building->GetX(),y = building->GetY();
	TestNode(x,y,building->GetPlayer(),0);

	for(unsigned char r = 1;r<=radius;++r)
	{
		x=building->GetX()-r;
		// links oben
		for(unsigned short i = 0;i<r;++i)
		{
			TestNode(x,y,building->GetPlayer(),r);
			x+=(y&1);
			--y;
		}
		// oben
		for(unsigned short i = 0;i<r;++i)
		{
			TestNode(x,y,building->GetPlayer(),r);
			++x;
		}
		// rechts oben
		for(unsigned short i = 0;i<r;++i)
		{
			TestNode(x,y,building->GetPlayer(),r);
			x+=(y&1);
			++y;
		}
		// rechts unten
		for(unsigned short i = 0;i<r;++i)
		{
			TestNode(x,y,building->GetPlayer(),r);
			x-=!(y&1);
			++y;
		}
		// unten
		for(unsigned short i = 0;i<r;++i)
		{
			TestNode(x,y,building->GetPlayer(),r);
			--x;
		}
		// links unten
		for(unsigned short i = 0;i<r;++i)
		{
			TestNode(x,y,building->GetPlayer(),r);
			x-=!(y&1);
			--y;
		}
	}

	
	// Reihe, in der das Militärgebäude steh

	//if(building->GetY() >= y1 && building->GetY() < y2)
	//{
	//	//// 1. Punkt --> der Punkt, auf dem das Militärgebäude steht
	//	//TestNode(building->GetX(),building->GetY(),building->GetPlayer(),0);

	//	// mittlere Reihe
	//	for(unsigned short x = (building->GetX() >= x1+radius) ? (building->GetX()-radius) : x1;
	//		x < ((building->GetX()+radius < x2) ? (building->GetX()+radius+1) : x2); ++x)
	//		TestNode(x,building->GetY(),building->GetPlayer(),
	//		(x<building->GetX()) ? (building->GetX()-x) : (x-building->GetX()),building->GetAge());
	//}
 

	// Restliche Reihen ober- und unterhalb
	//for(unsigned char y = 1;y<=radius;++y)
	//{
	//	unsigned short dx = radius-(y+(building->GetY()&1))/2; // Relativ zum Gebäude Ausschweif in linke Richtung

	//	// Wenn sichs gar nicht überschneidet, brauchen wir nicht groß weiter rumrätseln
	//	if(building->GetX()+radius*2-y-dx < x1 || building->GetX()-dx >= x2)
	//		continue;

	//	//building->GetX()+dx

	//	unsigned short fx = (building->GetX() >= x1+dx) ? (building->GetX()-dx) : x1; // Erster Punkt in linker Richtung
	//	unsigned short lx = (building->GetX()+radius*2+1-y-dx <= x2) ? (building->GetX()+radius*2+1-y-dx) : x2;

	//	/*unsigned short lx = (fx+radius*2+1-y < x2+1) ? (fx+radius*2+1-y) : x2;*/


	//	// unterhalb (+y)
	//	if(building->GetY()+y >= y1 && building->GetY()+y < y2)
	//	{
	//		for(unsigned short x = fx;x<lx;++x)
	//			TestNode(x,building->GetY()+y,building->GetPlayer()
	//			,(y+!(building->GetY()&1))/2+((building->GetX()>x)?(building->GetX()-x):(x-building->GetX())),building->GetAge());
	//	}

	//	// oberhalb (-y)
	//	if(building->GetY() >= y1+y && building->GetY() < y2+y)
	//	{
	//			for(unsigned short x = fx;x<lx;++x)
	//			TestNode(x,building->GetY()-y,building->GetPlayer()
	//			,(y+!(building->GetY()&1))/2+((building->GetX()>x)?(building->GetX()-x):(x-building->GetX())),building->GetAge());
	//	}
	//}
}


//for(unsigned short x = fx;x < building->GetX()-radius+(y+(building->GetY()&1))/2+radius*2+1-y;++x)
//				TestNode(x,building->GetY()+y,building->GetPlayer(),y);
