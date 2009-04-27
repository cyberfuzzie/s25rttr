// $Id: nofMiller.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "nofMiller.h"

#include "Loader.h"
#include "macros.h"
#include "GameClient.h"
#include "nobUsual.h"
#include "VideoDriverWrapper.h"
#include "SoundManager.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofMiller::nofMiller(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace)
: nofWorkman(JOB_MILLER,x,y,player,workplace), last_sound(0), next_interval(0)
{
}

void nofMiller::Serialize_nofMiller(SerializedGameData * sgd) const
{
	Serialize_nofWorkman(sgd);
}

nofMiller::nofMiller(SerializedGameData * sgd, const unsigned obj_id) : nofWorkman(sgd,obj_id), last_sound(0), next_interval(0)
{
}

void nofMiller::DrawWorking(int x, int y)
{
	signed char offsets[4][2] = { {20,8},{20,8},{20,8},{20,8} };
	signed char offsets_sitdown[4][2] = { {23,8},{23,8},{23,8},{23,8} };
	signed char walkoffsets[8][2] = { {8,8},{10,9},{12,10},{14,11},{16,10},{18,9},{20,8},{22,8} };

    unsigned int max_id = 120;
	unsigned now_id = GAMECLIENT.Interpolate(max_id,current_ev);
	bool rotate_sails=true;

    if(now_id<4){  //hinauslaufen teil 1
        GetBobImage(workplace->GetNation(),250+5*BLD_MILL+4)->Draw(x,y,0,0,0,0,0,0);
        GetBobFile(jobs_bob)->Draw(16,4,true,now_id%8,x+walkoffsets[now_id%8][0],y+walkoffsets[now_id%8][1],COLORS[GAMECLIENT.GetPlayer(player)->color]);
        rotate_sails=false;
    }
    if( (now_id>=4) && (now_id<8) ){  //hinauslaufen teil 2
        GetBobFile(jobs_bob)->Draw(16,3,true,now_id%8,x+walkoffsets[now_id%8][0],y+walkoffsets[now_id%8][1],COLORS[GAMECLIENT.GetPlayer(player)->color]);
    }
    if( (now_id>=8) && (now_id<16)){  //hinsetzen
        GetRomBob(166+(now_id%8))
            ->Draw(x+offsets_sitdown[workplace->GetNation()][0],y+offsets_sitdown[workplace->GetNation()][1],0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(workplace->GetPlayer())->color]);
    }
    if( (now_id>=16) && (now_id<max_id-16)){  //schlafen
        GetRomBob(174+(now_id%8))
            ->Draw(x+offsets[workplace->GetNation()][0],y+offsets[workplace->GetNation()][1],0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(workplace->GetPlayer())->color]);
    }
    if( (now_id>=max_id-16) && (now_id<max_id-8)){ //aufstehn
        GetRomBob(166+7-(now_id%8))
            ->Draw(x+offsets_sitdown[workplace->GetNation()][0],y+offsets_sitdown[workplace->GetNation()][1],0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(workplace->GetPlayer())->color]);
    }
    if( (now_id>=max_id-8) && (now_id<max_id-4)){  //zurücklaufen teil 1
        GetBobFile(jobs_bob)->Draw(16,0,true,now_id%8,x+walkoffsets[7-(now_id%8)][0],y+walkoffsets[7-(now_id%8)][1],COLORS[GAMECLIENT.GetPlayer(player)->color]);
    }
    if( (now_id>=max_id-4) && (now_id<max_id)){  //zurücklaufen teil 2
        GetBobImage(workplace->GetNation(),250+5*BLD_MILL+4)->Draw(x,y,0,0,0,0,0,0);
        GetBobFile(jobs_bob)->Draw(16,1,true,now_id%8,x+walkoffsets[7-(now_id%8)][0],y+walkoffsets[7-(now_id%8)][1],COLORS[GAMECLIENT.GetPlayer(player)->color]);
        rotate_sails=false;
    }

    if (rotate_sails){
        // Flügel der Mühle
        GetBobImage(workplace->GetNation(),250+5*(42+((now_id+4)%8)))->Draw(x,y,0,0,0,0,0,0);
        // Schatten der Flügel
        GetBobImage(workplace->GetNation(),250+(5*(42+((now_id+4)%8)))+1)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);

		// Mühlensound abspielen in zufälligen Intervallen
		if(VideoDriverWrapper::inst().GetTickCount() - last_sound > next_interval)
		{
			SoundManager::inst().PlayNOSound(77,this,now_id);
			was_sounding = true;

			last_sound = VideoDriverWrapper::inst().GetTickCount();
			next_interval = 500+rand()%1400;
		}
			

    }else{
        // Flügel der Mühle
        GetBobImage(workplace->GetNation(),250+5*49)->Draw(x,y,0,0,0,0,0,0);
        // Schatten der Flügel
        GetBobImage(workplace->GetNation(),250+5*49+1)->Draw(x,y,0,0,0,0,0,0,COLOR_SHADOW);
    }

}

GoodType nofMiller::ProduceWare()
{
	return GD_FLOUR;
}
