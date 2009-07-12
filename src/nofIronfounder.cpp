// $Id: nofIronfounder.cpp 5253 2009-07-12 14:42:18Z FloSoft $
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
#include "nofIronfounder.h"

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

nofIronfounder::nofIronfounder(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace)
: nofWorkman(JOB_IRONFOUNDER,x,y,player,workplace)
{
}

nofIronfounder::nofIronfounder(SerializedGameData * sgd, const unsigned obj_id) : nofWorkman(sgd,obj_id)
{
}

void nofIronfounder::DrawWorking(int x, int y)
{
	signed char offsets[4][2] = { {-22,12},{-23,3},{-19,8},{-18,4} };

	unsigned now_id = GAMECLIENT.Interpolate(272,current_ev);


	if(now_id<182)
	{
		LOADER.GetImageN("rom_bobs", 100+(now_id%8))
			->Draw(x+offsets[workplace->GetNation()][0],y+offsets[workplace->GetNation()][1],0,0,0,0,0,0, COLOR_WHITE, COLORS[GAMECLIENT.GetPlayer(workplace->GetPlayer())->color]);

		// Evtl Sound abspielen
		if(now_id%16 == 3)
		{
			SoundManager::inst().PlayNOSound(58,this,now_id/16);
			was_sounding = true;
		}
	}
}

GoodType nofIronfounder::ProduceWare()
{
	return GD_IRON;
}
