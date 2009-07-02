// $Id: nofFarmer.cpp 5167 2009-07-02 18:49:25Z FloSoft $
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
#include "nofFarmer.h"

#include "GameWorld.h"
#include "noGranite.h"
#include "Loader.h"
#include "macros.h"
#include "GameClient.h"
#include "Ware.h"
#include "noEnvObject.h"
#include "noGrainfield.h"
#include "SoundManager.h"
#include "nobUsual.h"
#include "SerializedGameData.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofFarmer::nofFarmer(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace)
: nofFarmhand(JOB_FARMER,x,y,player,workplace), harvest(false)
{
}


void nofFarmer::Serialize_nofFarmer(SerializedGameData * sgd) const
{
	Serialize_nofFarmhand(sgd);

	sgd->PushBool(harvest);
}

nofFarmer::nofFarmer(SerializedGameData * sgd, const unsigned obj_id) : nofFarmhand(sgd,obj_id),
harvest(sgd->PopBool())
{
}


/// Malt den Arbeiter beim Arbeiten
void nofFarmer::DrawWorking(int x,int y)
{
	unsigned now_id;


    if(harvest){
        GetRomBob(140+(now_id=GAMECLIENT.Interpolate(88,current_ev))%8)
            ->Draw(x,y,0,0,0,0,0,0, COLOR_WHITE, COLORS[GAMECLIENT.GetPlayer(player)->color]);

		// Evtl Sound abspielen
		if(now_id%8 == 3)
		{
			SoundManager::inst().PlayNOSound(64,this,now_id/8);
			was_sounding = true;
		}

    }else{
        GetRomBob(132+GAMECLIENT.Interpolate(88,current_ev)%8)
            ->Draw(x,y,0,0,0,0,0,0, COLOR_WHITE, COLORS[GAMECLIENT.GetPlayer(player)->color]);
    }


}

/// Fragt die abgeleitete Klasse um die ID in JOBS.BOB, wenn der Beruf Waren rausträgt (bzw rein)
unsigned short nofFarmer::GetCarryID() const
{
	return 71;
}

/// Abgeleitete Klasse informieren, wenn sie anfängt zu arbeiten (Vorbereitungen)
void nofFarmer::WorkStarted()
{
	// Wenn ich zu einem Getreidefeld gehe, ernte ich es ab, ansonsten sähe ich
	harvest = (gwg->GetNO(x,y)->GetType() == NOP_GRAINFIELD);

	// Getreidefeld Bescheid sagen, damits nicht plötzlich verschwindet, während wir arbeiten
	if(harvest)
		gwg->GetSpecObj<noGrainfield>(x,y)->BeginHarvesting();
}

/// Abgeleitete Klasse informieren, wenn fertig ist mit Arbeiten
void nofFarmer::WorkFinished()
{
    if(harvest)
	{
		// Getreidefeld vernichten und vorher noch die ID von dem abgeernteten Feld holen, was dann als
		// normales Zierobjekt gesetzt wird
		noGrainfield * gf = gwg->GetSpecObj<noGrainfield>(x,y);
        //unsigned env_obj_id = gf->GetHarvestMapLstID();
        gwg->SetNO(new noEnvObject(x, y, gf->GetHarvestMapLstID()), x, y);
		gf->Destroy();
		delete gf;

		// Getreide, was wir geerntet haben, in die Hand nehmen
        ware = GD_GRAIN;
    }
	else
	{
		// Was stand hier vorher?
		NodalObjectType nop = gwg->GetNO(x,y)->GetType();

		// Nur Zierobjekte und Schilder dürfen weggerissen werden
		if(nop == NOP_ENVIRONMENT || nop == NOP_NOTHING)
		{
			// ggf. vorher wegreißen
			noBase * no = gwg->GetSpecObj<noBase>(x,y);
			if(no)
			{
				no->Destroy();
				delete no;
			}
			// neues Getreidefeld setzen
			gwg->SetNO(new noGrainfield(x,y),x,y);
		}

		// Wir haben nur gesäht (gar nichts in die Hand nehmen)
		ware = GD_NOTHING;
    }

	// BQ drumrum neu berechnen
	gwg->RecalcBQAroundPoint(x,y);
}

/// Fragt abgeleitete Klasse, ob hier Platz bzw ob hier ein Baum etc steht, den z.B. der Holzfäller braucht
bool nofFarmer::IsPointGood(const unsigned short x, const unsigned short y)
{
	
	// Entweder gibts ein Getreidefeld, das wir abernten können...
	if(gwg->GetNO(x,y)->GetType() == NOP_GRAINFIELD)
	{
		return (gwg->GetSpecObj<noGrainfield>(x,y)->IsHarvestable());
	}
	// oder einen freien Platz, wo wir ein neues sähen können
	else
	{
		// Nicht auf Straßen bauen!
		for(unsigned char i = 0;i<6;++i)
		{
			if(gwg->GetPointRoad(x,y,i))
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
		if (good_terrains != 6) 
			return false;


		// Ist Platz frei?
		NodalObjectType nop = gwg->GetNO(x,y)->GetType();
		if(nop != NOP_ENVIRONMENT && nop && nop != NOP_NOTHING)
			return false;

		for(unsigned char i = 0;i<6;++i)
		{
			// Nicht direkt neben andere Getreidefelder und Gebäude setzen!
			nop = gwg->GetNO(gwg->GetXA(x,y,i),gwg->GetYA(x,y,i))->GetType();
			if(nop == NOP_GRAINFIELD || nop == NOP_BUILDING || nop == NOP_BUILDINGSITE)
				return false;
		}

		// Nicht direkt neben den Bauernhof pflanzen!
		if(x == workplace->GetX()+1)
			return false;

		return true;
	}

}


void nofFarmer::WorkAborted_Farmhand()
{
	// dem Getreidefeld Bescheid sagen, damit es wieder verdorren kann, wenn wir abernten
	if(harvest && state == STATE_WORK)
		gwg->GetSpecObj<noGrainfield>(x,y)->EndHarvesting();
}

