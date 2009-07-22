// $Id: nofCarpenter.cpp 5312 2009-07-22 18:02:04Z OLiver $
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
#include "nofCarpenter.h"
#include "Loader.h"
#include "macros.h"
#include "GameClient.h"
#include "nobUsual.h"
#include "SoundManager.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofCarpenter::nofCarpenter(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace)
: nofWorkman(JOB_CARPENTER,x,y,player,workplace)
{
}

nofCarpenter::nofCarpenter(SerializedGameData * sgd, const unsigned obj_id) : nofWorkman(sgd,obj_id)
{
}

void nofCarpenter::DrawWorking(int x, int y)
{
	signed char offsets[4][2] = { {30,3},{38,3},{30,8},{17,-2} };

	unsigned now_id;

	LOADER.GetImageN("rom_bobs", 32+((now_id = GAMECLIENT.Interpolate(136,current_ev))%8))
		->Draw(x+offsets[workplace->GetNation()][0],y+offsets[workplace->GetNation()][1],0,0,0,0,0,0,COLOR_WHITE, COLORS[gwg->GetPlayer(workplace->GetPlayer())->color]);

	// Evtl Sound abspielen
	if(now_id%8 == 3 || now_id%8 == 7)
	{
		SoundManager::inst().PlayNOSound(54+((now_id)%8)/4,this,now_id/4);
		was_sounding = true;
	}

	last_id = now_id;
}

GoodType nofCarpenter::ProduceWare()
{
	return GD_BOARDS;
}
