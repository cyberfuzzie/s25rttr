// $Id: nofStonemason.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "nofStonemason.h"

#include "GameWorld.h"
#include "noGranite.h"
#include "Loader.h"
#include "macros.h"
#include "GameClient.h"
#include "Ware.h"
#include "SoundManager.h"
#include "GameInterface.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofStonemason::nofStonemason(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace)
: nofFarmhand(JOB_STONEMASON,x,y,player,workplace)
{
}

nofStonemason::nofStonemason(SerializedGameData * sgd, const unsigned obj_id) : nofFarmhand(sgd,obj_id)
{
}

/// Malt den Arbeiter beim Arbeiten
void nofStonemason::DrawWorking(int x,int y)
{
	unsigned now_id;

	// Stein hauen
	GetRomBob(40+(now_id=GAMECLIENT.Interpolate(64,current_ev))%8)
		->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);

	if(now_id%8 == 5)
	{
		SoundManager::inst().PlayNOSound(56,this,now_id);
		was_sounding = true;
	}

}

/// Fragt die abgeleitete Klasse um die ID in JOBS.BOB, wenn der Beruf Waren rausträgt (bzw rein)
unsigned short nofStonemason::GetCarryID() const
{
	return 63;
}

/// Abgeleitete Klasse informieren, wenn sie anfängt zu arbeiten (Vorbereitungen)
void nofStonemason::WorkStarted()
{
}

/// Abgeleitete Klasse informieren, wenn fertig ist mit Arbeiten
void nofStonemason::WorkFinished()
{
	// Stein abhauen (wenn er nur noch ganz klein ist, dann wird er von der Landkarte getilgt)
	if(gwg->GetSpecObj<noGranite>(x,y)->IsSmall())
	{
		// Granitklötzchen löschen
		gwg->GetSpecObj<noGranite>(x,y)->Destroy();
		delete gwg->GetSpecObj<noGranite>(x,y);
		gwg->SetNO(0,x,y);

		// Minimap Bescheid geben (Granitglötzchen muss weg)
		gwg->GetGameInterface()->GI_UpdateMinimap(x,y);

		// Drumherum BQ neu berechnen, da diese sich ja jetzt hätten ändern können
		gwg->RecalcBQAroundPoint(x,y);
	}
	else
		// ansonsten wird er um 1 kleiner
		gwg->GetSpecObj<noGranite>(x,y)->Hew();

	// Stein in die Hand nehmen
	ware = GD_STONES;
}

/// Fragt abgeleitete Klasse, ob hier Platz bzw ob hier ein Baum etc steht, den z.B. der Holzfäller braucht
bool nofStonemason::IsPointGood(const unsigned short x, const unsigned short y)
{
	// An dieser Position muss es nur Stein geben
	return (gwg->GetNO(x,y)->GetType() == NOP_GRANITE);
}

