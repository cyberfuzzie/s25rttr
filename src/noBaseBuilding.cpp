// $Id: noBaseBuilding.cpp 6005 2010-02-12 10:08:09Z FloSoft $
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
#include "noBaseBuilding.h"
#include "GameWorld.h"
#include "GameClient.h"
#include "GameClientPlayer.h"
#include "DoorConsts.h"
#include "noExtension.h"
#include "Random.h"
#include "nobBaseWarehouse.h"
#include "Ware.h"
#include "WindowManager.h"
#include "SerializedGameData.h"
#include "GameInterface.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

noBaseBuilding::noBaseBuilding(const NodalObjectType nop,const BuildingType type,const unsigned short x, const unsigned short y,const unsigned char player)
: noRoadNode(nop,x,y,player), type(type), nation(GAMECLIENT.GetPlayer(player)->nation), door_point_x(1000000), door_point_y(DOOR_CONSTS[GAMECLIENT.GetPlayer(player)->nation][type])
{
	// Evtl Flagge setzen, wenn noch keine da ist
 	if(gwg->GetNO(x + (y&1),y+1)->GetType() != NOP_FLAG)
	{
		// ggf. vorherige Objekte lˆschen
		noBase *no = gwg->GetSpecObj<noBase>(x + (y&1),y+1);
		if(no)
		{
			no->Destroy();
			delete no;
		}
		gwg->SetNO(new noFlag(x + (y&1),y+1,player), x + (y&1), y+1);
	}

	// Straﬂeneingang setzen (wenn nicht schon vorhanden z.b. durch vorherige Baustelle!)
	if(!gwg->GetPointRoad(x,y,4))
	{
		gwg->SetPointRoad(x,y, 4, 1);

		// Straﬂenverbindung erstellen zwischen Flagge und Haus
		// immer von Flagge ZU Geb‰ude (!)
		std::vector<unsigned char> route(1,1);
		// Straﬂe zuweisen
		gwg->GetSpecObj<noRoadNode>(x + (y&1),y+1)->routes[1] = // der Flagge
		routes[4] = // dem Geb‰ude
		new RoadSegment(RoadSegment::RT_NORMAL,gwg->GetSpecObj<noRoadNode>(x + (y&1),y+1),this,route);
	}
	else
	{
		// vorhandene Straﬂe der Flagge nutzen
		noFlag * flag = gwg->GetSpecObj<noFlag>(x + (y&1),y+1);

		assert(flag->routes[1]);
		routes[4] = flag->routes[1];
		routes[4]->SetF2(this);
		
	}

	// Werde/Bin ich (mal) ein groﬂes Schloss? Dann m¸ssen die Anbauten gesetzt werden
	if(GetSize() == BQ_CASTLE || GetSize() == BQ_HARBOR)
	{
		unsigned short coords[6] = {x-1,y,x-!(y&1),y-1,x+ (y&1),y-1};

		for(unsigned short i = 0;i<3;++i)
		{
			noBase *no = gwg->GetSpecObj<noBase>(coords[i*2],coords[i*2+1]);
			if(no)
			{
				no->Destroy();
				delete no;
			}
			gwg->SetNO(new noExtension(this),coords[i*2],coords[i*2+1]);
		}
	}
}

noBaseBuilding::~noBaseBuilding()
{
}

void noBaseBuilding::Destroy_noBaseBuilding()
{
	gwg->GetGameInterface()->GI_UpdateMinimap(x,y);

	// evtl Anbauten wieder abreiﬂen
	DestroyBuildingExtensions();

	// ggf. Fenster schlieﬂen
	gwg->ImportantObjectDestroyed(x,y);

	Destroy_noRoadNode();
}

void noBaseBuilding::Serialize_noBaseBuilding(SerializedGameData * sgd) const
{
	Serialize_noRoadNode(sgd);

	sgd->PushUnsignedChar(static_cast<unsigned char>(type));
	sgd->PushUnsignedChar(nation);
	sgd->PushSignedInt(door_point_x);
	sgd->PushSignedInt(door_point_y);
}

noBaseBuilding::noBaseBuilding(SerializedGameData * sgd, const unsigned obj_id) : noRoadNode(sgd,obj_id),
type(BuildingType(sgd->PopUnsignedChar())),
nation(Nation(sgd->PopUnsignedChar())),
door_point_x(sgd->PopSignedInt()),
door_point_y(sgd->PopSignedInt())
{
}

short noBaseBuilding::GetDoorPointX()
{
	// Door-Points ausrechnen (Punkte, bis wohin die Menschen gehen, wenn sie ins
	// Geb‰ude gehen, woa uﬂerdem der Bauarbeiter baut und wo die Waren liegen

	// Door-Point noch nicht ausgerechnet?
	if(door_point_x == 1000000)
	{
		int x1 = static_cast<int>(gwg->GetTerrainX(this->x,this->y));
		int y1 = static_cast<int>(gwg->GetTerrainY(this->x,this->y));
		int x2 = static_cast<int>(gwg->GetTerrainX(this->x+(this->y&1),this->y+1));
		int y2 = static_cast<int>(gwg->GetTerrainY(this->x+(this->y&1),this->y+1));

		door_point_x = (DOOR_CONSTS[GAMECLIENT.GetPlayer(player)->nation][type]*(x1-x2))/(y1-y2);
	}

	return (short)(door_point_x & 0xFFFF);
}

noFlag * noBaseBuilding::GetFlag() const
{
	return gwg->GetSpecObj<noFlag>(x + (y&1),y+1);
}


void noBaseBuilding::WareNotNeeded(Ware * ware)
{
	if(ware->LieInWarehouse())
	{
		// Bestellung im Lagerhaus stornieren
		static_cast<nobBaseWarehouse*>(ware->GetLocation())->CancelWare(ware);
		// Ware muss auch noch vernichtet werden!
		// Inventur entsprechend verringern
		//GAMECLIENT.GetPlayer(player)->DecreaseInventoryWare(ware->type,1);
		GAMECLIENT.GetPlayer(player)->RemoveWare(ware);
		delete ware;
	}
		
	else
		ware->GoalDestroyed();
}


void noBaseBuilding::DestroyBuildingExtensions()
{
	// Nur bei groﬂen Geb‰uden gibts diese Anbauten
	if(GetSize() == BQ_CASTLE || GetSize() == BQ_HARBOR)
	{
		noBase *no;
		
		no = gwg->GetSpecObj<noBase>(x-1, y);
		if(no)
		{
			no->Destroy();
			delete no;
			gwg->SetNO(0, x-1, y);
		}
		
		no = gwg->GetSpecObj<noBase>(x - !(y&1), y-1);
		if(no)
		{
			no->Destroy();
			delete no;
			gwg->SetNO(0, x - !(y&1), y-1);
		}
		
		no = gwg->GetSpecObj<noBase>(x + (y&1), y-1);
		if(no)
		{
			no->Destroy();
			delete no;
			gwg->SetNO(0, x + (y&1), y-1);
		}
	}
}

