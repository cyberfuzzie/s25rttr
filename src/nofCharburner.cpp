// $Id: nofCharburner.cpp 6458 2010-05-31 11:38:51Z FloSoft $
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
: nofFarmhand(JOB_FORESTER,x,y,player,workplace)
{
}

nofCharburner::nofCharburner(SerializedGameData * sgd, const unsigned obj_id) : nofFarmhand(sgd,obj_id)
{
}

/// Malt den Arbeiter beim Arbeiten
void nofCharburner::DrawWorking(int x,int y)
{
	/*unsigned short now_id;

	// Schaufel-Sound
	if(now_id == 7 || now_id == 18)
	{
		SoundManager::inst().PlayNOSound(76,this,(now_id == 7)?0:1,200);
		was_sounding = true;
	}*/

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

	// Wenn irgendwo ne Straﬂe schon ist, NICHT einsetzen!
	for(unsigned i = 0;i<6;++i)
	{
		if(gwg->GetPointRoad(x,y,i))
			return;
	}

	// Wenn Objekt ein Zierobjekt ist, dann lˆschen, ansonsten den Holzberg nicht setzen
	if(no->GetType() == NOP_ENVIRONMENT || no->GetType() == NOP_NOTHING)
	{
		if(no->GetType() == NOP_ENVIRONMENT)
		{
			no->Destroy();
			delete no;
		}
		
		// Holzberg setzen

	
		// BQ drumherum neu berechnen
		gwg->RecalcBQAroundPoint(x,y);
	}
}

/// Fragt abgeleitete Klasse, ob hier Platz bzw ob hier ein Baum etc steht, den z.B. der Holzf‰ller braucht
bool nofCharburner::IsPointGood(const MapCoord x, const MapCoord y)
{
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

	// es d¸rfen auﬂerdem keine Geb‰ude rund um den Baum stehen
	for(unsigned char i = 0;i<6;++i)
	{
		if(gwg->GetNO(gwg->GetXA(x,y,i),gwg->GetYA(x,y,i))->GetType() ==  NOP_BUILDING)
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
