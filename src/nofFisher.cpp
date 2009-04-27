// $Id: nofFisher.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "nofFisher.h"

#include "GameWorld.h"
#include "Loader.h"
#include "macros.h"
#include "GameClient.h"
#include "Random.h"
#include "SoundManager.h"
#include "SerializedGameData.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

nofFisher::nofFisher(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace)
: nofFarmhand(JOB_FISHER,x,y,player,workplace), fishing_dir(0), successful(false)
{
}

void nofFisher::Serialize_nofFisher(SerializedGameData * sgd) const
{
	Serialize_nofFarmhand(sgd);

	sgd->PushUnsignedChar(fishing_dir);
	sgd->PushBool(successful);
}

nofFisher::nofFisher(SerializedGameData * sgd, const unsigned obj_id) : nofFarmhand(sgd,obj_id),
fishing_dir(sgd->PopUnsignedChar()),
successful(sgd->PopBool())
{
}

/// Malt den Arbeiter beim Arbeiten
void nofFisher::DrawWorking(int x,int y)
{
	unsigned short id = GAMECLIENT.Interpolate(232,current_ev);
	unsigned short draw_id;

	if(id<16)
	{
		// Angel reinlassen
		if(fishing_dir < 3)
			draw_id = 1566+8*fishing_dir+(id/2);
		else
			draw_id = 108+8*(fishing_dir-3)+(id/2);

		if(id/2 == 1)
		{
			SoundManager::inst().PlayNOSound(62,this,0);
			was_sounding = true;
		}
	}
	else if(id < 216)
	{
		// Angel im Wasser h�ngen lassen und warten
		draw_id = 1590+8*((fishing_dir+3)%6)+(id%8);
	}
	else
	{
		// Angel wieder rausholn
		if(successful)
			// Mit Fisch an der Angel
			draw_id = 1638+8*((fishing_dir+3)%6)+(id-216)/2;
		else
		{
			// Ohne Fisch (wie das Reinlassen, blo� umgekehrt)
			if(fishing_dir < 3)
				draw_id = 1566+8*fishing_dir+7-+(id-216)/2;
			else
				draw_id = 108+8*(fishing_dir-3)+7-(id-216)/2;
		}

		if((id-216)/2 == 1)
		{
			SoundManager::inst().PlayNOSound(62,this,1);
			was_sounding = true;
		}
	}

	GetRomBob(draw_id)->Draw(x,y,0,0,0,0,0,0,COLORS[GAMECLIENT.GetPlayer(player)->color]);
	DrawShadow(x,y,0,fishing_dir);
}

/// Fragt die abgeleitete Klasse um die ID in JOBS.BOB, wenn der Beruf Waren raustr�gt (bzw rein)
unsigned short nofFisher::GetCarryID() const
{
	return 70;
}

/// Abgeleitete Klasse informieren, wenn sie anf�ngt zu arbeiten (Vorbereitungen)
void nofFisher::WorkStarted()
{
	unsigned char doffset = RANDOM.Rand(__FILE__,__LINE__,obj_id,6);
	// Punkt mit Fisch suchen (mit zuf�lliger Richtung beginnen)
	fishing_dir = 0xFF;
	for(unsigned char i = 0;i<6;++i)
	{
		fishing_dir = (i+doffset)%6;
		if(gwg->GetNode(gwg->GetXA(x,y,fishing_dir),gwg->GetYA(x,y,fishing_dir)).resources > 0x80 && 
			gwg->GetNode(gwg->GetXA(x,y,fishing_dir),gwg->GetYA(x,y,fishing_dir)).resources < 0x90)
			break;
	}

	// Wahrscheinlichkeit, einen Fisch zu fangen sinkt mit abnehmendem Bestand
	unsigned short probability = 40+(gwg->GetNode(gwg->GetXA(x,y,fishing_dir),gwg->GetYA(x,y,fishing_dir)).resources-0x80)*10;
	successful = (RANDOM.Rand(__FILE__,__LINE__,obj_id,100) < probability);
}



/// Abgeleitete Klasse informieren, wenn fertig ist mit Arbeiten
void nofFisher::WorkFinished()
{
	// Wenn ich einen Fisch gefangen habe, den Fisch "abbauen" und in die Hand nehmen
	if(successful)
	{
		--(gwg->GetNode(gwg->GetXA(x,y,fishing_dir),gwg->GetYA(x,y,fishing_dir)).resources);
		ware = GD_FISH;
	}
	else
		ware = GD_NOTHING;
}

/// Fragt abgeleitete Klasse, ob hier Fisch gefangen wrden kann
bool nofFisher::IsPointGood(const unsigned short x, const unsigned short y)
{
	// Der Punkt muss passierbar sein f�r Figuren
	if(!gwg->IsNodeForFigures(x,y))
		return false;

	// irgendwo drumherum muss es Fisch geben
	for(unsigned char i = 0;i<6;++i)
	{
		if(gwg->GetNode(gwg->GetXA(x,y,i),gwg->GetYA(x,y,i)).resources > 0x80 && 
			gwg->GetNode(gwg->GetXA(x,y,i),gwg->GetYA(x,y,i)).resources < 0x90)
			return true;
	}

	return false;
}

