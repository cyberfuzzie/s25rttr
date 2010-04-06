// $Id: nofScout_LookoutTower.cpp 6280 2010-04-06 12:40:52Z OLiver $
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
#include "nofScout_LookoutTower.h"

#include "GameWorld.h"
#include "MilitaryConsts.h"
#include "nobUsual.h"
#include "GameClient.h"
#include "nobHarborBuilding.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofScout_LookoutTower::nofScout_LookoutTower(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace)
: nofBuildingWorker(JOB_SCOUT,x,y,player,workplace)
{
}

nofScout_LookoutTower::nofScout_LookoutTower(SerializedGameData * sgd, const unsigned obj_id)
: nofBuildingWorker(sgd,obj_id)
{
}


void nofScout_LookoutTower::Serialize_nofScout_LookoutTower(SerializedGameData * sgd) const
{
	Serialize_nofBuildingWorker(sgd);
}


void nofScout_LookoutTower::WalkedDerived()
{
}

void nofScout_LookoutTower::DrawWorking(int x,int y)
{
}

void nofScout_LookoutTower::HandleDerivedEvent(const unsigned int id)
{
}

void nofScout_LookoutTower::WorkAborted()
{
	// Im enstprechenden Radius alles neu berechnen
	gwg->RecalcVisibilitiesAroundPoint(x,y,VISUALRANGE_LOOKOUTTOWER,player,workplace);
}

void nofScout_LookoutTower::WorkplaceReached()
{
	// Sind wir ggf. in einem Hafen angekommen, um einer Expedition beizutreten?
	noBase * rn;
	if((rn = gwg->GetNO(x,y))->GetGOT() == GOT_NOB_HARBORBUILDING)
	{
		// Mich hier einquartieren
		gwg->RemoveFigure(this,x,y);
		static_cast<nobHarborBuilding*>(rn)->AddFigure(this);
		
	}
	else
	{
		// Im enstprechenden Radius alles sichtbar machen
		gwg->SetVisibilitiesAroundPoint(x,y,VISUALRANGE_LOOKOUTTOWER,player);

		// Und Post versenden
		if(GameClient::inst().GetPlayerID() == this->player)
			GameClient::inst().SendPostMessage(new ImagePostMsgWithLocation(
				_("Lookout-tower occupied"), PMC_MILITARY, x, y, workplace->GetBuildingType(), workplace->GetNation()));
	}
}
