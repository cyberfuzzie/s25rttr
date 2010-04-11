// $Id: TerritoryRegion.cpp 6309 2010-04-11 09:09:40Z OLiver $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "TerritoryRegion.h"

#include "nobBaseMilitary.h"
#include "MilitaryConsts.h"
#include "GameWorld.h"

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

void TerritoryRegion::CalcTerritoryOfBuilding(const GameWorldBase * const gwb, const noBaseBuilding * const building)
{
	unsigned short radius;
	
	if(building->GetBuildingType() == BLD_HARBORBUILDING)
		radius = HARBOR_ALONE_RADIUS;
	else
		radius = static_cast<const nobBaseMilitary*>(building)->GetMilitaryRadius();

	// Punkt, auf dem das Militärgebäude steht
	MapCoord x = building->GetX(),y = building->GetY();
	TestNode(x,y,building->GetPlayer(),0);
	
	for(unsigned r = 1;r<=radius;++r)
	{
		// Eins weiter nach links gehen
		gwb->GetPointA(x,y,0);
		
		for(unsigned dir = 0;dir<6;++dir)
		{
			for(unsigned short i = 0;i<r;++i)
			{
				TestNode(x,y,building->GetPlayer(),r);
				// Nach rechts oben anfangen
				gwb->GetPointA(x,y,(2+dir)%6);
			}
		}
	}
}


//for(unsigned short x = fx;x < building->GetX()-radius+(y+(building->GetY()&1))/2+radius*2+1-y;++x)
//				TestNode(x,building->GetY()+y,building->GetPlayer(),y);
