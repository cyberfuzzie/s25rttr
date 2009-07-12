// $Id: nofForester.cpp 5253 2009-07-12 14:42:18Z FloSoft $
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
#include "nofForester.h"

#include "GameWorld.h"
#include "noGranite.h"
#include "Loader.h"
#include "macros.h"
#include "GameClient.h"
#include "Ware.h"
#include "Random.h"
#include "noTree.h"
#include "SoundManager.h"
#include "GameWorld.h"
#include "GameInterface.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofForester::nofForester(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace)
: nofFarmhand(JOB_FORESTER,x,y,player,workplace)
{
}

nofForester::nofForester(SerializedGameData * sgd, const unsigned obj_id) : nofFarmhand(sgd,obj_id)
{
}

/// Malt den Arbeiter beim Arbeiten
void nofForester::DrawWorking(int x,int y)
{
	unsigned short now_id;
	// Baum pflanzen
	LOADER.GetImageN("rom_bobs", 48+(now_id=GAMECLIENT.Interpolate(36,current_ev)))
		->Draw(x,y,0,0,0,0,0,0, COLOR_WHITE, COLORS[GAMECLIENT.GetPlayer(player)->color]);

	// Schaufel-Sound
	if(now_id == 7 || now_id == 18)
	{
		SoundManager::inst().PlayNOSound(76,this,(now_id == 7)?0:1,200);
		was_sounding = true;
	}
	// Baum-Einpflanz-Sound
	else if(now_id == 35)
	{
		SoundManager::inst().PlayNOSound(57,this,2);
		was_sounding = true;
	}

}

/// Fragt die abgeleitete Klasse um die ID in JOBS.BOB, wenn der Beruf Waren raustr‰gt (bzw rein)
unsigned short nofForester::GetCarryID() const
{
	return 0;
}

/// Abgeleitete Klasse informieren, wenn sie anf‰ngt zu arbeiten (Vorbereitungen)
void nofForester::WorkStarted()
{
}

/// Abgeleitete Klasse informieren, wenn fertig ist mit Arbeiten
void nofForester::WorkFinished()
{
	noBase * no = gwg->GetNO(x,y);

	// Wenn irgendwo ne Straﬂe schon ist, NICHT einsetzen!
	for(unsigned i = 0;i<6;++i)
	{
		if(gwg->GetPointRoad(x,y,i))
			return;
	}

	// Wenn Objekt ein Zierobjekt ist, dann lˆschen, ansonsten den Baum NICHT einsetzen!
	if(no->GetType() == NOP_ENVIRONMENT || no->GetType() == NOP_NOTHING)
	{
		if(no->GetType() == NOP_ENVIRONMENT)
		{
			no->Destroy();
			delete no;
		}

		// Je nach Landschaft andere B‰ume pflanzbar!
		const unsigned char AVAILABLE_TREES_COUNT[3] =
		{
			6,3,4
		};
		const unsigned char AVAILABLE_TREES[3][8] =
		{
			{0,1,2,6,7,8,	0xFF,0xFF},
			{0,1,7,			0xFF,0xFF,0xFF,0xFF,0xFF},
			{0,1,6,8,		0xFF,0xFF,0xFF,0xFF}
		};

		// jungen Baum einsetzen
		gwg->SetNO(new noTree(x,y,AVAILABLE_TREES[gwg->GetLandscapeType()]
		[RANDOM.Rand(__FILE__,__LINE__,obj_id,AVAILABLE_TREES_COUNT[gwg->GetLandscapeType()])],0),x,y);

		// BQ drumherum neu berechnen
		gwg->RecalcBQAroundPoint(x,y);

		// Minimap Bescheid geben (neuer Baum)
		gwg->GetGameInterface()->GI_UpdateMinimap(x,y);
	}
}

/// Fragt abgeleitete Klasse, ob hier Platz bzw ob hier ein Baum etc steht, den z.B. der Holzf‰ller braucht
bool nofForester::IsPointGood(const unsigned short x, const unsigned short y)
{
	// Der Platz muss frei sein
	noBase * no = gwg->GetNO(x,y);

	if(	no->GetType() == NOP_FLAG || 
		no->GetGOT()  == GOT_SIGN ||
		no->GetType() == NOP_GRANITE || 
		no->GetType() == NOP_TREE || 
		no->GetType() == NOP_GRAINFIELD ||
		no->GetType() == NOP_BUILDING || 
		no->GetType() == NOP_BUILDINGSITE || 
		no->GetType() == NOP_EXTENSION || 
		no->GetType() == NOP_FIRE ||
		no->GetType() == NOP_OBJECT)
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
