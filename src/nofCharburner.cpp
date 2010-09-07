// $Id: nofCharburner.cpp 6713 2010-09-07 06:57:58Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "nofCharburner.h"

#include "GameWorld.h"
#include "noGranite.h"
#include "Loader.h"
#include "macros.h"
#include "GameClient.h"
#include "Ware.h"
#include "Random.h"
#include "SoundManager.h"
#include "GameWorld.h"
#include "GameInterface.h"
#include "noCharburnerPile.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofCharburner::nofCharburner(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace)
: nofFarmhand(JOB_CHARBURNER,x,y,player,workplace), harvest(false)
{
}

nofCharburner::nofCharburner(SerializedGameData * sgd, const unsigned obj_id) : nofFarmhand(sgd,obj_id),
harvest(sgd->PopBool())
{
}

/// Malt den Arbeiter beim Arbeiten
void nofCharburner::DrawWorking(int x,int y)
{
	if(harvest)
	{
		unsigned short now_id = GameClient::inst().Interpolate(39,current_ev);

		// Schaufel-Sound
		if(now_id == 6 || now_id == 18 || now_id == 30)
		{
			SoundManager::inst().PlayNOSound(76,this,now_id/12,200);
			was_sounding = true;
		}

		unsigned draw_id;
		if(now_id < 36)
			draw_id = 9 + now_id % 12;
		else
			draw_id = 9 + 12 + (now_id-36);


		LOADER.GetImageN("charburner_bobs",draw_id)->Draw(x,y,0,0,0,0,0,0, COLOR_WHITE, COLORS[gwg->GetPlayer(player)->color]);
	}
	else
		LOADER.GetImageN("charburner_bobs",1+GameClient::inst().Interpolate(18,current_ev)%6)->Draw(x,y,0,0,0,0,0,0, COLOR_WHITE, COLORS[gwg->GetPlayer(player)->color]);

}

/// Fragt die abgeleitete Klasse um die ID in JOBS.BOB, wenn der Beruf Waren raustr‰gt (bzw rein)
unsigned short nofCharburner::GetCarryID() const
{
	return 1000;
}

/// Abgeleitete Klasse informieren, wenn sie anf‰ngt zu arbeiten (Vorbereitungen)
void nofCharburner::WorkStarted()
{
}

/// Abgeleitete Klasse informieren, wenn fertig ist mit Arbeiten
void nofCharburner::WorkFinished()
{
	noBase * no = gwg->GetNO(x,y);

	// Is a charburner pile is already there?
	if(no->GetGOT() == GOT_CHARBURNERPILE)
	{
		// One step further
		static_cast<noCharburnerPile*>(no)->NextStep();
		return;
	
	}

	// Point still good?
	if(IsPointGood(x,y))
	{
		// Delete previous elements
		// Only environt objects and signs are allowed to be removed by the worker!
		// Otherwise just do nothing
		NodalObjectType nop = no->GetType();

		if(nop == NOP_ENVIRONMENT || nop == NOP_NOTHING)
		{
			no = gwg->GetSpecObj<noBase>(x,y);
			if(no)
			{
				no->Destroy();
				delete no;
			}

			// Plant charburner pile
			gwg->SetNO(new noCharburnerPile(x,y),x,y);

			// BQ drumrum neu berechnen
			gwg->RecalcBQAroundPoint(x,y);
		}
	}
}

/// Fragt abgeleitete Klasse, ob hier Platz bzw ob hier ein Baum etc steht, den z.B. der Holzf‰ller braucht
bool nofCharburner::IsPointGood(const MapCoord x, const MapCoord y)
{
	noBase * no = gwg->GetNO(x,y);

	// Is a charburner pile already here?
	if(no->GetGOT() == GOT_CHARBURNERPILE)
	{
		// Can it be harvested?
		if(!static_cast<noCharburnerPile*>(no)->IsSmoldering())
		{
			// Then let's take it
			return true;
		}
	}

	// Der Platz muss frei sein
	noBase::BlockingManner bm = gwg->GetNO(x,y)->GetBM();

	if(bm != noBase::BM_NOTBLOCKING)
		return false;

	// Kein Grenzstein darf da stehen
	if(gwg->GetNode(x,y).boundary_stones[0])
		return false;


	// darf auﬂerdem nich auf einer Straﬂe liegen
	for(unsigned char i = 0;i<6;++i)
	{
		if(gwg->GetPointRoad(x,y,i))
			return false;
	}

	for(unsigned char i = 0;i<6;++i)
	{
		// Don't set it next to buildings and other charburner piles and grain fields
		BlockingManner bm = gwg->GetNO(gwg->GetXA(x,y,i),gwg->GetYA(x,y,i))->GetBM();
		if(bm == BM_GRANITE || bm == BM_CASTLE || bm == BM_HOUSE || bm == BM_HUT)
			return false;
	}

	// Terrain untersuchen (nur auf Wiesen und Savanne und Steppe pflanzen
	unsigned char t,good_terrains = 0;

	for(unsigned char i = 0;i<6;++i)
	{
		t = gwg->GetTerrainAround(x,y,i);
		if(t == 3 || (t>=8 && t<=12))
			++good_terrains;
	}

	return (good_terrains == 6);
}


void nofCharburner::Serialize(SerializedGameData *sgd) const
{
	Serialize_nofFarmhand(sgd);

	sgd->PushBool(harvest);
}

