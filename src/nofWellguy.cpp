// $Id: nofWellguy.cpp 5167 2009-07-02 18:49:25Z FloSoft $
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
#include "nofWellguy.h"

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

nofWellguy::nofWellguy(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace)
: nofWorkman(JOB_HELPER,x,y,player,workplace)
{
}

nofWellguy::nofWellguy(SerializedGameData * sgd, const unsigned obj_id) : nofWorkman(sgd,obj_id)
{
}

void nofWellguy::DrawWorking(int x, int y)
{
	signed char offsets[4][2] = { {-20,17},{-18,17},{-20,13},{-20,15} };

	signed char walkoffsets[4][8][2] = { //nation, schrit, x-y
	    { {7,7},{9,9},{5,12},{2,14},{-1,17},{-4,17},{-7,17},{-10,17} },
	    { {4,4},{8,8},{5,12},{2,14},{-1,17},{-3,19},{-6,19},{-8,19} },
	    { {5,5},{8,8},{5,10},{2,13},{-1,13},{-4,13},{-7,13},{-10,13} },
	    { {5,5},{8,8},{5,10},{2,13},{-1,15},{-4,15},{-7,15},{-10,15} }
	    };
    signed char walkdirection[4][6]= {
        {4,5,0,3,2,1},
        {4,5,0,3,2,1},
        {4,5,0,3,2,1},
        {4,5,0,3,2,1}
        };

    unsigned int max_id = 112;
	unsigned now_id = GAMECLIENT.Interpolate(max_id,current_ev);
	unsigned char wpNation = workplace->GetNation();
	unsigned int plColor = GAMECLIENT.GetPlayer(player)->color;

	//position zum rauslaufen berechnen
	int walkx = x+walkoffsets[wpNation][now_id%8][0];
	int walky = y+walkoffsets[wpNation][now_id%8][1];
	//position zum reinlaufen berechnen
    int walkx_r = x+walkoffsets[wpNation][7-(now_id%8)][0];
    int walky_r = y+walkoffsets[wpNation][7-(now_id%8)][1];

    if(now_id<2){ //laufen 1
        if(wpNation==2) GetBobImage(workplace->GetNation(),250+5*BLD_WELL+4)->Draw(x,y,0,0,0,0,0,0);
        GetBobFile(carrier_bob)->Draw(10,walkdirection[wpNation][0],false,now_id%8,walkx,walky,COLORS[plColor]);
    }
	else if( (now_id>=2)&&(now_id<4) ){ //laufen 2
        if(wpNation==2)GetBobImage(workplace->GetNation(),250+5*BLD_WELL+4)->Draw(x,y,0,0,0,0,0,0);
        GetBobFile(carrier_bob)->Draw(10,walkdirection[wpNation][1],false,now_id%8,walkx,walky,COLORS[plColor]);
    }
    else if( (now_id>=4)&&(now_id<8) ){ //laufen 3
        GetBobFile(carrier_bob)->Draw(10,walkdirection[wpNation][2],false,now_id%8,walkx,walky,COLORS[plColor]);
    }
    else if( (now_id>=8)&&(now_id<16) ){ //eimer runter lassen
        if(now_id==8){
            GetRomBob(346)
                ->Draw(x+offsets[workplace->GetNation()][0],y+offsets[wpNation][1],0,0,0,0,0,0, COLOR_WHITE, COLORS[plColor]);
        }else{
            GetRomBob(346+(now_id%8)-1)
                ->Draw(x+offsets[workplace->GetNation()][0],y+offsets[wpNation][1],0,0,0,0,0,0, COLOR_WHITE, COLORS[plColor]);
        }
    }
    else if( (now_id>=16)&&(now_id<max_id-16) ){ //kurbeln
        GetRomBob(330+(now_id%8))
            ->Draw(x+offsets[workplace->GetNation()][0],y+offsets[wpNation][1],0,0,0,0,0,0, COLOR_WHITE, COLORS[plColor]);
    }
    else if( (now_id>=max_id-16)&&(now_id<max_id-8) ){ //eimer rauf kurbeln
        GetRomBob(338+(now_id%8))
            ->Draw(x+offsets[workplace->GetNation()][0],y+offsets[wpNation][1],0,0,0,0,0,0, COLOR_WHITE, COLORS[plColor]);
    }
    else if( (now_id>=max_id-8)&&(now_id<max_id-4) ){ //laufen 3
        GetBobFile(carrier_bob)->Draw(11,walkdirection[wpNation][3],false,now_id%8,walkx_r,walky_r,COLORS[plColor]);
    }
    else if( (now_id>=max_id-4)&&(now_id<max_id-2) ){ //laufen 2
        if(wpNation==2) GetBobImage(workplace->GetNation(),250+5*BLD_WELL+4)->Draw(x,y,0,0,0,0,0,0);
        GetBobFile(carrier_bob)->Draw(11,walkdirection[wpNation][4],false,now_id%8,walkx_r,walky_r,COLORS[plColor]);
    }
    else if(now_id>=max_id-2){ //laufen 1
        if(wpNation==2) GetBobImage(workplace->GetNation(),250+5*BLD_WELL+4)->Draw(x,y,0,0,0,0,0,0);
        GetBobFile(carrier_bob)->Draw(11,walkdirection[wpNation][5],false,now_id%8,walkx_r,walky_r,COLORS[plColor]);
    }


	if((now_id>=8) && (now_id<max_id-8) && now_id%8 == 4)
	{
		SoundManager::inst().PlayNOSound(82,this,now_id);
		was_sounding = true;
	}
}

GoodType nofWellguy::ProduceWare()
{
	return GD_WATER;
}
